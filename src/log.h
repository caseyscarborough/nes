#ifndef NES_LOG_H
#define NES_LOG_H

#include <string>
#include <iostream>

#define LOG_TRACE_ENABLED 1

// TODO Add more robust logging
#define LOG_WITH_LEVEL(ostream, level, s) ostream << "[" << std::setfill(' ') << std::setw(13)  << __FILE_NAME__ << ":" << std::setfill('0') << std::setw(3) << __LINE__ << "][" << level << "] " << s << std::endl;
#define LOG(s) LOG_WITH_LEVEL(std::cout, "DEBUG", s);
#define LOG_TRACE(s) if (LOG_TRACE_ENABLED) { LOG_WITH_LEVEL(std::cout, "TRACE", s) }
#define LOG_ERROR(s) LOG_WITH_LEVEL(std::cerr, "ERROR", s);

#endif //NES_LOG_H
