//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVKnobControl
//* .File Name       : $Workfile:   SVKnobControl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:46:24  $
//******************************************************************************

#pragma  once

#define WM_TRIGGER_CHANGE WM_USER + 401

namespace SvMc
{
	namespace sv
	{
		struct hold
		{
			template<typename T>
			struct pair
			{
				T face;
				T mask;
			};
			pair<CBitmap> bitmap;
			pair<CBitmap *> save;
			pair<CDC> mdc;
			CRect rect;
			CPoint center;
			//CBrush brush;
			void LoadBitmaps(UINT face, UINT mask, CDC & dc)
			{
				bitmap.face.LoadBitmap(face);
				BITMAP bmp;
				bitmap.face.GetBitmap(&bmp);
				rect = CRect(0, 0, bmp.bmWidth + 1, bmp.bmHeight);
				center = rect.CenterPoint();
				bitmap.mask.LoadBitmap(mask);
				mdc.face.CreateCompatibleDC(&dc);
				mdc.mask.CreateCompatibleDC(nullptr);
				save.face = mdc.face.SelectObject(&(bitmap.face));
				save.mask = mdc.mask.SelectObject(&(bitmap.mask));
				mdc.face.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &(mdc.mask), 0, 0, SRCAND);
				mdc.mask.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), nullptr, 0, 0, DSTINVERT);

			}
			~hold()
			{
			}
		};
	}

	class SVKnobControl : public CStatic
	{
	public:
		SVKnobControl(long minTimerInterval_ms, long maxTimerInterval_ms);
		virtual ~SVKnobControl();

	protected:
		DECLARE_MESSAGE_MAP()
		BOOL RegisterWindowClass();
		virtual void DoDataExchange(CDataExchange* pDX) override;
	public:
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnPaint();
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		int click() { return ++clicks; }

		void CreateBitmaps();
		int GetValue() const { return int_value(m_currentValue); }
		int SetValue(int value);
	private:
		bool TransparentBlt(CDC & dest, sv::hold & src, CPoint pt);
		bool BitBlt(CDC & dest, CRect & rect, CDC & src, CPoint pt, DWORD rop);
		void DrawTicks(CDC & dc);
		void Tick(CDC & dc, double angle, bool bold);
		sv::hold knob;
		sv::hold dot;
		CBitmap m_screen;
		CBitmap m_image;
		CPoint m_dotPoint;
		CPoint m_center;
		CDC m_dc;
		int clicks;
		int m_radius;
		double pi() const { return 3.14159265358979323846; }
		double pihalf() const { return pi()/2.0; }
		double pionehalf() const { return pi() + pihalf(); }
		double pitwo() const { return 2.0 * pi(); }
		double pithird() const { return pi()/3.0; }
		double pisixth() const { return pi()/6.0; }
		double pifourth() const { return pi()/4.0; }
		double m_minValue;
		double m_maxValue;
		double m_currentValue;
		double m_angle;
		double span() const { return (m_maxValue - m_minValue)/(pitwo() - pihalf()); }
		bool m_dragging;
		int int_value(double value) const { return static_cast<int>(exp(value*value*value) + 0.5); }//{ return static_cast<int>(exp(angle2val(value))); }
		int dotSize() const { return dot.rect.Width() / 2; }

		int m_maxy;

		CPoint val2xy(double val) const { return angle2xy(m_center, val2angle(val), m_radius); }
		CPoint angle2xy(const CPoint & c, double a, int r) const { return CPoint(c.x + static_cast<int>(r*cos(a)), c.y + static_cast<int>(r*sin(a))); }
		double xy2angle(CPoint & pt) const { return atan2(static_cast<double>(pt.y), static_cast<double>(pt.x)) + pi(); }
		double xy2val(const CPoint & pt) const;

		double angle2val(double angle) const { return fmod(pitwo() + angle - (pihalf() + pifourth()), pitwo())*span() + m_minValue; }
		double val2angle(double value) const { return fmod((value- m_minValue)/span() + pihalf() + pifourth(), pitwo()); }
		double long2val(long i) const { return pow(log(static_cast<double>(i)), 1.0/3.0); }

		CRect dotRect() const { CRect rect = dot.rect; rect.MoveToXY(m_dotPoint.x, m_dotPoint.y); return rect; }
	};

	inline bool SVKnobControl::BitBlt(CDC & dest, CRect & rect, CDC & src, CPoint pt, DWORD rop)
	{
		return dest.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &src, pt.x, pt.y, rop) != FALSE;
	}
	#define SVKNOB_CLASSNAME _T("SVKnob")

} //namespace SvMc
