#pragma once

#include <boost/noncopyable.hpp>

#include <map>
#include <string>
#include <memory>

namespace facade
{

class ResPool : boost::noncopyable
{
public:
	ResPool();
	~ResPool();

	template <typename T, typename... Arguments>
	std::shared_ptr<T> Fetch(const std::string& filepath, Arguments... parameters);

	template <typename T, typename... Arguments>
	std::pair<std::shared_ptr<T>, bool> FetchNoLoad(const std::string& filepath, Arguments... parameters);

	template <typename T>
	std::shared_ptr<T> Query(const std::string& filepath);

	template <typename T>
	std::string QueryFilepath(const std::shared_ptr<T>& res);

	template <typename T>
	bool Insert(const std::string& filepath, const std::shared_ptr<T>& res);

	static ResPool& Instance();

private:
	template <typename T>
	std::string Key(const std::string& filepath) const;

	std::string Key2Filepath(const std::string& key) const;

private:
	std::map<std::string, std::weak_ptr<void>> m_path2res;

	static std::unique_ptr<ResPool> m_instance;

}; // ResPool

}

#include "facade/ResPool.inl"