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

namespace file_transfer::exceptions {

namespace detail {
template <typename ExceptionT>
auto get_exception_message(const ExceptionT& exc) -> std::string {
    return std::string(exc.what()) + '\n' +
           to_string(boost::stacktrace::stacktrace());
}
} // namespace detail

auto convert_exceptions_to_status_codes(const std::function<void()>& fun)
    -> ::grpc::Status {
    try {
        fun();
    } catch (const exceptions::not_found& exc) {
        return {
            ::grpc::StatusCode::NOT_FOUND,
            std::string("Not found: ") + detail::get_exception_message(exc)};
    } catch (const exceptions::failed_precondition& exc) {
        return {
            ::grpc::StatusCode::FAILED_PRECONDITION,
            std::string("Failed precondition: ") +
                detail::get_exception_message(exc)};
    } catch (const exceptions::invalid_argument& exc) {
        return {
            ::grpc::StatusCode::INVALID_ARGUMENT,
            std::string("Invalid argument: ") +
                detail::get_exception_message(exc)};
    } catch (const exceptions::data_loss& exc) {
        return {
            ::grpc::StatusCode::DATA_LOSS,
            std::string("Data loss: ") + detail::get_exception_message(exc)};
    } catch (const exceptions::internal& exc) {
        return {
            ::grpc::StatusCode::INTERNAL,
            std::string("Internal error: ") +
                detail::get_exception_message(exc)};
    } catch (const std::exception& exc) {
        return {
            ::grpc::StatusCode::UNKNOWN,
            std::string("Unknown error: ") +
                detail::get_exception_message(exc)};
    } catch (...) {
        return {
            ::grpc::StatusCode::UNKNOWN,
            std::string("Fatal error: \n") +
                to_string(boost::stacktrace::stacktrace())};
    }
    return ::grpc::Status::OK;
}

} // namespace file_transfer::exceptions
