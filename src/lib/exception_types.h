#pragma once

#include <stdexcept>

namespace file_transfer::exceptions {
class not_found : public std::runtime_error {
public:
    not_found(std::string s) : runtime_error(s){};
};
class invalid_argument : public std::runtime_error {
public:
    invalid_argument(std::string s) : runtime_error(s){};
};
class failed_precondition : public std::runtime_error {
public:
    failed_precondition(std::string s) : runtime_error(s){};
};
class data_loss : public std::runtime_error {
public:
    data_loss(std::string s) : runtime_error(s){};
};
class internal : public std::runtime_error {
public:
    internal(std::string s) : runtime_error(s){};
};

} // namespace file_transfer::exceptions
