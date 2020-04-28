#pragma once

#include <cu/cu_macro.h>
#include <SM_Rect.h>
#include <sx/ResourceUID.h>
#include <unirender2/typedef.h>

namespace ur2 { class Device; class Context; class ShaderProgram; class VertexArray; }
namespace dtex { class PixelBuffer; class TextureBuffer; class TexRenderer; }

namespace facade
{

class DTex
{
public:
    void Init(const ur2::Device& dev);
	void InitHook(void(*draw_begin)(), void(*draw_end)(), void(*error_reload)());

	// cache GameObj
	void LoadSymStart();
	void LoadSymbol(sx::UID sym_id, const ur2::TexturePtr& tex, const sm::irect& region,
		int padding = 0, int extrude = 0, int src_extrude = 0);
	void LoadSymFinish(ur2::Context& ctx);
	const float* QuerySymbol(sx::UID sym_id, ur2::TexturePtr& texture, int& block_id) const;

	// cache glyph
	void LoadGlyph(ur2::Context& ctx, uint32_t* bitmap, int width, int height, uint64_t key);
	bool QueryGlyph(uint64_t key, float* texcoords, ur2::TexturePtr& texture) const;
	bool ExistGlyph(uint64_t key) const;
	void GetGlyphTexInfo(int& id, size_t& w, size_t& h) const;

	void Clear();

	bool Flush(ur2::Context& ctx);

	void DebugDraw(ur2::Context& ctx) const;

private:
    std::unique_ptr<dtex::TextureBuffer> m_texture_buffer = nullptr;
	bool m_c2_enable;

    std::unique_ptr<dtex::PixelBuffer> m_glyph_buffer = nullptr;
    std::unique_ptr<dtex::TexRenderer> m_tex_renderer = nullptr;

    std::shared_ptr<ur2::ShaderProgram> m_debug_shader = nullptr;
    std::shared_ptr<ur2::VertexArray>   m_debug_va = nullptr;

	CU_SINGLETON_DECLARATION(DTex)

}; // DTex

}