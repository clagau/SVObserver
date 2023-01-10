//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
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
LineObject::LineObject()
{
}

LineObject::~LineObject()
{
}

//LineObject::LineObject( long x1, long y1, long x2, long y2, COLORREF color, long lAllowEdit )
//{
//	m_StartPosX.emplace_back(x1, y1);
//	m_EndPos.emplace_back(x2, y2);
//	m_color = color;
//	m_SelectedColor = color;
//	SetEditAllowed(lAllowEdit);
//}
#pragma region Constructor

#pragma region Public Methods
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
	auto number = std::min<size_t>({m_StartPosX.size(), m_StartPosY.size(), m_EndPosX.size(), m_EndPosY.size()});
	for (int i = 0; i < number; ++i)
	{
		POINT newStartPos {static_cast<long>((m_StartPosX[i] - p_Offset.x) * p_fZoomWidth), static_cast<long>((m_StartPosY[i] - p_Offset.y) * p_fZoomHeight)};
		POINT newEndPos {static_cast<long>((m_EndPosX[i] - p_Offset.x) * p_fZoomWidth), static_cast<long>((m_EndPosY[i] - p_Offset.y) * p_fZoomHeight)};
		rDC.MoveTo(newStartPos);
		rDC.LineTo(newEndPos);
	}
	rDC.SelectObject( l_pOldPen );
	rDC.SetBkMode( l_OldBKMode );

	DrawObject::DrawChildren( p_Offset,  p_fZoomWidth, p_fZoomHeight, rDC, p_bSelected );
}

bool LineObject::IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const
{
	long l_lAllow = GetEditAllowed();

	if( l_lAllow > 0 )
	{
		auto number = std::min<size_t>({m_StartPosX.size(), m_StartPosY.size(), m_EndPosX.size(), m_EndPosY.size()});
		for (int i = 0; i < number; ++i)
		{
			if ((l_lAllow & AllowResizeAndMove) == AllowResizeAndMove)
			{
				CRect rect {m_StartPosX[i] - GRAB_SIZE, m_StartPosY[i] - GRAB_SIZE, m_StartPosX[i] + GRAB_SIZE, m_StartPosY[i] + GRAB_SIZE};
				if (rect.PtInRect(imagePoint))
				{
					SelType = HTX_POINT + i;
					return true;
				}

				rect = {m_EndPosX[i] - GRAB_SIZE, m_EndPosY[i] - GRAB_SIZE, m_EndPosX[i] + GRAB_SIZE, m_EndPosY[i] + GRAB_SIZE};
				if (rect.PtInRect(imagePoint))
				{
					SelType = (HTX_POINT + i) | HTX_STOP_EDGE;
					return true;
				}
			}

			if (isPointOnLine({m_StartPosX[i], m_StartPosY[i]}, {m_EndPosX[i], m_EndPosY[i]}, imagePoint))
			{
				SelType = HTX_POINT_MOVE + i;
				return true;
			}
		}
	}

	return IsValidChildObjectAtPoint( SelType, imagePoint, viewPoint );
}

bool LineObject::Move(HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint)
{
	int pos = (SelType & ~HTX_ALL_FLAG);
	auto number = std::min<size_t>({m_StartPosX.size(), m_StartPosY.size(), m_EndPosX.size(), m_EndPosY.size()});
	if (0 <= pos && number > pos)
	{
		auto flag = (SelType & HTX_ALL_FLAG);
		switch (flag)
		{
			case HTX_POINT:
				m_StartPosX[pos] += imageMovePoint.x;
				m_StartPosY[pos] += imageMovePoint.y;
				MoveChild(SelType, imageMovePoint, viewMovePoint);
				return true;
			case HTX_POINT | HTX_STOP_EDGE:
				m_EndPosX[pos] += imageMovePoint.x;
				m_EndPosY[pos] += imageMovePoint.y;
				MoveChild(SelType, imageMovePoint, viewMovePoint);
				return true;
			case HTX_POINT_MOVE:
				m_StartPosX[pos] += imageMovePoint.x;
				m_StartPosY[pos] += imageMovePoint.y;
				m_EndPosX[pos] += imageMovePoint.x;
				m_EndPosY[pos] += imageMovePoint.y;
				MoveChild(SelType, imageMovePoint, viewMovePoint);
				return true;
			default:
				break;
		}
	}

	return false;
}

std::vector<long> setPoints(const _variant_t& variantPoints)
{
	std::vector<long> retVec;
	//the variant must be an array of long or double
	if (VT_ARRAY == (variantPoints.vt & VT_ARRAY) && (VT_I4 == (variantPoints.vt & VT_I4) || VT_R8 == (variantPoints.vt & VT_R8)))
	{
		long length = variantPoints.parray->rgsabound[0].cElements;
		if (0 < length)
		{
			void* data = nullptr;
			SafeArrayAccessData(variantPoints.parray, &data);
			if (VT_I4 == (variantPoints.vt & VT_I4))
			{
				long* dataLong = reinterpret_cast<long*>(data);
				for (int i = 0; i < length; i++)
				{
					retVec.push_back(dataLong[i]);
				}
			}
			else
			{
				double* dataDouble = reinterpret_cast<double*>(data);
				for (int i = 0; i < length; i++)
				{
					retVec.push_back(static_cast<long>(dataDouble[i]));
				}
			}
			SafeArrayUnaccessData(variantPoints.parray);
		}
		else
		{
			Log_Assert(false);
		}
	}
	else
	{
		Log_Assert(false);
	}
	return retVec;
}

void LineObject::SetParameter(long parameterId, _variant_t parameterValue)
{
	switch (parameterId)
	{
	case P_Type:
		Log_Assert( LineROI == parameterValue.lVal );
		break;
	case P_X1:
		if (VT_I4 == parameterValue.vt || VT_R8 == parameterValue.vt)
		{
			m_StartPosX.push_back(static_cast<long>(parameterValue));
		}
		else
		{
			m_StartPosX = setPoints(parameterValue);
		}		
		break;
	case P_Y1:
		if (VT_I4 == parameterValue.vt || VT_R8 == parameterValue.vt)
		{
			m_StartPosY.push_back(static_cast<long>(parameterValue));
		}
		else
		{
			m_StartPosY = setPoints(parameterValue);
		}
		break;
	case P_X2:
		if (VT_I4 == parameterValue.vt || VT_R8 == parameterValue.vt)
		{
			m_EndPosX.push_back(static_cast<long>(parameterValue));
		}
		else
		{
			m_EndPosX = setPoints(parameterValue);
		}
		break;
	case P_Y2:
		if (VT_I4 == parameterValue.vt || VT_R8 == parameterValue.vt)
		{
			m_EndPosY.push_back(static_cast<long>(parameterValue));
		}
		else
		{
			m_EndPosY = setPoints(parameterValue);
		}
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
	COleSafeArray arraySafe;
	arraySafe.CreateOneDim(VT_I4, static_cast<DWORD>(m_StartPosX.size()), m_StartPosX.data());
	ParameterMap[P_X1] = arraySafe;
	arraySafe.CreateOneDim(VT_I4, static_cast<DWORD>(m_StartPosY.size()), m_StartPosY.data());
	ParameterMap[P_Y1] = arraySafe;
	arraySafe.CreateOneDim(VT_I4, static_cast<DWORD>(m_EndPosX.size()), m_EndPosX.data());
	ParameterMap[P_X2] = arraySafe;
	arraySafe.CreateOneDim(VT_I4, static_cast<DWORD>(m_EndPosY.size()), m_EndPosY.data());
	ParameterMap[P_Y2] = arraySafe;
}

RECT LineObject::GetRectangle() const
{
	auto number = std::min<size_t>({m_StartPosX.size(), m_StartPosY.size(), m_EndPosX.size(), m_EndPosY.size()});
	if (number > 0)
	{
		CRect retRec {std::min<long>(m_StartPosX[0], m_EndPosX[0]), std::min<long>(m_StartPosY[0], m_EndPosY[0]), std::max<long>(m_StartPosX[0], m_EndPosX[0]), std::max<long>(m_StartPosY[0], m_EndPosY[0])};
		for (unsigned int i = 1; i < number; ++i)
		{
			retRec.left = std::min<long>(retRec.left, m_StartPosX[i]);
			retRec.right = std::max<long>(retRec.right, m_StartPosX[i]);
			retRec.left = std::min<long>(retRec.left, m_EndPosX[i]);
			retRec.right = std::max<long>(retRec.right, m_EndPosX[i]);
			retRec.top = std::min<long>(retRec.top, m_StartPosY[i]);
			retRec.bottom = std::max<long>(retRec.bottom, m_StartPosY[i]);
			retRec.top = std::min<long>(retRec.top, m_EndPosY[i]);
			retRec.bottom = std::max<long>(retRec.bottom, m_EndPosY[i]);
		}
		return retRec;
	}
	else
	{
		return CRect(0, 0, 0, 0);
	}
}
#pragma endregion Public Methods

