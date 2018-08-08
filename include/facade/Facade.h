#pragma once

#include <cu/cu_macro.h>

#include <vector>
#include <functional>

namespace facade
{

class Facade
{
public:
	void Init();
	void Update(float dt);

	void AddInitCB(std::function<void()> cb) {
		m_init_cb.push_back(cb);
	}
	void AddUpdateCB(std::function<void(float)> cb) {
		m_update_cb.push_back(cb);
	}

private:
	std::vector<std::function<void()>>      m_init_cb;
	std::vector<std::function<void(float)>> m_update_cb;

	CU_SINGLETON_DECLARATION(Facade)

}; // Facade

}