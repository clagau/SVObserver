//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved 
//******************************************************************************
//* .Module Name     : SymmetricTrapezoid
//* .File Name       : $Workfile:   SymmetricTrapezoid.cpp  $
//* .Description     : PLEASE ENTER A DESCRIPTION
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Jun 2014 16:28:58  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SymmetricTrapezoid.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
SymmetricTrapezoid::SymmetricTrapezoid()
: m_TrapOffset( 0 )
, m_Subtype( VerticalAxisTop )
{
}

SymmetricTrapezoid::~SymmetricTrapezoid()
{
}
#pragma endregion Constructor

#pragma region Public Methods
void SymmetricTrapezoid::Draw( POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected )
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

	std::vector<POINT> aPoints;
	GetPoints(aPoints);
	TranslatePoints(p_Offset, p_fZoomWidth, p_fZoomHeight, aPoints);
	rDC.Polyline( &(aPoints[0]), static_cast<int>(aPoints.size()) );

	rDC.SelectObject( l_pOldPen );
	rDC.SetBkMode( l_OldBKMode );

	DrawObject::DrawChildren( p_Offset,  p_fZoomWidth, p_fZoomHeight, rDC, p_bSelected );
}

bool SymmetricTrapezoid::IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const
{
	bool bRet = false;
	long allow = GetEditAllowed();

	bool bAlMoveAndSize = AllowResizeAndMove == (allow & AllowResizeAndMove);
	bool bAlMove = AllowMove == (allow & AllowMove);
	bool bAlSize = AllowResize == (allow & AllowResize);
	bool bAlDelete = AllowDelete == (allow & AllowDelete);

	CRect rect(m_rect);
	CPoint ptTopLeft = rect.TopLeft();
	CPoint ptTopRight = CPoint( rect.right, rect.top );
	CPoint ptBottomLeft = CPoint( rect.left, rect.bottom );
	CPoint ptBottomRight = rect.BottomRight();

	switch ( m_Subtype )
	{
	case VerticalAxisTop:
		ptTopLeft.x += m_TrapOffset;
		ptTopRight.x -= m_TrapOffset;
		break;
	case VerticalAxisBottom:
		ptBottomLeft.x += m_TrapOffset;
		ptBottomRight.x -= m_TrapOffset;
		break;
	case HorizontalAxisLeft:
		ptTopLeft.y += m_TrapOffset;
		ptBottomLeft.y -= m_TrapOffset;
		break;
	case HorizontalAxisRight:
		ptTopRight.y += m_TrapOffset;
		ptBottomRight.y -= m_TrapOffset;
		break;
	default:
		break; // Do nothing.
	}

	CRgn region;
	CPoint apoints[4] = {ptTopRight, ptTopLeft, ptBottomLeft, ptBottomRight};
	region.CreatePolygonRgn(apoints, 4, WINDING);

	std::unique_ptr<CRgn> prgnTopLeft = GetHotSpot( ptTopLeft );
	std::unique_ptr<CRgn> prgnTopRight = GetHotSpot( ptTopRight );
	std::unique_ptr<CRgn> prgnBottomLeft = GetHotSpot( ptBottomLeft );
	std::unique_ptr<CRgn> prgnBottomRight = GetHotSpot(ptBottomRight );

	std::unique_ptr<CRgn> prgnTop = GetHotSpot( ptTopLeft, ptTopRight );
	std::unique_ptr<CRgn> prgnLeft = GetHotSpot( ptTopLeft, ptBottomLeft );
	std::unique_ptr<CRgn> prgnBottom = GetHotSpot( ptBottomLeft, ptBottomRight );
	std::unique_ptr<CRgn> prgnRight = GetHotSpot( ptTopRight, ptBottomRight );

	if( bAlMoveAndSize && prgnTopLeft->PtInRegion(imagePoint) )
	{
		SelType = HTTOPLEFT;
		bRet = true;
	}
	else if( bAlMoveAndSize && prgnTopRight->PtInRegion(imagePoint) )
	{
		SelType = HTTOPRIGHT;
		bRet = true;
	}
	else if( bAlMoveAndSize && prgnBottomLeft->PtInRegion(imagePoint) )
	{
		SelType = HTBOTTOMLEFT;
		bRet = true;
	}
	else if( bAlSize &&  prgnBottomRight->PtInRegion(imagePoint) )
	{
		SelType = HTBOTTOMRIGHT;
		bRet = true;
	}
	else if (bAlMoveAndSize &&  prgnTop->PtInRegion( imagePoint ) )
	{
		SelType = HTTOP;
		bRet = true;
	}
	else if ( bAlMoveAndSize &&  prgnLeft->PtInRegion( imagePoint ) )
	{	
		SelType = HTLEFT;
		bRet = true;
	}
	else if ( bAlMoveAndSize &&  prgnBottom->PtInRegion( imagePoint ) )
	{	
		SelType = HTBOTTOM;
		bRet = true;
	}
	else if ( bAlMoveAndSize &&  prgnRight->PtInRegion( imagePoint ) )
	{	
		SelType = HTRIGHT;
		bRet = true;
	}
	else if ( bAlMove &&  region.PtInRegion( imagePoint ) )
	{	
		SelType = HTOBJECT;
		bRet = true;
	}
	else if ( bAlDelete &&  region.PtInRegion( imagePoint ) )
	{	
		SelType = HTSELECTED;
		bRet = true;
	}

	if( bRet == false )
	{
		bRet = IsValidChildObjectAtPoint( SelType, imagePoint, viewPoint );
	}

	return bRet;
}

bool SymmetricTrapezoid::Move(HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint)
{
	bool handled = false;
	CRect rec(m_rect);

	switch (SelType )
	{
	case HTTOPLEFT:
		if ( m_Subtype == VerticalAxisTop )
		{
			m_TrapOffset = m_TrapOffset + imageMovePoint.x;

			handled = true;
		}
		else if ( m_Subtype == HorizontalAxisLeft )
		{
			m_TrapOffset = m_TrapOffset + imageMovePoint.y; 

			handled = true;
		}
		break;
	case HTTOPRIGHT:
		if ( m_Subtype == VerticalAxisTop )
		{
			m_TrapOffset = m_TrapOffset - imageMovePoint.x;
			handled = true;
		}
		else if ( m_Subtype == HorizontalAxisRight )
		{
			m_TrapOffset = m_TrapOffset + imageMovePoint.y;
			handled = true;
		}
		break;
	case HTBOTTOMLEFT:
		if ( m_Subtype == VerticalAxisBottom )
		{	
			m_TrapOffset = m_TrapOffset + imageMovePoint.x;
			handled = true;
		}
		else if ( m_Subtype == HorizontalAxisLeft )
		{	
			m_TrapOffset = m_TrapOffset - imageMovePoint.y;
			handled = true;
		}
		break;
	case HTBOTTOMRIGHT:
		if ( m_Subtype ==VerticalAxisBottom )
		{	
			m_TrapOffset = m_TrapOffset - imageMovePoint.x;
			handled = true;
		}
		else if ( m_Subtype == HorizontalAxisRight )
		{	
			m_TrapOffset = m_TrapOffset - imageMovePoint.y;
			handled = true;
		}
		break;
	default:
		break; // Do nothing.
	}

	if(!handled)
	{
		RectangleObject::Move(SelType, imageMovePoint, viewMovePoint);
	}

	NormOffset();
	return true;
}

void SymmetricTrapezoid::SetParameter(long parameterId, _variant_t parameterValue)
{
	switch (parameterId)
	{
		case P_Type:
			assert( TrapezoidROI == parameterValue.lVal );
			break;
		case P_Offset:
			m_TrapOffset= parameterValue.lVal;
			break;
		case P_SubType:
			m_Subtype = static_cast<ROISubType_Trapezoid>(parameterValue.lVal);
			break;
		default:
			RectangleObject::SetParameter(parameterId, parameterValue);
			break;
	}
}

void SymmetricTrapezoid::GetParameter(VariantParamMap& ParameterMap) const
{
	RectangleObject::GetParameter(ParameterMap);

	ParameterMap[P_Type] = static_cast<long>(TrapezoidROI);
	ParameterMap[ P_Offset ] = m_TrapOffset;
	ParameterMap[P_SubType] = static_cast<long>(m_Subtype);
}

int SymmetricTrapezoid::GetPoints(std::vector<POINT> &aPoints) const
{
	aPoints.clear();

	CRect rect(m_rect );

	aPoints.push_back( rect.TopLeft() );                  // 0
	aPoints.push_back( CPoint(rect.left, rect.bottom) );  // 1
	aPoints.push_back( rect.BottomRight() );              // 2
	aPoints.push_back( CPoint(rect.right, rect.top) );    // 3

	// if offset is bigger than half the width, we will get a triangle with the top not really at the top.

	switch ( m_Subtype )
	{
	case VerticalAxisTop:
		{
			if( m_TrapOffset* 2 < rect.Width() )
			{
				aPoints[0].x += m_TrapOffset;
				aPoints[3].x -= m_TrapOffset;
			}
			else
			{
				//degenerate
				aPoints[0].x = ( aPoints[2].x + aPoints[1].x )/2;
				aPoints[3].x = aPoints[0].x;
			}
			break;
		}
	case VerticalAxisBottom:
		{
			if(m_TrapOffset* 2 < rect.Width() )
			{
				aPoints[1].x += m_TrapOffset;
				aPoints[2].x -= m_TrapOffset;
			}
			else
			{
				//degenerate
				aPoints[1].x = (aPoints[3].x + aPoints[0].x )/2;
				aPoints[2].x = aPoints[1].x;
			}
			break;
		}
	case HorizontalAxisLeft:
		{
			if( m_TrapOffset* 2 < rect.Height() )
			{
				aPoints[0].y += m_TrapOffset;
				aPoints[1].y -= m_TrapOffset;
			}
			else
			{
				//degenerate
				aPoints[0].y = ( aPoints[3].y + aPoints[2].y )/2;
				aPoints[1].y = aPoints[0].y;
			}
			break;
		}
	case HorizontalAxisRight:
		{
			if( m_TrapOffset* 2 < rect.Height() )
			{
				aPoints[3].y += m_TrapOffset;
				aPoints[2].y -= m_TrapOffset;
			}
			else
			{
				//degenerate
				aPoints[3].y = ( aPoints[1].y  + aPoints[0].y )/2;
				aPoints[2].y = aPoints[3].y;
			}
			break;
		}
	default:
		{
			break; // Do nothing.
		}
	}
	aPoints.push_back( aPoints[0] ); // 4; close the Overlay (back to original point)

	return S_OK;
}
#pragma endregion Public Methods

#pragma region Protected Methods
void SymmetricTrapezoid::NormOffset()
{
	CRect rec(m_rect);
	m_TrapOffset = std::max<long>( 0, m_TrapOffset );

	switch(m_Subtype)
	{
	case VerticalAxisTop: // fall through...
	case VerticalAxisBottom:
		m_TrapOffset = std::min<long>( rec.Width()/2, m_TrapOffset );
		break;

	case HorizontalAxisLeft: // fall through...
	case HorizontalAxisRight:
		m_TrapOffset = std::min<long>( rec.Height()/2, m_TrapOffset );
		break;

	default:
		break; // Do nothing.
	}
}
#pragma endregion Protected Methods

