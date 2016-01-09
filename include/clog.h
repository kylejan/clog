#pragma once

#include <string>
#include <mutex>
#include <atomic>
#include <thread>

#include "details/utility.h"
#include "details/mpmc_bounded_queue.h"
#include "details/block_bounded_queue.h"

struct log_content
{
    log_content() = default;

    template <typename... Args>
    log_content(const char* level, const char* fmt, const Args... args)
    {
        timepoint_to_writer(writer, log_clock::now());
        writer.write("[{:s}] ", level);
        writer.write(fmt, args...);
    }

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
        : queue_(new mpmc_bounded_queue<log_content>(8192))
    {
        io_thread_ = new std::thread([this]
        {
            while (!queue_->empty())
            {
                log_content msg;
                queue_->dequeue(msg);
                if (msg.writer.size() != 0)
                {
                    std::cerr << msg.writer.c_str() << std::endl;
                }
            }
        });
    }

    ~clog()
    {
        io_thread_->join();
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
        log_content msg(level, fmt, args...);
        queue_->enqueue(std::move(msg));
    }

private:
    mpmc_bounded_queue<log_content>* queue_;
    std::thread* io_thread_;
};

clog* get_clog()
{
    static clog clog_instance;
    return &clog_instance;
}
