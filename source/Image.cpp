#include "facade/Image.h"
#include "facade/RenderContext.h"
#include "facade/ImageLoader.h"

#include <painting2/Texture.h>
#include <stat/StatImages.h>
#include <unirender/RenderContext.h>
#include <unirender/Blackboard.h>

#include <assert.h>

namespace facade
{

static int ALL_IMG_COUNT = 0;

Image::Image()
	: m_pkg_id(st::StatImages::UNKNOWN_IMG_ID)
	, m_texture(std::make_shared<pt2::Texture>(&ur::Blackboard::Instance()->GetRenderContext(), 0, 0, 0, 0))
{
	++ALL_IMG_COUNT;
}

Image::Image(int pkg_id, const std::string& res_path, bool async)
	: m_pkg_id(pkg_id)
	, m_res_path(res_path)
	, m_texture(std::make_shared<pt2::Texture>(&ur::Blackboard::Instance()->GetRenderContext(), 0, 0, 0, 0))
{
	++ALL_IMG_COUNT;

	if (!async)
	{
		ImageLoader loader(m_res_path);
		bool ret = loader.Load();
		if (ret) {
			LoadFromLoader(loader);
			st::StatImages::Instance()->Add(
				pkg_id, m_texture->Width(), m_texture->Height(), m_texture->Format());
		}
	}
}

Image::Image(const std::shared_ptr<pt2::Texture>& tex)
	: m_pkg_id(st::StatImages::UNKNOWN_IMG_ID)
	, m_texture(tex)
{
}

Image::~Image()
{
	--ALL_IMG_COUNT;

	if (m_texture->TexID() != 0) {
		st::StatImages::Instance()->Remove(
			m_pkg_id, m_texture->Width(), m_texture->Height(), m_texture->Format());
	}
}

bool Image::LoadFromFile(const std::string& filepath)
{
	m_pkg_id = 0xffff;
	m_res_path = filepath;

	ImageLoader loader(m_res_path);
	bool ret = loader.Load();
	if (!ret) {
		return false;
	}

	LoadFromLoader(loader);
	st::StatImages::Instance()->Add(
		m_pkg_id, m_texture->Width(), m_texture->Height(), m_texture->Format());

	return true;
}

//void Image::AsyncLoad(int pkg_id, int format, int width, int height)
//{
//	if (m_texture->TexID() != 0) {
//		return;
//	}
//
//	m_pkg_id = pkg_id;
//
//	ImageLoader loader(m_res_path);
//	loader.AsyncLoad(format, width, height, shared_from_this());
//
//	st::StatImages::Instance()->Add(pkg_id, width, height, format);
//}

uint16_t Image::GetWidth() const
{
	return m_texture->Width();
}

uint16_t Image::GetHeight() const
{
	return m_texture->Height();
}

uint32_t Image::GetTexID() const
{
	return m_texture->TexID();
}

bool Image::IsLoadFinished() const
{
	if (m_texture) {
		return m_texture->IsLoadFinished();
	} else {
		return true;
	}
}

void Image::SetLoadFinished(bool finished)
{
	if (m_texture) {
		m_texture->SetLoadFinished(finished);
	}
}

void Image::LoadFromLoader(const ImageLoader& loader)
{
	auto id     = loader.GetID();
	auto format = loader.GetFormat();
	auto width  = loader.GetWidth();
	auto height = loader.GetHeight();

	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	m_texture->Init(&rc, width, height, id, format);
	m_texture->InitOri(width, height);
}

int Image::GetAllImgCount()
{
	return ALL_IMG_COUNT;
}

}