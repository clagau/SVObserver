//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MarkerObject
//* .File Name       : $Workfile:   MarkerObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Jun 2014 16:26:42  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "MarkerObject.h"
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
MarkerObject::MarkerObject()
	: m_subtype(VerticalMarker)
	, m_min_Value(0)
	, m_max_Value(0)
	, m_Value(0)
{
}

MarkerObject::~MarkerObject()
{
}
#pragma region Constructor

#pragma region Public Methods
void MarkerObject::Draw( POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected )
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
	double startPosX = 0;
	double startPosY = 0;
	double endPosX = m_viewSize.cx;
	double endPosY = m_viewSize.cy;
	if (VerticalMarker == m_subtype)
	{
		startPosX = GetValuePositionInImage(m_viewSize.cx);
		endPosX = startPosX;
	}
	else
	{
		startPosY = GetValuePositionInImage(m_viewSize.cy);
		endPosY = startPosY;
	}
	POINT l_newStartPos;
	POINT l_newEndPos;
	l_newStartPos.x = static_cast<long>( startPosX );
	l_newStartPos.y = static_cast<long>( startPosY );
	l_newEndPos.x = static_cast<long>( endPosX );
	l_newEndPos.y = static_cast<long>( endPosY );
	rDC.MoveTo( l_newStartPos );
	rDC.LineTo( l_newEndPos );
	rDC.SelectObject( l_pOldPen );
	rDC.SetBkMode( l_OldBKMode );

	DrawObject::DrawChildren( p_Offset,  p_fZoomWidth, p_fZoomHeight, rDC, p_bSelected );
}

bool MarkerObject::IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const
{
	bool bRet = false;
	long lAllow = GetEditAllowed();

	if( lAllow > 0 )
	{
		CRect rec = GetRectangle();
		if (VerticalMarker == m_subtype)
		{
			//increase rectangle width by moving left and right value by one
			rec.left -= GRAB_SIZE/2;
			rec.right += GRAB_SIZE/2;
		}
		else
		{
			//increase rectangle height by moving top and bottom value by one
			rec.top -= GRAB_SIZE/2;
			rec.bottom += GRAB_SIZE/2;
		}

		// Check if we are inside
		if( rec.PtInRect( viewPoint ) )
		{
			if( (lAllow & AllowMove) == AllowMove )	// Allowed to move...
			{
				SelType = HTOBJECT;
				bRet = true;
			}
			else
				if( (lAllow & AllowDelete) == AllowDelete)	// Allowed to delete but not move...
				{
					SelType = HTSELECTED;
					bRet = true;
				}
		}
	}

	if( bRet == false )
	{
		bRet = IsValidChildObjectAtPoint( SelType, imagePoint, viewPoint );
	}

	return bRet;
}

bool MarkerObject::Move(HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint)
{
	switch (SelType)
	{
	case HTOBJECT:
		if (VerticalMarker == m_subtype)
		{
			double position = GetValuePositionInImage(m_viewSize.cx);
			position += viewMovePoint.x;
			m_Value = GetValueFromPositionInImage(position, m_viewSize.cx);
		}
		else
		{
			double position = GetValuePositionInImage(m_viewSize.cy);
			position += viewMovePoint.y;
			m_Value = GetValueFromPositionInImage(position, m_viewSize.cy);
		}
		MoveChild( HTOBJECT, imageMovePoint, viewMovePoint );
		return true;
	default:
		break;
	}

	return false;
}

void MarkerObject::SetParameter(long parameterId, _variant_t parameterValue)
{
	switch (parameterId)
	{
	case P_Type:
		ASSERT( MarkerROI == parameterValue.lVal );
		break;
	case P_SubType:
		m_subtype = static_cast<ROISubType_Marker>(parameterValue.lVal);
		break;
	case P_Min:
		if (VT_I4 == parameterValue.vt)
		{
			m_min_Value = parameterValue.lVal;
		}
		if (VT_R8 == parameterValue.vt)
		{
			m_min_Value = parameterValue.dblVal;
		}
		break;
	case P_Max:
		if (VT_I4 == parameterValue.vt)
		{
			m_max_Value = parameterValue.lVal;
		}
		if (VT_R8 == parameterValue.vt)
		{
			m_max_Value = parameterValue.dblVal;
		}
		break;
	case P_VALUE:
		if (VT_I4 == parameterValue.vt)
		{
			m_Value = parameterValue.lVal;
		}
		if (VT_R8 == parameterValue.vt)
		{
			m_Value = parameterValue.dblVal;
		}
		break;
	default:
		DrawObject::SetParameter(parameterId, parameterValue);
		break;
	}
}

void MarkerObject::GetParameter(VariantParamMap& ParameterMap) const
{
	DrawObject::GetParameter(ParameterMap);

	ParameterMap[P_Type] = static_cast<long>(MarkerROI);
	ParameterMap[P_SubType] = static_cast<long>(m_subtype);
	ParameterMap[P_Min] = m_min_Value;
	ParameterMap[P_Max] = m_max_Value;
	ParameterMap[P_VALUE] = m_Value;
}

RECT MarkerObject::GetRectangle() const
{
	CRect lRect( 0, 0, m_viewSize.cx, m_viewSize.cy );
	
	if (VerticalMarker == m_subtype)
	{
		double position = GetValuePositionInImage(m_viewSize.cx);
		lRect.left = static_cast<long>(position);
		lRect.right = static_cast<long>(position);
	}
	else
	{
		double position = GetValuePositionInImage(m_viewSize.cy);
		lRect.top = static_cast<long>(position);
		lRect.bottom = static_cast<long>(position);
	}
	return lRect;
}
#pragma endregion Public Methods
#pragma region Private Methods
double MarkerObject::GetValuePositionInImage(int imageSize) const
{
	return (m_Value-m_min_Value) * imageSize / static_cast<double>(m_max_Value - m_min_Value);
}

double MarkerObject::GetValueFromPositionInImage(double position, int imageSize) const
{
	return (position * (m_max_Value - m_min_Value) / imageSize) + m_min_Value;
}
#pragma endregion Private Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVPictureDisplay\MarkerObject.cpp_v  $
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
*/