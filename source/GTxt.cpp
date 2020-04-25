#include "facade/GTxt.h"
#include "facade/DTex.h"
#include "facade/Blackboard.h"
#include "facade/RenderContext.h"
#include "facade/LoadingList.h"

#include <node2/predef.h>
#include N2_MAT_HEADER
#include <gtxt_freetype.h>
#include <gtxt_label.h>
#include <gtxt_richtext.h>
#include <sx/ResourceUID.h>
#include <sx/GlyphStyle.h>
#include <cpputil/StringHelper.h>
#include <unirender2/RenderState.h>
#include <unirender2/Texture.h>
#include <tessellation/Painter.h>
#include <painting2/RenderSystem.h>
#include <painting2/RenderColorCommon.h>
#include <painting2/Text.h>
#include <painting2/RenderSystem.h>
#include <node0/SceneNode.h>
#include <node0/CompAsset.h>
#include <node2/CompBoundingBox.h>
#include <node2/RenderSystem.h>
#include <ns/NodeFactory.h>

namespace
{

const ur2::Device* UR_DEV = nullptr;

/************************************************************************/
/* render                                                               */
/************************************************************************/

struct render_params
{
	const N2_MAT*     mt  = nullptr;
	const pt0::Color* mul = nullptr;
	const pt0::Color* add = nullptr;
	tess::Painter*    pt  = nullptr;
	bool texcoords_relocate = true;
    ur2::Context*     ctx = nullptr;
};

void
render_glyph(const ur2::TexturePtr& tex, const float* _texcoords, float x, float y, float w, float h, const gtxt_draw_style* ds, render_params* rp)
{
	x += ds->offset_x;
	y += ds->offset_y;
	float hw = w * 0.5f * ds->scale, hh = h * 0.5f * ds->scale;

	std::array<sm::vec2, 4> vertices;
	vertices[0] = sm::vec2(x - hw, y - hh);
	vertices[1] = sm::vec2(x + hw, y - hh);
	vertices[2] = sm::vec2(x + hw, y + hh);
	vertices[3] = sm::vec2(x - hw, y + hh);
	for (int i = 0; i < 4; ++i) {
		vertices[i] = *rp->mt * vertices[i];
	}

	std::array<sm::vec2, 4> texcoords;
	texcoords[0].Set(_texcoords[0], _texcoords[1]);
	texcoords[1].Set(_texcoords[2], _texcoords[3]);
	texcoords[2].Set(_texcoords[4], _texcoords[5]);
	texcoords[3].Set(_texcoords[6], _texcoords[7]);

	pt2::RenderColorCommon col_common;
	if (rp->mul) {
		pt0::Color multi_col = *rp->mul;
		multi_col.a = static_cast<int>(multi_col.a * ds->alpha);
		col_common.mul = multi_col;
	}
	if (rp->add) {
		col_common.add = *rp->add;
	}

	// todo: gray text with filter shader
	if (rp->pt) {
		rp->pt->AddTexQuad(tex->GetTexID(), vertices, texcoords, 0xffffffff);
	} else {
        ur2::RenderState rs;
        rs.depth_test.enabled = false;
        rs.facet_culling.enabled = false;

        rs.blending.enabled = true;
        rs.blending.separately = false;
        rs.blending.src = ur2::BlendingFactor::One;
        rs.blending.dst = ur2::BlendingFactor::OneMinusSrcAlpha;
        rs.blending.equation = ur2::BlendEquation::Add;

		pt2::RenderSystem::DrawTexQuad(*UR_DEV, *rp->ctx, rs, &vertices[0].x, &texcoords[0].x, tex, 0xffffffff);
	}
}

void
render_decoration(const N2_MAT& mat, float x, float y, float w, float h, const gtxt_draw_style* ds, render_params* rp)
{
	const gtxt_decoration* d = &ds->decoration;
	if (d->type == GRDT_NULL) {
		return;
	}

	tess::Painter pt;

	float hw = w * 0.5f,
		  hh = h * 0.5f;
	const uint32_t col = 0xffffffff;
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
		pt.AddLine(mat * left, mat * right, col);
	} else if (d->type == GRDT_BORDER || d->type == GRDT_BG) {
		sm::vec2 min(x - hw, ds->row_y),
			     max(x + hw, ds->row_y + ds->row_h);
		min = mat * min;
		max = mat * max;
		if (d->type == GRDT_BG) {
			pt.AddRectFilled(min, max, col);
		} else if (ds->pos_type != GRPT_NULL) {
			pt.AddLine(min, sm::vec2(max.x, min.y), col);
			pt.AddLine(sm::vec2(min.x, max.y), max, col);
			if (ds->pos_type == GRPT_BEGIN) {
				pt.AddLine(min, sm::vec2(min.x, max.y), col);
			}
			if (ds->pos_type == GRPT_END) {
				pt.AddLine(sm::vec2(max.x, min.y), max, col);
			}
		}
	}

    ur2::RenderState rs;
	pt2::RenderSystem::DrawPainter(*UR_DEV, *rp->ctx, rs, pt);
}

void
render(const ur2::TexturePtr& tex, const float* texcoords, float x, float y, float w, float h, const gtxt_draw_style* ds, void* ud)
{
	render_params* rp = (render_params*)ud;
	if (ds) {
		if (ds->decoration.type == GRDT_BG) {
			render_decoration(*rp->mt, x, y, w, h, ds, rp);
			render_glyph(tex, texcoords, x, y, w, h, ds, rp);
		} else {
			render_glyph(tex, texcoords, x, y, w, h, ds, rp);
			render_decoration(*rp->mt, x, y, w, h, ds, rp);
		}
	} else {
		struct gtxt_draw_style ds;
		ds.alpha = 1;
		ds.scale = 1;
		ds.offset_x = ds.offset_y = 0;
		render_glyph(tex, texcoords, x, y, w, h, &ds, rp);
	}
}

bool
has_rotated_gradient_color(const gtxt_glyph_color& col)
{
	return (col.mode_type == 1 && fabs(col.mode.TWO.angle) > FLT_EPSILON)
		|| (col.mode_type == 2 && fabs(col.mode.THREE.angle) > FLT_EPSILON);
}

void
draw_glyph(int unicode, float x, float y, float w, float h, float start_x,
		   const gtxt_glyph_style* gs, const gtxt_draw_style* ds, void* ud)
{
	float line_x = 0;
	if (has_rotated_gradient_color(gs->font_color) ||
		gs->edge && has_rotated_gradient_color(gs->edge_color)) {
		line_x = x - start_x;
	}
	sx::UID uid = sx::ResourceUID::Glyph(unicode, sx::GlyphStyle(gs, line_x));

	auto dtex = facade::DTex::Instance();

	render_params* rp = (render_params*)ud;
	if (rp->texcoords_relocate)
	{
        ur2::TexturePtr texture = nullptr;
		int block_id;
		int ft_count = gtxt_ft_get_font_cout();

		const float* texcoords = nullptr;
		bool exist = false;
		if (gs->font < ft_count && !dtex->ExistGlyph(uid)) {
			exist = false;
			texcoords = nullptr;
		} else {
			exist = true;
			texcoords = dtex->QuerySymbol(uid, texture, block_id);
		}

		if (texcoords)
		{
			render(texture, texcoords, x, y, w, h, ds, ud);
		}
		else
		{
			if (gs->font < ft_count)
			{
				float texcoords[8];
				if (exist && dtex->QueryGlyph(uid, texcoords, texture))
				{
					render(texture, texcoords, x, y, w, h, ds, ud);
				}
				else
				{
					auto gtxt = facade::GTxt::Instance();
					facade::LoadingList::Instance()->AddGlyph(uid, unicode, line_x, *gs);
				}
			}
			else
			{
				int uf_font = gs->font - ft_count;
	//			dtex->DrawUFChar(unicode, uf_font, x, y, ud);
			}
		}
	}
	else
	{
		float texcoords[8];
		ur2::TexturePtr tex;
		if (dtex->QueryGlyph(uid, texcoords, tex)) {
			render(tex, texcoords, x, y, w, h, ds, ud);
		} else {
			facade::LoadingList::Instance()->AddGlyph(uid, unicode, line_x, *gs);
		}
	}
}

/************************************************************************/
/* richtext extern symbol                                               */
/************************************************************************/

void*
ext_sym_create(const char* str) {
	auto src = cpputil::StringHelper::UTF8ToGBK(str);
	std::string::size_type pos = src.find("export");
	if (pos != std::string::npos) {
		src.insert(pos, " ");
	}
	std::vector<std::string> tokens;
	cpputil::StringHelper::Split(src, " =", tokens);

	n0::SceneNodePtr node = nullptr;
	if (tokens.size() == 2) {
		if (tokens[0] == "path") {
			node = ns::NodeFactory::Create(*UR_DEV, tokens[1]);
		}
	} else if (tokens.size() == 4) {
		//if (tokens[0] == "pkg" && tokens[2] == "export") {
		//	uint32_t id = simp::NodeFactory::Instance()->GetNodeID(tokens[1], tokens[3]);
		//	if (id != 0xffffffff) {
		//		sym = SymbolPool::Instance()->Fetch(id);
		//	}
		//}
	}
	return new n0::SceneNodePtr(node);
}

void
ext_sym_release(void* ext_sym) {
	if (ext_sym) {
		delete static_cast<n0::SceneNodePtr*>(ext_sym);
	}
}

void
ext_sym_get_size(void* ext_sym, int* width, int* height) {
	if (!ext_sym) {
		*width= *height = 0;
		return;
	}

	auto node(*static_cast<n0::SceneNodePtr*>(ext_sym));
	auto& cbb = node->GetUniqueComp<n2::CompBoundingBox>();
	auto& sz = cbb.GetSize();
	*width  = static_cast<int>(sz.Width());
	*height = static_cast<int>(sz.Height());
}

void
ext_sym_render(void* ext_sym, float x, float y, void* ud) {
	if (!ext_sym) {
		return;
	}

	n2::RenderParams rp;
	render_params* _rp = (render_params*)ud;
	if (_rp->mt) {
		rp.SetMatrix(*_rp->mt);
	}
	pt2::RenderColorCommon col;
	if (_rp->mul) {
		col.mul = *_rp->mul;
	}
	if (_rp->add) {
		col.add = *_rp->add;
	}
	rp.SetColor(col);

	auto node(*static_cast<n0::SceneNodePtr*>(ext_sym));
	auto& casset = node->GetSharedComp<n0::CompAsset>();
    ur2::RenderState rs;
	n2::RenderSystem::Instance()->Draw(
        *UR_DEV, *_rp->ctx, rs, casset, sm::vec2(x, y), 0, sm::vec2(1, 1), sm::vec2(0, 0), rp
    );
}

/************************************************************************/
/* user font                                                            */
/************************************************************************/

void
get_uf_layout(int unicode, int font, struct gtxt_glyph_layout* layout) {
//	facade::GTxt::Instance()->GetUFLayout(unicode, font, layout);
}

void CopyColor(gtxt_glyph_color& dst, const pt2::GradientColor& src)
{
	GD_ASSERT(src.items.size() >= 1 && src.items.size() <= 3, "err col");
	switch (src.items.size())
	{
	case 1:
		{
			dst.mode_type = 0;
			dst.mode.ONE.color.integer = src.items[0].col.ToRGBA();
		}
		break;
	case 2:
		{
			dst.mode_type = 1;
			dst.mode.TWO.begin_col.integer = src.items[0].col.ToRGBA();
			dst.mode.TWO.begin_pos = src.items[0].pos;
			dst.mode.TWO.end_col.integer = src.items[1].col.ToRGBA();
			dst.mode.TWO.end_pos = src.items[1].pos;
			dst.mode.TWO.angle = src.angle;
		}
		break;
	case 3:
		{
			dst.mode_type = 2;
			dst.mode.THREE.begin_col.integer = src.items[0].col.ToRGBA();
			dst.mode.THREE.begin_pos = src.items[0].pos;
			dst.mode.THREE.mid_col.integer = src.items[1].col.ToRGBA();
			dst.mode.THREE.mid_pos = src.items[1].pos;
			dst.mode.THREE.end_col.integer = src.items[2].col.ToRGBA();
			dst.mode.THREE.end_pos = src.items[2].pos;
			dst.mode.THREE.angle = src.angle;
		}
		break;
	}
}

}

namespace facade
{

CU_SINGLETON_DEFINITION(GTxt)

int GTxt::m_cap_bitmap = 50;
int GTxt::m_cap_layout = 500;

GTxt::GTxt()
{
}

void GTxt::Init(const ur2::Device& dev)
{
    UR_DEV = &dev;

    gtxt_label_cb_init(draw_glyph);

    gtxt_ft_create();

    gtxt_glyph_create(m_cap_bitmap, m_cap_layout, nullptr, get_uf_layout);

    gtxt_richtext_ext_sym_cb_init(&ext_sym_create, &ext_sym_release, &ext_sym_get_size, &ext_sym_render, nullptr);
}

void GTxt::Draw(ur2::Context& ctx, const std::string& text, const pt2::Textbox& style, const sm::Matrix2D& mat,
	            const pt0::Color& mul, const pt0::Color& add, int time, bool richtext, tess::Painter* pt, bool texcoords_relocate)
{
	gtxt_label_style gtxt_style;
	LoadLabelStyle(gtxt_style, style);

	render_params rp;
	rp.mt  = &mat;
	rp.mul = &mul;
	rp.add = &add;
	rp.pt  = pt;
	rp.texcoords_relocate = texcoords_relocate;
    rp.ctx = &ctx;

	if (richtext) {
		gtxt_label_draw_richtext(text.c_str(), &gtxt_style, time, (void*)&rp);
	} else {
		gtxt_label_draw(text.c_str(), &gtxt_style, (void*)&rp);
	}
}

sm::vec2 GTxt::CalcLabelSize(const std::string& text, const pt2::Textbox& style)
{
	gtxt_label_style gtxt_style;
	LoadLabelStyle(gtxt_style, style);

	sm::vec2 sz;
	gtxt_get_label_size(text.c_str(), &gtxt_style, &sz.x, &sz.y);

	return sz;
}

void GTxt::LoadFonts(const std::vector<std::pair<std::string, std::string>>& fonts,
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

void GTxt::LoadLabelStyle(gtxt_label_style& dst, const pt2::Textbox& src)
{
	dst.width = src.width;
	dst.height = src.height;

	dst.align_h = src.align_hori;
	dst.align_v = src.align_vert;

	dst.space_h = src.space_hori;
	dst.space_v = src.space_vert;

	dst.gs.font = src.font_type;
	dst.gs.font_size = src.font_size;
	CopyColor(dst.gs.font_color, src.font_color);

	dst.gs.edge = src.has_edge;
	dst.gs.edge_size = src.edge_size;
	CopyColor(dst.gs.edge_color, src.edge_color);

	dst.over_label = src.overlabel;
}

}