#include "exception_handling.h"

#include <string>

#ifdef _MSC_VER
#pragma warning(push, 3)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <boost/stacktrace/stacktrace.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#else
#pragma GCC diagnostic pop
#endif

#include "exception_types.h"

namespace file_transfer {
namespace exceptions {

namespace detail {
template <typename ExceptionT>
std::string get_exception_message(const ExceptionT& e) {
    return std::string(e.what()) + '\n' +
           to_string(boost::stacktrace::stacktrace());
}
} // namespace detail

::grpc::Status convert_exceptions_to_status_codes(std::function<void()> fun) {
    try {
        fun();
    } catch (const exceptions::not_found& e) {
        return ::grpc::Status(
            ::grpc::StatusCode::NOT_FOUND,
            std::string("Not found: ") + detail::get_exception_message(e));
    } catch (const exceptions::failed_precondition& e) {
        return ::grpc::Status(
            ::grpc::StatusCode::FAILED_PRECONDITION,
            std::string("Failed precondition: ") +
                detail::get_exception_message(e));
    } catch (const exceptions::invalid_argument& e) {
        return ::grpc::Status(
            ::grpc::StatusCode::INVALID_ARGUMENT,
            std::string("Invalid argument: ") +
                detail::get_exception_message(e));
    } catch (const exceptions::data_loss& e) {
        return ::grpc::Status(
            ::grpc::StatusCode::DATA_LOSS,
            std::string("Data loss: ") + detail::get_exception_message(e));
    } catch (const exceptions::internal& e) {
        return ::grpc::Status(
            ::grpc::StatusCode::INTERNAL,
            std::string("Internal error: ") + detail::get_exception_message(e));
    } catch (const std::exception& e) {
        return ::grpc::Status(
            ::grpc::StatusCode::UNKNOWN,
            std::string("Unknown error: ") + detail::get_exception_message(e));
    } catch (...) {
        return ::grpc::Status(
            ::grpc::StatusCode::UNKNOWN,
            std::string(
                "Fatal error: " + '\n' +
                to_string(boost::stacktrace::stacktrace())));
    }
    return ::grpc::Status::OK;
}

} // namespace exceptions
} // namespace file_transfer
