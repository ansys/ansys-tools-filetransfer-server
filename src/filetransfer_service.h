#pragma once

#ifdef _MSC_VER
#pragma warning(push, 3)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <ansys/api/utilities/filetransfer/v1/file_transfer_service.grpc.pb.h>

#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

namespace file_transfer {

/// <summary>This class implements the file transfer service.</summary>
class FileTransferServiceImpl final
    : public ::ansys::api::utilities::filetransfer::v1::FileTransferService::
          Service {

public:
    // ---------- RPC services [file transfer] ----------

    /// <summary>Implements the "DownloadFile" operation.</summary>
    /// <param name="context">The server context to be used.</param>
    /// <param name="stream">The stream of requests and responses to be
    /// processes.</param> <returns>The result of the operation.</returns>
    virtual ::grpc::Status DownloadFile(
        ::grpc::ServerContext* context,
        ::grpc::ServerReaderWriter<
            ::ansys::api::utilities::filetransfer::v1::DownloadFileResponse,
            ::ansys::api::utilities::filetransfer::v1::DownloadFileRequest>*
            stream) override;

    /// <summary>Implements the "UploadFile" operation.</summary>
    /// <param name="context">The server context to be used.</param>
    /// <param name="stream">The stream of requests and responses to be
    /// processes.</param> <returns>The result of the operation.</returns>
    virtual ::grpc::Status UploadFile(
        ::grpc::ServerContext* context,
        ::grpc::ServerReaderWriter<
            ::ansys::api::utilities::filetransfer::v1::UploadFileResponse,
            ::ansys::api::utilities::filetransfer::v1::UploadFileRequest>*
            stream) override;

private:
};

} // namespace file_transfer
