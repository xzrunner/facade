#pragma once

#include "facade/Image.h"

#include <cu/cu_macro.h>

#include <map>
#include <memory>

namespace facade
{

class Model
{
private:
	std::map<std::string, std::shared_ptr<Image>> m_path2images;

private:
	CU_SINGLETON_DECLARATION(Model)

}; // Model

}