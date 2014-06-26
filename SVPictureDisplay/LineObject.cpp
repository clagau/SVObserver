//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : LineObject
//* .File Name       : $Workfile:   LineObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:26:42  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "LineObject.h"
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
LineObject::LineObject()
{
	m_StartPos.x = 0;
	m_StartPos.y = 0;
	m_EndPos.x = 0;
	m_EndPos.y = 0;
}

LineObject::~LineObject()
{
}

LineObject::LineObject( long x1, long y1, long x2, long y2, COLORREF color, long lAllowEdit )
{
	m_StartPos.x = x1;
	m_StartPos.y = y1;
	m_EndPos.x = x2;
	m_EndPos.y = y2;
	m_color = color;
	m_SelectedColor = color;
	SetEditAllowed(lAllowEdit);
}
#pragma region Constructor

#pragma region Public Methods
HRESULT LineObject::SetLine( long x1, long y1, long x2, long y2)
{
	m_StartPos.x = x1;
	m_StartPos.y = y1;
	m_EndPos.x = x2;
	m_EndPos.y = y2;
	return S_OK;
}

void LineObject::Draw( POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected )
{
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
	POINT l_newStartPos;
	POINT l_newEndPos;
	l_newStartPos.x = static_cast<long>( (m_StartPos.x - p_Offset.x) * p_fZoomWidth );
	l_newStartPos.y = static_cast<long>( (m_StartPos.y - p_Offset.y) * p_fZoomHeight );
	l_newEndPos.x = static_cast<long>( (m_EndPos.x - p_Offset.x) * p_fZoomWidth );
	l_newEndPos.y = static_cast<long>( (m_EndPos.y - p_Offset.y) * p_fZoomHeight );
	rDC.MoveTo( l_newStartPos );
	rDC.LineTo( l_newEndPos );
	rDC.SelectObject( l_pOldPen );
	rDC.SetBkMode( l_OldBKMode );

	DrawObject::DrawChildren( p_Offset,  p_fZoomWidth, p_fZoomHeight, rDC, p_bSelected );
}

bool LineObject::IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const
{
	bool bRet = false;

	long l_lAllow = GetEditAllowed();

	if( l_lAllow > 0 )
	{
		CRect l_rec( m_StartPos.x, m_StartPos.y, m_EndPos.x, m_EndPos.y);

		//if rectangle width == 0, increase it by one
		if (m_StartPos.x == m_EndPos.x)
		{
			l_rec.right++;
		}
		//if rectangle heigth == 0, increase it by one
		if (m_StartPos.y == m_EndPos.y)
		{
			l_rec.bottom++;
		}

		// Make sure we can deflate this rect.
		if( l_rec.Width() > 2 * GRAB_SIZE && l_rec.Height() > 2 * GRAB_SIZE )
		{
			l_rec.DeflateRect( GRAB_SIZE, GRAB_SIZE );
		}

		// Check if we are inside
		if( l_rec.PtInRect( imagePoint ) )
		{
			if( (l_lAllow & AllowMove) == AllowMove )	// Allowed to move...
			{
				SelType = HTOBJECT;
				bRet = true;
			}
			else
				if( (l_lAllow & AllowDelete) == AllowDelete)	// Allowed to delete but not move...
				{
					SelType = HTSELECTED;
					bRet = true;
				}
		}

		if( (l_lAllow & AllowResizeAndMove) == AllowResizeAndMove )	// Allowed to move and resize.
		{
			// check for upper left corner
			if( bRet == false )
			{
				CRect l_UpperLeft;
				l_UpperLeft.top = m_StartPos.y - GRAB_SIZE;
				l_UpperLeft.left = m_StartPos.x - GRAB_SIZE;
				l_UpperLeft.right = m_StartPos.x + GRAB_SIZE;
				l_UpperLeft.bottom = m_StartPos.y + GRAB_SIZE;

				if( l_UpperLeft.PtInRect( imagePoint ) )
				{
					SelType = HTTOPLEFT;
					bRet = true;
				}
			}

			// check for upper right corner
			if( bRet == false )
			{
				CRect l_UpperRight;
				l_UpperRight.top = m_StartPos.y - GRAB_SIZE;
				l_UpperRight.left = m_EndPos.x - GRAB_SIZE;
				l_UpperRight.right = m_EndPos.x + GRAB_SIZE;
				l_UpperRight.bottom = m_StartPos.y + GRAB_SIZE;

				if( l_UpperRight.PtInRect( imagePoint ) )
				{
					SelType = HTTOPRIGHT;
					bRet = true;
				}
			}

			// check for bottom left corner
			if( bRet == false )
			{
				CRect l_BottomLeft;
				l_BottomLeft.top = m_EndPos.y - GRAB_SIZE;
				l_BottomLeft.left = m_StartPos.x - GRAB_SIZE;
				l_BottomLeft.right = m_StartPos.x + GRAB_SIZE;
				l_BottomLeft.bottom = m_EndPos.y + GRAB_SIZE;

				if( l_BottomLeft.PtInRect( imagePoint ) )
				{
					SelType = HTBOTTOMLEFT;
					bRet = true;
				}
			}
		}
		if( (l_lAllow & AllowResize) == AllowResize )	// Only allowed to resize...
		{
			// check for bottom right corner
			if( bRet == false )
			{
				CRect l_BottomRight;
				l_BottomRight.top = m_EndPos.y - GRAB_SIZE;
				l_BottomRight.left = m_EndPos.x - GRAB_SIZE;
				l_BottomRight.right = m_EndPos.x + GRAB_SIZE;
				l_BottomRight.bottom = m_EndPos.y + GRAB_SIZE;

				if( l_BottomRight.PtInRect( imagePoint ) )
				{
					SelType = HTBOTTOMRIGHT;
					bRet = true;
				}
			}
		}
	}

	if( bRet == false )
	{
		bRet = IsValidChildObjectAtPoint( SelType, imagePoint, viewPoint );
	}

	return bRet;
}

bool LineObject::Move(HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint)
{
	switch (SelType)
	{
	case HTBOTTOMLEFT:
		m_EndPos.y += imageMovePoint.y;
		m_StartPos.x += imageMovePoint.x;
		MoveChild( HTBOTTOMLEFT, imageMovePoint, viewMovePoint );
		return true;
	case HTBOTTOMRIGHT:
		m_EndPos.x += imageMovePoint.x;
		m_EndPos.y += imageMovePoint.y;
		MoveChild( HTBOTTOMRIGHT, imageMovePoint, viewMovePoint );
		return true;
	case HTTOPLEFT:
		m_StartPos.x += imageMovePoint.x;
		m_StartPos.y += imageMovePoint.y;
		MoveChild( HTTOPLEFT, imageMovePoint, viewMovePoint );
		return true;
	case HTTOPRIGHT:
		m_StartPos.y += imageMovePoint.y;
		m_EndPos.x += imageMovePoint.x;
		MoveChild( HTTOPRIGHT, imageMovePoint, viewMovePoint );
		return true;
	case HTOBJECT:
		m_StartPos.x += imageMovePoint.x;
		m_StartPos.y += imageMovePoint.y;
		m_EndPos.x += imageMovePoint.x;
		m_EndPos.y += imageMovePoint.y;
		MoveChild( HTOBJECT, imageMovePoint, viewMovePoint );
		return true;
	default:
		break;
	}

	return false;
}

void LineObject::SetParameter(long parameterId, _variant_t parameterValue)
{
	switch (parameterId)
	{
	case P_Type:
		ASSERT( LineROI == parameterValue.lVal );
		break;
	case P_X1:
		m_StartPos.x = parameterValue.lVal;
		break;
	case P_Y1:
		m_StartPos.y = parameterValue.lVal;
		break;
	case P_X2:
		m_EndPos.x = parameterValue.lVal;
		break;
	case P_Y2:
		m_EndPos.y = parameterValue.lVal;
		break;
	default:
		DrawObject::SetParameter(parameterId, parameterValue);
		break;
	}
}

void LineObject::GetParameter(VariantParamMap& ParameterMap) const
{
	DrawObject::GetParameter(ParameterMap);

	ParameterMap[P_Type] = static_cast<long>(LineROI);
	ParameterMap[P_X1] = static_cast<long>(m_StartPos.x);
	ParameterMap[P_Y1] = static_cast<long>(m_StartPos.y);
	ParameterMap[P_X2] = static_cast<long>(m_EndPos.x);
	ParameterMap[P_Y2] = static_cast<long>(m_EndPos.y);
}

void LineObject::GetLine( POINT& p_rStart, POINT& p_rEnd ) const
{
	p_rStart = m_StartPos;
	p_rEnd = m_EndPos;
}

RECT LineObject::GetRectangle() const
{
	CRect lRect( m_StartPos, m_EndPos );

	return lRect;
}
#pragma endregion Public Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVPictureDisplay\LineObject.cpp_v  $
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
 *    Rev 1.2   16 Jun 2010 11:58:50   jspila
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  8
 * SCR Title:  Font Training
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated Picture Control to be able to used a different color for selected objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 May 2010 07:06:48   tbair
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  3
 * SCR Title:  Develop Picture Display Control
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed move functions to return true if they can be resized or moved. There is now a limit on minimum size.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Mar 2010 13:24:04   jspila
 * Project:  SVEZConfig
 * Change Request (SCR) nbr:  1
 * SCR Title:  Initial Check-in of Prototype
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial Check-in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/