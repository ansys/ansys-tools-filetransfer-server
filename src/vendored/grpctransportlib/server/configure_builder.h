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

#include <memory>
#include <stdexcept>
#include <string>

#include <grpcpp/grpcpp.h>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/stat.h>
#endif

#include <grpctransportlib/impl/utils.h>
#include <grpctransportlib/logging.h>
#include <grpctransportlib/options.h>

namespace grpctransportlib {

/**
 * Base class for transport resource handlers.
 *
 * This class serves as a base for resource handlers that manage resources
 * associated with different transport modes. Derived classes can implement
 * specific resource management logic as needed.
 **/
class TransportResourceHandler {
public:
    TransportResourceHandler() = default;
    TransportResourceHandler(const TransportResourceHandler&) = delete;
    TransportResourceHandler&
    operator=(const TransportResourceHandler&) = delete;
    TransportResourceHandler(TransportResourceHandler&&) noexcept = default;
    TransportResourceHandler&
    operator=(TransportResourceHandler&&) noexcept = default;
    virtual ~TransportResourceHandler() = default;
};

namespace impl_configure {

// Resource handler implementations

class TrivialTransportResourceHandler : public TransportResourceHandler {
public:
    TrivialTransportResourceHandler() = default;
    ~TrivialTransportResourceHandler() override = default;
};

class DeleteUDSFileHandler : public TransportResourceHandler {
public:
    DeleteUDSFileHandler(
        std::string uds_file_path_, std::shared_ptr<LoggerInterface> logger_
    )
        : m_uds_file_path(std::move(uds_file_path_)),
          m_logger(std::move(logger_)){};
    ~DeleteUDSFileHandler() override {
        try {
            if (!m_uds_file_path.empty() &&
                impl::file_exists(m_uds_file_path)) {
#ifdef _WIN32
                DeleteFileA(m_uds_file_path.c_str());
#else
                unlink(m_uds_file_path.c_str());
#endif
                m_logger->debug({"Deleted socket file: " + m_uds_file_path});
            }
        } catch (const std::exception& e) {
            m_logger->warning(
                {"Failed to delete socket file: " + m_uds_file_path +
                 ". Error: " + e.what()}
            );
        }
    };

private:
    std::string m_uds_file_path;
    std::shared_ptr<LoggerInterface> m_logger;
};

// Builder configuration implementations

inline std::unique_ptr<TrivialTransportResourceHandler>
configure_server_builder_impl(
    const std::string& /* app_name_ */,
    grpc::ServerBuilder& builder_,
    const InsecureOptions& options_,
    const std::shared_ptr<LoggerInterface>& logger_
) {

    const auto server_address = options_.host + ":" + options_.port;
    // TODO: check warning message
    logger_->warning(
        {"Starting gRPC server without TLS on " + server_address +
         ". Modification of these configurations is not recommended. "
         "Please see the documentation for your installed product for "
         "additional information."}
    );

    builder_.AddListeningPort(
        server_address, grpc::InsecureServerCredentials()
    );
    logger_->info({"Server listening on " + server_address});
    return std::make_unique<TrivialTransportResourceHandler>();
}

inline std::unique_ptr<DeleteUDSFileHandler> configure_server_builder_impl(
    const std::string& app_name_,
    grpc::ServerBuilder& builder_,
    const UDSOptions& options_,
    const std::shared_ptr<LoggerInterface>& logger_
) {
    const auto socket_filename =
        options_.uds_id.empty() ? app_name_ + ".sock"
                                : app_name_ + "-" + options_.uds_id + ".sock";
#ifdef _WIN32
    CreateDirectoryA(options_.uds_dir.c_str(), NULL);
#else
    // Create directory if it doesn't exist - permissions to write only by user
    // (0700)
    mkdir(options_.uds_dir.c_str(), 0700);
#endif
    const auto socket_path = options_.uds_dir + impl::PATHSEP + socket_filename;
    const auto server_address = "unix:" + socket_path;

    if (impl::file_exists(socket_path)) {
        throw std::runtime_error(
            "UDS socket file already exists at " + socket_path + ". " +
            "Another server may already be running on this socket. " +
            "Please stop the existing server or use a different UDS ID. " +
            "See option --uds-id."
        );
    }

    builder_.AddListeningPort(
        server_address, grpc::InsecureServerCredentials()
    );
    logger_->info({"Server listening on Unix Domain Socket " + server_address});

    return std::make_unique<DeleteUDSFileHandler>(socket_path, logger_);
}

inline std::unique_ptr<TrivialTransportResourceHandler>
configure_server_builder_impl(
    const std::string& /* app_name_ */,
    grpc::ServerBuilder& builder_,
    const MTLSOptions& options_,
    const std::shared_ptr<LoggerInterface>& logger_
) {
    const auto server_address = options_.host + ":" + options_.port;
    const auto certs = impl::read_cert_files(options_.certs_dir, logger_);

    // Create SSL server credentials with mutual TLS
    grpc::SslServerCredentialsOptions ssl_opts;
    ssl_opts.pem_root_certs = certs.root_certificates;
    ssl_opts.pem_key_cert_pairs.push_back(
        {certs.private_key, certs.certificate_chain}
    );
    ssl_opts.client_certificate_request =
        GRPC_SSL_REQUEST_AND_REQUIRE_CLIENT_CERTIFICATE_AND_VERIFY;

    const auto server_credentials = grpc::SslServerCredentials(ssl_opts);
    builder_.AddListeningPort(server_address, server_credentials);

    logger_->info({"Server listening on gRPC+mutualTLS " + server_address});
    return std::make_unique<TrivialTransportResourceHandler>();
}

} // namespace impl_configure

/**
 * Configures the gRPC server builder based on the provided validated transport
 *options.
 *
 * @param app_name_ The name of the application (used for UDS socket naming).
 * @param builder_ The gRPC server builder to configure.
 * @param options_ The validated transport options.
 * @param logger_ A shared pointer to a logger interface for logging messages.
 * @return A unique pointer to a TransportResourceHandler that manages any
 *resources associated with the selected transport mode. The caller is
 *responsible for maintaining the lifetime of this handler for as long as the
 *server is running.
 **/
[[nodiscard]] inline std::unique_ptr<TransportResourceHandler>
configure_server_builder(
    const std::string& app_name_,
    grpc::ServerBuilder& builder_,
    const ValidatedTransportOptions& options_,
    const std::shared_ptr<LoggerInterface>& logger_
) {
    switch (options_.mode()) {
    case TransportMode::INSECURE: {
        const auto& insecure_opts = options_.insecure();
        return impl_configure::configure_server_builder_impl(
            app_name_, builder_, insecure_opts, logger_
        );
    } break;
    case TransportMode::UDS: {
        const auto& uds_opts = options_.uds();
        return impl_configure::configure_server_builder_impl(
            app_name_, builder_, uds_opts, logger_
        );
    } break;
    case TransportMode::MTLS: {
        const auto& mtls_opts = options_.mtls();
        return impl_configure::configure_server_builder_impl(
            app_name_, builder_, mtls_opts, logger_
        );
    } break;
    default:
        throw std::runtime_error("Invalid transport mode.");
    }
}

} // namespace grpctransportlib
