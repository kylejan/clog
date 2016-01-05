#include "clog.h"

#include <chrono>
#include <iostream>

int main()
{
    block_bounded_queue<std::string> block_q(16);
    mpmc_bounded_queue<std::string> lock_free_q(4096);

    auto* thread_1 = new std::thread([&block_q]{
        static int number = 0;
        while (true)
        {
            block_q.enqueue(std::string("block queue ") + std::to_string(number++));
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    auto* thread_2 = new std::thread([&block_q]{
        while (true)
        {
            std::string tmp;

            block_q.dequeue(tmp);
            std::cout << tmp << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    thread_1->join();
    thread_2->join();

    return 0;
}
