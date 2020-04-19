//#pragma once
//
//#include <boost/noncopyable.hpp>
//
//#include <guard/check.h>
//
//#include <memory>
//
//namespace ur { class RenderContext; }
//namespace pt2 { class RenderContext; }
//
//namespace facade
//{
//
//class RenderContext : boost::noncopyable
//{
//public:
//	RenderContext();
//
////	void OnSize(int w, int h);
//
//	ur::RenderContext& GetUrRc() const {
//		GD_ASSERT(m_ur_rc, "null ur rc");
//		return *m_ur_rc;
//	}
//	pt2::RenderContext& GetPt2Rc() const {
//		GD_ASSERT(m_pt2_rc, "null pt2 ctx");
//		return *m_pt2_rc;
//	}
//
//	void Bind();
//	void Unbind();
//
//private:
//	std::shared_ptr<ur::RenderContext>  m_ur_rc  = nullptr;
//	std::shared_ptr<pt2::RenderContext> m_pt2_rc = nullptr;
//
//}; // RenderContext
//
//}