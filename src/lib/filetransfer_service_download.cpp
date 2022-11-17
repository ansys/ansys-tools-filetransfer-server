#include "filetransfer_service.h"

#include <cstdint>
#include <exception>
#include <ios>
#include <string>
#include <tuple>
#include <utility>

#ifdef _MSC_VER
#pragma warning(push, 3)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <boost/log/trivial.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <google/protobuf/arena.h>

#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

#include "exception_handling.h"
#include "exception_types.h"
#include "sha1_digest.h"

namespace file_transfer {
namespace download_impl {

namespace api = ::ansys::api::utilities::filetransfer::v1;
using stream_t = ::grpc::
    ServerReaderWriter<api::DownloadFileResponse, api::DownloadFileRequest>;

auto get_request_checked(
    google::protobuf::Arena& arena_,
    stream_t* stream_,
    const api::DownloadFileRequest::SubStepCase& expected_step_
) -> api::DownloadFileRequest* {
    auto* request =
        google::protobuf::Arena::CreateMessage<api::DownloadFileRequest>(&arena_
        );
    if (!stream_->Read(request)) {
        throw exceptions::invalid_argument("Request stream stopped prematurely."
        );
    }
    if (request->sub_step_case() != expected_step_) {
        throw exceptions::invalid_argument("Incorrect request step.");
    }
    return request;
}

auto initialize(google::protobuf::Arena& arena_, stream_t* stream_)
    -> std::tuple<
        const boost::filesystem::path,
        const std::size_t,
        const std::streamsize> {

    auto& request = *get_request_checked(
        arena_, stream_, api::DownloadFileRequest::kInitialize
    );

    const auto& initialize = request.initialize();
    const boost::filesystem::path file_path{initialize.filename()};
    const auto chunk_size = boost::numeric_cast<std::streamsize>(
        initialize.chunk_size() > 0 ? initialize.chunk_size() : 1 << 16
    );

    if (!boost::filesystem::exists(file_path)) {
        throw exceptions::not_found(
            "The desired file " + file_path.string() + " does not exist."
        );
    }

    auto& response =
        *google::protobuf::Arena::CreateMessage<api::DownloadFileResponse>(
            &arena_
        );

    auto& file_info = *(response.mutable_file_info());
    if (initialize.compute_sha1_checksum()) {
        const auto hex_digest = detail::get_sha1_hex_digest(file_path);
        file_info.mutable_sha1()->set_hex_digest(hex_digest);
    }

    file_info.set_name(file_path.string());
    const std::size_t file_size = boost::filesystem::file_size(file_path);
    file_info.set_size(boost::numeric_cast<pb_filesize_t>(file_size));
    response.mutable_progress()->set_state(Progress::INITIALIZED);

    BOOST_LOG_TRIVIAL(info)
        << "Initializing download of file " << file_path.generic_string()
        << "\n  file size: " << file_size << "\n  chunk size: " << chunk_size;

    stream_->Write(response);
    return std::make_tuple(file_path, file_size, chunk_size);
}

auto transfer(
    const boost::filesystem::path& file_path_,
    const std::size_t file_size_,
    const std::streamsize chunk_size_,
    google::protobuf::Arena& arena_,
    stream_t* stream_
) -> void {

    get_request_checked(
        arena_, stream_, api::DownloadFileRequest::kReceiveData
    );

    auto input_file_stream =
        boost::filesystem::ifstream{file_path_, std::ios_base::binary};

    const auto num_full_chunks =
        boost::numeric_cast<std::streamsize>(file_size_ / chunk_size_);
    const auto partial_chunk_size =
        boost::numeric_cast<std::streamsize>(file_size_ % chunk_size_);

    auto& transfer_response =
        *(google::protobuf::Arena::CreateMessage<api::DownloadFileResponse>(
            &arena_
        ));
    auto& file_chunk = *transfer_response.mutable_file_data();
    auto buffer = std::string(chunk_size_, '\0');

    auto chunk_index = std::streamsize{0};
    for (; chunk_index < num_full_chunks; ++chunk_index) {
        BOOST_LOG_TRIVIAL(debug) << "Sending chunk " << chunk_index;
        transfer_response.mutable_progress()->set_state(
            boost::numeric_cast<pb_progress_t>(
                (100 * chunk_index) / num_full_chunks
            )
        );

        file_chunk.set_offset(chunk_index * chunk_size_);

        input_file_stream.read(buffer.data(), chunk_size_);
        file_chunk.set_data(buffer);
        stream_->Write(transfer_response);
    }
    if (partial_chunk_size != 0) {
        BOOST_LOG_TRIVIAL(debug) << "Sending final partial chunk.";
        transfer_response.mutable_progress()->set_state(Progress::COMPLETED);

        file_chunk.set_offset(chunk_index * chunk_size_);

        buffer.resize(partial_chunk_size);
        input_file_stream.read(buffer.data(), partial_chunk_size);
        file_chunk.set_data(buffer);

        stream_->Write(transfer_response);
    }
}

auto finalize(google::protobuf::Arena& arena_, stream_t* stream_) -> void {
    get_request_checked(arena_, stream_, api::DownloadFileRequest::kFinalize);

    auto& response =
        *(google::protobuf::Arena::CreateMessage<api::DownloadFileResponse>(
            &arena_
        ));
    response.mutable_progress()->set_state(Progress::COMPLETED);

    stream_->Write(response);
    BOOST_LOG_TRIVIAL(info) << "Download complete.";
}

} // namespace download_impl

auto FileTransferServiceImpl::DownloadFile(
    ::grpc::ServerContext* /*unused*/,
    ::grpc::ServerReaderWriter<
        ::ansys::api::utilities::filetransfer::v1::
            DownloadFileResponse /*unused*/,
        ::ansys::api::utilities::filetransfer::v1::DownloadFileRequest>* stream
) -> ::grpc::Status {

    return exceptions::convert_exceptions_to_status_codes(
        std::function<void()>([&]() {
            google::protobuf::Arena message_arena;

            auto [file_path, file_size, chunk_size] =
                download_impl::initialize(message_arena, stream);

            download_impl::transfer(
                file_path, file_size, chunk_size, message_arena, stream
            );

            download_impl::finalize(message_arena, stream);
        })
    );
}

} // namespace file_transfer
