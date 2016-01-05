#pragma once

#include "queue/block_bounded_queue.h"
#include "queue/mpmc_bounded_queue.h"

#include <string>
#include <mutex>
#include <thread>

enum class LoggerType : int
{
    lockfree = 1,
    block = 2
};

template <typename T>
class clog
{
public:
    virtual void info(const T&) = 0;
    virtual void warning(const T&) = 0;
    virtual void debug(const T&) = 0;
    virtual void error(const T&) = 0;
    virtual void fatal(const T&) = 0;

    virtual ~clog() {}
};

clog* get_logger(LoggerType logger_type)
{
    static clog* log;
    switch (logger_type)
    {
    case LoggerType::lockfree:
        log = new lockfree_clog();
        break;
    case LoggerType::block:
        log = new block_clog();
        break;
    default:
        log = nullptr;
    }

    return log;
};
