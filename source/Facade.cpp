#include "facade/Facade.h"
#include "facade/DTex.h"
#include "facade/GTxt.h"
#include "facade/LoadingList.h"

#include <emitter/Particle3d.h>
#include <emitter/GlobalClock.h>
#include <anim/GlobalClock.h>
#include <model/GlobalClock.h>
#include <painting0/GlobalClock.h>
#include <painting2/Callback.h>
#include <painting2/RenderColorCommon.h>
#include <painting2/RenderSystem.h>
#include <painting2/Textbox.h>
#include <painting3/Painting3.h>
#include <node2/AABBSystem.h>
#include <renderpipeline/Callback.h>

namespace facade
{

CU_SINGLETON_DEFINITION(Facade);

Facade::Facade()
{
}

void Facade::Init(const ur2::Device& dev)
{
	et::Particle3d::Init();

	DTex::Instance()->Init(dev);
	GTxt::Instance()->Init(dev);

	for (auto& cb : m_init_cb) {
		cb();
	}

	// pt2
	pt2::Callback::Funs pt2_cb;
	pt2_cb.draw_text = [](
        ur2::Context& ctx, const std::string& text, const pt2::Textbox& style,
		const sm::Matrix2D& mat, const pt0::Color& mul_col, const pt0::Color& add_col) {
		GTxt::Draw(ctx, text, style, mat, mul_col, add_col, 0, false);
	};pt2_cb.calc_label_size = [](const std::string& text, const pt2::Textbox& style)->sm::vec2 {
		return GTxt::CalcLabelSize(text, style);
	};
	pt2_cb.get_bounding = [](const n0::CompAsset& casset)->sm::rect {
		return n2::AABBSystem::GetBounding(casset);
	};
	pt2_cb.query_cached_tex_quad = [](size_t tex_id, const sm::irect& r, int& out_tex_id)->const float* {
		sx::UID uid = sx::ResourceUID::TexQuad(tex_id, r.xmin, r.ymin, r.xmax, r.ymax);
		int block_id;
		return DTex::Instance()->QuerySymbol(uid, out_tex_id, block_id);
	};
	pt2_cb.add_cache_symbol = [](size_t tex_id, int tex_w, int tex_h, const sm::irect& r) {
		sx::UID uid = sx::ResourceUID::TexQuad(tex_id, r.xmin, r.ymin, r.xmax, r.ymax);
		LoadingList::Instance()->AddSymbol(uid, tex_id, tex_w, tex_h, r);
	};
	pt2::Callback::RegisterCallback(pt2_cb);

    // pt3
    pt3::Painting3::Init();

	// rendergraph
	rp::Callback::Funs rg_cb;
	rg_cb.query_cached_tex_quad = [](size_t tex_id, const sm::irect& r, int& out_tex_id)->const float* {
		sx::UID uid = sx::ResourceUID::TexQuad(tex_id, r.xmin, r.ymin, r.xmax, r.ymax);
		int block_id;
		return DTex::Instance()->QuerySymbol(uid, out_tex_id, block_id);
	};
	rg_cb.add_cache_symbol = [](size_t tex_id, int tex_w, int tex_h, const sm::irect& r) {
		sx::UID uid = sx::ResourceUID::TexQuad(tex_id, r.xmin, r.ymin, r.xmax, r.ymax);
		LoadingList::Instance()->AddSymbol(uid, tex_id, tex_w, tex_h, r);
	};
	rp::Callback::RegisterCallback(rg_cb);
}

void Facade::Update(float dt)
{
	for (auto& cb : m_update_cb) {
		cb(dt);
	}

	pt0::GlobalClock::Instance()->Update(dt);
	et::GlobalClock::Instance()->Update(dt);
	anim::GlobalClock::Instance()->Update(dt);
	model::GlobalClock::Instance()->Update(dt);
}

bool Facade::Flush(ur2::Context& ctx, bool dtex_cg_to_c2)
{
	bool dirty = false;
	if (DTex::Instance()->Flush(ctx, dtex_cg_to_c2)) {
		dirty = true;
	}
	if (LoadingList::Instance()->Flush(ctx)) {
		dirty = true;
	}
	m_last_frame_dirty = dirty;
	return dirty;
}

}