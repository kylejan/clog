#include "clog.h"

#include <chrono>
#include <iostream>

int main()
{
    get_clog()->init();

    std::thread* thread_1 = new std::thread([]{
        int num = 1000000;
        while (num--)
        {
            get_clog()->info("({:s}, {:d})", "thread_1", num);
        }
    });

    // std::thread* thread_2 = new std::thread([]{
    //     int num = 100000;
    //     while (num--)
    //     {
    //         get_clog()->warn("({:s}, {:d})", "thread_2", num);
    //     }
    // });

    thread_1->join();
    // thread_2->join();

    return 0;
}
