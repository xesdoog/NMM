#include <future>
#include "pattern_scanner.hpp"
#include "pattern_cache.hpp"
#include "module.hpp"


PatternScanner::PatternScanner(const Module* module) :
	m_Module(module),
	m_Patterns()
{
}

bool PatternScanner::Scan()
{
	if (!m_Module || !m_Module->Valid())
		return false;

	std::vector<std::future<bool>> jobs;
	for (const auto& [pattern, func] : m_Patterns)
	{
		jobs.emplace_back(std::async(&PatternScanner::ScanInternal, this, pattern, func));
	}

	bool scanSuccess = true;
	for (auto& job : jobs)
	{
		job.wait();

		if (scanSuccess)
			scanSuccess = job.get();
	}
	if (!scanSuccess)
	{
		LOG(WARNING) << "Some patterns have not been found, continuing would be foolish.";
	}
	return scanSuccess;
}

bool PatternScanner::ScanInternal(const IPattern* pattern, PatternFunc func) const
{
	const auto signature = pattern->Signature();

	if (PatternCache::IsInitialized())
	{
		auto offset = PatternCache::GetCachedOffset(pattern->Hash().Update(m_Module->Size()));
		if (offset.has_value())
		{
			LOGF(INFO, "Using cached pattern [{}] : [{:X}] [Hash(): {:X}]", pattern->Name(), m_Module->Base() + offset.value(), pattern->Hash().Update(m_Module->Size()).m_Hash);
			std::invoke(func, m_Module->Base() + offset.value());
			return true;
		}
	}

	for (auto i = m_Module->Base(); i < m_Module->End(); ++i)
	{
		if (signature.size() + i > m_Module->End())
			break;

		const auto instruction = reinterpret_cast<std::uint8_t*>(i);
		bool found = true;
		for (std::size_t instructionIdx = 0; instructionIdx < signature.size(); ++instructionIdx)
		{
			if (signature[instructionIdx] && signature[instructionIdx].value() != instruction[instructionIdx])
			{
				found = false;
			}
		}

		if (found)
		{
			LOGF(INFO, "Found pattern [{}] : [0x{:X}]", pattern->Name(), i);
			std::invoke(func, i);

			if (PatternCache::IsInitialized())
			{
				PatternCache::UpdateCachedOffset(pattern->Hash().Update(m_Module->Size()), i - m_Module->Base());
			}

			return true;
		}
	}

	LOGF(WARNING, "Failed to find pattern {}", pattern->Name());
	return false;
}
