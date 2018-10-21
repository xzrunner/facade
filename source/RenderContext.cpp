#include "facade/RenderContext.h"

#include <unirender/gl/RenderContext.h>
#include <unirender/Blackboard.h>
#include <shaderlab/RenderContext.h>
#include <shaderlab/Blackboard.h>
#include <painting2/RenderContext.h>
#include <painting2/Blackboard.h>

namespace facade
{

RenderContext::RenderContext()
{
	int max_texture;
#ifdef EASY_EDITOR
	max_texture = 4096;
#else
	max_texture = 1024;
#endif // S2_EDITOR
	auto ur_rc = std::make_shared<ur::gl::RenderContext>(max_texture);
	ur::Blackboard::Instance()->SetRenderContext(ur_rc);

	m_sl_rc = std::make_shared<sl::RenderContext>();
	sl::Blackboard::Instance()->SetRenderContext(m_sl_rc);

	ur::gl::RenderContext::Callback cb;
	cb.flush_shader = [&]() {
		m_sl_rc->GetShaderMgr().FlushShader();
	};
	cb.flush_render_shader = [&]() {
		m_sl_rc->GetShaderMgr().FlushRenderShader();
	};
	ur_rc->RegistCB(cb);

	m_ur_rc = ur_rc;

	m_pt2_rc = std::make_shared<pt2::RenderContext>();
	pt2::Blackboard::Instance()->SetRenderContext(m_pt2_rc);
}

void RenderContext::Bind()
{
	ur::Blackboard::Instance()->SetRenderContext(m_ur_rc);
	sl::Blackboard::Instance()->SetRenderContext(m_sl_rc);
	pt2::Blackboard::Instance()->SetRenderContext(m_pt2_rc);
}

void RenderContext::Unbind()
{
	m_pt2_rc.reset();
	m_sl_rc.reset();
	m_ur_rc.reset();

	pt2::Blackboard::Instance()->SetRenderContext(nullptr);
	sl::Blackboard::Instance()->SetRenderContext(nullptr);
	ur::Blackboard::Instance()->SetRenderContext(nullptr);
}

}