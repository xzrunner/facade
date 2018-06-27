#pragma once

#include <string>
#include <memory>

namespace facade
{

class Texture;
class ImageLoader;

class Image
{
public:
	Image();
	Image(int pkg_id, const std::string& res_path, bool async);
	Image(const std::shared_ptr<Texture>& tex);
	virtual ~Image();

	// for ResPool
	bool LoadFromFile(const std::string& filepath);

	//void AsyncLoad(int pkg_id, int format, int width, int height);

	uint32_t GetTexID() const;

	uint16_t GetWidth() const;
	uint16_t GetHeight() const;

	const std::shared_ptr<Texture>& GetTexture() const { return m_texture; }

	bool IsLoadFinished() const;
	void SetLoadFinished(bool finished);

	const std::string& GetResPath() const { return m_res_path; }

//private:
	void LoadFromLoader(const ImageLoader& loader);

	static int GetAllImgCount();

protected:
	int m_pkg_id;

	std::string m_res_path;

	std::shared_ptr<Texture> m_texture;

}; // Image

}