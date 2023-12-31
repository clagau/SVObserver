//******************************************************************************
//* COPYRIGHT (c) 2009 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : DrawObject
//* .File Name       : $Workfile:   DrawObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   26 Jun 2014 16:26:38  $
//******************************************************************************

#pragma region Includes
#include "stdAfx.h"
//Moved to precompiled header: #include <vector>
#include "DrawObject.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//! Note these values are set in SVPictureDisplay.idl of the SVPictureDisplay.ocx
//! To avoid direct dependencies to the project the constants are set locally
constexpr long cP_AllowEdit{ 10L };
constexpr long cP_Color{ 24L };
constexpr long cP_SelectedColor{ 25L };
#pragma endregion Declarations

DrawObject::DrawObject()
	: m_lEditAllowed(0)
	, m_zoomWidth(1.0)
	, m_zoomHeight(1.0)
	, m_imageSize(0, 0)
	, m_viewSize(0, 0)
	, m_PenWidth(1)
	, m_color(0)
	, m_SelectedColor(0)
{
}

DrawObject::~DrawObject()
{
}

HRESULT DrawObject::SetColor(COLORREF color)
{
	m_color = color;

	return S_OK;
}

HRESULT DrawObject::SetEditAllowed( long p_lAllowed )
{
	m_lEditAllowed = p_lAllowed;
	for( DrawObjectList::iterator it = m_ChildDrawObjects.begin(); it != m_ChildDrawObjects.end(); ++it )
	{
		it->second->SetEditAllowed( p_lAllowed );
	}
	return S_OK;
}

long DrawObject::GetEditAllowed() const
{
	return m_lEditAllowed;
}

long DrawObject::AddDrawObjectChild(long p_lHandle, DrawObjectRef p_DrawObject )
{
	m_ChildDrawObjects.insert( std::make_pair( p_lHandle, p_DrawObject ) );
	return S_OK;
}

void DrawObject::DrawChildren( POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CDC& rDC, bool p_bSelected )
{
	for( DrawObjectList::iterator l_it = m_ChildDrawObjects.begin(); l_it != m_ChildDrawObjects.end(); ++l_it )
	{
		l_it->second->Draw( p_Offset, p_fZoomWidth, p_fZoomHeight, rDC, p_bSelected );
	}
}

bool DrawObject::IsValidChildObjectAtPoint( HTTYPE& p_rType, const CPoint& imagePoint, const CPoint& viewPoint ) const
{
	bool l_bValid = false;
	for( DrawObjectList::const_iterator l_it = m_ChildDrawObjects.begin(); l_it != m_ChildDrawObjects.end(); ++l_it )
	{
		if( l_it->second->IsValidObjectAtPoint( p_rType, imagePoint, viewPoint ) )
		{
			l_bValid = true;
			break;
		}
	}
	return l_bValid;
}

bool DrawObject::MoveChild(HTTYPE SelType, POINT imageMovePoint, const POINT &viewMovePoint)
{
	bool l_bRet = true;
	for( DrawObjectList::iterator l_it = m_ChildDrawObjects.begin(); l_it != m_ChildDrawObjects.end(); ++l_it )
	{
		l_bRet = l_it->second->Move(SelType, imageMovePoint, viewMovePoint );
	}
	return l_bRet;
}

bool DrawObject::isPointOnLine(CPoint p1, CPoint p2, CPoint imagePoint)
{
	auto [stopLineMinX, stopLineMaxX] = std::minmax<int>(p1.x, p2.x);
	if (stopLineMinX - 1 < imagePoint.x && stopLineMaxX + 1 > imagePoint.x)
	{
		if (stopLineMaxX - stopLineMinX > 2)
		{
			double m = static_cast<double>(p1.y - p2.y) / (p1.x - p2.x);
			double ytemp = m * (imagePoint.x - p1.x) + p1.y;
			if (ytemp - 1 < imagePoint.y && ytemp + 1 > imagePoint.y)
			{
				return true;
			}
		}
		else if (imagePoint.y >= std::min<int>(p1.y, p2.y) && imagePoint.y <= std::max<int>(p1.y, p2.y))
		{	//if line to steep, do not calc m, but only check if point in rect of the two points
			return true;
		}
	}
	return false;
}

// Returns true if the handle was found and deleted.
bool DrawObject::RemoveChild( long Handle )
{
	bool bRet = false;
	DrawObjectList::iterator l_it = m_ChildDrawObjects.find( Handle );
	if( l_it != m_ChildDrawObjects.end() )
	{
		m_ChildDrawObjects.erase( l_it );
		bRet = true;
	}
	return bRet;
}

const DrawObjectList& DrawObject::GetChildList() const
{
	return m_ChildDrawObjects;
}

HRESULT DrawObject::GetBoundingRect(RECT& rec) const
{
	CRect lRect;
	lRect = GetRectangle();
	lRect.NormalizeRect();
	DrawObjectList::const_iterator l_it;
	for( l_it = m_ChildDrawObjects.begin(); l_it != m_ChildDrawObjects.end(); ++l_it )
	{
		CRect lChildRec = l_it->second->GetRectangle();
		lChildRec.NormalizeRect();
		lRect.UnionRect( lRect, lChildRec );
	}
	rec = lRect;
	return S_OK;
}

CSize DrawObject::GetHotSpotSize() const
{
	double x = HotSpotSizeX;
	if(m_zoomWidth > 0)
	{
		x /= m_zoomWidth;
	}

	double y = HotSpotSizeY;
	if(m_zoomHeight > 0)
	{
		y /= m_zoomWidth;
	}

	///make sure that the hotspotsize is at least (1 1)
	long lx = long( x + 1.0 );
	long ly = long( y+ 1.0 );

	CSize size(lx, ly);
	return size;
}

/*static*/ CSize DrawObject::GetMinSize()
{
	CSize size(MinSizeX, MinSizeY);
	return size;
}

std::unique_ptr<CRgn> DrawObject::GetHotSpot( CPoint pt ) const
{
	CRect rect;
	CSize sizeHotSpot = GetHotSpotSize();

	rect.top = pt.y - sizeHotSpot.cy;
	rect.bottom = pt.y + sizeHotSpot.cy;
	rect.left = pt.x - sizeHotSpot.cx;
	rect.right = pt.x + sizeHotSpot.cx;
	std::unique_ptr<CRgn> pRegion( new CRgn );

	pRegion->CreateRectRgnIndirect(&rect);
	return pRegion;
}

std::unique_ptr<CRgn> DrawObject::GetHotSpot( CPoint pt1, CPoint pt2 ) const
{
	std::unique_ptr<CRgn> pRegion( new CRgn );
	CSize sizeHotSpot = GetHotSpotSize();

	if ( pt1.x == pt2.x ) // vertical orientation
	{
		CRect rect;
		rect.left  = pt1.x - sizeHotSpot.cx;
		rect.right = pt1.x + sizeHotSpot.cx;
		if ( pt1.y < pt2.y )
		{
			rect.top = pt1.y;
			rect.bottom = pt2.y;
		}
		else
		{
			rect.top = pt2.y;
			rect.bottom = pt1.y;
		}
		pRegion->CreateRectRgnIndirect( &rect );
	}
	else if ( pt1.y == pt2.y ) // horizontal orientation
	{
		CRect rect;
		rect.top    = pt1.y - sizeHotSpot.cy;
		rect.bottom = pt1.y + sizeHotSpot.cy;
		if ( pt1.x < pt2.x )
		{
			rect.left = pt1.x;
			rect.right = pt2.x;
		}
		else
		{
			rect.left = pt2.x;
			rect.right = pt1.x;
		}
		pRegion->CreateRectRgnIndirect( &rect );
	}
	else
	{
		std::vector<CPoint> aPoints(4);
		if ( abs(pt1.x - pt2.x) > abs(pt1.y - pt2.y) ) // more horizontal than vertical
		{
			aPoints[0] = CPoint(pt1.x, pt1.y - sizeHotSpot.cy);
			aPoints[1] = CPoint(pt1.x, pt1.y + sizeHotSpot.cy);
			aPoints[2] = CPoint(pt2.x, pt2.y + sizeHotSpot.cy);
			aPoints[3] = CPoint(pt2.x, pt2.y - sizeHotSpot.cy);
		}
		else // more vertical than horizontal
		{
			aPoints[0] = CPoint(pt1.x + sizeHotSpot.cx, pt1.y);
			aPoints[1] = CPoint(pt1.x - sizeHotSpot.cx, pt1.y);
			aPoints[2] = CPoint(pt2.x - sizeHotSpot.cx, pt2.y);
			aPoints[3] = CPoint(pt2.x + sizeHotSpot.cx, pt2.y);
		}
		pRegion->CreatePolygonRgn(&aPoints[0], 4, WINDING);
	}

	return pRegion;
}

void DrawObject::setImageSize( CSize val )
{
	m_imageSize = val;
}

void DrawObject::setViewSize( CSize val )
{
	m_viewSize = val;
}

void DrawObject::SetParameter(const VariantParamMap& ParameterMap)
{
	VariantParamMap::const_iterator it;
	for( it = ParameterMap.begin(); it != ParameterMap.end(); ++it )
	{
		SetParameter (it->first, it->second);
	}
}

void DrawObject::SetParameter(long parameterId, _variant_t parameterValue)
{
	switch (parameterId)
	{
	case cP_AllowEdit:
		SetEditAllowed( parameterValue.lVal);
		break;
	case cP_Color:
		m_color = parameterValue.lVal;
		break;
	case cP_SelectedColor:
		m_SelectedColor = parameterValue.lVal;
		break;
	default:
		Log_Assert(FALSE);
		break;
	}
}

void DrawObject::GetParameter(VariantParamMap& ParameterMap) const
{
	ParameterMap[cP_AllowEdit] = GetEditAllowed();
	ParameterMap[cP_Color] = static_cast<long>(m_color);
	ParameterMap[cP_SelectedColor] = static_cast<long>(m_SelectedColor);
}

void DrawObject::TranslatePoints(POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, std::vector<POINT> &aPoints)
{
	for(int i=0;i< static_cast<int>(aPoints.size()); ++i)
	{
		aPoints[i].x = static_cast<long>( ( aPoints[i].x - p_Offset.x ) * p_fZoomWidth );
		aPoints[i].y = static_cast<long>( ( aPoints[i].y - p_Offset.y )* p_fZoomHeight );
	}
}

void DrawObject::TranslateRect(POINT p_Offset, double p_fZoomWidth, double p_fZoomHeight, CRect &rect) const
{
	rect.left = static_cast<long>( ( rect.left - p_Offset.x ) * p_fZoomWidth );
	rect.right = static_cast<long>( ( rect.right - p_Offset.x ) * p_fZoomWidth );
	rect.top = static_cast<long>( ( rect.top - p_Offset.y ) * p_fZoomHeight );
	rect.bottom = static_cast<long>( ( rect.bottom - p_Offset.y ) * p_fZoomHeight );
}

HTTYPE DrawObject::GetSelectionType( const POINT &center, const POINT &point) const
{
	double tan20 = 0.363970;  //tangent(20)
	double tan70 = 2.747477;  //tangent(70)
	int deltax = point.x - center.x;
	int deltay = point.y - center.y;
	deltay = -deltay;
	if(deltay == 0 && deltax == 0)
	{
		return HTOBJECT;
	}
	else if(deltax == 0)
	{
		if( deltay > 0 )
		{
			return HTTOP;
		}
		else
		{
			return HTBOTTOM;
		}
	}

	double dtan = double(deltay)/ double(deltax);
	
	if(deltax > 0)
	{
		if ( deltay == 0 )
		{
			return HTRIGHT;
		}

		if (dtan >= tan70)
		{
			return HTTOP;
		}
		else if(dtan >= tan20)
		{
			return HTTOPRIGHT;
		}
		else if (dtan >= -tan20)
		{
			return HTRIGHT;
		}
		else if (dtan >= -tan70)
		{
			return HTBOTTOMRIGHT;
		}
		else
		{
			return HTBOTTOM;
		}
	}
	else
	{
		if ( deltay == 0 )
		{
			return HTLEFT;
		}

		if (dtan >= tan70)
		{
			return HTBOTTOM;
		}
		else if(dtan >= tan20)
		{
			return HTBOTTOMLEFT;
		}
		else if (dtan >= -tan20)
		{
			return HTLEFT;
		}
		else if (dtan >= -tan70)
		{
			return HTTOPLEFT;
		}
		else
		{
			return HTTOP;
		}
	}
}

