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

#include <filesystem>
#include <string>

#ifdef _WIN32
#pragma warning(push, 3)
#endif
#include <boost/program_options.hpp>
#ifdef _WIN32
#pragma warning(pop)
#endif

#include <grpctransportlib/cli/impl/utils.h>
#include <grpctransportlib/options.h>

namespace grpctransportlib {
namespace cli {
namespace bpo {

/**
 * Returns a Boost Program Options description for transport-related
 *command-line options.
 *
 * @return A Boost Program Options description object containing transport
 *options.
 **/
inline boost::program_options::options_description
get_transport_options_description(
    const std::string& app_name_, const std::string& port_default_ = ""
) {
    namespace po = boost::program_options;
    auto description = po::options_description("Transport options");
    description.add_options()(
        "transport-mode",
        po::value<std::string>()->default_value("uds"),
        "Specify transport type: insecure, uds, mtls"
    )
    (
        "host",
        po::value<std::string>()->default_value("localhost"),
        "Host address on which the server is listening."
    )
    (
        "port",
        po::value<std::string>()->default_value(port_default_),
        "Port on which the server is listening."
    )
    (
        "certs-dir",
        po::value<std::string>()->default_value(""),
        "Directory path for mTLS certificate files. Defaults to the ANSYS_GRPC_CERTIFICATES "
        "environment variable, or 'certs' if the variable is not set."
    )
    (
        "uds-dir",
        po::value<std::string>()->default_value(""),
        "Directory path for UDS socket files (default: ~/.conn)."
    )
    (
        "uds-id",
        po::value<std::string>()->default_value(""),
        ("Optional ID for UDS socket file naming (" + app_name_ + "-<id>.sock).").c_str()
    )
    (
        "allow-remote-host",
        "Allow listening on interfaces other than localhost and 127.0.0.1. "
        "Enabling this option may allow remote access connections to be established, "
        "possibly permitting control of this machine and any data which resides on it. "
        "It is highly recommended to only utilize this feature on a trusted, secure network."
    );
    return description;
}

/**
 * Parses transport-related options from a Boost Program Options variables map.
 *
 * @param variables_ The Boost Program Options variables map containing parsed
 *command-line arguments.
 * @return An UnvalidatedTransportOptions struct with the parsed options.
 **/
inline UnvalidatedTransportOptions
get_transport_options(const boost::program_options::variables_map& variables_) {
    return {
        impl::parse_transport_mode(variables_["transport-mode"].as<std::string>(
        )),
        variables_["host"].as<std::string>(),
        variables_["port"].as<std::string>(),
        variables_.count("allow-remote-host") != 0U,
        variables_["uds-dir"].as<std::string>(),
        variables_["uds-id"].as<std::string>(),
        variables_["certs-dir"].as<std::string>()
    };
}

} // namespace bpo
} // namespace cli
} // namespace grpctransportlib
