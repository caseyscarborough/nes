#ifndef NES_LOG_H
#define NES_LOG_H

#include <string>
#include <iostream>

// TODO Add more robust logging
#define LOG(s) std::cout << "[" << __FILE_NAME__ << ":" << __LINE__ << "] " << s << std::endl;
#define LOG_ERROR(s) std::cerr << "[" << __FILE_NAME__ << ":" << __LINE__ << "] " << s << std::endl;

#endif //NES_LOG_H
