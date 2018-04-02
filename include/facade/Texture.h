#pragma once

#include <painting2/Texture.h>

namespace facade
{

class Texture : public pt2::Texture
{
public:
	Texture(uint16_t w, uint16_t h, uint32_t id, int format);
	virtual ~Texture();

}; // Texture

}