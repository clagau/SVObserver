//*****************************************************************************
/// \copyright (c) 2022 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Class of draw and control a segment object.
//*****************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "SegmentObject.h"
#include "SVPictureDisplayIdl.h"
#include <algorithm>
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace
{
static const double pi = std::acos(-1);

CPoint calcPoint(CRect rect, double angle)
{
	double angleRad = pi * angle / 180.0;
	CPoint retPoint {rect.CenterPoint()};

	retPoint.x += static_cast<int>(0.5 * rect.Width() * cos(angleRad));
	retPoint.y -= static_cast<int>(0.5 * rect.Height() * sin(angleRad));
	return retPoint;
}

double calcAngle(CRect rect, CPoint p2)
{
	double dx = p2.x - rect.CenterPoint().x;
	double dy = p2.y - rect.CenterPoint().y;
	double angle = 0;
	if (-0.01 < dx && 0.01 > dx)
	{
		if (0 <= dy)
		{
			angle = -pi / 2.;
		}
		else
		{
			angle = pi / 2.;
		}
	}
	else if (0 < dx)
	{
		angle = -std::atan(dy / dx);
	}
	else // (0 > dx)
	{
		if (0 <= dy)
		{
			angle = -std::atan(dy / dx) + pi;
		}
		else
		{
			angle = -std::atan(dy / dx) - pi;
		}
	}
	auto angleDeg = angle * 180 / pi;

	while (0 > angleDeg)
	{
		angleDeg += 360;
	}

	return angleDeg;
}
}

#pragma region Constructor
SegmentObject::SegmentObject()
{}

SegmentObject::~SegmentObject()
{}

#pragma endregion Constructor

void SegmentObject::Draw(POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected)
{
	m_zoomWidth = p_fZoomWidth;
	m_zoomHeight = p_fZoomHeight;

	CPen l_Pen;
	if (p_bSelected)
	{
		l_Pen.CreatePen(PS_DOT, m_PenWidth, m_SelectedColor);
	}
	else
	{
		l_Pen.CreatePen(PS_SOLID, m_PenWidth, m_color);
	}
	int l_OldBKMode = rDC.SetBkMode(TRANSPARENT);
	CPen* l_pOldPen = rDC.SelectObject(&l_Pen);
	CRect newRec = m_rect;
	CBrush* l_pOldBrush = (CBrush*)rDC.SelectStockObject(NULL_BRUSH);

	CSize l_size = newRec.Size();
	l_size.cx = static_cast<long>(l_size.cx * p_fZoomWidth);
	l_size.cy = static_cast<long>(l_size.cy * p_fZoomHeight);
	POINT l_Offset(p_Offset);
	l_Offset.x *= -1;
	l_Offset.y *= -1;
	newRec.OffsetRect(l_Offset);

	newRec.top = static_cast<long>(newRec.top * p_fZoomHeight);
	newRec.left = static_cast<long>(newRec.left * p_fZoomWidth);
	newRec.right = static_cast<long>(newRec.right * p_fZoomWidth);
	newRec.bottom = static_cast<long>(newRec.bottom * p_fZoomHeight);

	CPoint startPoint{calcPoint(newRec, m_startAngle)};
	CPoint stopPoint {calcPoint(newRec, m_stopAngle)};

	rDC.Pie(&newRec, startPoint, stopPoint);

	rDC.SelectObject(l_pOldBrush);
	rDC.SelectObject(l_pOldPen);
	rDC.SetBkMode(l_OldBKMode);

	DrawObject::DrawChildren(p_Offset, p_fZoomWidth, p_fZoomHeight, rDC, p_bSelected);
}

bool SegmentObject::IsValidObjectAtPoint(HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint) const
{
	long allow = GetEditAllowed();

	if (AllowResize == (allow & AllowResize))
	{
		CPoint center {m_rect.CenterPoint()};
		CPoint startPoint {calcPoint(m_rect, m_startAngle)};
		if (isPointOnLine(center, startPoint, imagePoint))
		{
			SelType = HTTOPRIGHT | HTX_INNER_EDGE;
			m_moveStartPoint = imagePoint;
			return true;
		}

		CPoint stopPoint {calcPoint(m_rect, m_stopAngle)};
		if (isPointOnLine(center, stopPoint, imagePoint))
		{
			SelType = HTTOPRIGHT | HTX_STOP_EDGE;
			m_moveStartPoint = imagePoint;
			return true;
		}
	}

	return __super::IsValidObjectAtPoint(SelType, imagePoint, viewPoint);
}

bool SegmentObject::Move(HTTYPE SelType, POINT imageMovePoint, const POINT& viewMovePoint)
{
	bool ret = false;
	if (imageMovePoint.x == 0 && imageMovePoint.y == 0)
	{
		return true;
	}

	switch (SelType)
	{
		case HTTOPRIGHT | HTX_INNER_EDGE:
		{
			m_moveStartPoint = {m_moveStartPoint.x + imageMovePoint.x, m_moveStartPoint.y + imageMovePoint.y};
			m_startAngle = calcAngle(m_rect, m_moveStartPoint);
			MoveChild(SelType, imageMovePoint, viewMovePoint);
			ret = true;
		}
		break;
		case HTTOPRIGHT | HTX_STOP_EDGE:
		{
			m_moveStartPoint = {m_moveStartPoint.x + imageMovePoint.x, m_moveStartPoint.y + imageMovePoint.y};
			m_stopAngle = calcAngle(m_rect, m_moveStartPoint);
			MoveChild(SelType, imageMovePoint, viewMovePoint);
			ret = true;
		}
		break;
		default:
			return __super::Move(SelType, imageMovePoint, viewMovePoint);
	}

	return ret;
}

void SegmentObject::SetParameter(long parameterId, _variant_t parameterValue)
{
	switch (parameterId)
	{
		case P_Type:
			assert(SegmentROI == parameterValue.lVal);
			break;
		case P_StartAngle:
			m_startAngle = parameterValue;
			break;
		case P_StopAngle:
			m_stopAngle = parameterValue;
			break;
		default:
			__super::SetParameter(parameterId, parameterValue);
			break;
	}
}

void SegmentObject::GetParameter(VariantParamMap& ParameterMap) const
{
	__super::GetParameter(ParameterMap);

	ParameterMap[P_Type] = static_cast<long>(SegmentROI);

	ParameterMap[P_StartAngle] = m_startAngle;
	ParameterMap[P_StopAngle] = m_stopAngle;
}

