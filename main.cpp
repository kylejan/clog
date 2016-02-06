#include "aslog.h"

#include <chrono>
#include <iostream>

int main()
{
    get_aslog()->init();

    std::thread* thread_1 = new std::thread([]{
        int num = 500000;
        while (num--)
        {
            get_aslog()->info("({:s}, {:d})", "thread_1", num);
        }
    });

    std::thread* thread_2 = new std::thread([]{
        int num = 500000;
        while (num--)
        {
            get_aslog()->warn("({:s}, {:d})", "thread_2", num);
        }
    });

    thread_1->join();
    thread_2->join();

    return 0;
}
