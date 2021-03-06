#pragma once

namespace facade
{

template <typename T, typename... Arguments>
inline std::shared_ptr<T> ResPool::Fetch(const std::string& filepath, Arguments... parameters)
{
	auto key = Key<T>(filepath);
	auto itr = m_path2res.find(key);
	if (itr != m_path2res.end())
	{
		auto ptr = itr->second.lock();
		if (ptr) {
			return std::static_pointer_cast<T>(ptr);
		} else {
			m_path2res.erase(itr);
		}
	}

	auto ptr = std::make_shared<T>(parameters...);
	ptr->LoadFromFile(filepath);
	m_path2res.insert(std::make_pair(key, ptr));
	return ptr;
}

template <typename T, typename... Arguments>
inline std::pair<std::shared_ptr<T>, bool> ResPool::FetchNoLoad(const std::string& filepath, Arguments... parameters)
{
	auto key = Key<T>(filepath);
	auto itr = m_path2res.find(key);
	if (itr != m_path2res.end())
	{
		auto ptr = itr->second.lock();
		if (ptr) {
			return std::make_pair(std::static_pointer_cast<T>(ptr), true);
		} else {
			m_path2res.erase(itr);
		}
	}

	auto ptr = std::make_shared<T>(parameters...);
	m_path2res.insert(std::make_pair(key, ptr));
	return std::make_pair(ptr, false);
}

template <typename T>
std::shared_ptr<T> ResPool::Query(const std::string& filepath)
{
	auto itr = m_path2res.find(Key<T>(filepath));
	if (itr != m_path2res.end())
	{
		auto ptr = itr->second.lock();
		if (ptr) {
			return std::static_pointer_cast<T>(ptr);
		} else {
			m_path2res.erase(itr);
		}
	}

	return nullptr;
}

template <typename T>
std::string ResPool::QueryFilepath(const std::shared_ptr<T>& res)
{
	for (auto itr = m_path2res.begin(); itr != m_path2res.end(); ++itr)
	{
		auto ptr = itr->second.lock();
		if (ptr && ptr == res) {
			return Key2Filepath(itr->first);
		}
	}
	return "";
}

template <typename T>
bool ResPool::Insert(const std::string& filepath, const std::shared_ptr<T>& res)
{
	auto status = m_path2res.insert(std::make_pair(Key<T>(filepath), res));
	return status.second;
}

template <typename T>
std::string ResPool::Key(const std::string& filepath) const
{
	return filepath + ";" + std::string(typeid(T).name());
}

}