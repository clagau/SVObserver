//******************************************************************************
//* COPYRIGHT (c) 2009 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVHistogram
//* .File Name       : $Workfile:   SVHistogram.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:46:00  $
//******************************************************************************
#pragma once

//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <math.h>
//Moved to precompiled header: #include <numeric>
//Moved to precompiled header: #include <vector>

#include "SVUtilityLibrary\SVHistogramBase.h"
#include "SVHistogramLabels.h"

// SVHistogram
#define ID_BCK_COLOR		(WM_USER + 701)
#define ID_OPTIONS		(WM_USER + 702)
#define ID_BOUND_CHANGE (WM_USER + 703)

#define ID_BOUND_HIDDEN  256
#define ID_BOUND_ACTIVE  257
#define ID_BOUND_PASSIVE 258

namespace SvMc
{
	class SVHistogram : public CStatic, public HistogramLabels, public SvUl::SVHistogramBase
	{
		DECLARE_DYNAMIC(SVHistogram)

	public:
		SVHistogram();
		virtual ~SVHistogram();

		enum Bound { low = 1, high, track };
		int SetBound(int pos, Bound bound);

		histogram::scale GetScale() const { return m_scale; }
		histogram::disp_options GetDisplay() const { return m_disp; }
		histogram::height_options GetHeightOption() const { return m_height; }
		unsigned GetDisabled() const { return m_disabled; }
		void Disable(unsigned b) { m_disabled |= (b & histogram::bound_mask); }
		void Enable(unsigned b) { m_disabled &= ~(b & histogram::bound_mask); }

	protected:
		DECLARE_MESSAGE_MAP()
	public:
		afx_msg void OnPaint();
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
		afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
		afx_msg void OnBckColor();
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

		afx_msg LRESULT OnBoundChange(WPARAM wParam, LPARAM lParam);

	private:
		std::vector<CPoint> m_ticks, m_oldTicks;
		int m_trackPos;
		COLORREF m_lowColor, m_highColor, m_backColor, m_trackColor, m_lowWarn, m_trackWarn;
		CBitmap m_bmp;
		int m_dragging;
		int m_boundState;

		HCURSOR m_ArrowCursor;
		HCURSOR m_WECursor;
		HCURSOR m_CurrentCursor;

		// Paint the bitmap off screen
		void PaintBitmap();
		BOOL RegisterWindowClass();
		virtual void DoDataExchange(CDataExchange* pDX) override;
	public:
		void DrawBound(CPaintDC & dc, int pos, COLORREF color);
		void DrawBounds(CPaintDC & dc);
		void DrawGrid(CPaintDC & dc);
		bool SetScale(histogram::scale s);
	};

	#include "SVHistogram.inl"

	#define HISTOGRAM_CLASSNAME _T("SVHistogram")
} //namespace SvMc
