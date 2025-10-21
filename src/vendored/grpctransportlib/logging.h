// Copyright (C) 2022 - 2025 ANSYS, Inc. and/or its affiliates.
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

#include <iostream>
#include <string>
#include <vector>

namespace grpctransportlib {

/**
 * Interface for logging messages in the server library.
 *
 * This interface allows adapting the logging from the transport_options
 * and interrupt_handler modules to the logging system of the application.
 **/
struct LoggerInterface {
    LoggerInterface() = default;
    virtual ~LoggerInterface() = default;
    /**
     * Emit debug-level log messages.
     *
     * @param lines_ Lines to log.
     **/
    virtual void debug(const std::vector<std::string>& lines_) = 0;

    /**
     * Emit info-level log messages.
     *
     * @param lines_ Lines to log.
     **/
    virtual void info(const std::vector<std::string>& lines_) = 0;

    /**
     * Emit warning-level log messages.
     *
     * @param lines_ Lines to log.
     **/
    virtual void warning(const std::vector<std::string>& lines_) = 0;
};

/**
 * Logger interface implementation that does nothing.
 **/
struct NullLogger : public LoggerInterface {
    void debug(const std::vector<std::string>& /*lines_*/) override {}
    void info(const std::vector<std::string>& /*lines_*/) override {}
    void warning(const std::vector<std::string>& /*lines_*/) override {}
};

/**
 * Logger interface implementation that logs to standard output and error
 *streams.
 **/
struct StdoutLogger : public LoggerInterface {
    void debug(const std::vector<std::string>& lines_) override {
        for (const auto& line : lines_) {
            std::cout << "[DEBUG] " << line << std::endl;
        }
    }
    void info(const std::vector<std::string>& lines_) override {
        for (const auto& line : lines_) {
            std::cout << "[INFO] " << line << std::endl;
        }
    }
    void warning(const std::vector<std::string>& lines_) override {
        for (const auto& line : lines_) {
            std::cerr << "[WARNING] " << line << std::endl;
        }
    }
};
} // namespace grpctransportlib
