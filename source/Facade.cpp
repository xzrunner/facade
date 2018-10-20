#include "facade/Facade.h"
#include "facade/DTex.h"
#include "facade/GTxt.h"

#include <emitter/Particle3d.h>
#include <emitter/GlobalClock.h>
#include <anim/GlobalClock.h>
#include <model/GlobalClock.h>
#include <painting0/GlobalClock.h>
#include <painting2/Callback.h>
#include <painting2/RenderColorCommon.h>
#include <node2/AABBSystem.h>

namespace facade
{

CU_SINGLETON_DEFINITION(Facade);

Facade::Facade()
{
}

void Facade::Init()
{
	et::Particle3d::Init();

	DTex::Instance();
	GTxt::Instance();

	for (auto& cb : m_init_cb) {
		cb();
	}

	// pt2
	pt2::Callback::Funs pt2_cb;
	pt2_cb.draw_text = [](
		const std::string& text, const pt2::Textbox& style,
		const sm::Matrix2D& mat, const pt2::Color& mul_col, const pt2::Color& add_col) {
		GTxt::Draw(text, style, mat, mul_col, add_col, 0, false);
	};
	pt2_cb.calc_label_size = [](const std::string& text, const pt2::Textbox& style)->sm::vec2 {
		return GTxt::CalcLabelSize(text, style);
	};
	pt2_cb.get_bounding = [](const n0::CompAsset& casset)->sm::rect {
		return n2::AABBSystem::GetBounding(casset);
	};
	pt2::Callback::RegisterCallback(pt2_cb);
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

void Facade::Flush()
{
	GTxt::Instance()->Flush();
	DTex::Instance()->Flush();
}

}