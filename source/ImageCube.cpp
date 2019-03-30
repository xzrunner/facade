#include "facade/ImageCube.h"
#include "facade/ImageLoader.h"

#include <unirender/TextureCube.h>
#include <unirender/Blackboard.h>

namespace facade
{

ImageCube::ImageCube()
    : m_texture(std::make_shared<ur::TextureCube>(&ur::Blackboard::Instance()->GetRenderContext()))
{
}

bool ImageCube::LoadFromFile(const std::string& filepath)
{
    ImageLoader loader(filepath);
    if (loader.Load()) {
        m_filepath = filepath;
        m_texture->BuildFromEquirectangularTex(loader.GetID());
        return true;
    } else {
        return false;
    }
}

}