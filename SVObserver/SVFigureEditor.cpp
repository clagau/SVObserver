//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFigureEditor
//* .File Name       : $Workfile:   SVFigureEditor.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:33:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <vector>
#include "SVFigureEditor.h"
#pragma endregion Includes

// default hot spot size
int SVFigureEditor::f_iHotSpotSizeX = 5;  // size is twice this
int SVFigureEditor::f_iHotSpotSizeY = 5;  // size is twice this

SVFigureEditor::SVFigureEditor()
{
	m_uiMouseFlags = 0;
	m_bCapture = false;
	m_rgbOuterOutlineColor = RGB(0,255,0);
	m_rgbInnerOutlineColor = RGB(0,0,0);
	m_bUseInnerOutline = false;
}


SVFigureEditor::~SVFigureEditor()
{
}

bool SVFigureEditor::IsCaptured()
{
	return m_bCapture;
}

HRESULT SVFigureEditor::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( m_bCapture )
	{
		HRESULT hr = S_FALSE;
#ifndef _DEBUG
		try
#endif
		{
			hr = MouseMove( nFlags, point );
		}
#ifndef _DEBUG
		catch ( ... )
		{
		}
#endif
		return hr;
	}
	else
		return S_OK;
}

HRESULT SVFigureEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
	return LButtonDown( nFlags, point );
}

HRESULT SVFigureEditor::OnLButtonUp(UINT nFlags, CPoint point)
{
	return LButtonUp( nFlags, point );
}

UINT SVFigureEditor::OnHitTest( CPoint point )	// visual hit test
{
	return HitTest( point ).CursorHT();
}

HRESULT SVFigureEditor::Draw( HDC  ) const
{
	return E_NOTIMPL;
}

HRESULT SVFigureEditor::PaintOverlay( CDC& , CRect , CRect  )
{
	return E_NOTIMPL;
}

CSize SVFigureEditor::GetSize()
{
	return GetSizeImpl();
}


HRESULT SVFigureEditor::LButtonDown(UINT nFlags, CPoint point)
{
	HRESULT hr = S_OK;

	m_bCapture = true;
	m_ptMouseDown = point;
	m_uiMouseFlags = nFlags;
	m_ptReference = GetReferencePoint();
	m_sizeMouseDelta = m_ptMouseDown - m_ptReference;

	m_HitType = HitTest(point);

	m_rectOriginal = GetRect();	// must be done after HitTest

	return hr;
}

HRESULT SVFigureEditor::LButtonUp(UINT, CPoint)
{
	HRESULT hr = S_OK;

	m_bCapture = false;
	m_HitType.nHitTest = HTNOWHERE;

	return hr;
}

CSize SVFigureEditor::GetHotSpotSize()
{
	CSize size(f_iHotSpotSizeX, f_iHotSpotSizeY);
	return size;
}

CRect SVFigureEditor::GetHotSpot( CPoint pt )
{
	CRect rect;
	CSize sizeHotSpot = GetHotSpotSize();

	rect.top = pt.y - sizeHotSpot.cy;
	rect.bottom = pt.y + sizeHotSpot.cy;
	rect.left = pt.x - sizeHotSpot.cx;
	rect.right = pt.x + sizeHotSpot.cx;
	
	return rect;
}

std::shared_ptr<CRgn> SVFigureEditor::GetHotSpot( CPoint pt1, CPoint pt2 )
{
	// this assumes right angles for hot spot
	//assert( pt1.x == pt2.x || pt1.y == pt2.y );

	std::shared_ptr<CRgn> pRegion = std::make_shared<CRgn>();
	CSize sizeHotSpot = GetHotSpotSize();

	if ( pt1.x == pt2.x )	// vertical orientation
	{
		CRect rect;
		rect.left  = pt1.x - sizeHotSpot.cx;
		rect.right = pt1.x + sizeHotSpot.cx;
		if ( pt1.y < pt2.y )
		{
			rect.top = pt1.y;
			rect.bottom = pt2.y;
		}
		else
		{
			rect.top = pt2.y;
			rect.bottom = pt1.y;
		}
		pRegion->CreateRectRgnIndirect( &rect );
	}
	else if ( pt1.y == pt2.y ) // horizontal orientation
	{
		CRect rect;
		rect.top    = pt1.y - sizeHotSpot.cy;
		rect.bottom = pt1.y + sizeHotSpot.cy;
		if ( pt1.x < pt2.x )
		{
			rect.left = pt1.x;
			rect.right = pt2.x;
		}
		else
		{
			rect.left = pt2.x;
			rect.right = pt1.x;
		}
		pRegion->CreateRectRgnIndirect( &rect );
	}
	else
	{
		std::vector<CPoint> aPoints(4);
		if ( abs(pt1.x - pt2.x) > abs(pt1.y - pt2.y) )	// more horizontal than vertical
		{
			aPoints[0] = CPoint(pt1.x, pt1.y - sizeHotSpot.cy);
			aPoints[1] = CPoint(pt1.x, pt1.y + sizeHotSpot.cy);
			aPoints[2] = CPoint(pt2.x, pt2.y + sizeHotSpot.cy);
			aPoints[3] = CPoint(pt2.x, pt2.y - sizeHotSpot.cy);
		}
		else	// more vertical than horizontal
		{
			aPoints[0] = CPoint(pt1.x + sizeHotSpot.cx, pt1.y);
			aPoints[1] = CPoint(pt1.x - sizeHotSpot.cx, pt1.y);
			aPoints[2] = CPoint(pt2.x - sizeHotSpot.cx, pt2.y);
			aPoints[3] = CPoint(pt2.x + sizeHotSpot.cx, pt2.y);
		}
		pRegion->CreatePolygonRgn(&aPoints[0], 4, WINDING);
	}

	return pRegion;
}

HRESULT SVFigureEditor::SetOverlayColor( COLORREF rgbOutlineColor )
{
	m_rgbOuterOutlineColor = rgbOutlineColor;
	m_bUseInnerOutline = false;
	return S_OK;
}

HRESULT SVFigureEditor::SetOverlayColor( COLORREF rgbOuterOutlineColor, COLORREF rgbInnerOutlineColor )
{
	m_rgbOuterOutlineColor = rgbOuterOutlineColor;
	m_rgbInnerOutlineColor = rgbInnerOutlineColor;
	m_bUseInnerOutline = true;
	return S_OK;
}

