#pragma once

#include <cu/cu_macro.h>

#include <vector>
#include <string>
#include <set>

namespace facade
{

class GTxt
{
public:
	void Init(const std::vector<std::pair<std::string, std::string>>& fonts,
		const std::vector<std::pair<std::string, std::string>>& user_fonts);

private:
	void LoadFont(const std::string& name, const std::string& filepath);
	void LoadUserFont(const std::string& name, const std::string& filepath);
	void LoadUserFontChar(const std::string& str, const std::string& pkg, const std::string& node);

private:
	static int m_cap_bitmap, m_cap_layout;

	std::set<std::string> m_fonts;

private:
	CU_SINGLETON_DECLARATION(GTxt)

}; // GTxt

}