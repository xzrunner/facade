#include "facade/Model.h"
#include "facade/ResPool.h"
#include "facade/Image.h"
#include "facade/Texture.h"

#include <unirender/Blackboard.h>
#include <unirender/RenderContext.h>
#include <model/Callback.h>
#include <guard/check.h>

namespace facade
{

CU_SINGLETON_DEFINITION(Model)

Model::Model()
{
	model::Callback::Funs cb;

	cb.create_img_from_file = [&](const std::string& filepath)->void*
	{
		auto itr = m_path2images.find(filepath);
		if (itr != m_path2images.end()) {
			return itr->second.get();
		} else {
			auto img = ResPool::Instance().Fetch<Image>(filepath);
			m_path2images.insert({ filepath, img });
			return img.get();
		}
	};

	cb.create_img_from_memory = [&](const unsigned char* pixels, int width, int height, int channels)->void*
	{
		auto& rc = ur::Blackboard::Instance()->GetRenderContext();
		int format = ur::TEXTURE_INVALID;
		switch (channels)
		{
		case 4:
			format = ur::TEXTURE_RGBA8;
			break;
		case 3:
			format = ur::TEXTURE_RGB;
			break;
		case 1:
			format = ur::TEXTURE_A8;
			break;
		default:
			assert(0);
		}
		int tex_id = rc.CreateTexture(pixels, width, height, format);
		auto tex = std::make_shared<Texture>(width, height, tex_id, format);
		auto img = std::make_shared<Image>(tex);
		m_nopath_imgs.push_back(img);
		return img.get();
	};

	cb.release_img = [&](void* img)
	{
		for (auto& itr = m_path2images.begin(); itr != m_path2images.end(); ++itr) {
			if (itr->second.get() == img) {
				m_path2images.erase(itr);
				return;
			}
		}
		for (auto& itr = m_nopath_imgs.begin(); itr != m_nopath_imgs.end(); ++itr) {
			if (itr->get() == img) {
				m_nopath_imgs.erase(itr);
				return;
			}
		}
		GD_REPORT_ASSERT("not find.");
	};

	cb.get_tex_id = [](const void* img)->unsigned int
	{
		return static_cast<const Image*>(img)->GetTexID();
	};

	model::Callback::RegisterCallback(cb);
}

}