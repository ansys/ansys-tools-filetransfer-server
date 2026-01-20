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

#include <atomic>
#include <csignal>
#include <memory>
#include <thread>

#ifdef _WIN32
#pragma warning(push, 3)
#endif

#include <grpcpp/grpcpp.h>

#ifdef _WIN32
#pragma warning(pop)
#endif

#include <grpctransportlib/logging.h>

namespace grpctransportlib {

/**
 * Resource handler for managing interrupt signals (e.g., Ctrl+C) to gracefully
 *shut down the gRPC server.
 *
 * This class sets up signal handlers to listen for interrupt signals and
 *initiates a server shutdown when such a signal is received.
 *
 * Note that only one instance of this handler can exist at a time.
 **/
class InterruptHandler {
public:
    /**
     * Constructs the interrupt handler.
     *
     * @param server_ Pointer to the gRPC server to be managed. Must not be
     *null. The lifetime of the server must exceed that of this handler.
     * @param logger_ Shared pointer to a logger instance.
     **/
    explicit InterruptHandler(
        grpc::Server* server_, std::shared_ptr<LoggerInterface> logger_
    )
        : m_logger(std::move(logger_)) {
        if (handler_exists().exchange(true)) {
            throw std::runtime_error("Interrupt handler already exists.");
        }
        if (server_ == nullptr) {
            throw std::runtime_error("Cannot set null server.");
        }
        shutdown_requested() = false;
        m_shutdown_thread = std::thread([this, server_]() {
            while (!shutdown_requested()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            m_logger->info({"Shutting down server."});
            server_->Shutdown();
        });
#ifdef _WIN32
        SetConsoleCtrlHandler(console_handler, TRUE);
#else
        std::signal(SIGINT, [](int /*signal*/) {
            shutdown_requested() = true;
        });
        std::signal(SIGTERM, [](int /*signal*/) {
            shutdown_requested() = true;
        });
#endif
    }

    ~InterruptHandler() {
        shutdown_requested() = true;
        if (m_shutdown_thread.joinable()) {
            m_shutdown_thread.join();
        }
        handler_exists() = false;
    }

    InterruptHandler(const InterruptHandler&) = delete;
    InterruptHandler& operator=(const InterruptHandler&) = delete;
    InterruptHandler(InterruptHandler&&) = default;
    InterruptHandler& operator=(InterruptHandler&&) = default;

private:
#ifdef _WIN32
    static BOOL WINAPI console_handler(DWORD dwType) {
        switch (dwType) {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
            shutdown_requested() = true;
            return TRUE;
        default:
            return FALSE;
        }
    }
#endif

    static volatile std::atomic<bool>& handler_exists() {
        static volatile std::atomic<bool> instance{false};
        return instance;
    }
    static volatile std::atomic<bool>& shutdown_requested() {
        static volatile std::atomic<bool> instance{false};
        return instance;
    }

    std::thread m_shutdown_thread;
    std::shared_ptr<LoggerInterface> m_logger;
};

} // namespace grpctransportlib
