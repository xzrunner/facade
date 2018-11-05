#include "facade/DTex.h"
#include "facade/RenderContext.h"

#include <dtex2/RenderAPI.h>
#include <dtex2/ResourceAPI.h>
#include <dtex2/CacheAPI.h>
#include <dtex2/CacheSymbol.h>
#include <dtex2/CacheGlyph.h>
#include <dtex2/CacheMgr.h>
#include <unirender/RenderContext.h>
#include <unirender/Blackboard.h>
#include <rendergraph/RenderMgr.h>
#include <rendergraph/SpriteRenderer.h>
#include <painting2/Blackboard.h>
#include <painting2/WindowContext.h>
#include <painting2/RenderContext.h>
#include <painting2/Shader.h>
#include <tessellation/Painter.h>
#include <stat/StatImages.h>

#include <stack>

namespace
{

const int IMG_ID = -3;

void (*DRAW_BEGIN)()   = nullptr;
void (*DRAW_END)()     = nullptr;
void (*ERROR_RELOAD)() = nullptr;

/************************************************************************/
/* draw                                                                 */
/************************************************************************/

static void
clear_color_part(float xmin, float ymin, float xmax, float ymax)
{
	auto& ur_rc = ur::Blackboard::Instance()->GetRenderContext();
	ur_rc.EnableBlend(false);
//	glBlendFunc(GL_ONE, GL_ZERO);

	auto& wc = pt2::Blackboard::Instance()->GetWindowContext();
	int w = wc->GetScreenWidth(),
		h = wc->GetScreenHeight();
	xmin = w * 0.5f * (xmin - 1);
	xmax = w * 0.5f * (xmax - 1);
	ymin = h * 0.5f * (ymin + 1);
	ymax = h * 0.5f * (ymax + 1);

	CU_VEC<sm::vec2> triangles;
	triangles.resize(4);
	triangles[0].Set(xmin, ymin);
	triangles[1].Set(xmin, ymax);
	triangles[2].Set(xmax, ymin);
	triangles[3].Set(xmax, ymax);

	tess::Painter pt;
	pt.AddRectFilled(sm::vec2(xmin, xmax), sm::vec2(xmax, ymax), 0, 0);

	auto sr = rg::RenderMgr::Instance()->SetRenderer(rg::RenderType::SPRITE);
	std::static_pointer_cast<rg::SpriteRenderer>(sr)->DrawPainter(pt);

//	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	ur_rc.EnableBlend(true);

	//////////////////////////////////////////////////////////////////////////

//	dtex_gl_clear_color(0, 0, 0, 0);
}

static void
set_program()
{
	// todo
	rg::RenderMgr::Instance()->SetRenderer(rg::RenderType::SPRITE);
}

static void
enable_blend(bool enable)
{
	auto& rc = ur::Blackboard::Instance()->GetRenderContext();
	if (enable) {
		rc.EnableBlend(true);
		rc.SetBlend(ur::BLEND_SRC_ALPHA, ur::BLEND_ONE_MINUS_SRC_ALPHA, false);
	} else {
	//	rc.SetBlend(ur::BLEND_ONE, ur::BLEND_ONE_MINUS_SRC_ALPHA, false);
		rc.EnableBlend(false);
	}
}

std::stack<std::shared_ptr<pt2::WindowContext>> wc_stack;

static void
draw_begin()
{
	rg::RenderMgr::Instance()->Flush();

	if (DRAW_BEGIN)
	{
		DRAW_BEGIN();
	}
	else
	{
		wc_stack.push(pt2::Blackboard::Instance()->GetWindowContext());
		auto new_wc = std::make_shared<pt2::WindowContext>(2.0f, 2.0f, 0, 0);
		new_wc->Bind();
		wc_stack.push(new_wc);

		// fixme:
		// curr shader not connect to the new wnd_ctx
		// should update its matrix manually
		auto sr = rg::RenderMgr::Instance()->SetRenderer(rg::RenderType::SPRITE);
		auto shader = std::static_pointer_cast<rg::SpriteRenderer>(sr)->GetShader();
		std::static_pointer_cast<pt2::Shader>(shader)->UpdateProjMat(2, 2);
		std::static_pointer_cast<pt2::Shader>(shader)->UpdateViewMat(sm::vec2(0, 0), 1);
		//shader->SetMat4("u_model", sm::mat4().x);
	}
}

static void
draw(const float _vertices[8], const float _texcoords[8], int texid)
{
	sm::vec2 vertices[4], texcoords[4];
	for (int i = 0; i < 4; ++i) {
		vertices[i].x  = _vertices[i * 2];
		vertices[i].y  = _vertices[i * 2 + 1];
		texcoords[i].x = _texcoords[i * 2];
		texcoords[i].y = _texcoords[i * 2 + 1];
	}

	auto sr = rg::RenderMgr::Instance()->SetRenderer(rg::RenderType::SPRITE);
	std::static_pointer_cast<rg::SpriteRenderer>(sr)->DrawQuad(&vertices[0].x, &texcoords[0].x, texid, 0xffffffff);
}

static void
draw_end()
{
	rg::RenderMgr::Instance()->Flush();

	if (DRAW_END) {
		DRAW_END();
	} else {
		wc_stack.pop();
		wc_stack.top()->Bind();
	}
}

static void
draw_flush()
{
	rg::RenderMgr::Instance()->Flush();
}

static void
scissor_push(int x, int y, int w, int h)
{
	auto& scissor = pt2::Blackboard::Instance()->GetRenderContext().GetScissor();
	scissor.Push(static_cast<float>(x), static_cast<float>(y), static_cast<float>(w), static_cast<float>(h), false, true);
}

static void
scissor_pop()
{
	auto& scissor = pt2::Blackboard::Instance()->GetRenderContext().GetScissor();
	scissor.Pop();
}

static void
scissor_disable()
{
	auto& scissor = pt2::Blackboard::Instance()->GetRenderContext().GetScissor();
	scissor.Disable();
}

static void
scissor_enable()
{
	auto& scissor = pt2::Blackboard::Instance()->GetRenderContext().GetScissor();
	scissor.Enable();
}

/************************************************************************/
/* Resource                                                             */
/************************************************************************/

//static void
//error_reload()
//{
//	if (ERROR_RELOAD) {
//		ERROR_RELOAD();
//	}
//}
//
//static void
//get_tex_filepath(int pkg_id, int tex_idx, int lod_layer, char* buf)
//{
//	const timp::Package* pkg = timp::PkgMgr::Instance()->Query(pkg_id);
//	assert(pkg);
//	const bimp::FilePath& res_path = pkg->GetTexPath(tex_idx, lod_layer);
//	res_path.Serialize(buf);
//}
//
//class FileLoader : public bimp::FileLoader
//{
//public:
//	FileLoader(const CU_STR& filepath, bool use_cache, void (*parser_cb)(const void* data, size_t size, void* ud), void* ud)
//		: bimp::FileLoader(filepath, use_cache)
//		, m_parser_cb(parser_cb)
//		, m_ud(ud)
//	{}
//	FileLoader(fs_file* file, uint32_t offset, bool use_cache, void (*parser_cb)(const void* data, size_t size, void* ud), void* ud)
//		: bimp::FileLoader(file, offset, use_cache)
//		, m_parser_cb(parser_cb)
//		, m_ud(ud)
//	{}
//
//protected:
//	virtual void OnLoad(bimp::ImportStream& is) override
//	{
//		m_parser_cb(is.Stream(), is.Size(), m_ud);
//	}
//
//private:
//	void (*m_parser_cb)(const void* data, size_t size, void* ud);
//	void* m_ud;
//
//}; // FileLoader
//
//static void
//load_file(const void* res_path, bool use_cache, void (*parser_cb)(const void* data, size_t size, void* ud), void* ud)
//{
//	bimp::FilePath file_path;
//	file_path.Deserialization(static_cast<const char*>(res_path));
//	if (file_path.IsSingleFile()) {
//		FileLoader loader(file_path.GetFilepath().c_str(), use_cache, parser_cb, ud);
//		loader.Load();
//	} else {
//		fs_file* file = fs_open(file_path.GetFilepath().c_str(), "rb");
//		FileLoader loader(file, file_path.GetOffset(), use_cache, parser_cb, ud);
//		loader.Load();
//		fs_close(file);
//	}
//}
//
//static void
//load_texture(int pkg_id, int tex_idx, int lod)
//{
//	const timp::Package* t_pkg = timp::PkgMgr::Instance()->Query(pkg_id);
//	assert(t_pkg);
//	const bimp::FilePath& filepath = t_pkg->GetTexPath(tex_idx, lod);
//
//	s2loader::ImageLoader loader(filepath);
//	bool ret = loader.Load();
//	if (!ret) {
//		return;
//	}
//
//	const dtex::Package* d_pkg = dtex::PkgMgr::Instance()->Query(pkg_id);
//	dtex::Texture* tex = d_pkg->GetTexture(tex_idx);
//	assert(tex->Type() == dtex::TEX_RAW);
//	dtex::TextureRaw* raw_tex = static_cast<dtex::TextureRaw*>(tex);
//	raw_tex->SetID(loader.GetID());
//	raw_tex->SetSize(loader.GetWidth(), loader.GetHeight());
//	raw_tex->SetFormat(loader.GetFormat());
//
//	s2::StatImages::Instance()->Add(pkg_id, loader.GetWidth(), loader.GetHeight(), loader.GetFormat());
//}
//
//static void
//load_texture_cb(int pkg_id, int tex_idx, void (*cb)(int format, int w, int h, const void* data, void* ud), void* ud)
//{
//	const timp::Package* t_pkg = timp::PkgMgr::Instance()->Query(pkg_id);
//	assert(t_pkg);
//	const bimp::FilePath& filepath = t_pkg->GetTexPath(tex_idx, 0);
//	if (filepath.IsSingleFile())
//	{
//		timp::TextureLoader loader(filepath.GetFilepath().c_str());
//		loader.Load();
//		s2::StatImages::Instance()->Add(pkg_id, loader.GetWidth(), loader.GetHeight(), loader.GetFormat());
//		cb(loader.GetFormat(), loader.GetWidth(), loader.GetHeight(), loader.GetData(), ud);
//	}
//	else
//	{
//		fs_file* file = fs_open(filepath.GetFilepath().c_str(), "rb");
//		timp::TextureLoader loader(file, filepath.GetOffset());
//		loader.Load();
//		fs_close(file);
//		s2::StatImages::Instance()->Add(pkg_id, loader.GetWidth(), loader.GetHeight(), loader.GetFormat());
//		cb(loader.GetFormat(), loader.GetWidth(), loader.GetHeight(), loader.GetData(), ud);
//	}
//}
//
//static void
//load_texture_cb2(const void* data, size_t size, void (*cb)(int format, int w, int h, const void* data, void* ud), void* ud)
//{
//	timp::TextureLoader loader(static_cast<const char*>(data), size);
//	loader.Load();
//
//	cb(loader.GetFormat(), loader.GetWidth(), loader.GetHeight(), loader.GetData(), ud);
//}
//
//static void
//cache_pkg_static_tex_ok()
//{
//	DTex::Instance()->SetC2Enable(true);
//}
//
//static void
//submit_task(mt::Task* task)
//{
//	ThreadPool::Instance()->Run(task);
//}

static void
stat_tex_add(int width, int height, int format)
{
	st::StatImages::Instance()->Add(IMG_ID, width, height, format);
}

static void
stat_tex_remove(int width, int height, int format)
{
	st::StatImages::Instance()->Remove(IMG_ID, width, height, format);
}

/************************************************************************/
/* Cache                                                                */
/************************************************************************/

//static std::map<bimp::FilePath, std::shared_ptr<Image>> DTEX_CACHED_IMAGES;
//
//static void
//relocate_pkg(int src_pkg, int src_tex, int src_lod, int dst_tex_id, int dst_fmt, int dst_w, int dst_h, int dst_xmin, int dst_ymin, int dst_xmax, int dst_ymax)
//{
//	simp::RelocateTexcoords::Item item;
//	item.src_pkg = src_pkg;
//	item.src_tex = src_tex;
//	item.src_lod = src_lod;
//	item.dst_tex_id = dst_tex_id;
//	item.dst_w = dst_w;
//	item.dst_h = dst_h;
//	item.dst_xmin = dst_xmin;
//	item.dst_ymin = dst_ymin;
//	item.dst_xmax = dst_xmax;
//	item.dst_ymax = dst_ymax;
//	simp::RelocateTexcoords::Instance()->Add(item);
//
//	bimp::FilePath res_path(ProxyImage::GetFilepath(dst_tex_id));
//	auto img = ImagePool::Instance()->Query(res_path);
//	if (img) {
//		auto p_img = std::static_pointer_cast<ProxyImage>(img);
//		p_img->Init(dst_tex_id, dst_w, dst_h, dst_fmt);
//	} else {
//		auto img = std::make_shared<ProxyImage>(dst_tex_id, dst_w, dst_h, dst_fmt);
//		DTEX_CACHED_IMAGES.insert(std::make_pair(res_path, img));
//		bool insert = ImagePool::Instance()->Add(res_path, img);
//		assert(insert);
//	}
//}
//
//class RelocatePageVisitor : public simp::PageVisitor
//{
//public:
//	virtual void Visit(const simp::Page* page) override
//	{
//		simp::RelocateTexcoords::Instance()->Do(page);
//	}
//
//}; // RelocatePageVisitor
//
//static void
//relocate_pkg_finish()
//{
//	RelocatePageVisitor visitor;
//	simp::NodeFactory::Instance()->Traverse(visitor);
//}
//
//static void
//remove_tex(int tex_id)
//{
//	bimp::FilePath res_path(ProxyImage::GetFilepath(tex_id));
//	DTEX_CACHED_IMAGES.erase(res_path);
//	ImagePool::Instance()->Delete(res_path);
//}
//
//static void
//on_clear_sym_block(int block_id)
//{
//	SymbolPool::Instance()->Traverse(
//		[=](const s2::SymPtr& sym)->bool
//		{
//			if (sym->Type() == s2::SYM_IMAGE) {
//				S2_VI_PTR_DOWN_CAST<ImageSymbol>(sym)->SetCacheDirty(block_id);
//			}
//			return true;
//		}
//	);
//}

/************************************************************************/
/* Glyph                                                                */
/************************************************************************/

static void
glyph_load_start()
{
	facade::DTex::Instance()->LoadSymStart();
}

static void
glyph_load(int tex_id, int tex_w, int tex_h, const dtex::Rect& r, uint64_t key)
{
	facade::DTex::Instance()->DrawGlyph(tex_id, tex_w, tex_h, r, key);
}

static void
glyph_load_finish()
{
	facade::DTex::Instance()->LoadSymFinish();
}

}

namespace facade
{

CU_SINGLETON_DEFINITION(DTex);

void DTex::InitHook(void(*draw_begin)(), void(*draw_end)(), void(*error_reload)())
{
	DRAW_BEGIN = draw_begin;
	DRAW_END = draw_end;
	ERROR_RELOAD = error_reload;
}

DTex::DTex()
{
	dtex::RenderAPI::Callback render_cb;
	render_cb.clear_color_part = clear_color_part;
	render_cb.set_program      = set_program;
	render_cb.enable_blend     = enable_blend;
	render_cb.draw_begin       = draw_begin;
	render_cb.draw             = draw;
	render_cb.draw_end         = draw_end;
	render_cb.scissor_push     = scissor_push;
	render_cb.scissor_pop      = scissor_pop;
	render_cb.scissor_disable  = scissor_disable;
	render_cb.scissor_enable   = scissor_enable;

	dtex::RenderAPI::InitCallback(render_cb);
	auto& ur_rc = ur::Blackboard::Instance()->GetRenderContext();
	dtex::RenderAPI::InitRenderContext(&ur_rc);

	dtex::ResourceAPI::Callback res_cb;
	//res_cb.error_reload            = error_reload;
	//res_cb.get_tex_filepath        = get_tex_filepath;
	//res_cb.load_file               = load_file;
	//res_cb.load_texture            = load_texture;
	//res_cb.load_texture_cb         = load_texture_cb;
	//res_cb.load_texture_cb2        = load_texture_cb2;
	//res_cb.cache_pkg_static_tex_ok = cache_pkg_static_tex_ok;
	//res_cb.submit_task             = submit_task;
	res_cb.stat_tex_add            = stat_tex_add;
	res_cb.stat_tex_remove         = stat_tex_remove;
	dtex::ResourceAPI::InitCallback(res_cb);

	//dtex::CacheAPI::Callback cache_cb;
	//cache_cb.relocate_pkg        = relocate_pkg;
	//cache_cb.relocate_pkg_finish = relocate_pkg_finish;
	//cache_cb.remove_tex          = remove_tex;
	//cache_cb.on_clear_sym_block  = on_clear_sym_block;
	//dtex::CacheAPI::InitCallback(cache_cb);

	m_c2 = new dtex::CacheSymbol(2048, 2048);
	dtex::CacheMgr::Instance()->Add(m_c2, "C2");
	m_c2_enable = true;

	dtex::CacheGlyph::Callback glyph_cb;
	glyph_cb.load_start  = glyph_load_start;
	glyph_cb.load        = glyph_load;
	glyph_cb.load_finish = glyph_load_finish;
	m_cg = new dtex::CacheGlyph(1024, 512, glyph_cb);
	dtex::CacheMgr::Instance()->Add(m_cg, "CG");
}

void DTex::LoadSymStart()
{
	m_c2->LoadStart();
}

void DTex::LoadSymbol(sx::UID sym_id, int tex_id, int tex_w, int tex_h, const sm::irect& region,
	int padding, int extrude, int src_extrude)
{
	dtex::Rect r;
	r.xmin = region.xmin;
	r.ymin = region.ymin;
	r.xmax = region.xmax;
	r.ymax = region.ymax;
	m_c2->Load(tex_id, tex_w, tex_h, r, sym_id, padding, extrude, src_extrude);
}

void DTex::LoadSymFinish()
{
	m_c2->LoadFinish();
}

const float* DTex::QuerySymbol(sx::UID sym_id, int& tex_id, int& block_id) const
{
	if (!m_c2_enable) {
		return nullptr;
	}

	int b_id;
	const dtex::CS_Node* node = m_c2->Query(sym_id, b_id);
	if (node) {
		tex_id = m_c2->GetTexID();
		block_id = b_id;
		return node->GetTexcoords();
	}
	else {
		return nullptr;
	}
}

void DTex::ClearSymbolCache()
{
	if (m_c2) {
		m_c2->Clear();
	}
//	DTexC2Strategy::Instance()->Clear();
}

void DTex::DrawGlyph(int tex_id, int tex_w, int tex_h, const dtex::Rect& r, uint64_t key)
{
	m_c2->Load(tex_id, tex_w, tex_h, r, key, 1, 0);
}

void DTex::LoadGlyph(uint32_t* bitmap, int width, int height, uint64_t key)
{
	m_cg->Load(bitmap, width, height, key);
}

bool DTex::QueryGlyph(uint64_t key, float* texcoords, int& tex_id) const
{
	return m_cg->QueryAndInsert(key, texcoords, tex_id);
}

void DTex::Clear()
{
}

bool DTex::Flush(bool cg_to_c2)
{
	return m_cg->Flush(cg_to_c2);
}

void DTex::DebugDraw() const
{
	// 	const CU_MAP<CU_STR, dtex::Cache*>& caches
	// 		= dtex::CacheMgr::Instance()->FetchAll();
	// 	CU_MAP<CU_STR, dtex::Cache*>::const_iterator itr = caches.begin();
	// 	for ( ; itr != caches.end(); ++itr) {
	// 		itr->second->DebugDraw();
	// 	}

	//////////////////////////////////////////////////////////////////////////

	m_c2->DebugDraw();
//	m_cg->DebugDraw();
}

}