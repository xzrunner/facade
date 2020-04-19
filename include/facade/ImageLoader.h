#pragma once

#include <unirender2/TextureWrap.h>
#include <unirender2/TextureMinificationFilter.h>
#include <unirender2/TextureMagnificationFilter.h>
#include <unirender2/TextureTarget.h>
#include <unirender2/TextureFormat.h>
#include <unirender2/typedef.h>

#include <memory>
#include <string>

namespace ur2 { class Device; }

namespace facade
{

class Image;

class ImageLoader
{
public:
	ImageLoader(const std::string& res_path);

    bool Load(const ur2::Device& dev, ur2::TextureWrap wrap = ur2::TextureWrap::Repeat,
        ur2::TextureMinificationFilter min_filter = ur2::TextureMinificationFilter::Linear,
        ur2::TextureMagnificationFilter mag_filter = ur2::TextureMagnificationFilter::Linear);
//	bool AsyncLoad(int format, int width, int height, const std::shared_ptr<Image>& img);

    //auto GetType() const { return m_type; }

	//int GetID() const { return m_id; }
	//auto GetFormat() const { return m_format; }

	//int GetWidth() const { return m_width; }
	//int GetHeight() const { return m_height; }

    auto GetTexture() const { return m_tex; }

private:
	bool LoadRaw(const ur2::Device& dev, ur2::TextureWrap wrap,
        ur2::TextureMinificationFilter min_filter, ur2::TextureMagnificationFilter mag_filter);
//	bool LoadBin();
//	bool LoadBin(const timp::TextureLoader& loader);

	//bool DecodePVR2(const void* data);
	//bool DecodePVR4(const void* data);
	//bool DecodeETC2(const void* data);

	static void LoadTextureCB(int format, int w, int h, const void* data, void* ud);

private:
	std::string m_res_path;

    ur2::TexturePtr m_tex = nullptr;

 //   ur2::TextureTarget m_type;



	//ur2::TextureFormat m_format;
	//int m_width, m_height;

}; // ImageLoader

}