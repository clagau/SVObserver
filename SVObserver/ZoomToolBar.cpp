//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ZoomToolBar
//* .File Name       : $Workfile:   ZoomToolBar.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 13:14:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ZoomToolBar.h"
#pragma endregion Includes

#pragma region Constructor
ZoomToolBar::ZoomToolBar()
: m_value(1.0)
{
	/*This is the multiplier between the int position of the slider and the zoom value.  */
	/* To avoid rounding errors, the min and max values of the zoom range multiplied with this value should be an integer number.*/
	/* The min and max zoom values are 0.625 (1/16) and 16, respectively.*/

	m_SliderProp = 160.0;
}

ZoomToolBar::~ZoomToolBar()
{
}
#pragma endregion Constructor

#pragma region Public Methods
double ZoomToolBar::GetValue() const
{
	return m_value;
}

void ZoomToolBar::SetValue(double value)
{
	m_value = value;
	int pos = static_cast<int>( m_SliderProp * value );
	m_wndZoomSlider.SetPos(pos);
	DisplayNumber( static_cast<int>(value * 100.0) );
}

void ZoomToolBar::SetRange(double dmin, double dmax , double dpage)
{
	int min = static_cast<int>( m_SliderProp * dmin );
	int max = static_cast<int>( m_SliderProp * dmax );

	m_wndZoomSlider.SetRange(min, max);

	if(dpage > 0)
	{
		int step = static_cast<int>( m_SliderProp * dpage );
		m_wndZoomSlider.SetPageSize(step);
	}
}

void ZoomToolBar::DisplayNumber(int fig)
{
	CString cs;
	cs.Format("%i%%",fig);
	m_wndZoomStatic.SetWindowText(cs);
}

void ZoomToolBar::Update()
{
	int pos = m_wndZoomSlider.GetPos();
	DisplayNumber(pos);
	m_value = static_cast<double>(pos / m_SliderProp);
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_ZOOM_SLIDER_MOVED, 0);
}
#pragma endregion Public Methods

BEGIN_MESSAGE_MAP(ZoomToolBar, CToolBar)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

int ZoomToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!LoadToolBar(IDR_TOOLBAR_ZOOM))
		return -1;

	// DC to select font into
	CClientDC dc(this);
	// CFont object used to set the desired control fonts

	m_font.CreatePointFont (80, _T ("MS Sans Serif"));
	CFont* pOldFont = dc.SelectObject(&m_font);

	// TEXTMETRIC is Useful to measure and place the controls
	// in the desired positions

	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	int cxChar = tm.tmAveCharWidth;
	int cyChar = tm.tmHeight + tm.tmExternalLeading;
	dc.SelectObject(pOldFont);
	
	CRect rect;
	SetButtonInfo(0, ID_EDIT_ZOOM, TBBS_SEPARATOR, cxChar * 6);

	GetItemRect(0, &rect);

	if(!m_wndZoomStatic.Create(_T("100%"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | WS_BORDER | WS_DISABLED, rect, this, ID_EDIT_ZOOM))
	{
		return -1;
	}

	m_wndZoomStatic.SetFont(&m_font);

	GetItemRect(1, &rect);
	SetButtonInfo(2, ID_SLIDER_ZOOM, TBBS_SEPARATOR, rect.Width() * 3);
	GetItemRect(2, &rect);

	if (!m_wndZoomSlider.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_DISABLED | TBS_HORZ | TBS_NOTICKS | TBS_BOTH, rect, this, ID_SLIDER_ZOOM))
	{
		return -1;
	}
	SetRange(0.1, 10.0, 0.1);
	SetValue(1.0);

	return 0;
}

void ZoomToolBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	Update();
	CToolBar::OnHScroll(nSBCode, nPos, pScrollBar);
}

void ZoomToolBar::Enable(bool enable)
{
	m_wndZoomSlider.EnableWindow(enable);
	m_wndZoomStatic.EnableWindow(enable);
}

void ZoomToolBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler)
{
	CToolBar::OnUpdateCmdUI(pTarget,bDisableIfNoHandler);
}

