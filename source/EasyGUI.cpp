#include "facade/EasyGUI.h"
#include "facade/GTxt.h"

#include <unirender2/Texture.h>
#include <tessellation/Painter.h>
#include <painting2/Textbox.h>
#include <easygui/Callback.h>
#include <renderpipeline/Callback.h>
#include <renderpipeline/SpriteRenderer.h>
#include <renderpipeline/RenderMgr.h>
#include <tessellation/Palette.h>

namespace
{

pt2::Textbox TEXTBOX;

std::shared_ptr<ur2::Device> UR_DEV = nullptr;
std::shared_ptr<ur2::Context> UR_CTX = nullptr;

enum TexType
{
	PALETTE,
	TEX_COUNT,
};
struct Texture
{
	int id;
	size_t w, h;
};
std::array<Texture, TEX_COUNT> TEXTURES;

}

namespace facade
{

CU_SINGLETON_DEFINITION(EasyGUI);

EasyGUI::EasyGUI()
{
	egui::style_colors_dark(ctx.style);

	auto sr = std::static_pointer_cast<rp::SpriteRenderer>(
		rp::RenderMgr::Instance()->SetRenderer(*UR_DEV, *UR_CTX, rp::RenderType::SPRITE)
	);
	auto tex = sr->GetPalette().GetTexture();
	TEXTURES[TexType::PALETTE].id = tex->GetTexID();
	TEXTURES[TexType::PALETTE].w = tex->GetWidth();
	TEXTURES[TexType::PALETTE].h = tex->GetHeight();

	TEXTBOX.width = 300;
	TEXTBOX.font_size = ctx.style.font_sz;
	TEXTBOX.align_vert = pt2::Textbox::VA_CENTER;
	egui::Callback::Funs cb;
	cb.get_label_sz = [](const char* label)->sm::vec2 {
		return GTxt::Instance()->CalcLabelSize(label, TEXTBOX);
	};
	cb.draw_label = [](ur2::Context& ctx, const char* label, const sm::vec2& pos, float angle, uint32_t color, tess::Painter& pt)
	{
		if (!label) {
			return;
		}

		sm::Matrix2D mat;
		mat.Rotate(angle);
		const float len = TEXTBOX.width * 0.5f;
		float dx = len * cos(angle);
		float dy = len * sin(angle);
		mat.Translate(pos.x + dx, pos.y + dy);

		pt0::Color col;
		col.FromABGR(color);

		GTxt::Instance()->Draw(ctx, label, TEXTBOX, mat, col, pt0::Color(0, 0, 0), 0, false, &pt, true);
	};
	cb.relocate_texcoords = [](tess::Painter& pt)
	{
		auto relocate_palette = [](const Texture& tex, tess::Painter::Buffer& buf, int begin, int end)
		{
			assert(begin < end);
			sm::irect qr(0, 0, tex.w, tex.h);
            ur2::TexturePtr cached_tex = nullptr;
			auto cached_texcoords = rp::Callback::QueryCachedTexQuad(tex.id, qr, cached_tex);
			if (cached_texcoords)
			{
				float x = cached_texcoords[0];
				float y = cached_texcoords[1];
				float w = cached_texcoords[2] - cached_texcoords[0];
				float h = cached_texcoords[5] - cached_texcoords[1];

				auto v_ptr = &buf.vertices[begin];
				for (size_t i = 0, n = end - begin + 1; i < n; ++i)
				{
					auto& v = *v_ptr++;
					v.uv.x = x + w * v.uv.x;
					v.uv.y = y + h * v.uv.y;
				}
			}
			else
			{
				rp::Callback::AddCacheSymbol(cached_tex, qr);
			}
		};

		auto& buf = const_cast<tess::Painter::Buffer&>(pt.GetBuffer());
		if (buf.vertices.empty()) {
			return;
		}

		auto& regions = pt.GetOtherTexRegion();
		if (regions.empty())
		{
			relocate_palette(TEXTURES[TexType::PALETTE], buf, 0, buf.vertices.size() - 1);
		}
		else
		{
			auto& palette = TEXTURES[TexType::PALETTE];
			if (regions.front().begin > 0) {
				relocate_palette(palette, buf, 0, regions.front().begin - 1);
			}
			for (int i = 0, n = regions.size(); i < n; ++i)
			{
				auto& r = regions[i];
				if (i > 0) {
					auto& prev = regions[i - 1];
					if (prev.end + 1 < r.begin) {
						relocate_palette(palette, buf, prev.end + 1, r.begin - 1);
					}
				}
			}
			if (regions.back().end < static_cast<int>(buf.vertices.size() - 1)) {
				relocate_palette(palette, buf, regions.back().end + 1, buf.vertices.size() - 1);
			}
		}
	};

	egui::Callback::RegisterCallback(cb);
}

}