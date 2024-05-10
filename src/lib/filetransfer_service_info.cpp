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
#include <boost/numeric/conversion/cast.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

#include "exception_handling.h"
#include "exception_types.h"
#include "sha1_digest.h"

namespace file_transfer {

auto FileTransferServiceImpl::GetFileInfo(
    ::grpc::ServerContext* /* unused */,
    const ::ansys::api::tools::filetransfer::v1::GetFileInfoRequest* request,
    ::ansys::api::tools::filetransfer::v1::GetFileInfoResponse* response
) -> ::grpc::Status {

    return exceptions::convert_exceptions_to_status_codes(
        std::function<void()>([&]() {
            const auto& filename = request->filename();
            const auto file_path = boost::filesystem::path{filename};
            const auto file_exists = boost::filesystem::exists(file_path);
            response->set_exists(file_exists);

            if (file_exists) {
                auto& file_info = *(response->mutable_file_info());
                if (request->compute_sha1_checksum()) {
                    const auto hex_digest =
                        detail::get_sha1_hex_digest(file_path);
                    file_info.mutable_sha1()->set_hex_digest(hex_digest);
                }

                file_info.set_name(file_path.string());
                const std::size_t file_size =
                    boost::filesystem::file_size(file_path);
                file_info.set_size(boost::numeric_cast<pb_filesize_t>(file_size)
                );
            }
        })
    );
}

} // namespace file_transfer
