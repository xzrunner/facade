#pragma once

#include <unirender/TextureWrap.h>
#include <unirender/TextureMinificationFilter.h>
#include <unirender/TextureMagnificationFilter.h>
#include <unirender/TextureTarget.h>
#include <unirender/TextureFormat.h>
#include <unirender/typedef.h>

#include <memory>
#include <string>

namespace ur { class Device; }

namespace facade
{

class Image;

class ImageLoader
{
public:
	ImageLoader(const std::string& res_path);

    bool Load(const ur::Device& dev);
//	bool AsyncLoad(int format, int width, int height, const std::shared_ptr<Image>& img);

    //auto GetType() const { return m_type; }

	//int GetID() const { return m_id; }
	//auto GetFormat() const { return m_format; }

	//int GetWidth() const { return m_width; }
	//int GetHeight() const { return m_height; }

    auto GetTexture() const { return m_tex; }

private:
	bool LoadRaw(const ur::Device& dev);
//	bool LoadBin();
//	bool LoadBin(const timp::TextureLoader& loader);

	//bool DecodePVR2(const void* data);
	//bool DecodePVR4(const void* data);
	//bool DecodeETC2(const void* data);

	static void LoadTextureCB(int format, int w, int h, const void* data, void* ud);

private:
	std::string m_res_path;

    ur::TexturePtr m_tex = nullptr;

 //   ur::TextureTarget m_type;



	//ur::TextureFormat m_format;
	//int m_width, m_height;

}; // ImageLoader

}