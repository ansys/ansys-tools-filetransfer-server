#pragma once

#include <functional>

#ifdef _MSC_VER
#pragma warning(push, 3)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <grpcpp/grpcpp.h>

#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

namespace file_transfer {
namespace exceptions {

auto convert_exceptions_to_status_codes(const std::function<void()>&)
    -> ::grpc::Status;

} // namespace exceptions
} // namespace file_transfer
