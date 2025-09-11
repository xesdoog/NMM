#pragma once
#include "memory/pointers.hpp"

struct Currency {
	char pad_0000[0xB8BC];
	uint32_t Units;       // 0xB8BC
	uint32_t Nanites;     // 0xB8C0
	uint32_t Quicksilver; // 0xB8C4
};
