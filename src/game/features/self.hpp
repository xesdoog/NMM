#pragma once
#include "game/classes/currency.hpp"
#include "memory/pointers.hpp"

class Self
{
public:
	static inline uint32_t GetCurrency(uint32_t Currency::* field);
	static inline void SetCurrency(uint32_t Currency::* field, uint32_t value);

	static uint32_t GetUnits();
	static uint32_t GetNanites();
	static uint32_t GetQuicksilver();
	static void SetUnits(uint32_t amount);
	static void SetNanites(uint32_t amount);
	static void SetQuicksilver(uint32_t amount);
	static void DrawCurrency();
	static void Draw();
};
