#include "facade/ImageCube.h"
#include "facade/ImageLoader.h"

namespace facade
{

bool ImageCube::LoadFromFile(const std::string& filepath)
{
    ImageLoader loader(filepath);
    if (loader.Load(*m_dev)) {
        m_filepath = filepath;
        m_texture = loader.GetTexture();
        return true;
    } else {
        return false;
    }
}

}