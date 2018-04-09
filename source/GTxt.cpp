#include "facade/GTxt.h"

#include <gtxt_freetype.h>
#include <gtxt_label.h>

namespace
{

static void
draw_glyph(int unicode, float x, float y, float w, float h, 
		   const gtxt_glyph_style* gs, const gtxt_draw_style* ds, void* ud) 
{	
	int tex_id, block_id;
	int ft_count = gtxt_ft_get_font_cout();
	UID uid = ResourceUID::Glyph(unicode, GlyphStyle(gs));

	const float* texcoords = nullptr;
	bool exist = false;
	if (gs->font < ft_count && !DTex::Instance()->ExistGlyph(uid)) {
		exist = false;
		texcoords = nullptr;
	} else {
		exist = true;
		texcoords = DTex::Instance()->QuerySymbol(uid, tex_id, block_id);
	}

	if (texcoords) 
	{
		render(tex_id, texcoords, x, y, w, h, ds, ud);
	} 
	else 
	{
		if (gs->font < ft_count) 
		{
			float texcoords[8];
			if (exist && DTex::Instance()->QueryGlyph(uid, texcoords, tex_id)) 
			{
				render(tex_id, texcoords, x, y, w, h, ds, ud);
			} 
			else 
			{
				struct gtxt_glyph_layout layout;
				uint32_t* bmp = gtxt_glyph_get_bitmap(unicode, gs, &layout);
				if (!bmp) {
					return;
				}
				w = layout.sizer.width;
				h = layout.sizer.height;
				DTex::Instance()->LoadGlyph(bmp, static_cast<int>(w), static_cast<int>(h), uid);
			}
		} 
		else 
		{
			int uf_font = gs->font - ft_count;
			GTxt::Instance()->DrawUFChar(unicode, uf_font, x, y, ud);
		}
	}
}

}

namespace facade
{

GTxt::GTxt() 
{
	gtxt_label_cb_init(draw_glyph);

	gtxt_ft_create();

	gtxt_glyph_create(m_cap_bitmap, m_cap_layout, nullptr, get_uf_layout);

//	gtxt_richtext_ext_sym_cb_init(&ext_sym_create, &ext_sym_release, &ext_sym_get_size, &ext_sym_render, nullptr);
}

void GTxt::Init(const std::vector<std::pair<std::string, std::string>>& fonts, 
	            const std::vector<std::pair<std::string, std::string>>& user_fonts)
{
	for (auto& pair : fonts) {
		LoadFont(pair.first, pair.second);
	}
	for (auto& pair : user_fonts) {
		LoadUserFont(pair.first, pair.second);
	}
}

void GTxt::LoadFont(const std::string& name, const std::string& filepath)
{
	auto itr = m_fonts.find(name);
	if (itr != m_fonts.end()) {
		return;
	}

	gtxt_ft_add_font(name.c_str(), filepath.c_str());
	m_fonts.insert(name);
}

void GTxt::LoadUserFont(const std::string& name, const std::string& filepath)
{
	// todo
}

void GTxt::LoadUserFontChar(const std::string& str, const std::string& pkg, const std::string& node)
{
	// todo
}

}