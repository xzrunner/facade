#pragma once

#include <cu/cu_macro.h>
#include <SM_Rect.h>
#include <sx/ResourceUID.h>

namespace ur2 { class Device; class Context; }
namespace dtex { class CacheSymbol; class CacheGlyph; class Texture; struct Rect; }

namespace facade
{

class DTex
{
public:
    void Init(const ur2::Device& dev);
	void InitHook(void(*draw_begin)(), void(*draw_end)(), void(*error_reload)());

	// C2, cache GameObj
	void LoadSymStart();
	void LoadSymbol(sx::UID sym_id, int tex_id, int tex_w, int tex_h, const sm::irect& region,
		int padding = 0, int extrude = 0, int src_extrude = 0);
	void LoadSymFinish();
	const float* QuerySymbol(sx::UID sym_id, int& tex_id, int& block_id) const;
	void ClearSymbolCache();
	int GetSymCacheTexID() const;

	// CG, cache glyph
	void DrawGlyph(int tex_id, int tex_w, int tex_h, const dtex::Rect& r, uint64_t key);
	void LoadGlyph(ur2::Context& ctx, uint32_t* bitmap, int width, int height, uint64_t key);
	bool QueryGlyph(uint64_t key, float* texcoords, int& tex_id) const;
	bool ExistGlyph(uint64_t key) const;
	void GetGlyphTexInfo(int& id, size_t& w, size_t& h) const;
	bool QueryGlyphRegion(uint64_t key, int& tex_id, int& xmin, int& ymin, int& xmax, int& ymax) const;

	void Clear();

	bool Flush(ur2::Context& ctx, bool cg_to_c2);

	void DebugDraw() const;

private:
	dtex::CacheSymbol* m_c2;
	bool m_c2_enable;

	dtex::CacheGlyph* m_cg;

	CU_SINGLETON_DECLARATION(DTex)

}; // DTex

}