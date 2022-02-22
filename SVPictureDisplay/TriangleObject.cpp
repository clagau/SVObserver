//*****************************************************************************
/// \copyright (c) 2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Class of draw and control a triangle object.
//*****************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "TriangleObject.h"
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

#pragma region Constructor
TriangleObject::TriangleObject()
{
	for (auto& point : m_points)
	{
		point.x = 0;
		point.y = 0;
	}
}

TriangleObject::~TriangleObject()
{}
#pragma region Constructor

#pragma region Public Methods
void TriangleObject::Draw(POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected)
{
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
	std::array<POINT, 3> newPoint;
	for (int i = 0; i < 3; ++i)
	{
		newPoint[i].x = static_cast<long>((m_points[i].x - p_Offset.x) * p_fZoomWidth);
		newPoint[i].y = static_cast<long>((m_points[i].y - p_Offset.y) * p_fZoomHeight);
	}
	rDC.MoveTo(newPoint[0]);
	rDC.LineTo(newPoint[1]);
	rDC.LineTo(newPoint[2]);
	rDC.LineTo(newPoint[0]);
	rDC.SelectObject(l_pOldPen);
	rDC.SetBkMode(l_OldBKMode);

	DrawObject::DrawChildren(p_Offset, p_fZoomWidth, p_fZoomHeight, rDC, p_bSelected);
}

bool isPointSelected(const CPoint& searchPoint, const CPoint& imagePoint)
{
	CRect rect {searchPoint.x - GRAB_SIZE, searchPoint.y - GRAB_SIZE, searchPoint.x + GRAB_SIZE, searchPoint.y + GRAB_SIZE};

	return rect.PtInRect(imagePoint);
}

bool TriangleObject::IsValidObjectAtPoint(HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint) const
{
	long lAllow = GetEditAllowed();

	if (0 < (lAllow & ~AllowMove))
	{
		CRect rec {GetRectangle()};

		for (int i = 0; i < m_points.size(); ++i)
		{
			if (isPointSelected(m_points[i], imagePoint))
			{
				SelType = HTX_POINT + i;
				return true;
			}
		}
	}

	CRect rec {GetRectangle()};
	// Make sure we can deflate this rect.
	if (rec.Width() > 2 * GRAB_SIZE && rec.Height() > 2 * GRAB_SIZE)
	{
		rec.DeflateRect(GRAB_SIZE, GRAB_SIZE);
	}

	// Check if we are inside
	if (rec.PtInRect(imagePoint))
	{
		if ((lAllow & AllowMove) == AllowMove)	// Allowed to move...
		{
			SelType = HTOBJECT;
			return true;
		}
		else if ((lAllow & AllowDelete) == AllowDelete)	// Allowed to delete but not move...
		{
			SelType = HTSELECTED;
			return true;
		}
	}
		
	return IsValidChildObjectAtPoint(SelType, imagePoint, viewPoint);
}

bool TriangleObject::Move(HTTYPE SelType, POINT imageMovePoint, const POINT& viewMovePoint)
{
	if (HTX_POINT == (SelType & HTX_POINT))
	{
		int pos = SelType - HTX_POINT;
		if (0 <= pos && m_points.size() > pos)
		{
			m_points[pos].x += imageMovePoint.x;
			m_points[pos].y += imageMovePoint.y;
			MoveChild(SelType, imageMovePoint, viewMovePoint);
			return true;
		}
	}
	else if (HTOBJECT == SelType)
	{
		for (auto& rPoint : m_points)
		{
			rPoint.x += imageMovePoint.x;
			rPoint.y += imageMovePoint.y;
		}
		MoveChild(HTOBJECT, imageMovePoint, viewMovePoint);
		return true;
	}

	return false;
}

void TriangleObject::SetParameter(long parameterId, _variant_t parameterValue)
{
	switch (parameterId)
	{
		case P_Type:
			assert(TriangleROI == parameterValue.lVal);
			break;
		case P_X1:
			m_points[0].x = parameterValue.lVal;
			break;
		case P_Y1:
			m_points[0].y = parameterValue.lVal;
			break;
		case P_X2:
			m_points[1].x = parameterValue.lVal;
			break;
		case P_Y2:
			m_points[1].y = parameterValue.lVal;
			break;
		case P_X3:
			m_points[2].x = parameterValue.lVal;
			break;
		case P_Y3:
			m_points[2].y = parameterValue.lVal;
			break;
		default:
			DrawObject::SetParameter(parameterId, parameterValue);
			break;
	}
}

void TriangleObject::GetParameter(VariantParamMap& ParameterMap) const
{
	DrawObject::GetParameter(ParameterMap);

	ParameterMap[P_Type] = static_cast<long>(TriangleROI);
	ParameterMap[P_X1] = static_cast<long>(m_points[0].x);
	ParameterMap[P_Y1] = static_cast<long>(m_points[0].y);
	ParameterMap[P_X2] = static_cast<long>(m_points[1].x);
	ParameterMap[P_Y2] = static_cast<long>(m_points[1].y);
	ParameterMap[P_X3] = static_cast<long>(m_points[2].x);
	ParameterMap[P_Y3] = static_cast<long>(m_points[2].y);
}

RECT TriangleObject::GetRectangle() const
{
	auto [minX, maxX] = std::minmax<int>({m_points[0].x, m_points[1].x, m_points[2].x});
	auto [minY, maxY] = std::minmax<int>({m_points[0].y, m_points[1].y, m_points[2].y});
	CRect lRect(minX, minY, maxX, maxY);

	return lRect;
}
#pragma endregion Public Methods

