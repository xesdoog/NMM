#include "pattern_cache.hpp"


std::optional<int> PatternCache::GetCachedOffsetImpl(PatternHash hash)
{
	std::lock_guard lock(m_Mutex);
	if (auto it = m_Data.find(hash.GetHash()); it != m_Data.end())
		return it->second;

	return std::nullopt;
}

void PatternCache::UpdateCachedOffsetImpl(PatternHash hash, int offset)
{
	std::lock_guard lock(m_Mutex);
	m_Data[hash.GetHash()] = offset;
}

void PatternCache::InitImpl()
{
	std::lock_guard lock(m_Mutex);

	auto file = g_ProjectPath / "pattern_cache.bin";
	if (std::filesystem::exists(file))
	{
		std::ifstream stream(file, std::ios_base::binary);
		while (!stream.eof())
		{
			std::uint64_t hash;
			int offset;

			stream.read(reinterpret_cast<char*>(&hash), sizeof(hash));
			stream.read(reinterpret_cast<char*>(&offset), sizeof(offset));

			m_Data.emplace(hash, offset);
		}
	}

	m_Initialized = true;
}

void PatternCache::UpdateImpl()
{
	std::lock_guard lock(m_Mutex);

	auto file = g_ProjectPath / "pattern_cache.bin";;
	std::ofstream stream(file, std::ios_base::binary);

	for (auto& [h, offset] : m_Data)
	{
		auto hash = h;
		stream.write(reinterpret_cast<char*>(&hash), sizeof(hash));
		stream.write(reinterpret_cast<char*>(&offset), sizeof(offset));
	}
}
