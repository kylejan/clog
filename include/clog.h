#pragma once

#include <string>
#include <mutex>
#include <atomic>
#include <thread>

#include <iostream>
#include <fstream>
#include <sstream>

#include "details/format.h"
#include "details/mpmc_bounded_queue.h"

typedef std::chrono::high_resolution_clock logger_clock;

struct log_content
{
    log_content() = default;
    log_content(fmt::MemoryWriter&& in_writer)
        : writer(std::move(in_writer))
    {}

    log_content(const log_content& other)
    {
        writer << fmt::BasicStringRef<char>(other.writer.data(), other.writer.size());
    }

    log_content(log_content&& other)
        : writer(std::move(other.writer))
    {}

    log_content& operator = (log_content&& other)
    {
        writer = std::move(other.writer);
        return *this;
    }

    fmt::MemoryWriter writer;
};

class clog
{
public:
    static clog* get_clog();

    clog()
        : queue_(new mpmc_bounded_queue<log_content>(4096))
    {
        io_thread_ = new std::thread([this]{
            while (true)
            {
                log_content msg;
                queue_->dequeue(msg);
                if (msg.writer.size() != 0) std::cerr << msg.writer.c_str() << std::endl;
            }
        });
    }

    template <typename... Args>
    void trace(const char* fmt, const Args&... args)
    {
        write("trace", fmt, args...);
    }

    template <typename... Args>
    void debug(const char* fmt, const Args&... args)
    {
        write("debug", fmt, args...);
    }

    template <typename... Args>
    void info(const char* fmt, const Args&... args)
    {
        write("info", fmt, args...);
    }

    template <typename... Args>
    void notice(const char* fmt, const Args&... args)
    {
        write("notice", fmt, args...);
    }

    template <typename... Args>
    void warn(const char* fmt, const Args&... args)
    {
        write("warn", fmt, args...);
    }

    template <typename... Args>
    void error(const char* fmt, const Args&... args)
    {
        write("error", fmt, args...);
    }

    template <typename... Args>
    void fatal(const char* fmt, const Args&... args)
    {
        write("fatal", fmt, args...);
    }

    template <typename... Args>
    void write(const char* level, const char* fmt, const Args&... args)
    {
        fmt::MemoryWriter out;
        out.write("[{:s}]", level);
        out.write(fmt, args...);
        log_content msg(std::move(out));
        queue_->enqueue(std::move(msg));
    }

private:
    mpmc_bounded_queue<log_content>* queue_;
    std::thread* io_thread_;

    static std::atomic<clog*> instance_;
    static std::mutex mutex_;
};

std::atomic<clog*> clog::instance_;
std::mutex clog::mutex_;

clog* clog::get_clog()
{
    if (instance_.load() == nullptr)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (instance_.load() == nullptr)
        {
            auto* tmp = new clog();
            instance_.store(tmp);
        }
    }
    return instance_.load();
}
