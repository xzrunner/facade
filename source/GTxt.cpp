#include "facade/GTxt.h"
#include "facade/DTex.h"
#include "facade/Blackboard.h"
#include "facade/RenderContext.h"

#include <node2/predef.h>
#include N2_MAT_HEADER
#include <gtxt_freetype.h>
#include <gtxt_label.h>
#include <gtxt_richtext.h>
#include <sx/ResourceUID.h>
#include <sx/GlyphStyle.h>
#include <shaderlab/RenderContext.h>
#include <shaderlab/FilterShader.h>
#include <shaderlab/Sprite2Shader.h>
#include <shaderlab/Shape2Shader.h>
#include <painting2/RenderColorCommon.h>
#include <painting2/PrimitiveDraw.h>
#include <sx/StringHelper.h>

namespace
{

/************************************************************************/
/* render                                                               */
/************************************************************************/

struct render_params
{
	const N2_MAT* mt = nullptr;
	const pt2::Color* mul = nullptr;
	const pt2::Color* add = nullptr;
//	void* ud = nullptr;
};

void
render_glyph(int id, const float* _texcoords, float x, float y, float w, float h, const gtxt_draw_style* ds, render_params* rp)
{
	x += ds->offset_x;
	y += ds->offset_y;
	float hw = w * 0.5f * ds->scale, hh = h * 0.5f * ds->scale;

	sm::vec2 vertices[4];
	vertices[0] = sm::vec2(x - hw, y - hh);
	vertices[1] = sm::vec2(x + hw, y - hh);
	vertices[2] = sm::vec2(x + hw, y + hh);
	vertices[3] = sm::vec2(x - hw, y + hh);
	for (int i = 0; i < 4; ++i) {
		vertices[i] = *rp->mt * vertices[i];
	}

	sm::vec2 texcoords[4];
	texcoords[0].Set(_texcoords[0], _texcoords[1]);
	texcoords[1].Set(_texcoords[2], _texcoords[3]);
	texcoords[2].Set(_texcoords[4], _texcoords[5]);
	texcoords[3].Set(_texcoords[6], _texcoords[7]);

	pt2::RenderColorCommon col_common;
	if (rp->mul) {
		pt2::Color multi_col = *rp->mul;
		multi_col.a = static_cast<int>(multi_col.a * ds->alpha);
		col_common.mul = multi_col;
	} 
	if (rp->add) {
		col_common.add = *rp->add;
	}

	auto& shader_mgr = facade::Blackboard::Instance()->GetRenderContext()->GetSlRc().GetShaderMgr();
	if (shader_mgr.GetShaderType() == sl::FILTER) {
		auto shader = static_cast<sl::FilterShader*>(shader_mgr.GetShader());
		shader->SetColor(col_common.mul.ToABGR(), col_common.add.ToABGR());
		shader->Draw(&vertices[0].x, &texcoords[0].x, id);
	} else {
		shader_mgr.SetShader(sl::SPRITE2);
	 	auto shader = static_cast<sl::Sprite2Shader*>(shader_mgr.GetShader());
		shader->SetColor(col_common.mul.ToABGR(), col_common.add.ToABGR());
		shader->SetColorMap(0x000000ff, 0x0000ff00, 0x00ff0000);
		shader->DrawQuad(&vertices[0].x, &texcoords[0].x, id);
	}
}

void 
render_decoration(const N2_MAT& mat, float x, float y, float w, float h, const gtxt_draw_style* ds)
{
	const gtxt_decoration* d = &ds->decoration;
	if (d->type == GRDT_NULL) {
		return;
	}

	auto& shader_mgr = facade::Blackboard::Instance()->GetRenderContext()->GetSlRc().GetShaderMgr();
	shader_mgr.SetShader(sl::SHAPE2);
	auto shader = static_cast<sl::Shape2Shader*>(shader_mgr.GetShader());
	shader->SetColor(d->color);

	float hw = w * 0.5f,
		hh = h * 0.5f;
	if (d->type == GRDT_OVERLINE || d->type == GRDT_UNDERLINE || d->type == GRDT_STRIKETHROUGH) {
		sm::vec2 left(x - hw, y), right(x + hw, y);
		switch (d->type) 
		{
		case GRDT_OVERLINE:
			left.y = right.y = ds->row_y + ds->row_h;
			break;
		case GRDT_UNDERLINE:
			left.y = right.y = ds->row_y;
			break;
		case GRDT_STRIKETHROUGH:
			left.y = right.y = ds->row_y + ds->row_h * 0.5f;
			break;
		}
		pt2::PrimitiveDraw::Line(nullptr, mat * left, mat * right);
	} else if (d->type == GRDT_BORDER || d->type == GRDT_BG) {
		sm::vec2 min(x - hw, ds->row_y), 
			max(x + hw, ds->row_y + ds->row_h);
		min = mat * min;
		max = mat * max;
		if (d->type == GRDT_BG) {
			pt2::PrimitiveDraw::Rect(nullptr, min, max, true);
		} else if (ds->pos_type != GRPT_NULL) {
			pt2::PrimitiveDraw::Line(nullptr, min, sm::vec2(max.x, min.y));
			pt2::PrimitiveDraw::Line(nullptr, sm::vec2(min.x, max.y), max);
			if (ds->pos_type == GRPT_BEGIN) {
				pt2::PrimitiveDraw::Line(nullptr, min, sm::vec2(min.x, max.y));
			}
			if (ds->pos_type == GRPT_END) {
				pt2::PrimitiveDraw::Line(nullptr, sm::vec2(max.x, min.y), max);
			}
		}
	}
}

void 
render(int id, const float* texcoords, float x, float y, float w, float h, const gtxt_draw_style* ds, void* ud) 
{
	render_params* rp = (render_params*)ud;
	if (ds) {
		if (ds->decoration.type == GRDT_BG) {
			render_decoration(*rp->mt, x, y, w, h, ds);
			render_glyph(id, texcoords, x, y, w, h, ds, rp);
		} else {
			render_glyph(id, texcoords, x, y, w, h, ds, rp);
			render_decoration(*rp->mt, x, y, w, h, ds);
		}
	} else {
		struct gtxt_draw_style ds;
		ds.alpha = 1;
		ds.scale = 1;
		ds.offset_x = ds.offset_y = 0;
		render_glyph(id, texcoords, x, y, w, h, &ds, rp);
	}
}

void
draw_glyph(int unicode, float x, float y, float w, float h, 
		   const gtxt_glyph_style* gs, const gtxt_draw_style* ds, void* ud) 
{	
	int tex_id, block_id;
	int ft_count = gtxt_ft_get_font_cout();
	sx::UID uid = sx::ResourceUID::Glyph(unicode, sx::GlyphStyle(gs));

	auto dtex = facade::DTex::Instance();

	const float* texcoords = nullptr;
	bool exist = false;
	if (gs->font < ft_count && !dtex->ExistGlyph(uid)) {
		exist = false;
		texcoords = nullptr;
	} else {
		exist = true;
		texcoords = dtex->QuerySymbol(uid, tex_id, block_id);
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
			if (exist && dtex->QueryGlyph(uid, texcoords, tex_id))
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
				dtex->LoadGlyph(bmp, static_cast<int>(w), static_cast<int>(h), uid);
			}
		} 
		else 
		{
			int uf_font = gs->font - ft_count;
//			dtex->DrawUFChar(unicode, uf_font, x, y, ud);
		}
	}
}

/************************************************************************/
/* richtext extern symbol                                               */
/************************************************************************/

void*
ext_sym_create(const char* str) {
	CU_STR src = sx::StringHelper::UTF8ToGBK(str);
	CU_STR::size_type pos = src.find("export");
	if (pos != CU_STR::npos) {
		src.insert(pos, " ");
	}
	CU_VEC<CU_STR> tokens;
	sx::StringHelper::Split(src, " =", tokens);
	s2::SymPtr sym = nullptr;
	if (tokens.size() == 2) {
		if (tokens[0] == "path") {
			sym = SymbolPool::Instance()->Fetch(tokens[1]);
		}
	} else if (tokens.size() == 4) {
		if (tokens[0] == "pkg" && tokens[2] == "export") {
			uint32_t id = simp::NodeFactory::Instance()->GetNodeID(tokens[1], tokens[3]);
			if (id != 0xffffffff) {
				sym = SymbolPool::Instance()->Fetch(id);
			}
		}
	}
	return new s2::SymPtr(sym);
}

void
ext_sym_release(void* ext_sym) {
	if (ext_sym) {
		delete static_cast<s2::SymPtr*>(ext_sym);
	}
}

void 
ext_sym_get_size(void* ext_sym, int* width, int* height) {
	if (!ext_sym) {
		*width= *height = 0;
		return;
	}

	s2::SymPtr sym(*static_cast<s2::SymPtr*>(ext_sym));
	sm::vec2 sz = sym->GetBounding().Size();
	*width  = static_cast<int>(sz.x);
	*height = static_cast<int>(sz.y);
}

void
ext_sym_render(void* ext_sym, float x, float y, void* ud) {
	if (!ext_sym) {
		return;
	}

	s2::RenderParams rp;
	render_params* _rp = (render_params*)ud;
	if (_rp->mt) {
		rp.mt = *_rp->mt;
	}
	if (_rp->mul) {
		rp.col_common.mul = *_rp->mul;
	}
	if (_rp->add) {
		rp.col_common.add = *_rp->add;
	}

	s2::SymPtr sym(*static_cast<s2::SymPtr*>(ext_sym));
	s2::DrawNode::Draw(*sym, rp, sm::vec2(x, y));
}

/************************************************************************/
/* user font                                                            */
/************************************************************************/

void
get_uf_layout(int unicode, int font, struct gtxt_glyph_layout* layout) {
//	facade::GTxt::Instance()->GetUFLayout(unicode, font, layout);
}

}

namespace facade
{

CU_SINGLETON_DEFINITION(GTxt)

int GTxt::m_cap_bitmap = 50;
int GTxt::m_cap_layout = 500;

GTxt::GTxt() 
{
	gtxt_label_cb_init(draw_glyph);

	gtxt_ft_create();

	gtxt_glyph_create(m_cap_bitmap, m_cap_layout, nullptr, get_uf_layout);

	gtxt_richtext_ext_sym_cb_init(&ext_sym_create, &ext_sym_release, &ext_sym_get_size, &ext_sym_render, nullptr);
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