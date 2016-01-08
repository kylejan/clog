#include "clog.h"

#include <chrono>
#include <iostream>

int main()
{
    std::thread* thread_1 = new std::thread([]{
        int num = 100000;
        while (num--)
        {
            clog::get_clog()->info("({:s}, {:d})", "thread_1", num);
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    std::thread* thread_2 = new std::thread([]{
        int num = 100000;
        while (num--)
        {
            clog::get_clog()->warn("({:s}, {:d})", "thread_2", num);
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    std::thread* thread_3 = new std::thread([]{
        while (true);
    });

    thread_1->join();
    thread_2->join();
    thread_3->join();

    return 0;
}
