#pragma once

#include <cu/cu_macro.h>
#include <SM_Matrix2D.h>
#include <gtxt_glyph.h>
#include <sx/ResourceUID.h>

#include <vector>
#include <string>
#include <set>
#include <map>

struct gtxt_label_style;

namespace pt2 { class Color; class Textbox; }

namespace facade
{

class GTxt
{
public:
	void LoadFonts(const std::vector<std::pair<std::string, std::string>>& fonts,
		const std::vector<std::pair<std::string, std::string>>& user_fonts);

	void AddLoadingGlyph(sx::UID uid, int unicode, float line_x, const gtxt_glyph_style& gs);
	void Flush();

	static void Draw(const std::string& text, const pt2::Textbox& style, const sm::Matrix2D& mat,
		const pt2::Color& mul, const pt2::Color& add, int time, bool richtext);

	static sm::vec2 CalcLabelSize(const std::string& text, const pt2::Textbox& style);

private:
	void LoadFont(const std::string& name, const std::string& filepath);
	void LoadUserFont(const std::string& name, const std::string& filepath);
	void LoadUserFontChar(const std::string& str, const std::string& pkg, const std::string& node);

	static void LoadLabelStyle(gtxt_label_style& dst, const pt2::Textbox& src);

private:
	struct Glyph
	{
		int unicode;
		float line_x;
		gtxt_glyph_style gs;
	};

private:
	static int m_cap_bitmap, m_cap_layout;

	std::set<std::string> m_fonts;

	std::map<sx::UID, Glyph> m_loading_list;

private:
	CU_SINGLETON_DECLARATION(GTxt)

}; // GTxt

}