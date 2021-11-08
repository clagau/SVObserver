//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ArrowObject
//* .File Name       : $Workfile:   ArrowObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:26:36  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "ArrowObject.h"
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
ArrowObject::ArrowObject()
{
}

ArrowObject::~ArrowObject()
{
}

ArrowObject::ArrowObject( long x1, long y1, long x2, long y2, long Orientation, COLORREF color, long lAllowEdit ) : 
	LineObject(x1, y1, x2, y2, color, lAllowEdit), 
	m_eOrient(static_cast<SVOrientation>(Orientation))
{
}
#pragma endregion Constructor

// This will draw the Line and the Arrow head
void ArrowObject::Draw( POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected )
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
	POINT l_ArrowStartPos;
	POINT l_ArrowEndPos;
	POINT TmpPos;

	switch( m_eOrient )
	{
		case LeftToRight:
		{
			// Right oriented arrow ...
			l_ArrowStartPos.x = static_cast<long>( ( m_StartPos.x - p_Offset.x) * p_fZoomWidth );
			l_ArrowStartPos.y = static_cast<long>( ( m_StartPos.y + ( m_EndPos.y - m_StartPos.y )/2 - p_Offset.y) * p_fZoomHeight );
			l_ArrowEndPos.x =   static_cast<long>( ( m_EndPos.x - p_Offset.x) * p_fZoomWidth );
			l_ArrowEndPos.y =   static_cast<long>( ( m_StartPos.y + ( m_EndPos.y - m_StartPos.y )/2 - p_Offset.y) * p_fZoomHeight );
			rDC.MoveTo( l_ArrowStartPos );
			rDC.LineTo( l_ArrowEndPos );
			TmpPos.x = l_ArrowEndPos.x - SVArrowPixels;
			TmpPos.y = l_ArrowEndPos.y - SVArrowPixels;
			rDC.LineTo( TmpPos );
			TmpPos.y = l_ArrowEndPos.y + SVArrowPixels;
			rDC.MoveTo( l_ArrowEndPos );
			rDC.LineTo( TmpPos );
			break;
		}
		case RightToLeft:
		{
			// Left oriented arrow ...
			l_ArrowStartPos.x = static_cast<long>( ( m_EndPos.x - p_Offset.x) * p_fZoomWidth );
			l_ArrowStartPos.y = static_cast<long>( ( m_StartPos.y + ( m_EndPos.y - m_StartPos.y )/2 - p_Offset.y) * p_fZoomHeight );
			l_ArrowEndPos.x =   static_cast<long>( ( m_StartPos.x - p_Offset.x) * p_fZoomWidth );
			l_ArrowEndPos.y =   static_cast<long>( ( m_StartPos.y + ( m_EndPos.y - m_StartPos.y )/2 - p_Offset.y) * p_fZoomHeight );
			rDC.MoveTo( l_ArrowStartPos );
			rDC.LineTo( l_ArrowEndPos );
			TmpPos.x = l_ArrowEndPos.x + SVArrowPixels;
			TmpPos.y = l_ArrowEndPos.y + SVArrowPixels;
			rDC.LineTo( TmpPos );
			TmpPos.y = l_ArrowEndPos.y - SVArrowPixels;
			rDC.MoveTo( l_ArrowEndPos );
			rDC.LineTo( TmpPos );
			break;
		}
		case BottomToTop:
		{
			// Top oriented arrow ...
			l_ArrowStartPos.x = static_cast<long>( (m_StartPos.x + (m_EndPos.x - m_StartPos.x)/2 - p_Offset.x) * p_fZoomWidth );
			l_ArrowStartPos.y = static_cast<long>( (m_EndPos.y - p_Offset.y) * p_fZoomHeight );
			l_ArrowEndPos.x =   static_cast<long>( (m_StartPos.x + (m_EndPos.x - m_StartPos.x)/2 - p_Offset.x) * p_fZoomWidth );
			l_ArrowEndPos.y =   static_cast<long>( (m_StartPos.y - p_Offset.y) * p_fZoomHeight );
			rDC.MoveTo( l_ArrowStartPos );
			rDC.LineTo( l_ArrowEndPos );
			TmpPos.x = l_ArrowEndPos.x + SVArrowPixels;
			TmpPos.y = l_ArrowEndPos.y + SVArrowPixels;
			rDC.LineTo( TmpPos );
			rDC.MoveTo( l_ArrowEndPos );
			TmpPos.x = l_ArrowStartPos.x - SVArrowPixels;
			rDC.LineTo( TmpPos );
			break;
		}
		case TopToBottom:
		{
			// Bottom oriented arrow ...
			l_ArrowStartPos.x = static_cast<long>( ( m_StartPos.x + (m_EndPos.x - m_StartPos.x)/2 - p_Offset.x) * p_fZoomWidth );
			l_ArrowStartPos.y = static_cast<long>( ( m_StartPos.y - p_Offset.y) * p_fZoomHeight );
			l_ArrowEndPos.x =   static_cast<long>( ( m_StartPos.x + (m_EndPos.x - m_StartPos.x)/2 - p_Offset.x) * p_fZoomWidth );
			l_ArrowEndPos.y =   static_cast<long>( ( m_EndPos.y - p_Offset.y) * p_fZoomHeight );
			rDC.MoveTo( l_ArrowStartPos );
			rDC.LineTo( l_ArrowEndPos );
			TmpPos.x = l_ArrowEndPos.x - SVArrowPixels;
			TmpPos.y = l_ArrowEndPos.y - SVArrowPixels;
			rDC.LineTo( TmpPos );
			rDC.MoveTo( l_ArrowEndPos );
			TmpPos.x = l_ArrowEndPos.x + SVArrowPixels;
			rDC.LineTo( TmpPos );
			break;
		}
	}
	rDC.SelectObject( l_pOldPen );
	rDC.SetBkMode( l_OldBKMode );

	DrawObject::DrawChildren( p_Offset,  p_fZoomWidth, p_fZoomHeight, rDC, p_bSelected );
}

void ArrowObject::SetParameter(long parameterId, _variant_t parameterValue)
{
	switch (parameterId)
	{
	case P_Type:
		assert( ArrowROI == parameterValue.lVal );
		break;
	case P_Orientation:
		m_eOrient = static_cast<SVOrientation>( parameterValue.lVal );
		break;
	default:
		LineObject::SetParameter(parameterId, parameterValue);
		break;
	}
}

void ArrowObject::GetParameter(VariantParamMap& ParameterMap) const
{
	LineObject::GetParameter(ParameterMap);

	ParameterMap[P_Type] = static_cast<long>(ArrowROI);
	ParameterMap[P_Orientation] = static_cast<long>(m_eOrient);
}

