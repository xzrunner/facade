#pragma once

#include <memory>
#include <string>

namespace pt3 { class Texture3D; }

namespace facade
{

class Image3D
{
public:
	Image3D();

	// for ResPool
	bool LoadFromFile(const std::string& filepath);

	uint32_t GetTexID() const;

	uint16_t GetWidth() const;
	uint16_t GetHeight() const;
	uint16_t GetDepth() const;

	auto& GetTexture() const { return m_texture; }

private:
	std::string m_filepath;

	std::shared_ptr<pt3::Texture3D> m_texture;

}; // Image3D

}