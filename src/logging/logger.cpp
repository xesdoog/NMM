#include <iostream>
#include <fstream>
#include <string>
#include "logger.hpp"

LogLevel Logger::m_level = DEBUG;

void Logger::set_level(LogLevel level) {
    m_level = level;
}

void Logger::Log(LogLevel level, const std::string& message) {
    if (level >= m_level) {
        std::ofstream file("S:\\Documents\\Coding\\C++\\NMS_TEST\\cout.log", std::ios::app);
        std::string prefix{};

        switch (level) {
        case DEBUG: prefix = "[DEBUG] "; break;
        case INFO:  prefix = "[INFO] ";  break;
        case WARN:  prefix = "[WARN] ";  break;
        case ERR:   prefix = "[ERROR] "; break;
        }

        std::cout << prefix << message << std::endl;
        file << prefix << message << std::endl;
        file.close();
    }
}
