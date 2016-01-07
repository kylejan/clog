#include "clog.h"

#include <chrono>
#include <iostream>

int main()
{
    std::thread* thread_1 = new std::thread([]{
        int num = 1000;
        while (num--)
        {
            clog::get_clog()->info("({:s}, {:d})", "thread_1", num);
        }
    });

    std::thread* thread_2 = new std::thread([]{
        int num = 1000;
        while (num--)
        {
            clog::get_clog()->warn("({:s}, {:d})", "thread_2", num);
        }
    });

    thread_1->join();
    thread_2->join();

    return 0;
}
