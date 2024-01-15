// Copyright (C) 2024 ANSYS, Inc. and/or its affiliates.
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

#include <cstdint>
#include <exception>
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

namespace upload_impl {

namespace api = ::ansys::api::tools::filetransfer::v1;
using stream_t =
    ::grpc::ServerReaderWriter<api::UploadFileResponse, api::UploadFileRequest>;

auto get_request_checked(
    api::UploadFileRequest* request_,
    stream_t* stream_,
    const api::UploadFileRequest::SubStepCase& expected_step_
) -> void {
    if (!stream_->Read(request_)) {
        throw exceptions::invalid_argument("Request stream stopped prematurely."
        );
    }
    if (request_->sub_step_case() != expected_step_) {
        throw exceptions::invalid_argument(
            "Incorrect request step. Expected " +
            std::to_string(expected_step_) + ", but got " +
            std::to_string(request_->sub_step_case()) + "."
        );
    }
}

auto get_request_checked(
    google::protobuf::Arena& arena_,
    stream_t* stream_,
    const api::UploadFileRequest::SubStepCase& expected_step_
) -> api::UploadFileRequest* {
    auto* request =
        google::protobuf::Arena::CreateMessage<api::UploadFileRequest>(&arena_);
    get_request_checked(request, stream_, expected_step_);
    return request;
}

auto initialize(google::protobuf::Arena& arena_, stream_t* stream_) -> std::
    tuple<const boost::filesystem::path, const std::size_t, const std::string> {
    auto& request = *get_request_checked(
        arena_, stream_, api::UploadFileRequest::kInitialize
    );

    const auto& file_info = request.initialize().file_info();

    const boost::filesystem::path file_path{file_info.name()};
    const auto file_size = boost::numeric_cast<std::size_t>(file_info.size());
    const std::string source_sha1_hex = file_info.sha1().hex_digest();

    auto& response = *(
        google::protobuf::Arena::CreateMessage<api::UploadFileResponse>(&arena_)
    );
    auto& progress = *response.mutable_progress();
    progress.set_state(Progress::INITIALIZED);
    stream_->Write(response);

    BOOST_LOG_TRIVIAL(info)
        << "Initializing upload of file:" << file_path.generic_string()
        << "\n  file size: " << file_size
        << "\n  SHA1 checksum: " << source_sha1_hex;

    return std::make_tuple(file_path, file_size, source_sha1_hex);
}

auto transfer(
    const boost::filesystem::path& file_path_,
    const std::size_t file_size_,
    google::protobuf::Arena& arena_,
    stream_t* stream_
) -> void {
    boost::filesystem::ofstream out_file;
    try {
        out_file.open(file_path_, std::ios_base::binary);
    } catch (const std::exception&) {
        throw exceptions::failed_precondition("Could not open output file.");
    }

    std::size_t num_bytes_received = 0;

    auto& request =
        *google::protobuf::Arena::CreateMessage<api::UploadFileRequest>(&arena_
        );
    auto& response =
        *google::protobuf::Arena::CreateMessage<api::UploadFileResponse>(&arena_
        );
    auto& progress = *response.mutable_progress();

    while (num_bytes_received < file_size_) {
        get_request_checked(
            &request, stream_, api::UploadFileRequest::kSendData
        );
        const auto chunk = request.send_data().file_data().data();
        const auto current_chunk_size = chunk.size();
        if (current_chunk_size <= 0) {
            throw exceptions::invalid_argument("Received empty file chunk.");
        }
        num_bytes_received += current_chunk_size;

        BOOST_LOG_TRIVIAL(debug) << "Received " << num_bytes_received << " of "
                                 << file_size_ << " bytes.";

        out_file << chunk;
        progress.set_state(boost::numeric_cast<pb_progress_t>(
            (100 * num_bytes_received) / file_size_
        ));
        stream_->Write(response);
    }
    if (num_bytes_received != file_size_) {
        throw exceptions::invalid_argument(
            "Received an incorrect number of bytes."
        );
    }
}

auto finalize(
    const boost::filesystem::path& file_path_,
    const std::string& source_sha1_hex_,
    google::protobuf::Arena& arena_,
    stream_t* stream_
) -> void {
    get_request_checked(arena_, stream_, api::UploadFileRequest::kFinalize);
    auto& response =
        *google::protobuf::Arena::CreateMessage<api::UploadFileResponse>(&arena_
        );
    auto& progress = *response.mutable_progress();

    if (!source_sha1_hex_.empty()) {
        const auto dest_sha1_hex = detail::get_sha1_hex_digest(file_path_);
        if (source_sha1_hex_ != dest_sha1_hex) {
            throw exceptions::data_loss("Checksum of the received file "
                                        "does not match expected value.");
        }
    }

    progress.set_state(Progress::COMPLETED);
    stream_->Write(response);
    BOOST_LOG_TRIVIAL(info) << "Upload complete.";
}

} // namespace upload_impl

auto FileTransferServiceImpl::UploadFile(
    ::grpc::ServerContext* /*unused*/,
    ::grpc::ServerReaderWriter<
        ::ansys::api::tools::filetransfer::v1::UploadFileResponse,
        ::ansys::api::tools::filetransfer::v1::UploadFileRequest>* stream_
) -> ::grpc::Status {

    return exceptions::convert_exceptions_to_status_codes(
        std::function<void()>([&]() {
            google::protobuf::Arena arena;

            auto [file_path, file_size, source_sha1_hex] =
                upload_impl::initialize(arena, stream_);

            upload_impl::transfer(file_path, file_size, arena, stream_);

            upload_impl::finalize(file_path, source_sha1_hex, arena, stream_);
        })
    );
}

} // namespace file_transfer
