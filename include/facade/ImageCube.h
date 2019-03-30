#pragma once

#include <string>

namespace ur { class TextureCube; }

namespace facade
{

class ImageCube
{
public:
    ImageCube();

    // for ResPool
    bool LoadFromFile(const std::string& filepath);

    auto GetTexture() { return m_texture; }

private:
    std::string m_filepath;

    std::shared_ptr<ur::TextureCube> m_texture = nullptr;

}; // ImageCube

}