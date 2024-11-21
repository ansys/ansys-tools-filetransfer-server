// Copyright (C) 2022 - 2024 ANSYS, Inc. and/or its affiliates.
// SPDX-License-Identifier: MIT
//
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#ifdef _MSC_VER
#pragma warning(push, 3)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <ansys/api/tools/filetransfer/v1/file_transfer_service.grpc.pb.h>

#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

namespace file_transfer {

using pb_progress_t =
    decltype(::ansys::api::tools::filetransfer::v1::ProgressResponse().state());
using pb_filesize_t =
    decltype(::ansys::api::tools::filetransfer::v1::FileInfo().size());

/**
 * @brief Named constants for the progress of a file transfer operation.
 */
enum Progress : pb_progress_t {
    INITIALIZED = 0,
    COMPLETED = 100,
};

/**
 * @brief This class implements the file transfer service.
 **/
class FileTransferServiceImpl final : public ::ansys::api::tools::filetransfer::
                                          v1::FileTransferService::Service {

public:
    // ---------- RPC services [file transfer] ----------

    /**
     * @brief Implements the "DownloadFile" operation.
     * @param context Server context to use.
     * @param stream Stream of requests and responses to process.
     * @return Result of the operation.
     */
    virtual auto DownloadFile(
        ::grpc::ServerContext* context,
        ::grpc::ServerReaderWriter<
            ::ansys::api::tools::filetransfer::v1::DownloadFileResponse,
            ::ansys::api::tools::filetransfer::v1::DownloadFileRequest>* stream
    ) -> ::grpc::Status override;

    /**
     * @brief Implements the "UploadFile" operation.
     * @param context Server context to use.
     * @param stream Stream of requests and responses to process.
     * @return Result of the operation.
     */
    virtual auto UploadFile(
        ::grpc::ServerContext* context,
        ::grpc::ServerReaderWriter<
            ::ansys::api::tools::filetransfer::v1::UploadFileResponse,
            ::ansys::api::tools::filetransfer::v1::UploadFileRequest>* stream
    ) -> ::grpc::Status override;

    /**
     * @brief Implements the "DeleteFile" operation.
     * @param context Server context to use.
     * @param request Request to process.
     * @param response Response to populate.
     */
    virtual auto DeleteFile(
        ::grpc::ServerContext* context,
        const ::ansys::api::tools::filetransfer::v1::DeleteFileRequest* request,
        ::ansys::api::tools::filetransfer::v1::DeleteFileResponse* response
    ) -> ::grpc::Status override;

    /**
     * @brief Implements the "GetFileInfo" operation.
     * @param context Server context to use.
     * @param request Request to process.
     * @param response Response to populate.
     */
    virtual auto GetFileInfo(
        ::grpc::ServerContext* context,
        const ::ansys::api::tools::filetransfer::v1::GetFileInfoRequest*
            request,
        ::ansys::api::tools::filetransfer::v1::GetFileInfoResponse* response
    ) -> ::grpc::Status override;

private:
};

} // namespace file_transfer
