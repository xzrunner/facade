#pragma once

#include <cu/cu_macro.h>
#include <gtxt_glyph.h>
#include <SM_Rect.h>
#include <sx/ResourceUID.h>
#include <unirender/typedef.h>

#include <boost/noncopyable.hpp>

#include <map>

namespace ur { class Context; }

namespace facade
{

class LoadingList : boost::noncopyable
{
public:
	void AddGlyph(sx::UID uid, int unicode, float line_x, const gtxt_glyph_style& gs);
	void AddSymbol(sx::UID uid, const ur::TexturePtr& tex, const sm::irect& region);

	bool Flush(ur::Context& ctx);

private:
	bool FlushGlyphs(ur::Context& ctx);
	bool FlushSymbols(ur::Context& ctx);

private:
	struct Glyph
	{
		int unicode;
		float line_x;
		gtxt_glyph_style gs;
	};

	struct Symbol
	{
        ur::TexturePtr tex = nullptr;
		sm::irect region;
	};

private:
	std::map<sx::UID, Glyph> m_glyphs;
	std::map<sx::UID, Symbol> m_symbols;

	CU_SINGLETON_DECLARATION(LoadingList)

}; // LoadingList

}