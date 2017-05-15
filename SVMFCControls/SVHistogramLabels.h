//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHistogramLabels
//* .File Name       : $Workfile:   SVHistogramLabels.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:48:24  $
//******************************************************************************
#pragma once

namespace SvMc
{
	namespace color
	{
		const COLORREF
			Red = RGB(255, 0, 0),
			Green = RGB(0, 128, 0),
			Blue = RGB(0, 0, 255),
			Black = RGB(0, 0, 0),
			Aqua = RGB(0, 255, 255),
			Aquamarine = RGB(127, 255, 212),
			Azure = RGB(240, 255, 255),
			Beige = RGB(245, 245, 220),
			Bisque = RGB(255, 228, 196),
			Brown = RGB(165, 42, 42),
			Charcoal = RGB(64, 64, 64),
			Chartreuse = RGB(127, 255, 0),
			Chocolate = RGB(210, 105, 30),
			Coral = RGB(255, 127, 80),
			Cornsilk = RGB(255, 248, 220),
			Crimson = RGB(237, 24, 61),
			Gray = RGB(128, 128, 128),
			Fuchsia = RGB(255, 0, 255),
			Gainsboro = RGB(220, 220, 220),
			Gold = RGB(255, 215, 0),
			Silver = RGB(192, 192, 192),
			White = RGB(255, 255, 255),
			Yellow = RGB(255, 255, 0),
			Magenta = RGB(255, 0, 255),
			Indigo = RGB(75, 0, 130),
			Ivory = RGB(255, 255, 240),
			Khaki = RGB(240, 230, 140),
			Lavender = RGB(230, 230, 250),
			Lime = RGB(0, 255, 0),
			Linen = RGB(250, 240, 230),
			Maroon = RGB(128, 0, 0),
			Moccasin = RGB(255, 228, 181),
			Navy = RGB(0, 0, 128),
			Olive = RGB(128, 128, 0),
			Orchid = RGB(218, 112, 214),
			Orange = RGB(255, 165, 0),
			Peru = RGB(205, 133, 63),
			Pink = RGB(255, 192, 203),
			Plum = RGB(221, 160, 221),
			Purple = RGB(128, 0, 128),
			Salmon = RGB(250, 128, 114),
			Sienna = RGB(160, 82, 45),
			Slate = RGB(106, 90, 205),
			Tan = RGB(210, 180, 140),
			Teal = RGB(0, 128, 128),
			Thistle = RGB(216, 191, 216),
			Tomato = RGB(255, 99, 71),
			Turquoise = RGB(64, 224, 208),
			Violet = RGB(238, 130, 238),
			Wheat = RGB(245, 222, 179),
			Cyan = RGB(0, 255, 255);
	}

	namespace label
	{
		enum position
		{
			left = 0x0001,
			right = 0x0002,
			top = 0x0004,
			bottom = 0x0008,
			leftTop = 0x0010,
			topLeft = 0x0020,
			topRight = 0x0040,
			rightTop = 0x0080,
			leftBottom = 0x0100,
			bottomLeft = 0x0200,
			bottomRight = 0x0400,
			rightBottom = 0x0800,
			leftCenter = left,
			leftUpper = 0x1000,
			leftLower = 0x2000,
			bottomMidLeft = 0x4000,
			bottomMidRight = 0x8000,
			all = 0xFFFF
		};

		template<typename T>
		LPCTSTR format();

		template<>
		__forceinline LPCTSTR format<int>()
		{
			return _T("%d");
		}

		template<>
		__forceinline LPCTSTR format<double>()
		{
			return _T("%5.2f%%");
		}

		struct Sentinel
		{
			typedef Sentinel Type;
			enum { index = -1, flag = 0 };
		};
	}

	template<label::position Anchor>
	struct TheAnchor
	{
		typedef histogram::Func Func;
		void AnchorUpdate(const CRect & rect, const Func & func);
	protected:
		CPoint  m_anchor;
	};

	template<>
	inline void TheAnchor<label::top>::AnchorUpdate(const CRect & rect, const Func & func)
	{
		m_anchor.x = rect.CenterPoint().x;
		m_anchor.y = rect.top - 2;
	}

	template<>
	inline void TheAnchor<label::leftTop>::AnchorUpdate(const CRect & rect, const Func & func)
	{
		m_anchor.x = rect.left - 2;
		m_anchor.y = rect.top;
	}

	template<>
	inline void TheAnchor<label::leftUpper>::AnchorUpdate(const CRect & rect, const Func & func)
	{
		m_anchor.x = rect.left - 2;
		m_anchor.y = rect.bottom - static_cast<int>(rect.Height()*func(75.0) / func(100.0));
	}

	template<>
	inline void TheAnchor<label::leftCenter>::AnchorUpdate(const CRect & rect, const Func & func)
	{
		m_anchor.x = rect.left - 2;
		m_anchor.y = rect.bottom - static_cast<int>(rect.Height()*func(50.0) / func(100.0));
	}

	template<>
	inline void TheAnchor<label::leftLower>::AnchorUpdate(const CRect & rect, const Func & func)
	{
		m_anchor.x = rect.left - 2;
		m_anchor.y = rect.bottom - static_cast<int>(rect.Height()*func(25.0) / func(100.0));
	}

	template<>
	inline void TheAnchor<label::leftBottom>::AnchorUpdate(const CRect & rect, const Func & func)
	{
		m_anchor.x = rect.left - 2;
		m_anchor.y = rect.bottom;
	}

	template<>
	inline void TheAnchor<label::bottomLeft>::AnchorUpdate(const CRect & rect, const Func & func)
	{
		m_anchor.x = rect.left;
		m_anchor.y = rect.bottom + 2;
	}

	template<>
	inline void TheAnchor<label::bottom>::AnchorUpdate(const CRect & rect, const Func & func)
	{
		m_anchor.x = rect.CenterPoint().x;
		m_anchor.y = rect.bottom + 2;
	}

	template<>
	inline void TheAnchor<label::bottomRight>::AnchorUpdate(const CRect & rect, const Func & func)
	{
		m_anchor.x = rect.right;
		m_anchor.y = rect.bottom + 2;
	}

	template<label::position Anchor>
	struct TheRect
	{
		void RectUpdate(const CPoint & pt, const CSize & sz);
		const CRect & GetRect() const
		{
			return m_rect;
		}
	protected:
		CRect  m_rect;
	};

	template<>
	inline void TheRect<label::top>::RectUpdate(const CPoint & pt, const CSize & sz)
	{
		m_rect = CRect(CPoint(pt.x - (sz.cx >> 1), pt.y - sz.cy), sz);
	}

	template<>
	inline void TheRect<label::leftTop>::RectUpdate(const CPoint & pt, const CSize & sz)
	{
		m_rect = CRect(CPoint(pt.x - sz.cx, pt.y), sz);
	}

	template<>
	inline void TheRect<label::leftUpper>::RectUpdate(const CPoint & pt, const CSize & sz)
	{
		m_rect = CRect(CPoint(pt.x - sz.cx, pt.y - (sz.cy >> 1)), sz);
	}

	template<>
	inline void TheRect<label::leftCenter>::RectUpdate(const CPoint & pt, const CSize & sz)
	{
		m_rect = CRect(CPoint(pt.x - sz.cx, pt.y - (sz.cy >> 1)), sz);
	}

	template<>
	inline void TheRect<label::leftLower>::RectUpdate(const CPoint & pt, const CSize & sz)
	{
		m_rect = CRect(CPoint(pt.x - sz.cx, pt.y - (sz.cy >> 1)), sz);
	}

	template<>
	inline void TheRect<label::leftBottom>::RectUpdate(const CPoint & pt, const CSize & sz)
	{
		m_rect = CRect(pt - sz, sz);
	}

	template<>
	inline void TheRect<label::bottomLeft>::RectUpdate(const CPoint & pt, const CSize & sz)
	{
		m_rect = CRect(pt, sz);
	}

	template<>
	inline void TheRect<label::bottom>::RectUpdate(const CPoint & pt, const CSize & sz)
	{
		m_rect = CRect(CPoint(pt.x - (sz.cx >> 1), pt.y), sz);
	}

	template<>
	inline void TheRect<label::bottomRight>::RectUpdate(const CPoint & pt, const CSize & sz)
	{
		m_rect = CRect(CPoint(pt.x - sz.cx, pt.y), sz);
	}

	template<int Mode>
	struct DrawMode
	{
		void Erase(CPaintDC & dc, const CString & old, const CRect & rect) const
		{
			if (old.GetLength())
			{
				int l_mode = Mode;
				COLORREF tmp_color = GetSysColor(COLOR_BTNFACE);
				tmp_color = dc.SetTextColor(tmp_color);
				l_mode = dc.SetBkMode(l_mode);
				dc.ExtTextOut(rect.left, rect.top, 0, nullptr, old, nullptr);
				dc.SetBkMode(l_mode);
				tmp_color = dc.SetTextColor(tmp_color);
			}
		}
	};

	template<>
	inline void DrawMode<OPAQUE>::Erase(CPaintDC &, const CString &, const CRect &) const
	{
	}

	template<label::position Anchor, int Mode = TRANSPARENT>
	class Label :
		public DrawMode<Mode>,
		public TheAnchor<Anchor>,
		public TheRect<Anchor>
	{
	public:
		enum { pos = Anchor, mode = Mode };

		void UpdateRect(const CSize & sz)
		{
			RectUpdate(m_anchor, sz);
		}

		void UpdateAnchor(const CRect & rect, const Func & func)
		{
			AnchorUpdate(rect, func);
			m_changed = true;
		}

		void Draw(CPaintDC & dc, CWnd * wnd)
		{
			if (m_changed)
			{
				Erase(dc, m_oldtxt, m_rect);
			}
			if (m_text.GetLength())
			{
				dc.SetBkMode(mode);
				UpdateRect(dc.GetOutputTextExtent(m_text));
				dc.ExtTextOut(m_rect.left, m_rect.top, 0, nullptr, m_text, nullptr);
				m_oldtxt = m_text;
				m_changed = false;
			}
		}

		template<typename T>
		__forceinline void SetText(const T & t)
		{
			m_text.Format(label::format<T>(), t);
			m_changed = true;
		}
		__forceinline void SetText(const CString & str)
		{
			m_text = str;
			m_changed = true;
		}
		Label()
		{
			m_text = "";
			m_changed = false;
			m_anchor = CPoint(0, 0);
			m_rect = CRect(0, 0, 0, 0);
		}

	private:
		CString m_text;
		CString m_oldtxt;
		bool	  m_changed;
	};

	template<typename LabelType, typename Tail>
	struct Labels : public LabelType, public Tail
	{
		typedef LabelType LabelBase;
		enum { index = Tail::index + 1, flag = LabelType::pos, mode = LabelType::mode };

		void DrawLabels(CPaintDC & dc, CWnd * wnd)
		{
			int tmp_mode = dc.SetBkMode(mode);
			static_cast<LabelBase &>(*this).Draw(dc, wnd);
			static_cast<Tail &>(*this).DrawLabels(dc, wnd);
			dc.SetBkMode(tmp_mode);
		}

		template<typename T>
		__forceinline void SetLabelText(const T & i, int flags)
		{
			if (flags)
			{
				if (flags & flag)
					static_cast<LabelBase &>(*this).SetText(i);
				static_cast<Tail &>(*this).SetLabelText(i, flags & ~flag);
			}
		}

		__forceinline void SetLabelText(const CString & str, int flags)
		{
			if (flags)
			{
				if (flags & flag)
					static_cast<LabelBase &>(*this).SetText(str);
				static_cast<Tail &>(*this).SetLabelText(str, flags & ~flag);
			}

		}

		void UpdateAnchors(const CRect & rect, const histogram::Func & func)
		{
			static_cast<LabelBase &>(*this).UpdateAnchor(rect, func);
			static_cast<Tail &>(*this).UpdateAnchors(rect, func);
		}

		const CRect & GetLabelRect(int flags) const
		{
			if (flags & flag)
				return static_cast<const LabelBase &>(*this).GetRect();
			else
				return static_cast<const Tail &>(*this).GetLabelRect(flags);
		}

	};

	template<typename LabelType>
	struct Labels<LabelType, label::Sentinel> : public LabelType
	{
		typedef LabelType LabelBase;
		enum { index = 0, flag = LabelType::pos, mode = LabelType::mode };

		void DrawLabels(CPaintDC &dc, CWnd * wnd)
		{
			int tmp_mode = dc.SetBkMode(mode);
			static_cast<LabelBase &>(*this).Draw(dc, wnd);
			dc.SetBkMode(tmp_mode);
		}

		template<typename T>
		__forceinline void SetLabelText(const T & i, int flags)
		{
			if (flags & flag)
				static_cast<LabelBase &>(*this).SetText(i);
		}

		__forceinline void SetLabelText(const CString & str, int flags)
		{
			if (flags & flag)
				static_cast<LabelBase &>(*this).SetText(str);
		}

		void SetLabelDirty(bool state, int flags)
		{
			if (flags & flag)
				static_cast<LabelBase &>(*this).SetDirty(state);
		}

		void UpdateAnchors(const CRect & rect, const histogram::Func & func)
		{
			static_cast<LabelBase &>(*this).UpdateAnchor(rect, func);
		}

		const CRect & GetLabelRect(int) const
		{
			return static_cast<const LabelBase &>(*this).GetRect();
		}
	};

	typedef
		Labels <
		Label<label::leftTop>, // index 7
		Labels <
		Label<label::leftUpper>, // index 6
		Labels<
		Label<label::leftCenter>, // index 5
		Labels<
		Label<label::leftLower>, // index 4
		Labels<
		Label<label::leftBottom>, // index 3
		Labels<
		Label<label::bottomLeft>, // index 2
		Labels<
		Label<label::bottom>, // index 1
		Labels<
		Label<label::bottomRight>, // index 0
		label::Sentinel
		>
		>
		>
		>
		>
		>
		>
		> HistogramLabels;
} //namespace SvMc
