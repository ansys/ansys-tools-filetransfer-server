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

#include "filetransfer_service.h"

#ifdef _MSC_VER
#pragma warning(push, 3)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/log/trivial.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

#include "exception_handling.h"
#include "exception_types.h"

namespace file_transfer {

auto FileTransferServiceImpl::DeleteFile(
    ::grpc::ServerContext* /* unused */,
    const ::ansys::api::tools::filetransfer::v1::DeleteFileRequest* request,
    ::ansys::api::tools::filetransfer::v1::DeleteFileResponse* /* unused */
) -> ::grpc::Status {

    return exceptions::convert_exceptions_to_status_codes(
        std::function<void()>([&]() {
            const auto& filename = request->filename();
            const auto file_path = boost::filesystem::path{filename};

            BOOST_LOG_TRIVIAL(info) << "Got deletion request for file: "
                                    << file_path.generic_string();

            if (!boost::filesystem::exists(file_path)) {
                throw exceptions::failed_precondition(
                    "File does not exist: " + filename
                );
            }
            if (!boost::filesystem::is_regular_file(file_path)) {
                throw exceptions::failed_precondition(
                    "Path " + filename + " is not a regular file."
                );
            }

            boost::filesystem::remove(file_path);
            BOOST_LOG_TRIVIAL(info)
                << "Deleted file: " << file_path.generic_string();
        })
    );
}

} // namespace file_transfer
