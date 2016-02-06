#pragma once

template <typename T>
class aslog_queue {
public:
    virtual bool empty() = 0;
    virtual void enqueue(T&&) = 0;
    virtual void dequeue(T&) = 0;
    virtual ~aslog_queue() {}
};
