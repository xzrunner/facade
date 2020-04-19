#pragma once

#include <unirender2/typedef.h>

#include <memory>
#include <string>

namespace ur2 { class Device; }

namespace facade
{

class Image3D
{
public:
    Image3D(const ur2::Device* dev) : m_dev(dev) {}

	// for ResPool
	bool LoadFromFile(const std::string& filepath);

	uint32_t GetTexID() const;

	uint16_t GetWidth() const;
	uint16_t GetHeight() const;
	uint16_t GetDepth() const;

	auto& GetTexture() const { return m_texture; }

private:
    const ur2::Device* m_dev;

	std::string m_filepath;

	ur2::TexturePtr m_texture = nullptr;

}; // Image3D

}