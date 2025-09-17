#pragma once
#include <AsyncLogger/Logger.hpp>
#include <memory>
#include <string>

enum class LogColor;

class Logsink
{
private:
	static LogColor GetColor(const eLogLevel level);
	static const char* GetLevelStr(const eLogLevel level);

public:
	static std::string FormatConsole(const LogMessagePtr msg);
	static std::string FormatFile(const LogMessagePtr msg);
};
