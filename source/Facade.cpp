#include "facade/Facade.h"
#include "facade/DTex.h"
#include "facade/GTxt.h"

#include <emitter/Particle3d.h>
#include <emitter/GlobalClock.h>
#include <anim/GlobalClock.h>
#include <painting2/Callback.h>
#include <painting2/RenderColorCommon.h>

namespace
{

void draw_text(const pt2::Text& text, const sm::Matrix2D& mat, const pt2::RenderColorCommon& col)
{
	facade::GTxt::Draw(text, mat, col.mul, col.add, 0, false);
}

}

namespace facade
{

void Facade::Init()
{
	et::Particle3d::Init();

	DTex::Instance();
	GTxt::Instance();

	pt2::Callback::Funs pt2_cb;
	pt2_cb.draw_text = draw_text;
	pt2::Callback::RegisterCallback(pt2_cb);
}

void Facade::Update(float dt)
{
	et::GlobalClock::Instance()->Update(dt);
	anim::GlobalClock::Instance()->Update(dt);
}

}