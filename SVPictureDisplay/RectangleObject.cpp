//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RectangleObject
//* .File Name       : $Workfile:   RectangleObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:26:42  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "RectangleObject.h"
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
RectangleObject::RectangleObject()
	:m_rect(0,0,0,0)
{
}

RectangleObject::~RectangleObject()
{
}

RectangleObject::RectangleObject( long left, long top, long right, long bottom, COLORREF color, long lAllowEdit )
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

#pragma region Public Methods
HRESULT RectangleObject::SetRectangle(long left, long top, long right, long bottom)
{
	m_rect.left = left;
	m_rect.top = top;
	m_rect.right = right;
	m_rect.bottom = bottom;
	return S_OK;
}

void RectangleObject::Draw( POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected )
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

	rDC.MoveTo( l_NewRec.left, l_NewRec.top );
	rDC.LineTo( l_NewRec.left, l_NewRec.bottom );
	rDC.LineTo( l_NewRec.right, l_NewRec.bottom );
	rDC.LineTo( l_NewRec.right, l_NewRec.top );
	rDC.LineTo( l_NewRec.left, l_NewRec.top );

	rDC.SelectObject( l_pOldPen );
	rDC.SetBkMode( l_OldBKMode );

	// We could use some of these functions instead of lineto moveto
	//rDC.DrawFocusRect(
	//rDC.DrawDragRect(
	//rDC.Rectangle(
	//HBRUSH hBrush = ::GetStockObject( NULL_BRUSH );
	//HBRUSH hOldBrush = SelectObject( hBrush );
	DrawObject::DrawChildren( p_Offset,  p_fZoomWidth, p_fZoomHeight, rDC, p_bSelected );
}

bool RectangleObject::IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const
{
	bool bRet = false;
	long allow = GetEditAllowed();

	bool bAlMoveAndSize = 0 != (allow & AllowResizeAndMove);
	bool bAlMove = 0 != (allow & AllowMove);
	bool bAlSize = 0 != (allow & AllowResize);
	bool bAlDelete = 0 != (allow & AllowDelete);
	
	CRect rect(m_rect);
	CPoint ptTopLeft = rect.TopLeft();
	CPoint ptTopRight = CPoint( rect.right, rect.top );
	CPoint ptBottomLeft = CPoint( rect.left, rect.bottom );
	CPoint ptBottomRight = rect.BottomRight();

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
	else if ( bAlMove &&  rect.PtInRect( imagePoint ) )
	{	
		SelType = HTOBJECT;
		bRet = true;
	}
	else if ( bAlDelete &&  rect.PtInRect( imagePoint ) )
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

RECT RectangleObject::GetRectangle() const
{
	return m_rect;
}

bool RectangleObject::Move(HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint)
{
	bool ret = false;
	if(imageMovePoint.x == 0 && imageMovePoint.y == 0)
		return true;
	CSize MinSize = GetMinSize();

	switch (SelType)
	{
	case HTOBJECT:
		m_rect.OffsetRect(imageMovePoint);
		ret = true;
		MoveChild( SelType, imageMovePoint, viewMovePoint );
		break;
	case HTTOPLEFT:
		if( m_rect.bottom - (m_rect.top + imageMovePoint.y) >= MinSize.cy
			&& m_rect.right - (m_rect.left + imageMovePoint.x) >= MinSize.cx )
		{
			m_rect.top += imageMovePoint.y;
			m_rect.left += imageMovePoint.x;
			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTTOPRIGHT:
		if( m_rect.bottom - (m_rect.top + imageMovePoint.y)  >= MinSize.cy
			&& (m_rect.right + imageMovePoint.x) - m_rect.left >= MinSize.cx )
		{
			m_rect.top += imageMovePoint.y;
			m_rect.right += imageMovePoint.x;
			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTBOTTOMLEFT:
		if( (m_rect.bottom + imageMovePoint.y) - m_rect.top >= MinSize.cy
			&&  m_rect.right - (m_rect.left + imageMovePoint.x) >= MinSize.cx )
		{
			m_rect.bottom += imageMovePoint.y;
			m_rect.left += imageMovePoint.x;
			MoveChild( SelType,  imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTBOTTOMRIGHT:
		if( (m_rect.bottom + imageMovePoint.y) - m_rect.top >= MinSize.cy
			&& (m_rect.right + imageMovePoint.x) - m_rect.left >= MinSize.cx )
		{
			m_rect.bottom += imageMovePoint.y;
			m_rect.right += imageMovePoint.x;
			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTLEFT:
		{
			if( m_rect.right - (m_rect.left + imageMovePoint.x) >= MinSize.cx )
			{
				m_rect.left += imageMovePoint.x;
				MoveChild( SelType,  imageMovePoint, viewMovePoint );
				ret = true;
			}
			break;
		}
	case HTRIGHT:
		{
			if( ( m_rect.right + imageMovePoint.x ) - m_rect.left >= MinSize.cx )
			{
				m_rect.right += imageMovePoint.x;
				MoveChild( SelType, imageMovePoint, viewMovePoint );
				ret = true;
			}
			break;
		}
	case HTTOP:
		if( m_rect.bottom - (m_rect.top + imageMovePoint.y) >= MinSize.cy )
		{
			m_rect.top += imageMovePoint.y;
			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	case HTBOTTOM:
		if( (m_rect.bottom + imageMovePoint.y) - m_rect.top >= MinSize.cy )
		{
			m_rect.bottom += imageMovePoint.y;

			MoveChild( SelType, imageMovePoint, viewMovePoint );
			ret = true;
		}
		break;
	default:
		break; // Do nothing.
	}

	return ret;
}

void RectangleObject::SetParameter(long parameterId, _variant_t parameterValue)
{
	switch (parameterId)
	{
	case P_Type:
		ASSERT( RectangleROI == parameterValue.lVal );
		break;
	case P_X1:
		m_rect.left = parameterValue.lVal;
		break;
	case P_Y1:
		m_rect.top = parameterValue.lVal;
		break;
	case P_X2:
		m_rect.right = parameterValue.lVal;
		break;
	case P_Y2:
		m_rect.bottom = parameterValue.lVal;
		break;
	default:
		DrawObject::SetParameter(parameterId, parameterValue);
		break;
	}
}

void RectangleObject::GetParameter(VariantParamMap& ParameterMap) const
{
	DrawObject::GetParameter(ParameterMap);

	ParameterMap[P_Type] = static_cast<long>(RectangleROI);
	ParameterMap[P_X1] = m_rect.left;
	ParameterMap[P_Y1] = m_rect.top;
	ParameterMap[P_X2] = m_rect.right;
	ParameterMap[P_Y2] = m_rect.bottom;
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVPictureDisplay\RectangleObject.cpp_v  $
 * 
 *    Rev 1.0   26 Jun 2014 16:26:42   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Jun 2010 11:58:52   jspila
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  8
 * SCR Title:  Font Training
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated Picture Control to be able to used a different color for selected objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 May 2010 07:06:50   tbair
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  3
 * SCR Title:  Develop Picture Display Control
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed move functions to return true if they can be resized or moved. There is now a limit on minimum size.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Mar 2010 13:24:06   jspila
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  1
 * SCR Title:  Initial Check-in of Prototype
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/