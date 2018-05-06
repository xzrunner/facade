#include "facade/Model.h"
#include "facade/ResPool.h"
#include "facade/Image.h"

#include <model/Callback.h>
#include <guard/check.h>

namespace facade
{

CU_SINGLETON_DEFINITION(Model)

Model::Model()
{
	model::Callback::Funs cb;

	cb.create_img = [&](const std::string& filepath)->void*
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

	cb.release_img = [&](void* img)
	{
		for (auto& itr = m_path2images.begin(); itr != m_path2images.end(); ++itr) {
			if (itr->second.get() == img) {
				m_path2images.erase(itr);
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