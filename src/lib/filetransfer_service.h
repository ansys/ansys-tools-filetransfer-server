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

// namespace detail {
using pb_progress_t =
    decltype(::ansys::api::utilities::filetransfer::v1::ProgressResponse()
                 .state());
using pb_filesize_t =
    decltype(::ansys::api::utilities::filetransfer::v1::FileInfo().size());

enum Progress : pb_progress_t {
    INITIALIZED = 0,
    COMPLETED = 100,
};
// }

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
    virtual auto DownloadFile(
        ::grpc::ServerContext* context,
        ::grpc::ServerReaderWriter<
            ::ansys::api::utilities::filetransfer::v1::DownloadFileResponse,
            ::ansys::api::utilities::filetransfer::v1::DownloadFileRequest>*
            stream
    ) -> ::grpc::Status override;

    /// <summary>Implements the "UploadFile" operation.</summary>
    /// <param name="context">The server context to be used.</param>
    /// <param name="stream">The stream of requests and responses to be
    /// processes.</param> <returns>The result of the operation.</returns>
    virtual auto UploadFile(
        ::grpc::ServerContext* context,
        ::grpc::ServerReaderWriter<
            ::ansys::api::utilities::filetransfer::v1::UploadFileResponse,
            ::ansys::api::utilities::filetransfer::v1::UploadFileRequest>*
            stream
    ) -> ::grpc::Status override;

private:
};

} // namespace file_transfer
