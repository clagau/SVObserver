//******************************************************************************
//* COPYRIGHT (c) 2009 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVKnobControl
//* .File Name       : $Workfile:   SVKnobControl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:46:24  $
//******************************************************************************

#ifndef SVHISTOGRAMBASE_H
#define SVHISTOGRAMBASE_H

#define WM_TRIGGER_CHANGE WM_USER + 401
#define KNOB_MAX 120000

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
			mdc.mask.CreateCompatibleDC(NULL);
			save.face = mdc.face.SelectObject(&(bitmap.face));
			save.mask = mdc.mask.SelectObject(&(bitmap.mask));
			mdc.face.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &(mdc.mask), 0, 0, SRCAND);
			//CDC tmpDC;
			//tmpDC.CreateCompatibleDC(NULL);
			//CBitmap tmpBmp;
			//tmpBmp.LoadBitmap(mask);
			//CBitmap * sav = tmpDC.SelectObject(&tmpBmp);
			mdc.mask.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), NULL, 0, 0, DSTINVERT);
			//tmpDC.SelectObject(sav);
		}
		~hold()
		{
			//mdc.face.SelectObject(save.face);
			//mdc.mask.SelectObject(save.mask);
		}
	};
}

// SVKnobControl

class SVKnobControl : public CStatic
{
	DECLARE_DYNAMIC(SVKnobControl)

public:
	SVKnobControl();
	virtual ~SVKnobControl();

protected:
	DECLARE_MESSAGE_MAP()
	BOOL RegisterWindowClass();
	virtual void DoDataExchange(CDataExchange* pDX);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	int click() { return ++clicks; }

	void CreateBitmaps();
	int GetValue() const { return int_value(m_value); }
	int SetValue(int value);
	void RunTest() const;
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
	double m_min;
	double m_max;
	double m_value;
	double m_angle;
	double min_val() const { return int2val(20); }
	double max_val() const { return int2val(KNOB_MAX); }
	double span() const { return (max_val() - min_val())/(pitwo() - pihalf()); }
	bool m_dragging;
	int int_value(double value) const { return static_cast<int>(exp(value*value*value) + 0.5); }//{ return static_cast<int>(exp(angle2val(value))); }
	int dotSize() const { return dot.rect.Width() / 2; }

	int m_maxy;

	CPoint val2xy(double val) const { return angle2xy(m_center, val2angle(val), m_radius); }
	CPoint angle2xy(const CPoint & c, double a, int r) const { return CPoint(c.x + static_cast<int>(r*cos(a)), c.y + static_cast<int>(r*sin(a))); }
	double xy2angle(CPoint & pt) const { return atan2(static_cast<double>(pt.y), static_cast<double>(pt.x)) + pi(); }
	double xy2val(const CPoint & pt) const;

	double angle2val(double angle) const { return fmod(pitwo() + angle - (pihalf() + pifourth()), pitwo())*span() + min_val(); }
	double val2angle(double value) const { return fmod((value-min_val())/span() + pihalf() + pifourth(), pitwo()); }
	double int2val(int i) const { return pow(log(static_cast<double>(i)), 1.0/3.0); }

	CRect dotRect() const { CRect rect = dot.rect; rect.MoveToXY(m_dotPoint.x, m_dotPoint.y); return rect; }
};

inline bool SVKnobControl::BitBlt(CDC & dest, CRect & rect, CDC & src, CPoint pt, DWORD rop)
{
	return dest.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &src, pt.x, pt.y, rop) != FALSE;
}

#define SVKNOB_CLASSNAME _T("SVKnob")

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVKnobControl.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 11:46:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2009 11:10:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/