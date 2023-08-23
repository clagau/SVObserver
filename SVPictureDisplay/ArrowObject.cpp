//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVStatusLibrary/MessageManagerHelper.h"
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
#pragma endregion Constructor

// This will draw the Line and the Arrow head
void ArrowObject::Draw( POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected )
{
	CPen Pen;
	if( p_bSelected )
	{
		Pen.CreatePen( PS_DOT, m_PenWidth, m_SelectedColor );
	}
	else
	{
		Pen.CreatePen( PS_SOLID, m_PenWidth, m_color );
	}
	int OldBKMode = rDC.SetBkMode( TRANSPARENT );
	CPen* pOldPen = rDC.SelectObject( &Pen );
	POINT ArrowStartPos;
	POINT ArrowEndPos;
	POINT TmpPos;

	auto number = std::min<size_t>({m_StartPosX.size(), m_StartPosY.size(), m_EndPosX.size(), m_EndPosY.size()});
	for (int i = 0; i < number; ++i)
	{
		switch (m_eOrient)
		{
			case LeftToRight:
			{
				// Right oriented arrow ...
				ArrowStartPos.x = static_cast<long>((m_StartPosX[i] - p_Offset.x) * p_fZoomWidth);
				ArrowStartPos.y = static_cast<long>((m_StartPosY[i] + (m_EndPosY[i] - m_StartPosY[i]) / 2 - p_Offset.y) * p_fZoomHeight);
				ArrowEndPos.x = static_cast<long>((m_EndPosX[i] - p_Offset.x) * p_fZoomWidth);
				ArrowEndPos.y = static_cast<long>((m_StartPosY[i] + (m_EndPosY[i] - m_StartPosY[i]) / 2 - p_Offset.y) * p_fZoomHeight);
				rDC.MoveTo(ArrowStartPos);
				rDC.LineTo(ArrowEndPos);
				TmpPos.x = ArrowEndPos.x - SVArrowPixels;
				TmpPos.y = ArrowEndPos.y - SVArrowPixels;
				rDC.LineTo(TmpPos);
				TmpPos.y = ArrowEndPos.y + SVArrowPixels;
				rDC.MoveTo(ArrowEndPos);
				rDC.LineTo(TmpPos);
				break;
			}
			case RightToLeft:
			{
				// Left oriented arrow ...
				ArrowStartPos.x = static_cast<long>((m_EndPosX[i] - p_Offset.x) * p_fZoomWidth);
				ArrowStartPos.y = static_cast<long>((m_StartPosY[i] + (m_EndPosY[i] - m_StartPosY[i]) / 2 - p_Offset.y) * p_fZoomHeight);
				ArrowEndPos.x = static_cast<long>((m_StartPosX[i] - p_Offset.x) * p_fZoomWidth);
				ArrowEndPos.y = static_cast<long>((m_StartPosY[i] + (m_EndPosY[i] - m_StartPosY[i]) / 2 - p_Offset.y) * p_fZoomHeight);
				rDC.MoveTo(ArrowStartPos);
				rDC.LineTo(ArrowEndPos);
				TmpPos.x = ArrowEndPos.x + SVArrowPixels;
				TmpPos.y = ArrowEndPos.y + SVArrowPixels;
				rDC.LineTo(TmpPos);
				TmpPos.y = ArrowEndPos.y - SVArrowPixels;
				rDC.MoveTo(ArrowEndPos);
				rDC.LineTo(TmpPos);
				break;
			}
			case BottomToTop:
			{
				// Top oriented arrow ...
				ArrowStartPos.x = static_cast<long>((m_StartPosX[i] + (m_EndPosX[i] - m_StartPosX[i]) / 2 - p_Offset.x) * p_fZoomWidth);
				ArrowStartPos.y = static_cast<long>((m_EndPosY[i] - p_Offset.y) * p_fZoomHeight);
				ArrowEndPos.x = static_cast<long>((m_StartPosX[i] + (m_EndPosX[i] - m_StartPosX[i]) / 2 - p_Offset.x) * p_fZoomWidth);
				ArrowEndPos.y = static_cast<long>((m_StartPosY[i] - p_Offset.y) * p_fZoomHeight);
				rDC.MoveTo(ArrowStartPos);
				rDC.LineTo(ArrowEndPos);
				TmpPos.x = ArrowEndPos.x + SVArrowPixels;
				TmpPos.y = ArrowEndPos.y + SVArrowPixels;
				rDC.LineTo(TmpPos);
				rDC.MoveTo(ArrowEndPos);
				TmpPos.x = ArrowStartPos.x - SVArrowPixels;
				rDC.LineTo(TmpPos);
				break;
			}
			case TopToBottom:
			{
				// Bottom oriented arrow ...
				ArrowStartPos.x = static_cast<long>((m_StartPosX[i] + (m_EndPosX[i] - m_StartPosX[i]) / 2 - p_Offset.x) * p_fZoomWidth);
				ArrowStartPos.y = static_cast<long>((m_StartPosY[i] - p_Offset.y) * p_fZoomHeight);
				ArrowEndPos.x = static_cast<long>((m_StartPosX[i] + (m_EndPosX[i] - m_StartPosX[i]) / 2 - p_Offset.x) * p_fZoomWidth);
				ArrowEndPos.y = static_cast<long>((m_EndPosY[i] - p_Offset.y) * p_fZoomHeight);
				rDC.MoveTo(ArrowStartPos);
				rDC.LineTo(ArrowEndPos);
				TmpPos.x = ArrowEndPos.x - SVArrowPixels;
				TmpPos.y = ArrowEndPos.y - SVArrowPixels;
				rDC.LineTo(TmpPos);
				rDC.MoveTo(ArrowEndPos);
				TmpPos.x = ArrowEndPos.x + SVArrowPixels;
				rDC.LineTo(TmpPos);
				break;
			}
		}
	}
	rDC.SelectObject( pOldPen );
	rDC.SetBkMode( OldBKMode );

	DrawObject::DrawChildren( p_Offset,  p_fZoomWidth, p_fZoomHeight, rDC, p_bSelected );
}

void ArrowObject::SetParameter(long parameterId, _variant_t parameterValue)
{
	switch (parameterId)
	{
	case P_Type:
		Log_Assert( ArrowROI == parameterValue.lVal );
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

