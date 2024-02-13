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

#include <cstdlib>
#include <locale>

#ifdef _MSC_VER
#pragma warning(push, 3)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#ifdef _WIN32
#include <boost/filesystem/path.hpp>
#include <boost/locale.hpp>
#endif

#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

#include <filetransfer_service.h>

auto run_server(const std::string& server_address) -> void {
// Set encoding for paths to UTF-8
// This is only needed on Windows, because Linux uses UTF-8 by default.
#ifdef _WIN32
    boost::filesystem::path::imbue(
        boost::locale::generator().generate("en_US.UTF-8")
    );
#endif

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    auto builder = grpc::ServerBuilder{};

    file_transfer::FileTransferServiceImpl file_transfer_service{};
    builder.RegisterService(&file_transfer_service);

    // TODO: Add secure channel option
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    // Assemble the server.
    auto server = builder.BuildAndStart();

    BOOST_LOG_TRIVIAL(info) << "File transfer server started.";

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
};

namespace po = boost::program_options;

/**
 * Parse command-line options and start the server.
 */
auto main(int argc, char** argv) -> int {
    po::options_description description("Ansys Filetransfer Tool server options"
    );
    description.add_options()("help", "Show CLI help.")(
        "server-address",
        po::value<std::string>()->required(),
        "Address on which the server is listening."
    );

    auto variables = po::variables_map{};
    try {
        po::store(po::parse_command_line(argc, argv, description), variables);
    } catch (std::exception& e) {
        std::cout << "Invalid command line arguments: " << e.what()
                  << std::endl;
        return EXIT_FAILURE;
    }
    if (variables.count("help") != 0U) {
        std::cout << description;
        return EXIT_SUCCESS;
    }

    // Only check for required arguments if the 'help' flag was not present.
    try {
        po::notify(variables);
    } catch (std::exception& e) {
        std::cout << "Invalid command line arguments: " << e.what()
                  << std::endl;
        return EXIT_FAILURE;
    }

    run_server(variables["server-address"].as<std::string>());
    return EXIT_SUCCESS;
}
