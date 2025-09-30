#pragma once
#include "game/classes/currency.hpp"


struct GenericUIItems
{
	const char* name;
	std::function<uint32_t()> GetValue;
	std::function<void(uint32_t)> SetValue;
};

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
	static void DrawMain();
	// static void DrawSpawner();
	static void Draw();

	static inline std::vector<GenericUIItems> m_CurrencyScalars = {
	    {"Units", &Self::GetUnits, &Self::SetUnits},
	    {"Nanites", &Self::GetNanites, &Self::SetNanites},
	    {"Quicksilver", &Self::GetQuicksilver, &Self::SetQuicksilver}};

	static inline bool InfiniteHealth;
	static inline bool InfiniteStamina;
	static inline bool InfiniteJetpack;
	static inline bool SuperJetpack;
	static inline bool InfiniteLifeSupport;
	static inline bool InfiniteEnvProtection;
	static inline bool InfiniteExosuitShields;
	static inline bool FreeCrafting;

	static inline float DefaultGroundSpeed = 4.400000095;
	static inline float GroundSpeed = DefaultGroundSpeed;
};
