#include "facade/Texture.h"

#include <unirender/RenderContext.h>
#include <unirender/Blackboard.h>

namespace facade
{

Texture::Texture(uint16_t w, uint16_t h, uint32_t id, int format)
	: pt2::Texture(w, h, id, format)
{
}

Texture::~Texture()
{
	auto& ur_rc = ur::Blackboard::Instance()->GetRenderContext();
	ur_rc.ReleaseTexture(GetTexID());
}

}
