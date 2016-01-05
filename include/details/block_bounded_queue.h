#pragma once

#include <deque>
#include <mutex>
#include <assert.h>
#include <condition_variable>

template <typename T>
class block_bounded_queue
{
public:
    block_bounded_queue(size_t buffer_size)
        : buffer_(new cell_t [buffer_size])
        , buffer_mask_(buffer_size - 1)
        , enqueue_pos_(0)
        , dequeue_pos_(0)
    {
        assert((buffer_size >= 2) && ((buffer_size & (buffer_size - 1)) == 0));
        for (size_t i = 0; i != buffer_size; ++i)
            buffer_[i].sequence = i;
    }

    ~block_bounded_queue()
    {
        delete[] buffer_;
    }

    void enqueue(T&& data)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto pos = enqueue_pos_;
        auto* cell = &buffer_[pos & buffer_mask_];
        intptr_t dif = (intptr_t)cell->sequence - (intptr_t)pos;

        if (dif != 0)
        {
            cv_.wait(lock, [this, &pos]{ return buffer_[pos & buffer_mask_].sequence == pos; });
        }
        enqueue_pos_++;

        cell->data = std::move(data);
        cell->sequence = pos + 1;
        cv_.notify_one();
    }

    void dequeue(T& data)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto pos = dequeue_pos_;
        auto* cell = &buffer_[pos & buffer_mask_];
        intptr_t dif = (intptr_t)cell->sequence - ((intptr_t)pos + 1);

        if (dif != 0)
        {
            cv_.wait(lock, [this, &pos]{ return buffer_[pos & buffer_mask_].sequence == pos + 1; });
        }
        dequeue_pos_++;

        data = std::move(cell->data);
        cell->sequence = pos + buffer_mask_ + 1;
        cv_.notify_one();
    }

    block_bounded_queue(const block_bounded_queue&) = delete;
    void operator = (const block_bounded_queue&) = delete;

private:
    struct cell_t
    {
        size_t  sequence;
        T data;
    };

    cell_t* const buffer_;
    size_t const buffer_mask_;

    size_t enqueue_pos_;
    size_t dequeue_pos_;

    std::mutex mutex_;
    std::condition_variable cv_;
};
