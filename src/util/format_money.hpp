#pragma once
#include <string>

inline std::string FormatMoney(uint32_t n) {
    std::string s = std::to_string(n);
    for (int i = static_cast<int>(s.length()) - 3; i > 0; i -= 3)
        s.insert(i, ",");
    return s;
}
