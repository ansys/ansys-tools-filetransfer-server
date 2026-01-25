// Copyright (C) 2022 - 2026 ANSYS, Inc. and/or its affiliates.
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

#include <fstream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <grpctransportlib/logging.h>

namespace grpctransportlib {
namespace impl {
constexpr const char* PATHSEP =
#ifdef _WIN32
    "\\";
#else
    "/";
#endif

#ifdef _WIN32
#include <io.h>
inline bool file_exists(const std::string& path) {
    return _access(path.c_str(), 0) == 0;
}
#else
#include <unistd.h>
inline bool file_exists(const std::string& path) {
    return access(path.c_str(), F_OK) == 0;
}
#endif

inline std::string read_file(const std::string& file_path_) {
    std::ifstream file(file_path_, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + file_path_);
    }
    return std::string(
        (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()
    );
}

struct Certificates {
    std::string certificate_chain;
    std::string private_key;
    std::string root_certificates;
};

enum CertKind { SERVER, CLIENT };

Certificates read_cert_files(
    const std::string& certs_dir_,
    const std::shared_ptr<LoggerInterface>& logger_,
    CertKind kind_ = CertKind::SERVER
) {
    const auto kind_name = (kind_ == SERVER) ? "server" : "client";
    const auto cert_file = certs_dir_ + PATHSEP + kind_name + ".crt";
    const auto key_file = certs_dir_ + PATHSEP + kind_name + ".key";
    const auto ca_file = certs_dir_ + PATHSEP + "ca.crt";

    // Check if all required files exist
    std::vector<std::string> missing_files;
    if (!file_exists(cert_file)) {
        missing_files.push_back(cert_file);
    }
    if (!file_exists(key_file)) {
        missing_files.push_back(key_file);
    }
    if (!file_exists(ca_file)) {
        missing_files.push_back(ca_file);
    }

    if (!missing_files.empty()) {
        std::string error_msg = "Missing required TLS file(s) for mutual TLS: ";
        for (std::size_t i = 0; i < missing_files.size(); ++i) {
            if (i > 0) {
                error_msg += ", ";
            }
            error_msg += missing_files[i];
        }
        throw std::runtime_error(error_msg);
    }
    logger_->debug(
        {"Reading mTLS certificate files cert: " + cert_file +
         ", key: " + key_file + ", CA: " + ca_file}
    );

    // Read certificate files
    const std::string certificate_chain = read_file(cert_file);
    const std::string private_key = read_file(key_file);
    const std::string root_certificates = read_file(ca_file);
    return {
        std::move(certificate_chain),
        std::move(private_key),
        std::move(root_certificates)
    };
}

} // namespace impl
} // namespace grpctransportlib
