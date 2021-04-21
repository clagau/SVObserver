//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved 
//******************************************************************************
//* .Module Name     : Doughnut
//* .File Name       : $Workfile:   Doughnut.cpp  $
//* .Description     : New DrawObject for Doughnut
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Jun 2014 16:26:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "Doughnut.h"
#include "SVPictureDisplayidl.h" // for _ParameterType
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
Doughnut::Doughnut()
{
	m_TwiceSideThickness = 0;
	m_TwiceTopThickness = 0;
}

Doughnut::~Doughnut()
{
}
#pragma endregion Constructor

#pragma region Public Methods
void Doughnut::SetParameter(const VariantParamMap& ParameterMap)
{
	DrawObject::SetParameter(ParameterMap);

	AdjustThickness();
	CalculateInnerRect();
}

void Doughnut::SetParameter(long parameterId, _variant_t parameterValue)
{
	switch (parameterId)
	{
	case P_Type:
		assert( DoughnutROI == parameterValue.lVal );
		break;
	case P_SideThickness:
		m_TwiceSideThickness = parameterValue.lVal * 2;
		break;
	case P_TopThickness:
		m_TwiceTopThickness = parameterValue.lVal * 2;
		break;
	default:
		EllipseObject::SetParameter(parameterId, parameterValue);
		break;
	}
}

void Doughnut::AdjustThickness(long x, long y)
{
	CSize MinSize = GetMinSize();
	m_TwiceSideThickness +=x;
	m_TwiceTopThickness +=y;

	m_TwiceSideThickness = std::max<long>( MinSize.cx, m_TwiceSideThickness );
	m_TwiceTopThickness = std::max<long>( MinSize.cy, m_TwiceTopThickness );
	m_TwiceSideThickness = std::min<long>( m_rect.Width() - GetMinSize().cx, m_TwiceSideThickness );
	m_TwiceTopThickness = std::min<long>( m_rect.Height() - GetMinSize().cy, m_TwiceTopThickness );
}

void Doughnut::CalculateInnerRect()
{
	m_InnerRect = m_rect;
	m_InnerRect.DeflateRect(m_TwiceSideThickness/2, m_TwiceTopThickness/2);
}

void Doughnut::GetParameter(VariantParamMap& ParameterMap) const
{
	EllipseObject::GetParameter(ParameterMap);

	ParameterMap[P_Type] = static_cast<long>(DoughnutROI);
	ParameterMap[P_SideThickness] = static_cast<long>(m_TwiceSideThickness/2);
	ParameterMap[P_TopThickness] = static_cast<long>(m_TwiceTopThickness/2);
}

bool Doughnut::IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const
{
	bool bRet = false;
	long allow = GetEditAllowed();

	bool bAlMoveAndSize = AllowResizeAndMove == (allow & AllowResizeAndMove);
	bool bAlMove = AllowMove == (allow & AllowMove);
	bool bAlSize = AllowResize == (allow & AllowResize);
	bool bAlDelete = AllowDelete == (allow & AllowDelete);

	CPoint Center;
	Center.x = (m_InnerRect.right + m_InnerRect.left ) / 2;
	Center.y = (m_InnerRect.bottom + m_InnerRect.top ) / 2;

	CSize sizeHotSpot = GetHotSpotSize();
	CRect rectI(m_InnerRect);

	if( rectI.Width() > 2 * sizeHotSpot.cx && rectI.Height() > 2 * sizeHotSpot.cy )
	{
		rectI.DeflateRect( sizeHotSpot.cx, sizeHotSpot.cy );
	}

	std::unique_ptr<CRgn> pInnerEllepticalRgn( new CRgn );
	pInnerEllepticalRgn->CreateEllipticRgnIndirect( rectI );

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
		CRect rectA(m_InnerRect);
		rectA.InflateRect( sizeHotSpot.cx, sizeHotSpot.cy );
		std::unique_ptr<CRgn> pInflateElRegion( new CRgn );
		pInflateElRegion->CreateEllipticRgnIndirect( rectA );
		BOOL InflEllipse = pInflateElRegion->PtInRegion(imagePoint);
		if( InflEllipse && !bInEllipse )
		{
			bBoundary = true;
		}
	}

	if(!bRet && bBoundary)
	{
		DWORD sel = GetSelectionType( Center, imagePoint ); 
		if( bAlMoveAndSize || bAlSize )
		{
			SelType = (sel | HTX_INNER_EDGE);
			bRet = true;
		}
	}

	if( bRet == false )
	{
		bRet = ::EllipseObject::IsValidObjectAtPoint( SelType, imagePoint, viewPoint );
	}

	return bRet;
}

bool Doughnut::Move(HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint)
{
	bool ret = false;
	if(imageMovePoint.x == 0 && imageMovePoint.y == 0)
	{
		return true;
	}

	CSize MinSize = GetMinSize();
	MinSize.cx = MinSize.cx * 2;
	MinSize.cy = MinSize.cy * 2;

	switch (SelType)
	{
	case HTOBJECT:
		m_rect.OffsetRect(imageMovePoint);
		CalculateInnerRect();
		ret = true;
		MoveChild( SelType, imageMovePoint, viewMovePoint );
		break;
	case HTTOPLEFT:
		if( m_rect.bottom - (m_rect.top + imageMovePoint.y) >= MinSize.cy
			&& m_rect.right - (m_rect.left + imageMovePoint.x) >= MinSize.cx )
		{
			m_rect.top += imageMovePoint.y;
			m_rect.left += imageMovePoint.x;

			AdjustThickness( -imageMovePoint.x,-imageMovePoint.y);
			CalculateInnerRect();
			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTTOPLEFT | HTX_INNER_EDGE:
		{
			AdjustThickness( imageMovePoint.x, imageMovePoint.y );
			CalculateInnerRect();
			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTTOPRIGHT:
		if( m_rect.bottom - (m_rect.top + imageMovePoint.y) >= MinSize.cy
			&& (m_rect.right + imageMovePoint.x) - m_rect.left >= MinSize.cx )
		{
			m_rect.top += imageMovePoint.y;
			m_rect.right += imageMovePoint.x;

			AdjustThickness( imageMovePoint.x, -imageMovePoint.y );
			CalculateInnerRect();
			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTTOPRIGHT | HTX_INNER_EDGE:
		{
			AdjustThickness( -imageMovePoint.x, imageMovePoint.y );
			CalculateInnerRect();
			MoveChild(SelType,  imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTBOTTOMLEFT:
		if( (m_rect.bottom + imageMovePoint.y) - m_rect.top >= MinSize.cy 
			&& m_rect.right - (m_rect.left + imageMovePoint.x) >= MinSize.cx )
		{
			m_rect.bottom += imageMovePoint.y;
			m_rect.left += imageMovePoint.x;
			AdjustThickness( -imageMovePoint.x, imageMovePoint.y );
			CalculateInnerRect();
			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTBOTTOMLEFT | HTX_INNER_EDGE:
		{
			AdjustThickness( imageMovePoint.x,-imageMovePoint.y);
			CalculateInnerRect();
			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTBOTTOMRIGHT:
		if( (m_rect.bottom + imageMovePoint.y) - m_rect.top >= MinSize.cy 
			&& (m_rect.right + imageMovePoint.x) - m_rect.left >= MinSize.cx )
		{
			m_rect.bottom += imageMovePoint.y;
			m_rect.right += imageMovePoint.x;
			AdjustThickness( imageMovePoint.x, imageMovePoint.y);
			CalculateInnerRect();
			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTBOTTOMRIGHT | HTX_INNER_EDGE:
		{
			AdjustThickness( -imageMovePoint.x,-imageMovePoint.y);
			CalculateInnerRect();
			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTLEFT:
		{
			if( m_rect.right - (m_rect.left + imageMovePoint.x) >= MinSize.cx )
			{
				m_rect.left += imageMovePoint.x;

				AdjustThickness( -imageMovePoint.x, 0 );
				CalculateInnerRect();
				MoveChild( SelType, imageMovePoint, viewMovePoint );
				ret = true;
			}
			break;
		}
	case HTLEFT | HTX_INNER_EDGE:
		{
			AdjustThickness( imageMovePoint.x, 0 );
			CalculateInnerRect();
			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTRIGHT:
		{
			if(( m_rect.right + imageMovePoint.x) - m_rect.left >= MinSize.cx )
			{
				m_rect.right += imageMovePoint.x;
				AdjustThickness( imageMovePoint.x, 0 );
				CalculateInnerRect();
				MoveChild( SelType, imageMovePoint, viewMovePoint );
				ret = true;
			}
			break;
		}
	case HTRIGHT | HTX_INNER_EDGE:
		{
			AdjustThickness( -imageMovePoint.x, 0 );
			CalculateInnerRect();
			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTTOP:
		if( m_rect.bottom - (m_rect.top + imageMovePoint.y) >= MinSize.cx )
		{
			m_rect.top += imageMovePoint.y;
			AdjustThickness(0, -imageMovePoint.y);
			CalculateInnerRect();
			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTTOP | HTX_INNER_EDGE:
		{
			AdjustThickness( 0, imageMovePoint.y );
			CalculateInnerRect();
			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTBOTTOM:
		if( (m_rect.bottom + imageMovePoint.y) - m_rect.top >= MinSize.cy )
		{
			m_rect.bottom += imageMovePoint.y;
			AdjustThickness(0, imageMovePoint.y);
			CalculateInnerRect();
			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTBOTTOM | HTX_INNER_EDGE:
		{
			AdjustThickness( 0, -imageMovePoint.y );
			CalculateInnerRect();
			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	default:
		break; // Do nothing.
	}

	return ret;
}

void Doughnut::Draw( POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected )
{
	m_zoomWidth = p_fZoomWidth;
	m_zoomHeight = p_fZoomHeight;

	CPen Pen;
	if( p_bSelected )
	{
		Pen.CreatePen( PS_DOT, m_PenWidth, m_SelectedColor );
	}
	else
	{

		Pen.CreatePen( PS_SOLID, m_PenWidth, m_color );
	}
	int  OldBKMode = rDC.SetBkMode( TRANSPARENT );
	CPen* pOldPen = rDC.SelectObject( &Pen );
	CBrush* pOldBrush = (CBrush*)rDC.SelectStockObject( NULL_BRUSH );

	CRect OuterRect(m_rect);
	CRect InnerRect(m_InnerRect);
	TranslateRect(p_Offset, p_fZoomWidth, p_fZoomHeight, OuterRect);
	TranslateRect(p_Offset, p_fZoomWidth, p_fZoomHeight, InnerRect);

	rDC.Ellipse( &InnerRect );
	rDC.Ellipse( &OuterRect );
	rDC.SelectObject( pOldBrush );
	rDC.SelectObject( pOldPen );
	rDC.SetBkMode( OldBKMode );

	DrawObject::DrawChildren( p_Offset,  p_fZoomWidth, p_fZoomHeight, rDC, p_bSelected );
}
#pragma endregion Public Methods

