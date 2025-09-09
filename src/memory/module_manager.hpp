#pragma once

#include "module.hpp"
#include "util/common.hpp"
#include <unordered_map>
#include <string_view>
#include <memory>

class ModuleMgr
{
	public:
		ModuleMgr()                                = default;
		virtual ~ModuleMgr()                       = default;
		ModuleMgr(const ModuleMgr&)                = delete;
		ModuleMgr(ModuleMgr&&) noexcept            = delete;
		ModuleMgr& operator=(const ModuleMgr&)     = delete;
		ModuleMgr& operator=(ModuleMgr&&) noexcept = delete;

		Module* Get(const std::string_view name);

		/**
			* @brief Loads the modules from PEB and caches them.
			* 
			* @return true If the peb is found and modules have been cached.
			* @return false If the peb or peb->Ldr pointer were invalid. 
			*/
		bool LoadModules();

	private:
		std::unordered_map<std::string_view, std::unique_ptr<Module>> m_CachedModules;

	public:
		ModuleMgr(std::unordered_map<std::string_view, std::unique_ptr<Module>> cachedModules)
			: m_CachedModules(std::move(cachedModules))
		{
		}
};

inline ModuleMgr module_manager;
