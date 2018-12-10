#include "facade/Image3D.h"

#include <painting3/Texture3D.h>
#include <unirender/RenderContext.h>
#include <unirender/Blackboard.h>
#include <volume/Loader.h>
#include <volume/VolumeData.h>

namespace facade
{

Image3D::Image3D()
	: m_texture(std::make_shared<pt3::Texture3D>(&ur::Blackboard::Instance()->GetRenderContext(), 0, 0, 0, 0, 0))
{
}

bool Image3D::LoadFromFile(const std::string& filepath)
{
	vol::VolumeData data;
	if (!vol::Loader::Load(data, filepath)) {
		return false;
	}

	m_filepath = filepath;
	
	const int n = data.width * data.height * data.depth;
	auto rgba_buf = new unsigned char[n * 4];
	if (!rgba_buf) {
		return false;
	}

	for (int i = 0; i < n; ++i) {
		rgba_buf[i * 4 + 0] = data.buf[i];
		rgba_buf[i * 4 + 1] = data.buf[i];
		rgba_buf[i * 4 + 2] = data.buf[i];
		rgba_buf[i * 4 + 3] = data.buf[i];
	}

	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	m_texture->Upload(&rc, data.width, data.height, data.depth, ur::TEXTURE_RGBA8, rgba_buf);

	return true;
}

uint32_t Image3D::GetTexID() const
{
	return m_texture->TexID();
}

uint16_t Image3D::GetWidth() const
{
	return m_texture->Width();
}

uint16_t Image3D::GetHeight() const
{
	return m_texture->Height();
}

uint16_t Image3D::GetDepth() const
{
	return m_texture->Depth();
}

}