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

inline std::string timepoint_str(const log_clock::time_point& tp)
{
    auto nanods = std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();

	std::time_t time_t_ = nanods / 1000000000;
	nanods = nanods % 1000000000;

	std::tm tm;
	localtime(&tm);

	char buf_time[24]; // big enough to cover the result
	size_t cnt = std::strftime(buf_time, sizeof(buf_time), "%Y-%m-%d %H:%M:%S", &tm);

	if(cnt == 0)
	{
		std::cerr << "FATAL: failed to use strftime in logger" << std::endl;
		std::terminate();
	}

	std::ostringstream oss;
	oss << buf_time << "." << std::setfill('0') << std::setw(9) << nanods;
	return oss.str();
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
