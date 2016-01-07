#pragma once

#include <ctime>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>

#include "format.h"

using log_clock = std::chrono::high_resolution_clock;

inline void localtime(std::tm* tm, const std::time_t* time_tt)
{
#ifdef _WIN32
	localtime_s(tm, time_tt);
#else
	localtime_r(time_tt, tm);
#endif
}

inline void localtime(std::tm* tm)
{
    std::time_t now_t = time(nullptr);
    localtime(tm, &now_t);
}

inline void timepoint_to_writer(fmt::MemoryWriter& writer, const log_clock::time_point& tp)
{
    auto duration = tp.time_since_epoch();
    auto nanos = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000000000;

    std::tm tm;
    localtime(&tm);

    // Faster (albeit uglier) way to format the line (5.6 million lines/sec under 10 threads)
    writer << static_cast<unsigned int>(tm.tm_year + 1900) << '-'
           << fmt::pad(static_cast<unsigned int>(tm.tm_mon + 1), 2, '0') << '-'
           << fmt::pad(static_cast<unsigned int>(tm.tm_mday), 2, '0') << ' '
           << fmt::pad(static_cast<unsigned int>(tm.tm_hour), 2, '0') << ':'
           << fmt::pad(static_cast<unsigned int>(tm.tm_min), 2, '0') << ':'
           << fmt::pad(static_cast<unsigned int>(tm.tm_sec), 2, '0') << '.'
           << fmt::pad(static_cast<unsigned int>(nanos), 9, '0') << " ";
}

//fopen_s on non windows for writing
inline int fopen_s(FILE** fp, const std::string& filename, const char* mode)
{
#ifdef _WIN32
    *fp = _fsopen((filename.c_str()), mode, _SH_DENYWR);
    return *fp == nullptr;
#else
    *fp = fopen((filename.c_str()), mode);
    return *fp == nullptr;
#endif

}

//Return if file exists
inline bool file_exists(const std::string& filename)
{
#ifdef _WIN32
    auto attribs = GetFileAttributesA(filename.c_str());
    return (attribs != INVALID_FILE_ATTRIBUTES && !(attribs & FILE_ATTRIBUTE_DIRECTORY));
#elif __linux__
    struct stat buffer;
    return (stat (filename.c_str(), &buffer) == 0);
#else
    auto *file = fopen(filename.c_str(), "r");
    if (file != nullptr)
    {
        fclose(file);
        return true;
    }
    return false;

#endif
}
