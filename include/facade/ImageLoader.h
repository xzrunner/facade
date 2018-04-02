#pragma once

#include <memory>
#include <string>

namespace facade
{

class Image;

class ImageLoader
{
public:
	ImageLoader(const std::string& res_path);

	bool Load();
//	bool AsyncLoad(int format, int width, int height, const std::shared_ptr<Image>& img);

	int GetID() const { return m_id; }
	int GetFormat() const { return m_format; }

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

private:
	bool LoadRaw();
//	bool LoadBin();
//	bool LoadBin(const timp::TextureLoader& loader);

	//bool DecodePVR2(const void* data);
	//bool DecodePVR4(const void* data);
	//bool DecodeETC2(const void* data);
	
	static void LoadTextureCB(int format, int w, int h, const void* data, void* ud);

private:
	std::string m_res_path;

	int m_id;
	int m_format;
	int m_width, m_height;

}; // ImageLoader

}