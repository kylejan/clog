#include "queue/block_bounded_queue.h"
#include "queue/mpmc_bounded_queue.h"

#include <string>
#include <thread>

enum class LoggerType : int
{
    lockfree_bounded_queue = 1,
    block_bounded_queue = 2,
    block_unbounded_queue = 3
};

class clog
{
public:
    static clog* get_logger(LoggerType);

    virtual void info(const char*) = 0;
    virtual void warning(const char*) = 0;
    virtual void debug(const char*) = 0;
    virtual void error(const char*) = 0;
    virtual void fatal(const char*) = 0;

    virtual ~clog() {}
};
