#include "facade/RenderContext.h"

#include <unirender/gl/RenderContext.h>
#include <unirender/Blackboard.h>
#include <painting2/RenderContext.h>
#include <painting2/Blackboard.h>
#include <rendergraph/RenderMgr.h>

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

	auto ur_rc = std::make_shared<ur::gl::RenderContext>(max_texture, [&]() {
		rg::RenderMgr::Instance()->Flush();
	});
	ur::Blackboard::Instance()->SetRenderContext(ur_rc);

	m_ur_rc = ur_rc;

	m_pt2_rc = std::make_shared<pt2::RenderContext>();
	pt2::Blackboard::Instance()->SetRenderContext(m_pt2_rc);
}

void RenderContext::Bind()
{
	ur::Blackboard::Instance()->SetRenderContext(m_ur_rc);
	pt2::Blackboard::Instance()->SetRenderContext(m_pt2_rc);
}

void RenderContext::Unbind()
{
	m_pt2_rc.reset();
	m_ur_rc.reset();

	pt2::Blackboard::Instance()->SetRenderContext(nullptr);
	ur::Blackboard::Instance()->SetRenderContext(nullptr);
}

}