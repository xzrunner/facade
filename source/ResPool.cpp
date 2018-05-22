#include "facade/ResPool.h"

namespace facade
{

std::unique_ptr<ResPool> ResPool::m_instance = nullptr;

ResPool& ResPool::Instance()
{
	if (!m_instance) {
		m_instance = std::make_unique<ResPool>();
	}
	return *m_instance;
}

ResPool::ResPool()
{
}

ResPool::~ResPool()
{
}

std::string ResPool::Key2Filepath(const std::string& key) const
{
	return key.substr(0, key.find_last_of(';'));
}

}