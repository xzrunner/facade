#include "facade/ImageLoader.h"

#include <gimg_import.h>
#include <gimg_typedef.h>
#include <gimg_pvr.h>
#include <gimg_etc2.h>
#include <gimg_utility.h>
#include <fs_file.h>
#include <guard/check.h>
#include <unirender2/Device.h>
#include <unirender2/Bitmap.h>
#include <renderpipeline/HDREquirectangularToCubemap.h>

#include <boost/filesystem.hpp>

namespace facade
{

ImageLoader::ImageLoader(const std::string& res_path)
	: m_res_path(res_path)
	//, m_id(0)
	//, m_width(0)
	//, m_height(0)
{
}

bool ImageLoader::Load(const ur2::Device& dev, ur2::TextureWrap wrap,
                       ur2::TextureMinificationFilter min_filter,
                       ur2::TextureMagnificationFilter mag_filter)
{
	return LoadRaw(dev, wrap, min_filter, mag_filter);

	//auto& filepath = m_res_path.GetFilepath();
	//if (filepath.find(".ept") != CU_STR::npos ||
	//	filepath.find(".pkg") != CU_STR::npos) {
	//	return LoadBin();
	//} else {
	//	return LoadRaw();
	//}
}

//bool ImageLoader::AsyncLoad(int format, int width, int height, const std::shared_ptr<Image>& img)
//{
//	if (m_res_path.IsSingleFile() &&
//		m_res_path.GetFilepath().find(".ept") == CU_STR::npos) {
//		return false;
//	}
//
//	auto& ur_rc = ur::Blackboard::Instance()->GetRenderContext();
//
//	int real_fmt = format;
//	if (real_fmt == timp::TEXTURE_ETC2) {
//		if (!ur_rc.IsSupportETC2()) {
//			real_fmt = timp::TEXTURE_RGBA4;
//		}
//	}
//
//	m_id = ur_rc.CreateTextureID(width, height, real_fmt);
//	m_format = format;
//	m_width = width;
//	m_height = height;
//
//	img->LoadFromLoader(*this);
//
//	gum::ThreadPool::Instance()->Run(LoadImageTaskMgr::Instance()->Fetch(img));
//
//	return true;
//}

bool ImageLoader::LoadRaw(const ur2::Device& dev, ur2::TextureWrap wrap,
                          ur2::TextureMinificationFilter min_filter,
                          ur2::TextureMagnificationFilter mag_filter)
{
	if (!boost::filesystem::is_regular_file(m_res_path)) {
		return false;
	}

	int w, h, fmt;
	uint8_t* pixels = gimg_import(m_res_path.c_str(), &w, &h, &fmt);
	if (!pixels) {
		return false;
	}

    //m_type = ur2::TextureTarget::Texture2D;
    //auto file_type = gimg_file_type(m_res_path.c_str());
    //switch (file_type)
    //{
    //case FILE_HDR:
    //    m_type = ur2::TextureTarget::TextureCubeMap;
    //    break;
    //}

	//if (fmt == GPF_RGBA8 && gum::Config::Instance()->GetPreMulAlpha()) {
	//	gimg_pre_mul_alpha(pixels, w, h);
	//}

    ur2::TextureFormat tf;
    int channels = 0;
	switch (fmt)
	{
	case GPF_ALPHA: case GPF_LUMINANCE: case GPF_LUMINANCE_ALPHA:
		tf =  ur2::TextureFormat::A8;
        channels = 1;
		break;
    case GPF_RED:
        tf =  ur2::TextureFormat::RED;
        channels = 1;
        break;
	case GPF_RGB:
		tf =  ur2::TextureFormat::RGB;
        channels = 3;
		break;
	case GPF_RGBA8:
		tf =  ur2::TextureFormat::RGBA8;
        channels = 4;
		break;
	case GPF_BGRA_EXT:
		tf =  ur2::TextureFormat::BGRA_EXT;
        channels = 4;
		break;
	case GPF_BGR_EXT:
		tf =  ur2::TextureFormat::BGR_EXT;
        channels = 3;
		break;
    case GPF_RGBA16F:
        tf =  ur2::TextureFormat::RGBA16F;
        channels = 4;
        break;
    case GPF_RGB16F:
        tf =  ur2::TextureFormat::RGB16F;
        channels = 3;
        break;
    case GPF_RGB32F:
        tf =  ur2::TextureFormat::RGB32F;
        channels = 3;
        break;
	case GPF_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		tf =  ur2::TextureFormat::COMPRESSED_RGBA_S3TC_DXT1_EXT;
        channels = 4;
		break;
	case GPF_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		tf =  ur2::TextureFormat::COMPRESSED_RGBA_S3TC_DXT3_EXT;
        channels = 4;
		break;
	case GPF_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		tf =  ur2::TextureFormat::COMPRESSED_RGBA_S3TC_DXT5_EXT;
        channels = 4;
		break;
	default:
		GD_REPORT_ASSERT("unknown type.");
	}

    auto bmp = std::make_shared<ur2::Bitmap>(w, h, channels, pixels);
	free(pixels);
    m_tex = dev.CreateTexture(*bmp, tf);


    //if (file_type == FILE_HDR)
    //{
    //    auto cube_id = rp::HDREquirectangularToCubemap(m_id);
    //    ur_rc.ReleaseTexture(m_id);
    //    m_id = cube_id;
    //}

	return true;
}

//bool ImageLoader::LoadBin()
//{
//	if (m_res_path.IsSingleFile())
//	{
//		timp::TextureLoader loader(m_res_path.GetFilepath().c_str());
//		loader.Load();
//		return LoadBin(loader);
//	}
//	else
//	{
//		fs_file* file = fs_open(m_res_path.GetFilepath().c_str(), "rb");
//		timp::TextureLoader loader(file, m_res_path.GetOffset());
//		loader.Load();
//		fs_close(file);
//		return LoadBin(loader);
//	}
//}
//
//bool ImageLoader::LoadBin(const timp::TextureLoader& loader)
//{
//	bool ret = true;
//
//	m_format = loader.GetFormat();
//	m_width  = loader.GetWidth();
//	m_height = loader.GetHeight();
//	switch (m_format)
//	{
//	case timp::TEXTURE_RGBA4: case timp::TEXTURE_RGBA8:
//		{
//			auto& ur_rc = ur::Blackboard::Instance()->GetRenderContext();
//			m_id = ur_rc.CreateTexture(static_cast<const uint8_t*>(loader.GetData()), m_width, m_height, m_format);
//		}
//		break;
//	case timp::TEXTURE_PVR2:
//		ret = DecodePVR2(loader.GetData());
//		break;
//	case timp::TEXTURE_PVR4:
//		ret = DecodePVR4(loader.GetData());
//		break;
//	case timp::TEXTURE_ETC1:
//		ret = false;
//		break;
//	case timp::TEXTURE_ETC2:
//		ret = DecodeETC2(loader.GetData());
//		break;
//	}
//
//	return ret;
//}

//bool ImageLoader::DecodePVR2(const void* data)
//{
//#if defined( __APPLE__ ) && !defined(__MACOSX)
////	int internal_format = 0;
////	internal_format = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
//	m_id = gum::RenderContext::Instance()->GetImpl()->CreateTexture(data, m_width, m_height, ur::TEXTURE_PVR2);
//	return true;
//#else
//	return false;
//#endif
//}
//
//bool ImageLoader::DecodePVR4(const void* data)
//{
//#if defined( __APPLE__ ) && !defined(__MACOSX)
////	int internal_format = 0;
////	internal_format = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
//	m_id = gum::RenderContext::Instance()->GetImpl()->CreateTexture(data, m_width, m_height, ur::TEXTURE_PVR4);
//#else
//	uint8_t* rgba8 = gimg_pvr_decode_rgba8(static_cast<const uint8_t*>(data), m_width, m_height);
//	uint8_t* rgba4 = gimg_rgba8_to_rgba4_dither(rgba8, m_width, m_height);
//	gimg_revert_y((uint8_t*)rgba4, m_width, m_height, GPF_RGBA4);
//	auto& ur_rc = ur::Blackboard::Instance()->GetRenderContext();
//	m_id = ur_rc.CreateTexture(rgba4, m_width, m_height, timp::TEXTURE_RGBA8);
//	free(rgba4);
//	free(rgba8);
//#endif
//	return true;
//}
//
//bool ImageLoader::DecodeETC2(const void* data)
//{
//	auto& ur_rc = ur::Blackboard::Instance()->GetRenderContext();
//	if (ur_rc.IsSupportETC2()) {
//		m_id = ur_rc.CreateTexture(data, m_width, m_height, timp::TEXTURE_ETC2);
//	} else {
//		uint8_t* rgba8 = gimg_etc2_decode_rgba8(static_cast<const uint8_t*>(data), m_width, m_height, ETC2PACKAGE_RGBA_NO_MIPMAPS);
//		uint8_t* rgba4 = gimg_rgba8_to_rgba4_dither(rgba8, m_width, m_height);
//		m_id = ur_rc.CreateTexture(rgba4, m_width, m_height, timp::TEXTURE_RGBA4);
//		free(rgba4);
//		free(rgba8);
//	}
//	return true;
//}

}
