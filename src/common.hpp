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

#define ICON_FA_HEART             "\xEF\x80\x84"
#define ICON_FA_SPACE_SHUTTLE     "\xEF\x86\x97"
#define ICON_FA_MONEY_BILL        "\xEF\x83\x96"
#define ICON_FA_MONEY_BILL_1      "\xF3\x8F\x8F\x91"
#define ICON_FA_COG               "\xEF\x80\x93"
#define ICON_FA_USER_ASTRONAUT    "\xF3\x8F\x93\xBB"
#define ICON_FA_USER              "\xEF\x80\x87"
