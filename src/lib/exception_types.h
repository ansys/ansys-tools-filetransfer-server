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
