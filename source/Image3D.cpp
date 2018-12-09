#include "facade/Image3D.h"

#include <fs_file.h>
#include <painting3/Texture3D.h>
#include <unirender/RenderContext.h>
#include <unirender/Blackboard.h>

#include <boost/filesystem.hpp>

namespace facade
{

Image3D::Image3D()
	: m_texture(std::make_shared<pt3::Texture3D>(&ur::Blackboard::Instance()->GetRenderContext(), 0, 0, 0, 0, 0))
{
}

bool Image3D::LoadFromFile(const std::string& filepath)
{
	if (!boost::filesystem::is_regular_file(filepath)) {
		return false;
	}

	int w = 256;
	int h = 256;
	int d = 109;

	const int n = w * h * d;

	auto alpha_buf = new unsigned char[n];
	if (!alpha_buf) {
		return false;
	}
	auto rgba_buf = new unsigned char[n * 4];
	if (!rgba_buf) {
		return false;
	}

	struct fs_file* file = fs_open(filepath.c_str(), "rb");
	if (!file) {
		return false;
	}
	fs_read(file, alpha_buf, n);
	fs_close(file);

	for (int i = 0; i < n; ++i) {
		rgba_buf[i * 4 + 0] = alpha_buf[i];
		rgba_buf[i * 4 + 1] = alpha_buf[i];
		rgba_buf[i * 4 + 2] = alpha_buf[i];
		rgba_buf[i * 4 + 3] = alpha_buf[i];
	}

	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	m_texture->Upload(&rc, 256, 256, 109, ur::TEXTURE_RGBA8, rgba_buf);

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