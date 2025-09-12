#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS

#include <MinHook.h>
#include <Windows.h>
#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <imgui.h>
#include <iostream>
#include <map>
#include <memory>
#include <ranges>
#include <span>
#include <stack>
#include <string_view>
#include <thread>
#include <vector>

#include "Logging/Logger.hpp"

#undef Yield

using namespace std::chrono_literals;
using namespace std::string_literals;
using namespace std::string_view_literals;

extern std::atomic<bool> g_Running;
extern HINSTANCE g_DllInstance;
extern HANDLE g_MainThread;
extern DWORD g_MainThreadId;
