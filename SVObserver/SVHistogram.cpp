//******************************************************************************
//* COPYRIGHT (c) 2009 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHistogram
//* .File Name       : $Workfile:   SVHistogram.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:45:50  $
//******************************************************************************

#include "stdafx.h"
#pragma inline_recursion (on)
#include "SVHistogram.h"

// SVHistogram

namespace
{
	class GDICanvas
	{
	public:
		GDICanvas(CWnd * owner, const CRect & client, CBitmap * bmp, COLORREF color)
		{
			CPaintDC dc(owner); // device context for painting
			if (bmp->GetSafeHandle() == NULL)
			{
				bmp->CreateCompatibleBitmap(&dc, client.Width(), client.Height());
			}
			CBrush l_brush(color);
			memDC.CreateCompatibleDC(&dc);
			temp_bmp = (CBitmap *)memDC.SelectObject(bmp);
			tmp_brush = (CBrush *)memDC.SelectObject(&l_brush);
			memDC.FillRect(&client, &l_brush);
		}
		~GDICanvas()
		{
			memDC.SelectObject(tmp_brush);
			memDC.SelectObject(temp_bmp);
		}

		void Draw(const CRect & rect, int color)
		{
			CBrush l_brush(RGB(color, color, color));
			memDC.SelectObject(&l_brush);
			memDC.FillRect(&rect, &l_brush);
		}
	private:
		CBitmap * temp_bmp;
		CBrush * tmp_brush;
		CDC memDC;
	};
}

IMPLEMENT_DYNAMIC(SVHistogram, CStatic)

SVHistogram::SVHistogram(): 
	CStatic(),
	SVHistogramBase(),
	m_lowColor(color::Crimson),
	m_highColor(color::Green),
	m_backColor(color::Slate),
	m_lowWarn(color::Gold),
	m_trackWarn(color::Aqua),
	m_boundState(0),
	m_dragging(0),
	m_trackColor(color::Plum)
{
	std::vector<int>::iterator it = m_values.begin();
	m_max = m_total = 0;
	for(int i = 0; i < 256; ++i, ++it)
	{
		*it = i;
		m_total += *it;
	}
	m_max = 256;
	RegisterWindowClass();
	m_lowBound = 40;
	m_highBound = 190;
	SetLabelText(128, label::bottom);
	SetLabelText(100.0, label::leftTop);
	SetLabelText(75.0, label::leftUpper);
	SetLabelText(50.0, label::leftCenter);
	SetLabelText(25.0, label::leftLower);
	SetLabelText(0,label::bottomLeft);
	SetLabelText(0.0, label::leftBottom);
	SetLabelText(255, label::bottomRight);
	m_ArrowCursor   = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_WECursor   = AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
	m_CurrentCursor = m_ArrowCursor;
}

SVHistogram::~SVHistogram()
{
}


BEGIN_MESSAGE_MAP(SVHistogram, CStatic)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_BCK_COLOR, OnBckColor)
	//ON_COMMAND(ID_OPTIONS, OnOptions)
	ON_MESSAGE(ID_BOUND_CHANGE, OnBoundChange)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_CONTEXTMENU()
	ON_WM_SIZE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
END_MESSAGE_MAP()

// SVHistogram message handlers

inline bool not_valid(const CRect & r)
{
	return (r.left | r.right | r.top | r.bottom) <= 0;
}

void SVHistogram::OnPaint()
{
	CString msg;
	if (needs_repaint)
	{
		//GetClientRect(m_client);
		//UpdateAnchors(m_client);
		PaintBitmap();
		//return;
	}

	CPaintDC dc(this); // device context for painting
	CRect l_rect;
	//GetClientRect(l_client);
	//if ((dc.GetBoundsRect(&l_rect, DCB_RESET) & DCB_SET) != DCB_SET || not_valid(l_rect))
	{
		l_rect = m_client;
	}

	//msg.FormatMessage(_T("BoundsRect: %1!d!, %2!d!, %3!d!, %4!d!\n"), l_rect.left, l_rect.top, l_rect.right, l_rect.bottom);
	//OutputDebugString(msg);
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap * temp_bmp = (CBitmap *)memDC.SelectObject(&m_bmp);

	// one of the many reasons I hate MFC...
	dc.BitBlt(l_rect.left, l_rect.top, l_rect.right, l_rect.bottom, &memDC, l_rect.left, l_rect.top, SRCCOPY);

	memDC.SelectObject(temp_bmp);
	
	CFont l_font;
	l_font.CreateFont(
	   10,                        // nHeight
	   0,                         // nWidth
	   0,                         // nEscapement
	   0,                         // nOrientation
	   FW_NORMAL,                 // nWeight
	   FALSE,                     // bItalic
	   FALSE,                     // bUnderline
	   0,                         // cStrikeOut
	   ANSI_CHARSET,              // nCharSet
	   OUT_DEFAULT_PRECIS,        // nOutPrecision
	   CLIP_DEFAULT_PRECIS,       // nClipPrecision
	   DEFAULT_QUALITY,           // nQuality
	   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
	   _T("Lucida Console"));     // lpszFacename

	CFont * tmp_font = (CFont *)dc.SelectObject(&l_font);
	DrawLabels(dc, this);
	dc.SelectObject(tmp_font);
	DrawBounds(dc);
	DrawGrid(dc);
}

void SVHistogram::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_boundState&histogram::active)
	{
		int pos = Screen2Pos(point.x);
		CRect l_bar = Pos2Screen(pos);
		CRect l_low = Pos2Screen(m_lowBound);
		CRect l_high = Pos2Screen(m_highBound);
		switch (m_dragging)
		{
		case low:
			{
				m_lowBound = pos;
				GetParent()->PostMessage(ID_BOUND_CHANGE, m_lowBound, low);
				InvalidateRect(l_low, FALSE);
				InvalidateRect(l_bar, FALSE);
				break;
			}
		case high:
			{
				m_highBound = pos;
				GetParent()->PostMessage(ID_BOUND_CHANGE, m_highBound, high);
				InvalidateRect(l_high, FALSE);
				InvalidateRect(l_bar, FALSE);
				break;
			}
		}
		l_low.InflateRect(5, 0);
		l_high.InflateRect(5, 0);
		if ((l_low.PtInRect(point) && !(m_disabled & histogram::low)) 
			|| (l_high.PtInRect(point) &&  !(m_disabled & histogram::high)))
		{
			::SetCursor(m_WECursor);
		}
		else
		{
			if (!m_dragging)
			{
				::SetCursor(m_ArrowCursor);
			}
		}
		//
	}
}

void SVHistogram::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_boundState&histogram::active)
	{
		CRect l_low = Pos2Screen(m_lowBound);
		CRect l_high = Pos2Screen(m_highBound);
		l_low.InflateRect(1,0);
		l_high.InflateRect(1,0);
		if (l_low.PtInRect(point) && !(m_disabled & histogram::low))
		{
			m_dragging = low;
		}
		else
		if (l_high.PtInRect(point) &&  !(m_disabled & histogram::high))
		{
			m_dragging = high;
		}
		else
		{
			m_dragging = 0;
		}
	
		if(m_dragging)
		{
			::SetCursor(m_WECursor);
			SetCapture();
		}
	}
	SetFocus();
}

BOOL SVHistogram::OnEraseBkgnd(CDC* pDC)
{
	return TRUE; // no need to erase...
}

void SVHistogram::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_dragging = 0;
	ReleaseCapture();
	::SetCursor(m_ArrowCursor);
	//CStatic::OnLButtonUp(nFlags, point);
}

BOOL SVHistogram::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_placement == histogram::analyzer)
		return FALSE;
	scale *= 1.0 + (zDelta / 120) * .1;
	if (scale < .2)
		scale = .2;
	else if (scale > 12.0)
		scale = 12.0;
	PaintBitmap();
	return TRUE;
}

// Paint the bitmap off screen
void SVHistogram::PaintBitmap()
{
	GetClientRect(m_client);
	UpdateAnchors(m_client, m_func);
	GDICanvas canvas(this, m_client, &m_bmp, m_colors?m_backColor:color::White);
	DrawHistogram(canvas);

	SetLabelText(m_factor*100.0/scale, label::leftTop);
	//static_cast<Label<label::leftTop> &>(*this).SetText(100.0/scale);
	SetLabelText(m_factor*75.0/scale, label::leftUpper);
	SetLabelText(m_factor*50.0/scale, label::leftCenter);
	SetLabelText(m_factor*25.0/scale, label::leftLower);
	SetLabelText(m_start, label::bottomLeft);
	SetLabelText(m_start + ((m_end - m_start)>>1), label::bottom);
	SetLabelText(m_end - 1, label::bottomRight);

	needs_repaint = false;
	Invalidate(FALSE);
}

BOOL SVHistogram::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, HISTOGRAM_CLASSNAME, &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_DBLCLKS | CS_PARENTDC | CS_BYTEALIGNCLIENT;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_GRADIENTACTIVECAPTION);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = HISTOGRAM_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}


void SVHistogram::DoDataExchange(CDataExchange* pDX)
{
	CStatic::DoDataExchange(pDX);
}

void SVHistogram::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	if (m_colors == histogram::plain)
		return;
	CMenu l_menu;

	l_menu.CreatePopupMenu();
	l_menu.AppendMenu(MF_STRING, ID_BCK_COLOR, _T("&Background..."));
	//l_menu.AppendMenu(MF_SEPARATOR);
	//l_menu.AppendMenu(MF_STRING, ID_OPTIONS, (LPCSTR)"&Options...");
	l_menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
}

void SVHistogram::OnBckColor()
{
	CColorDialog l_dlg;
	if (l_dlg.DoModal() == IDOK)
	{
		m_backColor = l_dlg.GetColor();
		PaintBitmap();
	}
}

//void SVHistogram::OnOptions()
//{
//	OptionsDialog l_dlg;
//	l_dlg.scale = m_scale;
//	l_dlg.verticalAxis = m_axis;
//
//	if (l_dlg.DoModal() == IDOK)
//	{
//		m_scale = static_cast<histogram::scale>(l_dlg.scale);
//		m_axis = static_cast<histogram::v_axis>(l_dlg.verticalAxis);
//		PaintBitmap();
//	}
//}


int SVHistogram::SetBound(int pos, Bound bound)
{
	// extract bound state from pos
	m_boundState = pos >> 8;
	if (!m_dragging)
	{
		pos &= 0xFF;
	
		int & l_bound = (bound == low)?m_lowBound:m_highBound;
		
		CRect l_old = Pos2Screen(l_bound);
		CRect l_new = Pos2Screen(pos);
		l_bound = pos;
		InvalidateRect(l_old, FALSE);
		InvalidateRect(l_new, FALSE);
	}
	return 0;
}

void SVHistogram::OnSize(UINT nType, int cx, int cy)
{
	CStatic::OnSize(nType, cx, cy);
	GetClientRect(m_client);
	UpdateAnchors(m_client, m_func);
	PaintBitmap();
}

void SVHistogram::OnMButtonDown(UINT nFlags, CPoint point)
{
	if (m_placement != histogram::analyzer)
	{
		SetFocus();
		scale = 1.0;
	}
}

void SVHistogram::OnMButtonUp(UINT nFlags, CPoint point)
{
	if (m_placement != histogram::analyzer)
	{
		PaintBitmap();
	}
}

LRESULT SVHistogram::OnBoundChange(WPARAM wParam, LPARAM lParam)
{
	SetBound(static_cast<int>(lParam), static_cast<Bound>(wParam)); 
	return 0;
}

void SVHistogram::DrawBound(CPaintDC & dc, int pos, COLORREF color)
{
	CRect l_rect = Pos2Screen(pos);
	if (m_client.PtInRect(l_rect.CenterPoint()))
	{
		CPoint l_top = l_rect.CenterPoint();
		CPoint l_bottom = l_top;
		l_top.y = l_rect.top;
		l_bottom.y = l_rect.bottom - 1;
		CPen l_pen(PS_SOLID, 1, color); //l_rect.Width()/2 + 1, color);
		CPen * tmp_pen = dc.SelectObject(&l_pen);
		dc.MoveTo(l_top);
		dc.LineTo(l_bottom);
		dc.SelectObject(tmp_pen);
	}
}

void SVHistogram::DrawBounds(CPaintDC & dc)
{
	if(m_placement == histogram::analyzer)
	{
		DrawBound(dc, low_peak(), m_lowColor);
		DrawBound(dc, high_peak(), m_highColor);
		DrawBound(dc, valley(), m_trackColor);
	}
	else if (m_boundState&histogram::shown)
	{
		DrawBound(dc, m_lowBound, m_lowColor);
		DrawBound(dc, m_highBound, m_highColor);
	}
}

void SVHistogram::DrawGrid(CPaintDC & dc)
{
	CPen l_pen(PS_DASHDOT, 1, color::Gray);
	int tmp_mode = dc.SetBkMode(TRANSPARENT);
	CPen * tmp_pen = dc.SelectObject(&l_pen);
	CPoint pt = TheAnchor<label::leftUpper>::m_anchor;
	dc.MoveTo(pt);
	pt.x = m_client.right;
	dc.LineTo(pt);
	pt = TheAnchor<label::leftCenter>::m_anchor;
	dc.MoveTo(pt);
	pt.x = m_client.right;
	dc.LineTo(pt);
	pt = TheAnchor<label::leftLower>::m_anchor;
	dc.MoveTo(pt);
	pt.x = m_client.right;
	dc.LineTo(pt);
	dc.SelectObject(tmp_pen);
	dc.SetBkMode(tmp_mode);
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVHistogram.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:45:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   10 Feb 2011 17:01:38   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  716
 * SCR Title:  Fix the Histogram control interaction with threshold tab.
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Initialized m_dragging flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   31 Jan 2011 13:31:44   jHanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  629
 * SCR Title:  Add Histogram analyzer
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Change of m_dragging flag related logic. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   27 Jan 2011 11:12:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Jul 2010 07:59:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  697
 * SCR Title:  Add Free Memory Information to Status Bar
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix a problem with the histogram capture problem with the mouse.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
