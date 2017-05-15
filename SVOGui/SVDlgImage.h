//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDlgImage
//* .File Name       : $Workfile:   SVDlgImage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:15:44  $
//******************************************************************************

#pragma once

#include "SVUtilityLibrary/SVGUID.h"

namespace SvOg
{
	class SVDlgImageScrollBar : public CScrollBar
	{
	public:
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		DECLARE_MESSAGE_MAP()
	};

	/////////////////////////////////////////////////////////////////////////////
	// SVDlgImageClass window

	class SVDlgImageClass : public CStatic
	{
	public:
		SVDlgImageClass();
		virtual ~SVDlgImageClass();

		void UpdateImageInfo( const SVGUID& inspectionId, const SVGUID& imageId);
		void refresh();

		CSize   GetDisplaySize();
		double  GetZoom();
		HRESULT SetZoom(double dPercent);
		HRESULT SetZoomFullSize();// scale image to size of display window
		HRESULT SetViewportOrigin( CPoint ptOrigin, bool bRedraw = false );
		CPoint  GetViewportOrigin();
		HRESULT SetViewportRect( CRect rect );	// sets origin and zoom

	protected:

		void DestroyBuffers();
		virtual void OnSetZoom();
		CRect GetOutputRectFromImage();

		CRect m_SourceRect;
		CRect m_ClientRect;
		CRect m_OriginalClientRect;

		bool m_bShowScrollBars;
		CRect m_ViewportRect;
		double m_dZoomX;
		double m_dZoomY;
		double m_dFullSizeZoomFactor;
		double m_dFullSizeZoomFactorY;
		double m_dFullSizeZoomFactorScrollX;
		double m_dFullSizeZoomFactorScrollY;

		SVGUID m_inspectionId;
		SVGUID m_imageId;
		bool mbInit;

		SVDlgImageScrollBar m_sbHorizontal;
		SVDlgImageScrollBar m_sbVertical;

		static const int m_iScrollBarSize;

	protected:

		//{{AFX_VIRTUAL(SVDlgImageClass)
		//}}AFX_VIRTUAL

		// Generated message map functions
		//{{AFX_MSG(SVDlgImageClass)
		afx_msg void OnDestroy();
		afx_msg void OnPaint();
		//}}AFX_MSG
		afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pBar );
		afx_msg void OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pBar );
		virtual void OnPaintOverlay(CPaintDC& dc);	// derived classes implement this to draw after picture is displayed

		DECLARE_MESSAGE_MAP()
	};
} //namespace SvOg

