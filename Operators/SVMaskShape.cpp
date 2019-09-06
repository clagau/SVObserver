//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaskShape
//* .File Name       : $Workfile:   SVMaskShape.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   26 Jun 2014 17:49:56  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <algorithm>
#include "SVMaskShape.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxErrorEnum.h"
#include "SVObjectLibrary/SVClsIds.h"
#include "SVLibrary/Intersect.h"
#include "Definitions/Color.h"
#pragma endregion Includes

#pragma warning ( disable : 4503 )

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVMaskShape::SVMaskShape()
{
	m_mapProperties[ SVShapeMaskPropertyCenterXGuid ] = 0;
	m_mapProperties[ SVShapeMaskPropertyCenterYGuid ] = 0;
	m_mapProperties[ SVShapeMaskPropertyWidthGuid ] = 0;
	m_mapProperties[ SVShapeMaskPropertyHeightGuid ] = 0;

	m_mapProperties[ SVShapeMaskPropertyCenterXGuid ].order = 1;
	m_mapProperties[ SVShapeMaskPropertyCenterYGuid ].order = 2;
	m_mapProperties[ SVShapeMaskPropertyWidthGuid ].order = 3;
	m_mapProperties[ SVShapeMaskPropertyHeightGuid ].order = 4;
}

SVMaskShape::~SVMaskShape()	// this is a base class
{
	if ( nullptr != m_dib.hbm )
	{
		::DeleteObject( m_dib.hbm );
		m_dib.FreeBitmapInfo();
		m_dib.Clear();
		::DeleteDC(m_RenderDC);
	}
}

HRESULT SVMaskShape::Draw( HDC hDC ) const
{
	HRESULT hr = S_OK;
	if ( nullptr != hDC )
	{
		hr = SVIHBitmapUtilities::SVHBitmapToDC(m_dib, hDC);
	}
	return hr;
}

HRESULT SVMaskShape::DrawOutline(HDC hDC, RECT rectViewport, RECT rectDisplay, COLORREF rgb) const
{
	return RenderOutline(hDC, rectViewport, rectDisplay, rgb);
}

HRESULT SVMaskShape::Draw( SVMatroxBuffer mil )
{
	HRESULT hr = S_OK;
	if ( !mil.empty() )
	{
		HRESULT MatroxCode = SVMatroxBufferInterface::CopyBuffer(  mil, m_dib.hbm );
		hr = (S_OK != MatroxCode) ? MatroxCode | SVMEE_MATROX_ERROR : S_OK;
	}
	return hr;
}

HRESULT SVMaskShape::Refresh()
{
	// take the properties and draw them on the internal bitmap
	HRESULT hr = S_OK;

	if ( m_bImageInfoChanged && nullptr != m_dib.hbm )
	{
		::DeleteObject( m_dib.hbm );
		m_dib.FreeBitmapInfo();
		m_dib.Clear();
		::DeleteDC(m_RenderDC);
	}

	if ( nullptr == m_dib.hbm )
	{
		m_RenderDC = ::CreateCompatibleDC(nullptr);
		hr = SVIHBitmapUtilities::SVImageInfoToNewDIB( m_svImageInfo, m_dib );
		m_bImageInfoChanged = false;
	}

	if ( S_OK == hr )
	{
		HBITMAP hOldBitmap = static_cast<HBITMAP> (::SelectObject(m_RenderDC, m_dib.hbm));

		// compute fore & back colors
		COLORREF rgbShape = SvDef::White;
		COLORREF rgbBackground = SvDef::Black;

		ComputeColors(rgbShape, rgbBackground);

		HBRUSH hBrush = ::CreateSolidBrush(rgbBackground);
		RECT rect;
		m_svImageInfo.GetOutputRectangle(rect);
		::FillRect(m_RenderDC, &rect, hBrush);
		::DeleteObject(hBrush);

		hBrush = ::CreateSolidBrush(rgbShape);
		HBRUSH hOldBrush = static_cast<HBRUSH> (::SelectObject(m_RenderDC, hBrush));
		HPEN hPen = ::CreatePen(PS_SOLID, 1, rgbShape);
		HPEN hOldPen = static_cast<HPEN> (::SelectObject(m_RenderDC, hPen));

		// Render Shape
		hr = Render( m_RenderDC, rgbShape, rgbBackground );

		::SelectObject(m_RenderDC, hOldPen);
		::SelectObject(m_RenderDC, hOldBrush);
		::SelectObject(m_RenderDC, hOldBitmap);

		::DeleteObject(hBrush);
		::DeleteObject(hPen);
	}

	return hr;
}// SVMaskShape::Refresh

HRESULT SVMaskShape::SetProperties(const MapType& p_mapProperties)
{
	HRESULT hr = S_FALSE;
	
	assert( m_bImageInfoSet );
	
	if ( m_bImageInfoSet )
	{
		hr = S_OK;
		MapType l_mapProperties(p_mapProperties);

		// let derived classes put constraints on the values, etc.
		hr = ValidateProperties( l_mapProperties );

		if ( S_OK == hr )//|| SVMSG_SVO_32_VALUE_OUT_OF_RANGE == hr)
		{
			MapType::const_iterator iterSource;
			MapType::iterator iterDest;

			for ( iterSource = l_mapProperties.begin(); iterSource != l_mapProperties.end(); ++iterSource )
			{
				// is each item in the input a valid property for this shape?
				iterDest = m_mapProperties.find(iterSource->first);
				if ( iterDest != m_mapProperties.end() )
				{
					iterDest->second = iterSource->second;
				}
			}
		}
	}

	return hr;
}// SVMaskShape::SetProperties

HRESULT SVMaskShape::ValidateProperties(MapType& rProperties) const
{
	HRESULT hr = S_OK;

	if (   rProperties.find( SVShapeMaskPropertyWidthGuid ) != rProperties.end()
		&& rProperties.find( SVShapeMaskPropertyHeightGuid ) != rProperties.end()
		&& rProperties.find( SVShapeMaskPropertyCenterXGuid ) != rProperties.end()
		&& rProperties.find( SVShapeMaskPropertyCenterYGuid ) != rProperties.end() )
	{
		SVImageInfoClass info;
		hr = GetImageInfo( info );
		if ( S_OK == hr)
		{
			RECT rect;
			hr = info.GetOutputRectangle(rect);
		}
		if ( S_OK == hr )
		{
			if ( S_OK == hr )
			{
				hr = rProperties[ SVShapeMaskPropertyHeightGuid ].value >= 0 ? S_OK : S_FALSE;//SVMSG_SVO_32_VALUE_OUT_OF_RANGE;
			}
			if ( S_OK == hr )
			{
				hr = rProperties[ SVShapeMaskPropertyWidthGuid  ].value >= 0 ? S_OK : S_FALSE;//SVMSG_SVO_32_VALUE_OUT_OF_RANGE;
			}
		}
	}

	return hr;
}// SVMaskShape::ValidateProperties

HRESULT SVMaskShape::GetProperties(MapType& rProperties) const
{
	rProperties = m_mapProperties;
	return S_OK;
}

HRESULT SVMaskShape::SetImageInfo( const SVImageInfoClass& svImageInfo )
{
	m_bImageInfoChanged = true;
	m_bImageInfoSet = true;
	m_svImageInfo = svImageInfo;
	return S_OK;
}

HRESULT SVMaskShape::GetImageInfo( SVImageInfoClass& rsvImageInfo ) const
{
	rsvImageInfo = m_svImageInfo;
	return S_OK;
}

HRESULT SVMaskShape::SetFillProperties( SVMaskFillPropertiesStruct svFillStruct )
{
	m_svFillStruct = svFillStruct;
	return S_OK;
}

HRESULT SVMaskShape::GetFillProperties( SVMaskFillPropertiesStruct& rsvFillStruct ) const
{
	rsvFillStruct = m_svFillStruct;
	return S_OK;
}

void SVMaskShape::ComputeColors(COLORREF& rgbShape, COLORREF& rgbBackground)
{
	if ( m_svFillStruct.bMaskInsideShape )
	{
		if ( m_svFillStruct.bFillMaskArea )
		{
			rgbShape = m_svFillStruct.rgbFillColor;
			rgbBackground = SvDef::White;
		}
		else
		{
			rgbShape = SvDef::Black;
			rgbBackground = m_svFillStruct.rgbFillColor;
		}
	}
	else // mask outside shape (default)
	{
		if ( m_svFillStruct.bFillMaskArea )
		{
			rgbShape = SvDef::White;
			rgbBackground = m_svFillStruct.rgbFillColor;
		}
		else
		{
			rgbShape = m_svFillStruct.rgbFillColor;
			rgbBackground = SvDef::Black;
		}
	}
}

RECT SVMaskShape::GetMaskImageRect() const
{
	RECT rect;
	m_svImageInfo.GetExtents().GetOutputRectangle(rect);
	return rect;
}

RECT SVMaskShape::GetRect() const
{
	if ( m_bAutoResize )
	{
		return GetMaskImageRect();
	}
	else
	{
		MapTypeConstIterator iter;
		long lCenterX = m_mapProperties.end() != (iter = m_mapProperties.find(SVShapeMaskPropertyCenterXGuid)) ? iter->second.value : 0L;
		long lCenterY = m_mapProperties.end() != (iter = m_mapProperties.find(SVShapeMaskPropertyCenterYGuid)) ? iter->second.value : 0L;
		long lWidth = m_mapProperties.end() != (iter = m_mapProperties.find(SVShapeMaskPropertyWidthGuid)) ? iter->second.value : 0L;
		long lHeight = m_mapProperties.end() != (iter = m_mapProperties.find(SVShapeMaskPropertyHeightGuid)) ? iter->second.value : 0L;
		long lHalfWidth = lWidth / 2;
		long lHalfHeight = lHeight / 2;

		long lTop = lCenterY - lHalfHeight;
		long lLeft = lCenterX - lHalfWidth;
		long lRight = lLeft + lWidth;
		long lBottom = lTop + lHeight;

		RECT rect{lLeft, lTop, lRight, lBottom};
		return rect;
	}
}

SIZE SVMaskShape::GetSize() const
{
	if ( m_bAutoResize )
	{
		RECT rect = GetMaskImageRect();
		return SIZE{std::abs(rect.right - rect.left), std::abs(rect.top - rect.bottom)};
	}
	else
	{
		MapTypeConstIterator iter;
		long lWidth = m_mapProperties.end() != (iter = m_mapProperties.find(SVShapeMaskPropertyWidthGuid)) ? iter->second.value : 0L;
		long lHeight = m_mapProperties.end() != (iter = m_mapProperties.find(SVShapeMaskPropertyHeightGuid)) ? iter->second.value : 0L;
		return SIZE{lWidth, lHeight};
	}
}

POINT SVMaskShape::GetCenter() const
{
	if ( m_bAutoResize )
	{
		RECT rect = GetMaskImageRect();
		return POINT {std::abs(rect.right - rect.left) / 2, std::abs(rect.top - rect.bottom) / 2};
	}
	else
	{
		MapTypeConstIterator iter;
		long lCenterX = m_mapProperties.end() != (iter = m_mapProperties.find(SVShapeMaskPropertyCenterXGuid)) ? iter->second.value : 0L;
		long lCenterY = m_mapProperties.end() != (iter = m_mapProperties.find(SVShapeMaskPropertyCenterYGuid)) ? iter->second.value : 0L;
		return POINT{lCenterX, lCenterY};
	}
}

SVMaskShape::SortedMapViewType SVMaskShape::GetTempSortedPropertyMapView(const MapType& map)
{
	SortedMapViewType set;
	for ( MapTypeConstIterator iter = map.begin(); iter != map.end(); ++iter ) 
	{
		set.insert(iter);
	}
	return set;
}

HRESULT SVMaskShape::TranslateToDisplay(const RECT rectViewport, const RECT rectDisplay, std::vector<POINT>& rvecPoints) const
{
	HRESULT hr = S_OK;
	
	std::vector<SVPoint<double>> vecPoints(rvecPoints.size());
	std::copy( rvecPoints.begin(), rvecPoints.end(), vecPoints.begin() );	// from POINT to ExtentPoint
	

	RECT rectImage = GetMaskImageRect();
	
	SVExtentFigureStruct figureSource(rectImage);  // mask image is the input
	SVExtentFigureStruct figureDest(rectViewport); // viewport is the output

	// translate mask image to viewport
	SVPoint<double> ptOffset = figureDest.m_svTopLeft - figureSource.m_svTopLeft;
	std::transform( vecPoints.begin(), vecPoints.end(), vecPoints.begin(),
	                std::bind2nd( std::minus<SVPoint<double>>(), ptOffset) );	// subtract the offset from each point

	if ( S_OK == hr )
	{
		figureSource = figureDest;	// viewport is now the input
		figureDest = rectDisplay;   // display is the output
		// translate viewport to display
		hr = TranslateCoordinates(rectViewport, rectDisplay, vecPoints);
	}


	std::transform( vecPoints.begin(), vecPoints.end(), rvecPoints.begin(), [](SVPoint<double> point) { return static_cast<POINT> (point); });

	return hr;
}// SVMaskShape::TranslateToDisplay

HRESULT SVMaskShape::TranslateToDisplay(const RECT rectViewport, const RECT rectDisplay, RECT& rShapeRect) const
{
	HRESULT hr = S_OK;

	SVExtentFigureStruct figure(rShapeRect);

	RECT rectImage = GetMaskImageRect();

	SVExtentFigureStruct figureSource(rectImage);  // mask image is the input
	SVExtentFigureStruct figureDest(rectViewport); // viewport is the output

	// translate mask image to viewport
	figure -= (figureDest.m_svTopLeft - figureSource.m_svTopLeft); // simple offset, no scaling

	if ( S_OK == hr )
	{
		figureSource = figureDest;	// viewport is now the input
		figureDest = rectDisplay;   // display is the output
		// translate viewport to display
		hr = TranslateCoordinates(figureSource, figureDest, figure);
		if ( S_OK == hr )
		{
			rShapeRect = figure.Rect();
		}
	}

	return hr;
}// SVMaskShape::TranslateToDisplay

HRESULT SVMaskShape::TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, SVExtentFigureStruct& rFigure)
{
	SVPoint<double> ptTopLeft = rFigure.m_svTopLeft;
	SVPoint<double> ptBottomRight = rFigure.m_svBottomRight;
	TranslateCoordinates(rectSource, rectDest, ptTopLeft);
	TranslateCoordinates(rectSource, rectDest, ptBottomRight);
	rFigure.SetRect( ptTopLeft.m_y, ptTopLeft.m_x, ptBottomRight.m_y, ptBottomRight.m_x);
	return S_OK;
}

HRESULT SVMaskShape::TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, SVPoint<double>& rPoint)
{
	double dScaleX = rectDest.Size().m_dCX / rectSource.Size().m_dCX;
	double dScaleY = rectDest.Size().m_dCY / rectSource.Size().m_dCY;

	rPoint -= rectDest.m_svTopLeft;

	rPoint.m_x *= dScaleX;
	rPoint.m_y *= dScaleY;
	return S_OK;
}

HRESULT SVMaskShape::TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, std::vector<SVPoint<double>>& rvecPoints)
{
	HRESULT hr = S_OK;
	for ( size_t i=0; i < rvecPoints.size(); i++ )
	{
		HRESULT hrPoint = TranslateCoordinates(rectSource, rectDest, rvecPoints[i]);
		if ( S_OK == hr )
		{
			hr = hrPoint;
		}
	}
	return hr;
}

HRESULT SVMaskShape::SetAutoResize( bool p_bAutoResize )
{
	m_bAutoResize = p_bAutoResize;
	return S_OK;
}

bool SVMaskShape::IsAutoResize() const
{
	return m_bAutoResize;
}

HRESULT SVMaskShapeRectangle::Render(HDC hDC, COLORREF rgbShape, COLORREF rgbBackground) const
{
	HBRUSH hBrush = ::CreateSolidBrush(rgbShape);
	HRESULT result = (0 != ::FillRect(hDC, &GetRect(), hBrush)) ? S_OK : E_FAIL;
	::DeleteObject(hBrush);

	return result;
}

HRESULT SVMaskShapeRectangle::RenderOutline(HDC hDC, RECT rectViewport, RECT rectDisplay, COLORREF rgb) const
{
	RECT rect(GetRect());
	TranslateToDisplay(rectViewport, rectDisplay, rect);

	HBRUSH hBrush = ::CreateSolidBrush(rgb);
	HRESULT result = (0 != ::FrameRect(hDC, &rect, hBrush)) ? S_OK : E_FAIL;

	::DeleteObject(hBrush);
	return result;
}

HRESULT SVMaskShapeOval::Render(HDC hDC, COLORREF rgbShape, COLORREF rgbBackground) const
{
	RECT rect(GetRect());

	return ::Ellipse(hDC, rect.left, rect.top, rect.right, rect.bottom) ? S_OK : E_FAIL;
}

HRESULT SVMaskShapeOval::RenderOutline(HDC hDC, RECT rectViewport, RECT rectDisplay, COLORREF rgb) const
{
	RECT rect(GetRect());
	TranslateToDisplay( rectViewport, rectDisplay, rect );

	HRESULT result = ::Arc(hDC, rect.left, rect.top, rect.right, rect.bottom, rect.left, rect.top, rect.right, rect.bottom) ? S_OK : E_FAIL;
	result = ::Arc(hDC, rect.left, rect.top, rect.right, rect.bottom, rect.right, rect.bottom, rect.left, rect.top) ? result : E_FAIL;

	return result;
}

//////////////////////////////////////////////////////////////////////////////////

SVMaskShapeSymmetricTrapezoid::SVMaskShapeSymmetricTrapezoid()
{
	m_mapProperties[ SVShapeMaskPropertyOffsetGuid ] = 0;
	m_mapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ] = 0;

	m_mapProperties[ SVShapeMaskPropertyOffsetGuid ].order = 5;
	m_mapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ].order = 6;

	m_mapProperties[ SVShapeMaskPropertyOffsetGuid ].bAvailableWithAutoResize = true;
	m_mapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ].bAvailableWithAutoResize = true;
}

HRESULT SVMaskShapeSymmetricTrapezoid::Render(HDC hDC, COLORREF rgbShape, COLORREF rgbBackground) const
{
	std::vector<POINT> points = GetPoints();

	return ::Polygon(hDC, &(points[0]), static_cast<int>(points.size())) ? S_OK : E_FAIL;
}

HRESULT SVMaskShapeSymmetricTrapezoid::RenderOutline(HDC hDC, RECT rectViewport, RECT rectDisplay, COLORREF rgb) const
{
	std::vector<POINT> points = GetPoints();
	TranslateToDisplay(rectViewport, rectDisplay, points);

	 return ::Polyline(hDC, &(points[0]), static_cast<int>(points.size())) ? S_OK : E_FAIL;
}

std::vector<POINT> SVMaskShapeSymmetricTrapezoid::GetPoints() const
{
	std::vector<POINT> result;
	RECT rect(GetRect());

	result.reserve(5);

	POINT point;
	point.x = rect.left;
	point.y = rect.top;
	result.emplace_back(point);
	point.y = rect.bottom;
	result.emplace_back(point);
	point.x = rect.right;
	result.emplace_back(point);
	point.y = rect.top;
	result.emplace_back(point);

	MapTypeConstIterator iter;
	long lOffset = m_mapProperties.end() != (iter = m_mapProperties.find(SVShapeMaskPropertyOffsetGuid)) ? iter->second.value : 0L;
	long symmetryOrientation = m_mapProperties.end() != (iter = m_mapProperties.find(SVShapeMaskPropertySymmetryOrientationGuid)) ? iter->second.value : 0L;
	SymmetryOrientation eSymmetryOrientation = static_cast <SymmetryOrientation> (symmetryOrientation);

	// if offset is bigger than half the width, we will get a triangle with the top not really at the top.
	
	switch ( eSymmetryOrientation )
	{
		case VerticalAxisTop:
		{
			result[0].x += lOffset;
			result[3].x -= lOffset;
			POINT ptIntersect;
			Intersect::LineResultEnum eIntersect = Intersect::Lines(result[0], result[1],
																	result[2], result[3],
			                                                         ptIntersect );
			if ( eIntersect == Intersect::DO_INTERSECT )
			{
				result[0] = ptIntersect;
				result[3] = ptIntersect;
			}
			break;
		}
		case VerticalAxisBottom:
		{
			result[1].x += lOffset;
			result[2].x -= lOffset;
			POINT ptIntersect;
			Intersect::LineResultEnum eIntersect = Intersect::Lines(result[0], result[1],
																	result[2], result[3],
			                                                         ptIntersect );
			if ( eIntersect == Intersect::DO_INTERSECT )
			{
				result[1] = ptIntersect;
				result[2] = ptIntersect;
			}
			break;
		}
		case HorizontalAxisLeft:
		{
			result[0].y += lOffset;
			result[1].y -= lOffset;
			POINT ptIntersect;
			Intersect::LineResultEnum eIntersect = Intersect::Lines(result[0], result[3],
																	result[1], result[2],
			                                                         ptIntersect );
			if ( eIntersect == Intersect::DO_INTERSECT )
			{
				result[0] = ptIntersect;
				result[1] = ptIntersect;
			}
			break;
		}
		case HorizontalAxisRight:
		{
			result[3].y += lOffset;
			result[2].y -= lOffset;
			POINT ptIntersect;
			Intersect::LineResultEnum eIntersect = Intersect::Lines(result[0], result[3],
																	result[1], result[2],
			                                                         ptIntersect );
			if ( eIntersect == Intersect::DO_INTERSECT )
			{
				result[3] = ptIntersect;
				result[2] = ptIntersect;
			}
			break;
		}
	}

	result.emplace_back(result[0]); //close the shape (back to original point)

	return result;
}

HRESULT SVMaskShapeSymmetricTrapezoid::ValidateProperties(MapType& rProperties) const
{
	HRESULT hrRet = S_OK;
	HRESULT hr = S_OK;

	if (   rProperties.find( SVShapeMaskPropertyOffsetGuid ) != rProperties.end()
		&& rProperties.find( SVShapeMaskPropertySymmetryOrientationGuid ) != rProperties.end() )
	{
		rProperties[ SVShapeMaskPropertyOffsetGuid ].value =  std::max< long >( 0, rProperties[ SVShapeMaskPropertyOffsetGuid ].value );
	
		SymmetryOrientation eSymmetryOrientation = static_cast <SymmetryOrientation> ( rProperties[ SVShapeMaskPropertySymmetryOrientationGuid ].value );
		switch ( eSymmetryOrientation )
		{
			case VerticalAxisTop:
			case VerticalAxisBottom:
				rProperties[ SVShapeMaskPropertyWidthGuid  ].value =  std::max< long >( rProperties[ SVShapeMaskPropertyWidthGuid  ].value, rProperties[ SVShapeMaskPropertyOffsetGuid ].value * 2 );
				break;
			case HorizontalAxisLeft:
			case HorizontalAxisRight:
				rProperties[ SVShapeMaskPropertyHeightGuid ].value =  std::max< long >( rProperties[ SVShapeMaskPropertyHeightGuid ].value, rProperties[ SVShapeMaskPropertyOffsetGuid ].value * 2 );
				break;
		}
		rProperties[ SVShapeMaskPropertyWidthGuid  ].value =  std::max< long >( 2, rProperties[ SVShapeMaskPropertyWidthGuid  ].value );
		rProperties[ SVShapeMaskPropertyHeightGuid ].value =  std::max< long >( 2, rProperties[ SVShapeMaskPropertyHeightGuid ].value );
	}

	hr = SVMaskShape::ValidateProperties( rProperties );
	if ( S_OK == hrRet )
	{
		hrRet = hr;
	}

	return hrRet;
}// SVMaskShapeSymmetricTrapezoid::ValidateProperties

//////////////////////////////////////////////////////////////////////////////////
SVMaskShapeDoughnut::SVMaskShapeDoughnut()
{
	m_mapProperties[ SVShapeMaskPropertySideThicknessGuid ] = 20;
	m_mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ] = 20;

	m_mapProperties[ SVShapeMaskPropertySideThicknessGuid ].order = 5;
	m_mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].order = 6;

	m_mapProperties[ SVShapeMaskPropertySideThicknessGuid ].bAvailableWithAutoResize = true;
	m_mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].bAvailableWithAutoResize = true;
}

HRESULT SVMaskShapeDoughnut::Render(HDC hDC, COLORREF rgbShape, COLORREF rgbBackground) const
{
	RECT rect(GetRect());
	HRESULT result = ::Ellipse(hDC, rect.left, rect.top, rect.right, rect.bottom) ? S_OK : E_FAIL;

	MapTypeConstIterator iter;
	SIZE sizeThickness;
	sizeThickness.cx = m_mapProperties.end() != (iter = m_mapProperties.find(SVShapeMaskPropertySideThicknessGuid)) ? iter->second.value : 0L;
	sizeThickness.cy = m_mapProperties.end() != (iter = m_mapProperties.find(SVShapeMaskPropertyTopBottomThicknessGuid)) ? iter->second.value : 0L;

	RECT rectDoughnutHole(rect);
	::InflateRect(&rectDoughnutHole, -sizeThickness.cx, -sizeThickness.cy);

	HBRUSH hBrush = ::CreateSolidBrush(rgbBackground);
	HBRUSH hOldBrush = static_cast<HBRUSH> (::SelectObject(hDC, hBrush));
	result = ::Ellipse(hDC, rectDoughnutHole.left, rectDoughnutHole.top, rectDoughnutHole.right, rectDoughnutHole.bottom) ? S_OK : E_FAIL;
	::SelectObject(hDC, hOldBrush);
	::DeleteObject(hBrush);

	return result;
}

HRESULT SVMaskShapeDoughnut::RenderOutline(HDC hDC, RECT rectViewport, RECT rectDisplay, COLORREF rgb) const
{
	RECT rect(GetRect());
	TranslateToDisplay(rectViewport, rectDisplay, rect);

	HRESULT result = ::Arc(hDC, rect.left, rect.top, rect.right, rect.bottom, rect.left, rect.top, rect.right, rect.bottom) ? S_OK : E_FAIL;
	result = ::Arc(hDC, rect.left, rect.top, rect.right, rect.bottom, rect.right, rect.bottom, rect.left, rect.top) ? result : E_FAIL;

	MapTypeConstIterator iter;
	SIZE sizeThickness;
	sizeThickness.cx = m_mapProperties.end() != (iter = m_mapProperties.find(SVShapeMaskPropertySideThicknessGuid)) ? iter->second.value : 0L;
	sizeThickness.cy = m_mapProperties.end() != (iter = m_mapProperties.find(SVShapeMaskPropertyTopBottomThicknessGuid)) ? iter->second.value : 0L;

	//Now calculate the dough nut hole rectangle
	::InflateRect(&rect, -sizeThickness.cx, -sizeThickness.cy);

	TranslateToDisplay(rectViewport, rectDisplay, rect);

	result = ::Arc(hDC, rect.left, rect.top, rect.right, rect.bottom, rect.left, rect.top, rect.right, rect.bottom) ? result : E_FAIL;
	result = ::Arc(hDC, rect.left, rect.top, rect.right, rect.bottom, rect.right, rect.bottom, rect.left, rect.top) ? result : E_FAIL;

	return result;
}

HRESULT SVMaskShapeDoughnut::ValidateProperties(MapType& rProperties) const
{
	HRESULT hr = S_OK;

	if (   rProperties.find( SVShapeMaskPropertyTopBottomThicknessGuid ) != rProperties.end()
		&& rProperties.find( SVShapeMaskPropertySideThicknessGuid ) != rProperties.end() )
	{
		rProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value =  std::max< long >( rProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value, 1 );
		rProperties[ SVShapeMaskPropertySideThicknessGuid  ].value =  std::max< long >( rProperties[ SVShapeMaskPropertySideThicknessGuid  ].value, 1 );
	
		rProperties[ SVShapeMaskPropertyHeightGuid ].value =  std::max< long >( rProperties[ SVShapeMaskPropertyHeightGuid ].value, rProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value * 2 );
		rProperties[ SVShapeMaskPropertyWidthGuid  ].value =  std::max< long >( rProperties[ SVShapeMaskPropertyWidthGuid  ].value, rProperties[ SVShapeMaskPropertySideThicknessGuid ].value * 2 );
	}

	hr = SVMaskShape::ValidateProperties( rProperties );

	return hr;
}

} //namespace SvIe
