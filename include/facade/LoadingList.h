#pragma once

#include <cu/cu_macro.h>
#include <gtxt_glyph.h>
#include <SM_Rect.h>
#include <sx/ResourceUID.h>

#include <boost/noncopyable.hpp>

#include <map>

namespace facade
{

class LoadingList : boost::noncopyable
{
public:
	void AddGlyph(sx::UID uid, int unicode, float line_x, const gtxt_glyph_style& gs);
	void AddSymbol(sx::UID uid, int tex_id, int tex_w, int tex_h, const sm::irect& region);

	bool Flush();

private:
	bool FlushGlyphs();
	bool FlushSymbols();

private:
	struct Glyph
	{
		int unicode;
		float line_x;
		gtxt_glyph_style gs;
	};

	struct Symbol
	{
		int tex_id;
		int tex_w, tex_h;
		sm::irect region;
	};

private:
	std::map<sx::UID, Glyph> m_glyphs;
	std::map<sx::UID, Symbol> m_symbols;

	CU_SINGLETON_DECLARATION(LoadingList)

}; // LoadingList

}