#pragma once

#include "facade/Image.h"

#include <cu/cu_macro.h>

#include <map>
#include <memory>
#include <vector>

namespace facade
{

struct Model
{
private:
	std::map<std::string, std::shared_ptr<Image>> m_path2images;
	std::vector<std::shared_ptr<Image>> m_nopath_imgs;

private:
	CU_SINGLETON_DECLARATION(Model)

}; // Model

}