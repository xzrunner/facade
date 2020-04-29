#pragma once

#include <unirender/typedef.h>

#include <string>

namespace ur { class Device; }

namespace facade
{

class ImageCube
{
public:
    ImageCube(const ur::Device* dev) : m_dev(dev) {}

    // for ResPool
    bool LoadFromFile(const std::string& filepath);

    auto GetTexture() { return m_texture; }

    const std::string& GetResPath() const { return m_filepath; }

private:
    const ur::Device* m_dev = nullptr;

    std::string m_filepath;

    ur::TexturePtr m_texture = nullptr;

}; // ImageCube

}