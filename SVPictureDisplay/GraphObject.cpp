//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : GraphObject
//* .File Name       : $Workfile:   GraphObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   :     $Date:   18 Jul 2014 03:53:40  $
//******************************************************************************

#pragma region Includes
#include "StdAfx.h"
#include "GraphObject.h"
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
GraphObject::GraphObject()
: m_borderRect(0, 0, 0, 0)
, m_subType_X(ViewArea_Fit)
, m_subType_Y(ViewArea_Fit)
, m_min_x(0)
, m_max_x(0)
, m_min_y(0)
, m_max_y(0)
, m_isVerticalFlip(false)
, m_isHorizontalFlip(false)
{
}

GraphObject::~GraphObject()
{
}
#pragma region Constructor

#pragma region Public Methods
void GraphObject::Draw( POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected )
{
	CPen pen;
	if( p_bSelected )
	{
		pen.CreatePen( PS_DOT, m_PenWidth, m_SelectedColor );
	}
	else
	{
		pen.CreatePen( PS_SOLID, m_PenWidth, m_color );
	}
	int oldBKMode = rDC.SetBkMode( TRANSPARENT );
	CPen* pOldPen = rDC.SelectObject( &pen );
	//create points
	POINT* points = new POINT[m_points.size()];

	switch (m_subType_X)
	{
	case ImageArea_NoScale:
		calcPointsForDrawing(X_DIRECTION, 0, m_imageSize.cx, m_imageSize.cx, p_Offset.x, p_fZoomWidth, m_isHorizontalFlip, points, m_points.size());
		break;
	case ViewArea_Fit:
		calcPointsForDrawing(X_DIRECTION, m_borderRect.left, m_borderRect.right, m_viewSize.cx, 0, 1, m_isHorizontalFlip, points, m_points.size());
		break;
	case ViewArea_ScalePerParameter:
		calcPointsForDrawing(X_DIRECTION, m_min_x, m_max_x, m_viewSize.cx, 0, 1, m_isHorizontalFlip, points, m_points.size());
		break;
	case ImageArea_ScalePerParameter:
		calcPointsForDrawing(X_DIRECTION, m_min_x, m_max_x, m_imageSize.cx, p_Offset.x, p_fZoomWidth, m_isHorizontalFlip, points, m_points.size());
		break;
	default: //should never happens
		ASSERT(0);
		break;
	}
	switch (m_subType_Y)
	{
	case ImageArea_NoScale:
		calcPointsForDrawing(Y_DIRECTION, 0, m_imageSize.cy, m_imageSize.cy, p_Offset.y, p_fZoomHeight, m_isVerticalFlip, points, m_points.size());
		break;
	case ViewArea_Fit:
		calcPointsForDrawing(Y_DIRECTION, m_borderRect.top, m_borderRect.bottom, m_viewSize.cy, 0, 1, m_isVerticalFlip, points, m_points.size());
		break;
	case ViewArea_ScalePerParameter:
		calcPointsForDrawing(Y_DIRECTION, m_min_y, m_max_y, m_viewSize.cy, 0, 1, m_isVerticalFlip, points, m_points.size());
		break;
	case ImageArea_ScalePerParameter:
		calcPointsForDrawing(Y_DIRECTION, m_min_y, m_max_y, m_imageSize.cy, p_Offset.y, p_fZoomHeight, m_isVerticalFlip, points, m_points.size());
		break;
	default: //should never happens
		ASSERT(0);
		break;
	}

	size_t numPoints = m_points.size();
	ASSERT( INT_MAX > numPoints );
	rDC.Polyline( points, static_cast< int >( numPoints ) );
	delete[] points;
	rDC.SelectObject( pOldPen );
	rDC.SetBkMode( oldBKMode );
	DrawObject::DrawChildren( p_Offset, p_fZoomWidth, p_fZoomHeight, rDC, p_bSelected );
}

bool GraphObject::IsValidObjectAtPoint( HTTYPE& SelType, const CPoint& imagePoint, const CPoint& viewPoint ) const
{
	bool bRet = false;

	long l_lAllow = GetEditAllowed();

	if( l_lAllow > 0 )
	{
		CRect l_rec = GetRectangle();

		// Always valid if not ImageArea_NoScale or check if we are inside
		if( (ImageArea_NoScale != m_subType_X || (l_rec.left <= imagePoint.x && l_rec.right >= imagePoint.x )) && 
			(ImageArea_NoScale != m_subType_Y || (l_rec.top <= imagePoint.y && l_rec.bottom >= imagePoint.y )))
		{
			if( (l_lAllow & AllowMove) == AllowMove )	// Allowed to move...
			{
				SelType = HTOBJECT;
				bRet = true;
			}
			else if( (l_lAllow & AllowDelete) == AllowDelete)	// Allowed to delete but not move...
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

bool GraphObject::Move(HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint)
{
	switch (SelType)
	{
	case HTOBJECT:
		//only movement possible if no scale. If scale the graph is over the whole image.
		if (ImageArea_NoScale == m_subType_X || ImageArea_NoScale != m_subType_Y)
		{
			//change move if flipped
			POINT movePoint = imageMovePoint;
			if (m_isHorizontalFlip)
			{
				movePoint.x = -movePoint.x;
			}
			if (m_isVerticalFlip)
			{
				movePoint.y = -movePoint.y;
			}

			//only movement possible if no scale. If scale the graph is over the whole image.
			if (ImageArea_NoScale != m_subType_X)
			{ //remove movement if scaled
				movePoint.x = 0;
			}
			if (ImageArea_NoScale != m_subType_Y)
			{ //remove movement if scaled
				movePoint.y = 0;
			}

			for (unsigned int i=0; i<m_points.size(); i++)
			{
				m_points[i].Offset(movePoint);
			}
			m_borderRect.OffsetRect(movePoint);
			MoveChild( HTOBJECT, imageMovePoint, viewMovePoint );
		}
		return true;
	default:
		break;
	}

	return false;
}

void GraphObject::SetParameter(const VariantParamMap& ParameterMap)
{
	DrawObject::SetParameter(ParameterMap);

	calcRect();
}

void GraphObject::SetParameter(long parameterId, _variant_t parameterValue)
{
	switch (parameterId)
	{
	case P_Type:
		ASSERT( GraphROI == parameterValue.lVal );
		break;
	//this parameter set scale option to both direction. 
	// ATTENTIONS: Should only be used if both should be the same, else it should used P_SubType_X and P_SubType_Y.
	case P_SubType: 
		m_subType_X = static_cast<ROISubType_Graph>(parameterValue.lVal);
		m_subType_Y = m_subType_X;
		break;
	case P_SubType_X:
		m_subType_X = static_cast<ROISubType_Graph>(parameterValue.lVal);
		break;
	case P_SubType_Y:
		m_subType_Y = static_cast<ROISubType_Graph>(parameterValue.lVal);
		break;
	case P_ARRAY_XY:
		setPoints(parameterValue);
		break;
	case P_X_Min:
		m_min_x = parameterValue.lVal;
		break;
	case P_X_Max:
		m_max_x = parameterValue.lVal;
		break;
	case P_Y_Min:
		m_min_y = parameterValue.lVal;
		break;
	case P_Y_Max:
		m_max_y = parameterValue.lVal;
		break;
	case P_Is_Flip_Vertical:
		m_isVerticalFlip = (VARIANT_TRUE == parameterValue.boolVal);
		break;
	case P_Is_Flip_Horizontal:
		m_isHorizontalFlip = (VARIANT_TRUE == parameterValue.boolVal);
		break;
	default:
		DrawObject::SetParameter(parameterId, parameterValue);
		break;
	}
}

void GraphObject::GetParameter(VariantParamMap& ParameterMap) const
{
	DrawObject::GetParameter(ParameterMap);

	ParameterMap[P_Type] = static_cast<long>(LineROI);
	ParameterMap[P_SubType_X] = static_cast<long>(m_subType_X);
	ParameterMap[P_SubType_Y] = static_cast<long>(m_subType_Y);
	ParameterMap[P_ARRAY_XY] = getPointsAsVariant();
	ParameterMap[P_X_Min] = m_min_x;
	ParameterMap[P_X_Max] = m_max_x;
	ParameterMap[P_Y_Min] = m_min_y;
	ParameterMap[P_Y_Max] = m_max_y;
	ParameterMap[P_Is_Flip_Vertical] = m_isVerticalFlip;
	ParameterMap[P_Is_Flip_Horizontal] = m_isHorizontalFlip;
}

RECT GraphObject::GetRectangle() const
{
	CRect retRec = m_borderRect;
	if (ImageArea_NoScale == m_subType_X)
	{
		//flip rectangle in image if necessary.
		if (m_isHorizontalFlip)
		{
			retRec.left = m_imageSize.cx - m_borderRect.right;
			retRec.right = m_imageSize.cx - m_borderRect.left;
		}
	}
	else
	{
		retRec.left = 0;
		retRec.right = m_viewSize.cx;
	}
	if (ImageArea_NoScale == m_subType_Y)
	{
		if (m_isVerticalFlip)
		{
			retRec.top = m_imageSize.cy - m_borderRect.bottom;
			retRec.bottom = m_imageSize.cy - m_borderRect.top;
		}
	}
	else
	{
		retRec.top = 0;
		retRec.bottom = m_viewSize.cy;
	}
	return retRec;
}

void GraphObject::setPoints( const _variant_t& variantPoints )
{
	m_points.clear();
	//the variant must be an array of long or double
	if (VT_ARRAY == (variantPoints.vt & VT_ARRAY) && (VT_I4 == (variantPoints.vt & VT_I4) || VT_R8 == (variantPoints.vt & VT_R8)))
	{
		long length = variantPoints.parray->rgsabound[0].cElements;
		//length must be modulo 2, because it is the xy pair
		ASSERT(0 == length % 2);
		if (1 < length)
		{
			void* data = nullptr;
			SafeArrayAccessData(variantPoints.parray, &data);
			if ( VT_I4 == ( variantPoints.vt & VT_I4 ) )
			{
				long* dataLong = reinterpret_cast< long* >( data );
				for (int i = 0; i < length / 2; i++)
				{
					m_points.push_back(CPoint(dataLong[i * 2], dataLong[i * 2 + 1]));
				}
			}
			else
			{
				double* dataDouble = reinterpret_cast< double* >( data );
				for (int i = 0; i < length / 2; i++)
				{
					m_points.push_back(CPoint(static_cast<long>(dataDouble[i * 2]), static_cast<long>(dataDouble[i * 2 + 1])));
				}
			}
			SafeArrayUnaccessData(variantPoints.parray);
		}
		else
		{
			ASSERT(false);
		}
	}
	else
	{
		ASSERT(false);
	}
}

_variant_t GraphObject::getPointsAsVariant() const
{
	if (0 < m_points.size())
	{
		long* pointList = new long[m_points.size()*2];
		for (unsigned int i=0; i<m_points.size(); i++)
		{
			pointList[i*2] = m_points[i].x;
			pointList[i*2+1] = m_points[i].y;
		}
		COleSafeArray arraySafe;
		size_t doubleNumPoints = m_points.size() * 2;
		ASSERT( ULONG_MAX > doubleNumPoints );
		arraySafe.CreateOneDim( VT_I4, static_cast< DWORD >( doubleNumPoints ), pointList );
		delete[] pointList;
		return arraySafe;
	}
	return 0;
}

void GraphObject::calcRect()
{
	if (m_points.size() > 0)
	{
		//calculate rect
		m_borderRect = CRect(m_points[0], m_points[0]);
		for (unsigned int i=1; i < m_points.size(); i++)
		{
			if (m_borderRect.left > m_points[i].x)
			{
				m_borderRect.left = m_points[i].x;
			}
			if (m_borderRect.right < m_points[i].x)
			{
				m_borderRect.right = m_points[i].x;
			}
			if (m_borderRect.top > m_points[i].y)
			{
				m_borderRect.top = m_points[i].y;
			}
			if (m_borderRect.bottom < m_points[i].y)
			{
				m_borderRect.bottom = m_points[i].y;
			}
		}
	}
	else
	{
		m_borderRect = CRect(0, 0, 0, 0);
	}
}

void GraphObject::calcPointsForDrawing( DirectionEnum direction, long minValue, long maxValue, long displaySize, long globalOffset, double globalZoom, bool isFlip, POINT* const & points, size_t sizePoints )
{
	assert(m_points.size() == sizePoints);
	UNREFERENCED_PARAMETER(sizePoints);

	long localOffset = 0;
	double localZoom = displaySize / static_cast<double>(maxValue - minValue);
	if(isFlip)
	{
		localOffset -= maxValue;
		localZoom *= -1.0;
	}
	else
	{
		localOffset -= minValue;
	}

	if (X_DIRECTION == direction)
	{
		for ( size_t i = 0; i < m_points.size(); i++ )
		{
			points[i].x = static_cast<long>( (((m_points[i].x + localOffset) * localZoom)- globalOffset) * globalZoom );
		}
	}
	else
	{
		for ( size_t i = 0; i < m_points.size(); i++ )
		{
			points[i].y = static_cast<long>( (((m_points[i].y + localOffset) * localZoom) - globalOffset) * globalZoom );
		}
	}
}
#pragma endregion Public Methods

