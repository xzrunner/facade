#pragma once

#include <cu/cu_macro.h>

#include <vector>
#include <functional>

namespace ur2 { class Device; class Context; }

namespace facade
{

class Facade
{
public:
	void Init(const ur2::Device& dev);
	void Update(float dt);
	bool Flush(ur2::Context& ctx, bool dtex_cg_to_c2);
	bool IsLastFrameDirty() const { return m_last_frame_dirty; }

	void AddInitCB(std::function<void()> cb) {
		m_init_cb.push_back(cb);
	}
	void AddUpdateCB(std::function<void(float)> cb) {
		m_update_cb.push_back(cb);
	}

private:
	std::vector<std::function<void()>>      m_init_cb;
	std::vector<std::function<void(float)>> m_update_cb;

	bool m_last_frame_dirty = false;

	CU_SINGLETON_DECLARATION(Facade)

}; // Facade

}