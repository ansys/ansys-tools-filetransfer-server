#include <cstdlib>

#include <boost/filesystem/path.hpp>
#include <boost/locale.hpp>
#include <boost/program_options.hpp>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <ansys/api/utilities/filetransfer/v1/file_transfer_service.grpc.pb.h>

void run_server(const std::string& server_address) {
    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    grpc::ServerBuilder builder;

    // TODO: Add secure channel option
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

    // Assemble the server.
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

    // Wait for the server to shutdown. Note that some other thread must be
    // responsible for shutting down the server for this call to ever return.
    server->Wait();
};

namespace po = boost::program_options;

/**
 * Parse command-line options and start the server.
 */
int main(int argc, char** argv) {
    po::options_description description(
        "Ansys Filetransfer Utility server options");
    description.add_options()("help", "Show CLI help.")(
        "server-address", po::value<std::string>()->required(),
        "Address on which the server is listening.");

    po::variables_map variables;
    try {
        po::store(po::parse_command_line(argc, argv, description), variables);
    } catch (std::exception& e) {
        std::cout << "Invalid command line arguments: " << e.what()
                  << std::endl;
        return EXIT_FAILURE;
    }
    if (variables.count("help")) {
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
