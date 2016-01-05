#include "clog.h"

#include <chrono>
#include <iostream>

int main()
{
    clog* log = clog::get_clog();

    std::thread* thread_1 = new std::thread([log]{
        int num = 0;
        while (true)
        {
            log->info("({:s}, {:d})", "thread_1", num);
            num++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    std::thread* thread_2 = new std::thread([log]{
        int num = 0;
        while (true)
        {
            log->warn("({:s}, {:d})", "thread_2", num);
            num++;

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });

    thread_1->join();
    thread_2->join();

    return 0;
}
