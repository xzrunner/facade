#include "facade/Image.h"
#include "facade/RenderContext.h"
#include "facade/ImageLoader.h"

#include <unirender/Device.h>
#include <unirender/Texture.h>
#include <stat/StatImages.h>

#include <assert.h>

namespace facade
{

static int ALL_IMG_COUNT = 0;

Image::Image(const ur::Device* dev)
	: m_dev(*dev)
    , m_pkg_id(st::StatImages::UNKNOWN_IMG_ID)
{
	++ALL_IMG_COUNT;
}

Image::Image(const ur::Device& dev, int pkg_id, const std::string& res_path, bool async)
	: m_dev(dev)
    , m_pkg_id(pkg_id)
	, m_res_path(res_path)
{
	++ALL_IMG_COUNT;

	if (!async)
	{
		ImageLoader loader(m_res_path);
		bool ret = loader.Load(dev);
		if (ret) {
			LoadFromLoader(loader);
			st::StatImages::Instance()->Add(
				pkg_id,
                m_texture->GetWidth(),
                m_texture->GetHeight(),
                static_cast<int>(m_texture->GetFormat())
            );
		}
	}
}

Image::Image(const ur::Device& dev, const ur::TexturePtr& tex)
	: m_dev(dev)
    , m_pkg_id(st::StatImages::UNKNOWN_IMG_ID)
	, m_texture(tex)
{
}

Image::~Image()
{
	--ALL_IMG_COUNT;

	if (m_texture) {
		st::StatImages::Instance()->Remove(
			m_pkg_id,
            m_texture->GetWidth(),
            m_texture->GetHeight(),
            static_cast<int>(m_texture->GetFormat())
        );
	}
}

bool Image::LoadFromFile(const std::string& filepath)
{
	m_pkg_id = 0xffff;
	m_res_path = filepath;

	ImageLoader loader(m_res_path);
	bool ret = loader.Load(m_dev);
	if (!ret) {
		return false;
	}

	LoadFromLoader(loader);
	st::StatImages::Instance()->Add(
		m_pkg_id,
        m_texture->GetWidth(),
        m_texture->GetHeight(),
        static_cast<int>(m_texture->GetFormat())
    );

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
	return m_texture->GetWidth();
}

uint16_t Image::GetHeight() const
{
	return m_texture->GetHeight();
}

uint32_t Image::GetTexID() const
{
	return m_texture->GetTexID();
}

bool Image::IsLoadFinished() const
{
	//if (m_texture) {
	//	return m_texture->IsLoadFinished();
	//} else {
	//	return true;
	//}

    return true;
}

void Image::SetLoadFinished(bool finished)
{
	//if (m_texture) {
	//	m_texture->SetLoadFinished(finished);
	//}
}

void Image::LoadFromLoader(const ImageLoader& loader)
{
	//auto id     = loader.GetID();
	//auto format = loader.GetFormat();
	//auto width  = loader.GetWidth();
	//auto height = loader.GetHeight();

	//auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	//m_texture->Init(&rc, width, height, id, format);
	//m_texture->InitOri(width, height);

    m_texture = loader.GetTexture();
}

int Image::GetAllImgCount()
{
	return ALL_IMG_COUNT;
}

}