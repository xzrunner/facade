#include "facade/LoadingList.h"
#include "facade/DTex.h"

namespace
{

const int SYM_EXTRUDE = 1;

}

namespace facade
{

CU_SINGLETON_DEFINITION(LoadingList)

LoadingList::LoadingList()
{
}

void LoadingList::AddGlyph(sx::UID uid, int unicode, float line_x, const gtxt_glyph_style& gs)
{
	auto itr = m_glyphs.find(uid);
	if (itr == m_glyphs.end()) {
		m_glyphs.insert({ uid,{ unicode, line_x, gs } });
	}
}

void LoadingList::AddSymbol(sx::UID uid, int tex_id, int tex_w, int tex_h, const sm::irect& region)
{
	auto itr = m_symbols.find(uid);
	if (itr == m_symbols.end()) {
		m_symbols.insert({ uid,{ tex_id, tex_w, tex_h, region } });
	}
}

void LoadingList::Flush()
{
	FlushGlyphs();
	FlushSymbols();
}

void LoadingList::FlushGlyphs()
{
	if (m_glyphs.empty()) {
		return;
	}

	auto dtex = DTex::Instance();
	for (auto& itr : m_glyphs)
	{
		auto& g = itr.second;

		struct gtxt_glyph_layout layout;
		uint32_t* bmp = gtxt_glyph_get_bitmap(g.unicode, g.line_x, &g.gs, &layout);
		if (!bmp) {
			continue;
		}
		int w = static_cast<int>(layout.sizer.width);
		int h = static_cast<int>(layout.sizer.height);
		dtex->LoadGlyph(bmp, w, h, itr.first);
	}
	m_glyphs.clear();
}

void LoadingList::FlushSymbols()
{
	if (m_symbols.empty()) {
		return;
	}

	auto dtex = DTex::Instance();
	dtex->LoadSymStart();
	for (auto& itr : m_symbols) {
		auto& s = itr.second;
		dtex->LoadSymbol(itr.first, s.tex_id, s.tex_w, s.tex_h, s.region, 0, SYM_EXTRUDE);
	}
	dtex->LoadSymFinish();
	m_symbols.clear();
}

}