#pragma once

#include <unirender2/typedef.h>

#include <string>

namespace ur2 { class Device; }

namespace facade
{

class ImageCube
{
public:
    ImageCube(const ur2::Device* dev) : m_dev(dev) {}

    // for ResPool
    bool LoadFromFile(const std::string& filepath);

    auto GetTexture() { return m_texture; }

    const std::string& GetResPath() const { return m_filepath; }

private:
    const ur2::Device* m_dev = nullptr;

    std::string m_filepath;

    ur2::TexturePtr m_texture = nullptr;

}; // ImageCube

}