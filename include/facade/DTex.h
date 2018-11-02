#pragma once

#include <cu/cu_macro.h>
#include <SM_Rect.h>
#include <sx/ResourceUID.h>
#include <dtex2/CacheGlyph.h>

namespace dtex { class CacheSymbol; class CacheGlyph; class Texture; struct Rect; }

namespace facade
{

class DTex
{
public:
	void InitHook(void(*draw_begin)(), void(*draw_end)(), void(*error_reload)());

	// C2, cache GameObj
	void LoadSymStart();
	void LoadSymbol(sx::UID sym_id, int tex_id, int tex_w, int tex_h, const sm::irect& region,
		int padding = 0, int extrude = 0, int src_extrude = 0);
	void LoadSymFinish();
	const float* QuerySymbol(sx::UID sym_id, int& tex_id, int& block_id) const;
	void ClearSymbolCache();

	// CG, cache glyph
	void DrawGlyph(int tex_id, int tex_w, int tex_h, const dtex::Rect& r, uint64_t key);
	void LoadGlyph(uint32_t* bitmap, int width, int height, uint64_t key);
	bool QueryGlyph(uint64_t key, float* texcoords, int& tex_id) const;
	bool ExistGlyph(uint64_t key) const { return m_cg->Exist(key); }
	void GetGlyphTexInfo(int& id, size_t& w, size_t& h) const { m_cg->GetFirstPageTexInfo(id, w, h); }
	bool QueryGlyphRegion(uint64_t key, int& tex_id, int& xmin, int& ymin, int& xmax, int& ymax) const {
		return m_cg->QueryRegion(key, tex_id, xmin, ymin, xmax, ymax);
	}

	void Clear();

	void Flush(bool cg_to_c2);

	void DebugDraw() const;

private:
	dtex::CacheSymbol* m_c2;
	bool m_c2_enable;

	dtex::CacheGlyph* m_cg;

	CU_SINGLETON_DECLARATION(DTex)

}; // DTex

}