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
        write_nano_timepoint(writer, log_clock::now());
        writer.write("[{:s}] ", level);
        writer.write(fmt, args...);
        writer.write("\n");
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
    clog(const std::string& filename = "clog.log", bool is_append_datetime = true)
        : file_name_(filename)
    {
        if (is_append_datetime) file_name_ = get_datetime_timepoint() + "." + file_name_;

        if (!file_exists(file_name_))
        {
            fopen_s(&file_, file_name_, "wb");
        }
        else
        {
            fopen_s(&file_, file_name_, "ab");
        }
    }

    ~clog()
    {
        while (!queue_->empty());
        exit_signal_.store(true, std::memory_order_release);
        std::fclose(file_);
    }

    clog() = delete;
    clog(const clog&) = delete;
    clog(clog&&) = delete;
    clog& operator = (const clog&) = delete;
    clog& operator = (clog&&) = delete;

    void init(std::int64_t queue_size = 8192)
    {
        queue_ = new mpmc_bounded_queue<log_content>(queue_size);
        io_thread_ = new std::thread(&clog::process_queue_msg, this);
    }

    void process_queue_msg()
    {
        static log_content msg;
        while (true)
        {
            if (queue_->empty()) continue;

            queue_->dequeue(msg);

            size_t msg_size = msg.writer.size();
            std::fwrite(msg.writer.data(), 1, msg_size, file_);
            std::fflush(file_);

            if (exit_signal_.load(std::memory_order_acquire)) break;
        }
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

    std::atomic<bool> exit_signal_{false};

    std::FILE* file_;
    std::string file_name_;
};

clog* get_clog(const std::string& filename = "clog.log", bool is_append_datetime = true)
{
    static clog clog_instance(filename, is_append_datetime);
    return &clog_instance;
}
