#include "facade/Facade.h"

#include <emitter/Particle3d.h>
#include <emitter/GlobalClock.h>
#include <anim/GlobalClock.h>

namespace facade
{

void Facade::Init()
{
	et::Particle3d::Init();
}

void Facade::Update(float dt)
{
	et::GlobalClock::Instance()->Update(dt);
	anim::GlobalClock::Instance()->Update(dt);
}

}