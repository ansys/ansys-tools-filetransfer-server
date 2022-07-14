#include "filetransfer_service.h"

#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
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

::grpc::Status FileTransferServiceImpl::UploadFile(
    ::grpc::ServerContext*,
    ::grpc::ServerReaderWriter<
        ::ansys::api::utilities::filetransfer::v1::UploadFileResponse,
        ::ansys::api::utilities::filetransfer::v1::UploadFileRequest>* stream) {

    return exceptions::convert_exceptions_to_status_codes(std::function<
                                                          void()>([&]() {
        google::protobuf::Arena requestArena;
        google::protobuf::Arena responseArena;
        ::ansys::api::utilities::filetransfer::v1::UploadFileRequest& request =
            *(google::protobuf::Arena::CreateMessage<
                ::ansys::api::utilities::filetransfer::v1::UploadFileRequest>(
                &requestArena));

        // ==== INITIALIZE ====
        if (!stream->Read(&request)) {
            throw exceptions::invalid_argument(
                "Empty download request stream.");
        }
        if (request.sub_step_case() != ::ansys::api::utilities::filetransfer::
                                           v1::UploadFileRequest::kInitialize) {
            throw exceptions::invalid_argument(
                "Upload stream did not start with an initialize step.");
        }
        const auto& file_info = request.initialize().file_info();

        const std::filesystem::path file_path{file_info.name()};
        const auto file_size =
            boost::numeric_cast<std::size_t>(file_info.size());
        const std::string source_sha1_hex = file_info.sha1().hex_digest();

        std::ofstream out_file;
        try {
            out_file.open(file_path);
        } catch (const std::exception&) {
            throw exceptions::failed_precondition(
                "Could not open output file.");
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
                throw exceptions::invalid_argument(
                    "Received more data than the specified file size.");
            }
            out_file << chunk;
            progress->set_state(boost::numeric_cast<pb_progress_t>(
                (100 * num_bytes_received) / file_size));
            stream->Write(response);
        }

        // ==== FINALIZE ====
        if (request.sub_step_case() != ::ansys::api::utilities::filetransfer::
                                           v1::UploadFileRequest::kFinalize) {
            throw exceptions::invalid_argument(
                "Unexpected upload step, should be 'finalize'.");
        }
        if (num_bytes_received != file_size) {
            throw exceptions::invalid_argument(
                "Received an incorrect number of bytes.");
        }
        out_file.close();

        if (!source_sha1_hex.empty()) {
            const auto dest_sha1_hex = detail::get_sha1_hex_digest(file_path);
            if (source_sha1_hex != dest_sha1_hex) {
                throw exceptions::data_loss("Checksum of the received file "
                                            "does not match expected value.");
            }
        }

        progress->set_state(Progress::COMPLETED);
        stream->Write(response);
    }));
}

} // namespace file_transfer
