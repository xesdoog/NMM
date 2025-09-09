#pragma once
#include <string>

enum LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERR
};

class Logger {
public:
    static void set_level(LogLevel level);
    static void Log(LogLevel level, const std::string& message);
private:
    static LogLevel m_level;
};
