#include "facade/Image3D.h"

#include <unirender/Texture.h>
#include <unirender/Device.h>
#include <unirender/TextureDescription.h>
#include <volume/Loader.h>
#include <volume/VolumeData.h>

namespace facade
{

bool Image3D::LoadFromFile(const std::string& filepath)
{
	vol::VolumeData data;
	if (!vol::Loader::Load(data, filepath)) {
		return false;
	}

	m_filepath = filepath;

    ur::TextureDescription desc;
    desc.target = ur::TextureTarget::Texture3D;
    desc.width  = data.width;
    desc.height = data.height;
    desc.depth  = data.depth;
    desc.format = ur::TextureFormat::RGBA8;
    desc.gen_mipmaps = false;
    m_texture = m_dev->CreateTexture(desc, data.rgba.get());

	return true;
}

uint32_t Image3D::GetTexID() const
{
	return m_texture->GetTexID();
}

uint16_t Image3D::GetWidth() const
{
	return m_texture->GetWidth();
}

uint16_t Image3D::GetHeight() const
{
	return m_texture->GetHeight();
}

uint16_t Image3D::GetDepth() const
{
	return m_texture->GetDepth();
}

}