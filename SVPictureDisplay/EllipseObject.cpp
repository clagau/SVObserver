//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : EllipseObject
//* .File Name       : $Workfile:   EllipseObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:26:40  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "EllipseObject.h"
#include "SVPictureDisplayIdl.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
EllipseObject::EllipseObject()
{
}

EllipseObject::~EllipseObject()
{
}

EllipseObject::EllipseObject( long left, long top, long right, long bottom, COLORREF color, long lAllowEdit )
{
	
	m_rect.left = left;
	m_rect.top = top;
	m_rect.right = right;
	m_rect.bottom = bottom;
	m_color = color;
	m_SelectedColor = color;
	SetEditAllowed( lAllowEdit );
}
#pragma endregion Constructor

HRESULT EllipseObject::SetEllipse(long left, long top, long right, long bottom)
{
	m_rect.left = left;
	m_rect.top = top;
	m_rect.right = right;
	m_rect.bottom = bottom;
	return S_OK;
}

void EllipseObject::Draw( POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected )
{
	m_zoomWidth = p_fZoomWidth;
	m_zoomHeight = p_fZoomHeight;

	CPen l_Pen;
	if( p_bSelected )
	{
		l_Pen.CreatePen( PS_DOT, m_PenWidth, m_SelectedColor );
	}
	else
	{
		l_Pen.CreatePen( PS_SOLID, m_PenWidth, m_color );
	}
	int l_OldBKMode = rDC.SetBkMode( TRANSPARENT );
	CPen* l_pOldPen = rDC.SelectObject( &l_Pen );
	CRect l_NewRec = m_rect;
	CBrush* l_pOldBrush = (CBrush*)rDC.SelectStockObject( NULL_BRUSH );

	CSize l_size = l_NewRec.Size();
	l_size.cx = static_cast<long>( l_size.cx * p_fZoomWidth );
	l_size.cy = static_cast<long>( l_size.cy * p_fZoomHeight );
	POINT l_Offset( p_Offset );
	l_Offset.x *= -1;
	l_Offset.y *= -1;
	l_NewRec.OffsetRect( l_Offset );

	l_NewRec.top = static_cast<long>(l_NewRec.top * p_fZoomHeight);
	l_NewRec.left = static_cast<long>(l_NewRec.left * p_fZoomWidth);
	l_NewRec.right = static_cast<long>(l_NewRec.right * p_fZoomWidth);
	l_NewRec.bottom = static_cast<long>(l_NewRec.bottom * p_fZoomHeight);

	rDC.Ellipse( &l_NewRec );

	rDC.SelectObject( l_pOldBrush );
	rDC.SelectObject( l_pOldPen );
	rDC.SetBkMode( l_OldBKMode );
	
	DrawObject::DrawChildren( p_Offset,  p_fZoomWidth, p_fZoomHeight, rDC, p_bSelected );
}

bool EllipseObject::IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const
{
	bool bRet = false;
	long allow = GetEditAllowed();

	bool bAlMoveAndSize = AllowResizeAndMove == (allow & AllowResizeAndMove);
	bool bAlMove = AllowMove == (allow & AllowMove);
	bool bAlSize = AllowResize == (allow & AllowResize);
	bool bAlDelete = AllowDelete == (allow & AllowDelete);

	CPoint Center;
	Center.x = ( m_rect.right + m_rect.left ) / 2;
	Center.y = ( m_rect.bottom + m_rect.top ) / 2;

	CSize sizeHotSpot = GetHotSpotSize();
	CRect InnerRect(m_rect);

	if( InnerRect.Width() > 2 * sizeHotSpot.cx  && InnerRect.Height() > 2 * sizeHotSpot.cy )
	{
		InnerRect.DeflateRect( sizeHotSpot.cx, sizeHotSpot.cy );
	}

	std::unique_ptr<CRgn> pInnerEllepticalRgn( new CRgn );
	pInnerEllepticalRgn->CreateEllipticRgnIndirect( InnerRect);

	bool bInEllipse = false;
	bool bBoundary = false;

	if( pInnerEllepticalRgn->PtInRegion(imagePoint))
	{
		bInEllipse = true;
	}

	if ( bAlMove &&  bInEllipse )
	{
		SelType = HTOBJECT;
		bRet = true;
	}
	else if ( bAlDelete && bInEllipse )
	{
		SelType = HTSELECTED;
		bRet = true;
	}

	if(!bRet)
	{
		CRect InflateRect(m_rect);
		InflateRect.InflateRect( sizeHotSpot.cx, sizeHotSpot.cy );
		std::unique_ptr<CRgn> pInflateElRegion( new CRgn );
		pInflateElRegion->CreateEllipticRgnIndirect( InflateRect );
		BOOL InflEllipse = pInflateElRegion->PtInRegion(imagePoint);
		if(InflEllipse && !bInEllipse )
		{
			bBoundary = true;
		}
	}

	if(!bRet && bBoundary)
	{
		DWORD sel = GetSelectionType( Center, imagePoint );
		if( bAlMoveAndSize || bAlSize )
		{
			SelType = sel;
			bRet = true;
		}
	}

	if( bRet == false )
	{
		bRet = IsValidChildObjectAtPoint( SelType, imagePoint, viewPoint );
	}

	return bRet;
}

void EllipseObject::SetParameter(long parameterId, _variant_t parameterValue)
{
	switch (parameterId)
	{
	case P_Type:
		assert( EllipseROI == parameterValue.lVal );
		break;
	default:
		RectangleObject::SetParameter(parameterId, parameterValue);
		break;
	}
}

void EllipseObject::GetParameter(VariantParamMap& ParameterMap) const
{
	RectangleObject::GetParameter(ParameterMap);

	ParameterMap[P_Type] = static_cast<long>(EllipseROI);
}

