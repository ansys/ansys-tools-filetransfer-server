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
#include <sstream>
#include <stdexcept>
#include <string>

#include <grpcpp/grpcpp.h>

#include <grpctransportlib/logging.h>

namespace grpctransportlib {

/**
 * Enumeration of supported transport modes for the gRPC server.
 **/
enum TransportMode { INVALID, INSECURE, UDS, MTLS };

/**
 * Structure to hold unvalidated transport options as parsed from the command
 *line.
 *
 * Static defaults are applied in the CLI parsing functions, whereas environment
 *variable defaults are applied during validation.
 **/
struct UnvalidatedTransportOptions {
    TransportMode mode;
    std::string host;
    std::string port;
    bool allow_remote_host;
    std::string uds_dir;
    std::string uds_id;
    std::string certs_dir;
};

/**
 * Validated transport options for insecure transport.
 **/
struct InsecureOptions {
    std::string host;
    std::string port;
};

/**
 * Validated transport options for Unix Domain Sockets (UDS) transport.
 **/
struct UDSOptions {
    std::string uds_dir;
    std::string uds_id;
};

/**
 * Validated transport options for mutual TLS transport.
 **/
struct MTLSOptions {
    std::string certs_dir;
    std::string host;
    std::string port;
};

/**
 * Class to hold validated transport options.
 *
 * This class uses a variant-like approach to store one of the specific
 *transport option structures based on the selected transport mode.
 **/
class ValidatedTransportOptions {
public:
    ValidatedTransportOptions() = default;
    explicit ValidatedTransportOptions(InsecureOptions opts_)
        : m_mode(TransportMode::INSECURE),
          m_insecure(std::make_unique<InsecureOptions>(std::move(opts_))) {}
    explicit ValidatedTransportOptions(UDSOptions opts_)
        : m_mode(TransportMode::UDS),
          m_uds(std::make_unique<UDSOptions>(std::move(opts_))) {}
    explicit ValidatedTransportOptions(MTLSOptions opts_)
        : m_mode(TransportMode::MTLS),
          m_mtls(std::make_unique<MTLSOptions>(std::move(opts_))) {}
    ValidatedTransportOptions(const ValidatedTransportOptions&) = delete;
    ValidatedTransportOptions&
    operator=(const ValidatedTransportOptions&) = delete;
    ValidatedTransportOptions(ValidatedTransportOptions&&) noexcept = default;
    ValidatedTransportOptions&
    operator=(ValidatedTransportOptions&&) noexcept = default;
    ~ValidatedTransportOptions() = default;

    TransportMode mode() const { return m_mode; }
    const InsecureOptions& insecure() const {
        if (m_mode != TransportMode::INSECURE || !m_insecure) {
            throw std::runtime_error("Invalid access to InsecureOptions.");
        }
        return *m_insecure;
    }
    const UDSOptions& uds() const {
        if (m_mode != TransportMode::UDS || !m_uds) {
            throw std::runtime_error("Invalid access to UDSOptions.");
        }
        return *m_uds;
    }
    const MTLSOptions& mtls() const {
        if (m_mode != TransportMode::MTLS || !m_mtls) {
            throw std::runtime_error("Invalid access to MTLSOptions.");
        }
        return *m_mtls;
    }

private:
    TransportMode m_mode;
    std::unique_ptr<InsecureOptions> m_insecure;
    std::unique_ptr<UDSOptions> m_uds;
    std::unique_ptr<MTLSOptions> m_mtls;
};

/**
 * Print the validated transport options using the provided logger interface.
 *
 * @param options_ The validated transport options to print.
 * @param logger_ The logger interface to use for printing.
 **/
inline void print_options(
    const ValidatedTransportOptions& options_, LoggerInterface& logger_
) {
    switch (options_.mode()) {
    case TransportMode::INSECURE: {
        const auto& insecure_opts = options_.insecure();
        logger_.debug({"Transport mode: INSECURE"});
        logger_.debug({"  Host: " + insecure_opts.host});
        logger_.debug({"  Port: " + insecure_opts.port});
    } break;
    case TransportMode::UDS: {
        const auto& uds_opts = options_.uds();
        logger_.debug({"Transport mode: UDS"});
        logger_.debug({" UDS directory: " + uds_opts.uds_dir});
        logger_.debug({" UDS ID: " + uds_opts.uds_id});
    } break;
    case TransportMode::MTLS: {
        const auto& mtls_opts = options_.mtls();
        logger_.debug({"Transport mode: mTLS"});
        logger_.debug({"  Host: " + mtls_opts.host});
        logger_.debug({"  Port: " + mtls_opts.port});
        logger_.debug({"  Certificates directory: " + mtls_opts.certs_dir});
    } break;
    default:
        throw std::runtime_error("Invalid transport mode.");
    }
}

namespace impl_validate {

inline void check_port(const std::string& port_) {
    try {
        const auto port_num = std::stoi(port_);
        if (port_num < 1 || port_num > 65535) {
            throw std::runtime_error("Port number must be in the range 1-65535."
            );
        }
    } catch (const std::invalid_argument&) {
        throw std::runtime_error("Port must be a valid integer.");
    } catch (const std::out_of_range&) {
        throw std::runtime_error("Port number is out of range.");
    }
}

inline void check_hostname_allowed(
    const UnvalidatedTransportOptions& options_, LoggerInterface& logger_
) {
    if (!options_.allow_remote_host) {
        if (options_.host != "localhost" && options_.host != "127.0.0.1") {
            throw std::runtime_error("Remote host connections are not allowed. "
                                     "Use --allow-remote-host to enable.");
        }
    } else {
        logger_.warning(
            {"The '--allow-remote-host' flag has been set. This option may "
             "allow "
             "remote access connections to be established, possibly permitting "
             "control "
             "of this machine and any data which resides on it. It is highly "
             "recommended to only utilize this feature on a trusted, secure "
             "network."}
        );
    }
}

// Helper function to split version string into integers
inline std::vector<int> split_version(const std::string& version) {
    std::vector<int> parts;
    std::stringstream ss(version);
    std::string item;
    while (std::getline(ss, item, '.')) {
        parts.push_back(std::stoi(item));
    }
    return parts;
}

// Helper function to check gRPC version
// IMPORTANT: UDS support on Windows requires gRPC version 1.63.0 or higher
inline void check_UDS_support() {
#ifdef _WIN32
    std::string version = grpc::Version();
    auto parts = split_version(version);

    int major = parts.size() > 0 ? parts[0] : 0;
    int minor = parts.size() > 1 ? parts[1] : 0;

    if (major < 1 || (major == 1 && minor < 63)) {
        throw std::runtime_error(
            "UDS transport mode is not yet supported on Windows."
        );
    }
#endif
}
} // namespace impl_validate

/**
 * Validate the provided unvalidated transport options.
 *
 * This function checks the validity of the options based on the selected
 *transport mode, applies defaults where necessary, and returns a
 *ValidatedTransportOptions object.
 *
 * @param options_ The unvalidated transport options to validate.
 * @param logger_ The logger interface to use for logging warnings or errors.
 * @return A ValidatedTransportOptions object containing the validated options.
 * @throws std::runtime_error if any validation checks fail.
 **/
inline ValidatedTransportOptions validate_options(
    const UnvalidatedTransportOptions& options_, LoggerInterface& logger_
) {
    switch (options_.mode) {
    case TransportMode::INSECURE:
        impl_validate::check_hostname_allowed(options_, logger_);
        impl_validate::check_port(options_.port);

        return ValidatedTransportOptions{
            InsecureOptions{options_.host, options_.port}
        };
    case TransportMode::UDS: {
        impl_validate::check_UDS_support();
        if (options_.host != "localhost") {
            logger_.warning({"The 'host' option is ignored when using UDS "
                             "transport mode."});
        }
        if (options_.port != "") {
            logger_.warning({"The 'port' option is ignored when using UDS "
                             "transport mode."});
        }

        std::string uds_dir = options_.uds_dir;
        if (uds_dir.empty()) {
#ifdef _WIN32
#pragma warning(                                                                                    \
    suppress : 4996,                                                                                \
    justification : "getenv pointer is not invalidated by getenv/setenv/unsetenv/putenv before use" \
)
            char* home = std::getenv("USERPROFILE");
            if (!home) {
#pragma warning(                                                                                    \
    suppress : 4996,                                                                                \
    justification : "getenv pointer is not invalidated by getenv/setenv/unsetenv/putenv before use" \
)
                home = std::getenv("HOME");
            }
            if (!home) {
                throw std::runtime_error("Cannot determine user home directory."
                );
            }
            uds_dir = std::string(home) + "\\.conn";
#else
            const char* home = getenv("HOME");
            if (!home) {
                throw std::runtime_error("Cannot determine user home directory."
                );
            };
            uds_dir = std::string(home) + "/.conn";
#endif
        }
        return ValidatedTransportOptions{UDSOptions{uds_dir, options_.uds_id}};
    }
    case TransportMode::MTLS: {
        impl_validate::check_hostname_allowed(options_, logger_);
        impl_validate::check_port(options_.port);
        std::string certs_dir = options_.certs_dir;
        if (certs_dir.empty()) {

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
            const char* env_certs_dir = std::getenv("ANSYS_GRPC_CERTIFICATES");
#ifdef _WIN32
#pragma warning(pop)
#endif
            if (env_certs_dir != nullptr) {
                certs_dir = std::string{env_certs_dir};
            } else {
                certs_dir = "certs";
            }
        }
        return ValidatedTransportOptions{
            MTLSOptions{certs_dir, options_.host, options_.port}
        };
    }
    default:
        throw std::runtime_error("Invalid transport mode.");
    }
}

} // namespace grpctransportlib
