#pragma once

#include <unirender2/typedef.h>

#include <string>
#include <memory>

namespace ur2 { class Device; }

namespace facade
{

class ImageLoader;

class Image
{
public:
	Image(const ur2::Device* dev);
	Image(const ur2::Device& dev, int pkg_id,
        const std::string& res_path, bool async);
	Image(const ur2::Device& dev, const ur2::TexturePtr& tex);
	virtual ~Image();

	// for ResPool
	bool LoadFromFile(const std::string& filepath);

	//void AsyncLoad(int pkg_id, int format, int width, int height);

	uint32_t GetTexID() const;

	uint16_t GetWidth() const;
	uint16_t GetHeight() const;

	auto GetTexture() const { return m_texture; }

	bool IsLoadFinished() const;
	void SetLoadFinished(bool finished);

	const std::string& GetResPath() const { return m_res_path; }

//private:
	void LoadFromLoader(const ImageLoader& loader);

	static int GetAllImgCount();

protected:
	int m_pkg_id;

	std::string m_res_path;

	ur2::TexturePtr m_texture = nullptr;

private:
    const ur2::Device& m_dev;

}; // Image

}