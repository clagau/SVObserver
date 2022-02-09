//*****************************************************************************
/// \copyright (c) 2022 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// Class of draw and control a polypoints object.
//*****************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "PolyPointsObject.h"
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
PolyPointsObject::PolyPointsObject()
	: m_subType(SubType_Points)
{}

PolyPointsObject::~PolyPointsObject()
{}
#pragma region Constructor

#pragma region Public Methods
void PolyPointsObject::Draw(POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool bSelected)
{
	CPen pen;
	if (bSelected)
	{
		pen.CreatePen(PS_DOT, m_PenWidth, m_SelectedColor);
	}
	else
	{
		pen.CreatePen(PS_SOLID, m_PenWidth, m_color);
	}
	int oldBKMode = rDC.SetBkMode(TRANSPARENT);
	CPen* pOldPen = rDC.SelectObject(&pen);

	//create points
	auto pointVec = m_points;
	if (SubType_Polygon == m_subType && 1 < pointVec.size())
	{
		pointVec.push_back(m_points[0]);
		pointVec.push_back(m_points[1]);
	}
	for (size_t i = 0; i < pointVec.size(); i++)
	{
		pointVec[i].x = static_cast<long>((pointVec[i].x - p_Offset.x) * p_fZoomWidth);
		pointVec[i].y = static_cast<long>((pointVec[i].y - p_Offset.y) * p_fZoomHeight);
	}
	POINT* points = pointVec.data();
	size_t numPoints = pointVec.size();
	assert(INT_MAX > numPoints);
	switch (m_subType)
	{
		case SubType_Points:
			for (const auto& point : pointVec)
			{
				CPoint secondPoint {static_cast<long>(point.x + (1 * p_fZoomWidth)), static_cast<long>(point.y + (1 * p_fZoomHeight))};
				rDC.MoveTo(point);
				rDC.LineTo(secondPoint.x, secondPoint.y);
				rDC.MoveTo(point.x, secondPoint.y);
				rDC.LineTo(secondPoint.x, point.y);
			}
			break;
		case SubType_Polyline:
		case SubType_Polygon:
			rDC.Polyline(points, static_cast<int>(numPoints));
			break;
		default:
			break;
	}
	rDC.SelectObject(pOldPen);
	rDC.SetBkMode(oldBKMode);
	DrawObject::DrawChildren(p_Offset, p_fZoomWidth, p_fZoomHeight, rDC, bSelected);
}

bool PolyPointsObject::IsValidObjectAtPoint(HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint) const
{
	long lAllow = GetEditAllowed();

	if (lAllow > 0)
	{
		for (int i = 0; i < m_points.size(); ++i)
		{
			CRect rect {m_points[i].x - GRAB_SIZE, m_points[i].y - GRAB_SIZE, m_points[i].x + GRAB_SIZE, m_points[i].y + GRAB_SIZE};
			if (rect.PtInRect(imagePoint))
			{
				if (SubType_Points == m_subType)
				{
					SelType = HTX_POINT_MOVE + i;
				}
				else
				{
					SelType = HTX_POINT + i;
				}
				return true;
			}
		}

		CRect rec = GetRectangle();
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
	}

	return IsValidChildObjectAtPoint(SelType, imagePoint, viewPoint);
}

bool PolyPointsObject::Move(HTTYPE SelType, POINT imageMovePoint, const POINT& viewMovePoint)
{
	if (HTX_POINT == (SelType & HTX_POINT))
	{
		int pos = (SelType & ~HTX_POINT_MOVE);
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

void PolyPointsObject::SetParameter(const VariantParamMap& ParameterMap)
{
	DrawObject::SetParameter(ParameterMap);
}

void PolyPointsObject::SetParameter(long parameterId, _variant_t parameterValue)
{
	switch (parameterId)
	{
		case P_Type:
			assert(PolyPointsROI == parameterValue.lVal);
			break;
			//this parameter set scale option to both direction. 
			// ATTENTIONS: Should only be used if both should be the same, else it should used P_SubType_X and P_SubType_Y.
		case P_SubType:
			m_subType = static_cast<ROISubType_PolyPoints>(parameterValue.lVal);
			break;
		case P_ARRAY_XY:
			setPoints(parameterValue);
			break;
		default:
			DrawObject::SetParameter(parameterId, parameterValue);
			break;
	}
}

void PolyPointsObject::GetParameter(VariantParamMap& ParameterMap) const
{
	DrawObject::GetParameter(ParameterMap);

	ParameterMap[P_Type] = static_cast<long>(PolyPointsROI);
	ParameterMap[P_SubType] = static_cast<long>(m_subType);
	ParameterMap[P_ARRAY_XY] = getPointsAsVariant();
}

RECT PolyPointsObject::GetRectangle() const
{
	if (m_points.size() > 0)
	{
		CRect retRec {m_points[0], m_points[0]};
		for (unsigned int i = 1; i < m_points.size(); ++i)
		{
			retRec.left = std::min<long>(retRec.left, m_points[i].x);
			retRec.right = std::max<long>(retRec.right, m_points[i].x);
			retRec.top = std::min<long>(retRec.top, m_points[i].y);
			retRec.bottom = std::max<long>(retRec.bottom, m_points[i].y);
		}
		return retRec;
	}
	else
	{
		return CRect(0, 0, 0, 0);
	}
}

void PolyPointsObject::setPoints(const _variant_t& variantPoints)
{
	m_points.clear();
	//the variant must be an array of long or double
	if (VT_ARRAY == (variantPoints.vt & VT_ARRAY) && (VT_I4 == (variantPoints.vt & VT_I4) || VT_R8 == (variantPoints.vt & VT_R8)))
	{
		long length = variantPoints.parray->rgsabound[0].cElements;
		//length must be modulo 2, because it is the xy pair
		assert(0 == length % 2);
		if (1 < length)
		{
			void* data = nullptr;
			SafeArrayAccessData(variantPoints.parray, &data);
			if (VT_I4 == (variantPoints.vt & VT_I4))
			{
				long* dataLong = reinterpret_cast<long*>(data);
				for (int i = 0; i < length / 2; i++)
				{
					m_points.push_back(CPoint(dataLong[i * 2], dataLong[i * 2 + 1]));
				}
			}
			else
			{
				double* dataDouble = reinterpret_cast<double*>(data);
				for (int i = 0; i < length / 2; i++)
				{
					m_points.push_back(CPoint(static_cast<long>(dataDouble[i * 2]), static_cast<long>(dataDouble[i * 2 + 1])));
				}
			}
			SafeArrayUnaccessData(variantPoints.parray);
		}
		else
		{
			assert(false);
		}
	}
	else
	{
		assert(false);
	}
}

_variant_t PolyPointsObject::getPointsAsVariant() const
{
	if (0 < m_points.size())
	{
		long* pointList = new long[m_points.size() * 2];
		for (unsigned int i = 0; i < m_points.size(); i++)
		{
			pointList[i * 2] = m_points[i].x;
			pointList[i * 2 + 1] = m_points[i].y;
		}
		COleSafeArray arraySafe;
		size_t doubleNumPoints = m_points.size() * 2;
		assert(ULONG_MAX > doubleNumPoints);
		arraySafe.CreateOneDim(VT_I4, static_cast<DWORD>(doubleNumPoints), pointList);
		delete[] pointList;
		return arraySafe;
	}
	return 0;
}
#pragma endregion Public Methods

