#include "facade/Facade.h"
#include "facade/DTex.h"
#include "facade/GTxt.h"
#include "facade/Model.h"

#include <emitter/Particle3d.h>
#include <emitter/GlobalClock.h>
#include <anim/GlobalClock.h>
#include <model/GlobalClock.h>
#include <painting2/Callback.h>
#include <painting2/RenderColorCommon.h>
#include <node2/AABBSystem.h>

namespace
{

template<typename T>
sm::rect get_bounding(const T& data)
{
	return n2::AABBSystem::GetBounding(data);
}

}

namespace facade
{

void Facade::Init()
{
	et::Particle3d::Init();

	DTex::Instance();
	GTxt::Instance();
	Model::Instance();

	// pt2
	pt2::Callback::Funs pt2_cb;
	pt2_cb.draw_text = [](const pt2::Text& text, const sm::Matrix2D& mat, const pt2::RenderColorCommon& col) {
		GTxt::Draw(text, mat, col.mul, col.add, 0, false);
	};
	pt2_cb.get_bounding = [](const n0::CompAsset& casset)->sm::rect {
		return n2::AABBSystem::GetBounding(casset);
	};
	pt2::Callback::RegisterCallback(pt2_cb);
}

void Facade::Update(float dt)
{
	et::GlobalClock::Instance()->Update(dt);
	anim::GlobalClock::Instance()->Update(dt);
	model::GlobalClock::Instance()->Update(dt);
}

}