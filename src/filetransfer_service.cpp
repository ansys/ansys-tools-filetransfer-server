#include "filetransfer_service.h"

#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <ios>
#include <sstream>
#include <string>
#include <utility>

#ifdef _MSC_VER
#pragma warning(push, 3)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <boost/log/trivial.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/uuid/detail/sha1.hpp>

#include <google/protobuf/arena.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

namespace file_transfer {

namespace detail {
std::string get_sha1_hex_digest(
    const std::filesystem::path& path_, const std::size_t chunk_size_ = 1024) {
    std::string buffer(chunk_size_, '\0');
    std::ifstream in_file{path_};
    boost::uuids::detail::sha1 sha_value{};
    if (!in_file.good()) {
        throw std::runtime_error("Could not open file.");
    }
    while (in_file.good()) {
        in_file.read(&buffer[0], chunk_size_);
        sha_value.process_bytes(&buffer[0], in_file.gcount());
    }
    boost::uuids::detail::sha1::digest_type res_int;
    sha_value.get_digest(res_int);

    // std::format not yet supported in our toolchains
    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 5; ++i) {
        ss << std::setfill('0') << std::setw(8) << res_int[i];
    }
    return ss.str();
}
} // namespace detail

using pb_progress_t =
    decltype(::ansys::api::utilities::filetransfer::v1::ProgressResponse()
                 .state());
using pb_filesize_t =
    decltype(::ansys::api::utilities::filetransfer::v1::FileInfo().size());

enum Progress : pb_progress_t {
    INITIALIZED = 0,
    COMPLETED = 100,
};

::grpc::Status FileTransferServiceImpl::DownloadFile(
    ::grpc::ServerContext*,
    ::grpc::ServerReaderWriter<
        ::ansys::api::utilities::filetransfer::v1::DownloadFileResponse,
        ::ansys::api::utilities::filetransfer::v1::DownloadFileRequest>*
        stream) {

    try {
        // receive a request
        google::protobuf::Arena requestArena;
        google::protobuf::Arena responseArena;
        ::ansys::api::utilities::filetransfer::v1::DownloadFileRequest&
            request = *(google::protobuf::Arena::CreateMessage<
                        ::ansys::api::utilities::filetransfer::v1::
                            DownloadFileRequest>(&requestArena));

        // ==== INITIALIZE ====
        if (!stream->Read(&request)) {
            return ::grpc::Status(
                ::grpc::StatusCode::INVALID_ARGUMENT,
                std::string("Empty download request stream."));
        }
        if (request.sub_step_case() !=
            ::ansys::api::utilities::filetransfer::v1::DownloadFileRequest::
                kInitialize) {
            return ::grpc::Status(
                ::grpc::StatusCode::INVALID_ARGUMENT,
                std::string("The download has not been initialized."));
        }

        ::ansys::api::utilities::filetransfer::v1::DownloadFileResponse&
            initialize_response = *(google::protobuf::Arena::CreateMessage<
                                    ::ansys::api::utilities::filetransfer::v1::
                                        DownloadFileResponse>(&responseArena));

        const ::ansys::api::utilities::filetransfer::v1::DownloadFileRequest::
            Initialize& initialize = request.initialize();
        const std::filesystem::path file_path{initialize.filename()};
        const std::size_t chunk_size =
            initialize.chunk_size() > 0 ? initialize.chunk_size() : 1 << 16;
        if (!std::filesystem::exists(file_path)) {
            return ::grpc::Status(
                ::grpc::StatusCode::FAILED_PRECONDITION,
                std::string("The desired file does not exist."));
        }

        auto& file_info = *(initialize_response.mutable_file_info());
        if (initialize.compute_sha1_checksum()) {
            const auto hex_digest = detail::get_sha1_hex_digest(file_path);
            file_info.mutable_sha1()->set_hex_digest(hex_digest);
        }

        std::ifstream in_file{file_path};
        if (!in_file.good()) {
            return ::grpc::Status(
                ::grpc::StatusCode::INTERNAL,
                std::string("Could not open the desired file."));
        }

        file_info.set_name(file_path.string());
        const std::size_t file_size = std::filesystem::file_size(file_path);
        file_info.set_size(boost::numeric_cast<pb_filesize_t>(file_size));
        initialize_response.mutable_progress()->set_state(
            Progress::INITIALIZED);

        BOOST_LOG_TRIVIAL(info)
            << "Initializing download of file " << file_path.generic_string()
            << "\n  file size: " << file_size
            << "\n  chunk_size: " << chunk_size;

        stream->Write(initialize_response);

        // ==== TRANSFER ====
        if (!stream->Read(&request)) {
            return ::grpc::Status(
                ::grpc::StatusCode::INVALID_ARGUMENT,
                std::string("Request stream ended prematurely."));
        }
        if (request.sub_step_case() !=
            ::ansys::api::utilities::filetransfer::v1::DownloadFileRequest::
                kReceiveData) {
            return ::grpc::Status(
                ::grpc::StatusCode::INVALID_ARGUMENT,
                std::string("Failed to ask for receiving data."));
        }

        std::ifstream input_file_stream{file_path};

        const std::size_t num_full_chunks = file_size / chunk_size;
        const std::size_t partial_chunk_size = file_size % chunk_size;

        ::ansys::api::utilities::filetransfer::v1::DownloadFileResponse&
            transfer_response = *(google::protobuf::Arena::CreateMessage<
                                  ::ansys::api::utilities::filetransfer::v1::
                                      DownloadFileResponse>(&responseArena));
        auto* file_chunk = transfer_response.mutable_file_data();
        std::string buffer(chunk_size, '\0');

        std::size_t chunk_index = 0;
        for (; chunk_index < num_full_chunks; ++chunk_index) {
            BOOST_LOG_TRIVIAL(debug) << "Sending chunk " << chunk_index;
            transfer_response.mutable_progress()->set_state(
                boost::numeric_cast<pb_progress_t>(
                    (100 * chunk_index) / num_full_chunks));

            file_chunk->set_offset(chunk_index * chunk_size);

            input_file_stream.read(&buffer[0], chunk_size);
            file_chunk->set_data(buffer);
            stream->Write(transfer_response);
        }
        if (partial_chunk_size) {
            BOOST_LOG_TRIVIAL(debug) << "Sending final partial chunk.";
            transfer_response.mutable_progress()->set_state(
                Progress::COMPLETED);

            file_chunk->set_offset(chunk_index * chunk_size);

            buffer.resize(partial_chunk_size);
            input_file_stream.read(&buffer[0], partial_chunk_size);
            file_chunk->set_data(buffer);

            stream->Write(transfer_response);
        }

        // ==== FINALIZE ====
        if (!stream->Read(&request)) {
            return ::grpc::Status(
                ::grpc::StatusCode::INVALID_ARGUMENT,
                std::string("Request stream ended prematurely."));
        }
        if (request.sub_step_case() != ::ansys::api::utilities::filetransfer::
                                           v1::DownloadFileRequest::kFinalize) {
            return ::grpc::Status(
                ::grpc::StatusCode::INVALID_ARGUMENT,
                std::string("File transfer has not been finalized."));
        }
        ::ansys::api::utilities::filetransfer::v1::DownloadFileResponse&
            finalize_response = *(google::protobuf::Arena::CreateMessage<
                                  ::ansys::api::utilities::filetransfer::v1::
                                      DownloadFileResponse>(&responseArena));
        finalize_response.mutable_progress()->set_state(Progress::COMPLETED);

        stream->Write(finalize_response);
        return ::grpc::Status::OK;
    } catch (const std::exception& e) {
        return ::grpc::Status(
            ::grpc::StatusCode::UNKNOWN,
            std::string("Exception while sending file:\n") + e.what());
    }
}

::grpc::Status FileTransferServiceImpl::UploadFile(
    ::grpc::ServerContext*,
    ::grpc::ServerReaderWriter<
        ::ansys::api::utilities::filetransfer::v1::UploadFileResponse,
        ::ansys::api::utilities::filetransfer::v1::UploadFileRequest>* stream) {

    try {

        google::protobuf::Arena requestArena;
        google::protobuf::Arena responseArena;
        ::ansys::api::utilities::filetransfer::v1::UploadFileRequest& request =
            *(google::protobuf::Arena::CreateMessage<
                ::ansys::api::utilities::filetransfer::v1::UploadFileRequest>(
                &requestArena));

        // ==== INITIALIZE ====
        if (!stream->Read(&request)) {
            return ::grpc::Status(
                ::grpc::StatusCode::INVALID_ARGUMENT,
                std::string("Empty download request stream."));
        }
        if (request.sub_step_case() != ::ansys::api::utilities::filetransfer::
                                           v1::UploadFileRequest::kInitialize) {
            return ::grpc::Status(
                ::grpc::StatusCode::INVALID_ARGUMENT,
                std::string(
                    "Upload stream did not start with an initialize step."));
        }
        const auto& file_info = request.initialize().file_info();

        const std::filesystem::path file_path{file_info.name()};
        const auto file_size =
            boost::numeric_cast<std::size_t>(file_info.size());
        const std::string source_sha1_hex = file_info.sha1().hex_digest();

        std::ofstream out_file;
        try {
            out_file.open(file_path);
        } catch (const std::exception& e) {
            return ::grpc::Status(
                ::grpc::StatusCode::FAILED_PRECONDITION,
                std::string("Could not open output file.\n") + e.what());
        }
        ::ansys::api::utilities::filetransfer::v1::UploadFileResponse&
            response = *(google::protobuf::Arena::CreateMessage<
                         ::ansys::api::utilities::filetransfer::v1::
                             UploadFileResponse>(&responseArena));
        auto* progress = response.mutable_progress();
        progress->set_state(Progress::INITIALIZED);
        stream->Write(response);

        // ==== TRANSFER ====
        std::size_t num_bytes_received = 0;
        while (stream->Read(&request) &&
               request.sub_step_case() ==
                   ::ansys::api::utilities::filetransfer::v1::
                       UploadFileRequest::kSendData) {
            const auto chunk = request.send_data().file_data().data();
            num_bytes_received += chunk.size();
            if (num_bytes_received > file_size) {
                return ::grpc::Status(
                    ::grpc::StatusCode::INVALID_ARGUMENT,
                    std::string(
                        "Received more data than the specified file size."));
            }
            out_file << chunk;
            progress->set_state(boost::numeric_cast<pb_progress_t>(
                (100 * num_bytes_received) / file_size));
            stream->Write(response);
        }

        // ==== FINALIZE ====
        if (request.sub_step_case() != ::ansys::api::utilities::filetransfer::
                                           v1::UploadFileRequest::kFinalize) {
            return ::grpc::Status(
                ::grpc::StatusCode::INVALID_ARGUMENT,
                std::string("Unexpected upload step, should be 'finalize'."));
        }
        if (num_bytes_received != file_size) {
            return ::grpc::Status(
                ::grpc::StatusCode::INVALID_ARGUMENT,
                std::string("Received an incorrect number of bytes."));
        }
        out_file.close();

        if (!source_sha1_hex.empty()) {
            const auto dest_sha1_hex = detail::get_sha1_hex_digest(file_path);
            if (source_sha1_hex != dest_sha1_hex) {
                return ::grpc::Status(
                    ::grpc::StatusCode::DATA_LOSS,
                    "SHA1 of the received file does not match indicated "
                    "value.");
            }
        }

        progress->set_state(Progress::COMPLETED);
        stream->Write(response);

        return ::grpc::Status::OK;
    } catch (const std::exception& e) {
        return ::grpc::Status(
            ::grpc::StatusCode::UNKNOWN,
            std::string("Exception while receiving file:\n") + e.what());
    }
}

} // namespace file_transfer
