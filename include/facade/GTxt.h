#pragma once

#include <cu/cu_macro.h>
#include <SM_Matrix2D.h>

#include <vector>
#include <string>
#include <set>

struct gtxt_label_style;

namespace pt0 { class Color; }
namespace pt2 { class Textbox; }
namespace tess { class Painter; }

namespace facade
{

class GTxt
{
public:
    void Init(const ur2::Device& dev);

	void LoadFonts(const std::vector<std::pair<std::string, std::string>>& fonts,
		const std::vector<std::pair<std::string, std::string>>& user_fonts);

	static void Draw(const std::string& text, const pt2::Textbox& style, const sm::Matrix2D& mat,
		const pt0::Color& mul, const pt0::Color& add, int time, bool richtext, tess::Painter* pt = nullptr, bool texcoords_relocate = true);

	static sm::vec2 CalcLabelSize(const std::string& text, const pt2::Textbox& style);

private:
	void LoadFont(const std::string& name, const std::string& filepath);
	void LoadUserFont(const std::string& name, const std::string& filepath);
	void LoadUserFontChar(const std::string& str, const std::string& pkg, const std::string& node);

	static void LoadLabelStyle(gtxt_label_style& dst, const pt2::Textbox& src);

private:
	static int m_cap_bitmap, m_cap_layout;

	std::set<std::string> m_fonts;

private:
	CU_SINGLETON_DECLARATION(GTxt)

}; // GTxt

}