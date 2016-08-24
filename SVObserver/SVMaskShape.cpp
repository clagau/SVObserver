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
#include "SVLibrary/Intersect.h"
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add SVColor declaration.
#include "SVOGui/SVColor.h"
#pragma endregion Includes

#pragma warning ( disable : 4503 )

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

	m_bImageInfoChanged = false;
	m_bImageInfoSet     = false;
	m_bAutoResize       = false;
}

SVMaskShape::~SVMaskShape()	// this is a base class
{
	if ( nullptr != m_dib.hbm )
	{
		::DeleteObject( m_dib.hbm );
		m_dib.FreeBitmapInfo();
		m_dib.Clear();
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

HRESULT SVMaskShape::DrawOutline( CDC& dc, CRect rectViewport, CRect rectDisplay, COLORREF rgb )
{
	HRESULT hr = S_OK;

	hr = RenderOutline(dc, rectViewport, rectDisplay, rgb);
	
	return hr;
}

HRESULT SVMaskShape::Draw( SVMatroxBuffer mil )
{
	HRESULT hr = S_OK;
	if ( !mil.empty() )
	{
		
		SVMatroxBufferInterface::SVStatusCode l_Code;
		l_Code = SVMatroxBufferInterface::CopyBuffer(  mil, m_dib.hbm );
		hr = (l_Code != SVMEE_STATUS_OK) ? l_Code | SVMEE_MATROX_ERROR : S_OK;
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
		m_RenderDC.DeleteDC();
	}

	if ( nullptr == m_dib.hbm )
	{
		m_RenderDC.CreateCompatibleDC(nullptr);
		hr = SVIHBitmapUtilities::SVImageInfoToNewDIB( m_svImageInfo, m_dib );
		m_bImageInfoChanged = false;
	}

	if ( S_OK == hr )
	{
		HBITMAP hbmOld = (HBITMAP) ::SelectObject(m_RenderDC.GetSafeHdc(), m_dib.hbm);

		// compute fore & back colors
		COLORREF rgbShape = SVColor::White;
		COLORREF rgbBackground = SVColor::Black;

		ComputeColors(rgbShape, rgbBackground);

		CBrush brush;
		brush.CreateSolidBrush(rgbShape);
		CBrush* pOldBrush = m_RenderDC.SelectObject(&brush);

		CPen pen;
		pen.CreatePen(PS_SOLID, 1, rgbShape);
		CPen* pOldPen = m_RenderDC.SelectObject(&pen);

		// fill background of dc
		CRect rect;
		m_svImageInfo.GetOutputRectangle(rect);
		m_RenderDC.FillSolidRect(rect, rgbBackground);

		// Render Shape
		hr = Render( m_RenderDC, rgbShape, rgbBackground );

		m_RenderDC.SelectObject( pOldPen );
		m_RenderDC.SelectObject( pOldBrush );
		::SelectObject(m_RenderDC.GetSafeHdc(), hbmOld);
	}

	return hr;
}// SVMaskShape::Refresh

HRESULT SVMaskShape::SetProperties(const MapType& p_mapProperties)
{
	HRESULT hr = S_FALSE;
	
	ASSERT( m_bImageInfoSet );
	
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

HRESULT SVMaskShape::ValidateProperties(MapType& p_rmapProperties)
{
	HRESULT hr = S_OK;

	if (   p_rmapProperties.find( SVShapeMaskPropertyWidthGuid ) != p_rmapProperties.end()
		&& p_rmapProperties.find( SVShapeMaskPropertyHeightGuid ) != p_rmapProperties.end()
		&& p_rmapProperties.find( SVShapeMaskPropertyCenterXGuid ) != p_rmapProperties.end()
		&& p_rmapProperties.find( SVShapeMaskPropertyCenterYGuid ) != p_rmapProperties.end() )
	{
		SVImageInfoClass info;
		CRect rect;
		hr = GetImageInfo( info );
		if ( S_OK == hr)
		{
			hr = info.GetOutputRectangle(rect);
		}
		if ( S_OK == hr )
		{
			if ( S_OK == hr )
			{
				hr = p_rmapProperties[ SVShapeMaskPropertyHeightGuid ].value >= 0 ? S_OK : S_FALSE;//SVMSG_SVO_32_VALUE_OUT_OF_RANGE;
			}
			if ( S_OK == hr )
			{
				hr = p_rmapProperties[ SVShapeMaskPropertyWidthGuid  ].value >= 0 ? S_OK : S_FALSE;//SVMSG_SVO_32_VALUE_OUT_OF_RANGE;
			}
		}
	}

	return hr;
}// SVMaskShape::ValidateProperties

HRESULT SVMaskShape::GetProperties(MapType& rmapProperties) const
{
	HRESULT hr = S_OK;
	rmapProperties = m_mapProperties;
	return hr;
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
			rgbBackground = SVColor::White;
		}
		else
		{
			rgbShape = SVColor::Black;
			rgbBackground = m_svFillStruct.rgbFillColor;
		}
	}
	else // mask outside shape (default)
	{
		if ( m_svFillStruct.bFillMaskArea )
		{
			rgbShape = SVColor::White;
			rgbBackground = m_svFillStruct.rgbFillColor;
		}
		else
		{
			rgbShape = m_svFillStruct.rgbFillColor;
			rgbBackground = SVColor::Black;
		}
	}
}// SVMaskShape::ComputeColors

CRect SVMaskShape::GetMaskImageRect()
{
	CRect rect;
	m_svImageInfo.GetExtents().GetOutputRectangle(rect);
	return rect;
}

CRect SVMaskShape::GetRect()
{
	if ( m_bAutoResize )
	{
		return GetMaskImageRect();
	}
	else
	{
		long lCenterX = m_mapProperties[ SVShapeMaskPropertyCenterXGuid ].value;
		long lCenterY = m_mapProperties[ SVShapeMaskPropertyCenterYGuid ].value;
		long lWidth = m_mapProperties[ SVShapeMaskPropertyWidthGuid ].value;
		long lHeight = m_mapProperties[ SVShapeMaskPropertyHeightGuid ].value;
		long lHalfWidth = lWidth / 2;
		long lHalfHeight = lHeight / 2;

		long lTop = lCenterY - lHalfHeight;
		long lLeft = lCenterX - lHalfWidth;
		long lRight = lLeft + lWidth;
		long lBottom = lTop + lHeight;

		CRect rect(lLeft, lTop, lRight, lBottom);
		return rect;
	}
}

CSize SVMaskShape::GetSize()
{
	if ( m_bAutoResize )
	{
		CRect rect = GetMaskImageRect();
		return CSize(rect.Width(), rect.Height());
	}
	else
	{
		long lWidth = m_mapProperties[ SVShapeMaskPropertyWidthGuid ].value;
		long lHeight = m_mapProperties[ SVShapeMaskPropertyHeightGuid ].value;
		return CSize(lWidth, lHeight);
	}
}

CPoint SVMaskShape::GetCenter()
{
	if ( m_bAutoResize )
	{
		CRect rect = GetMaskImageRect();
		return CPoint(rect.Width()/2, rect.Height()/2);
	}
	else
	{
		long lCenterX = m_mapProperties[ SVShapeMaskPropertyCenterXGuid ].value;
		long lCenterY = m_mapProperties[ SVShapeMaskPropertyCenterYGuid ].value;
		return CPoint(lCenterX, lCenterY);
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

HRESULT SVMaskShape::TranslateToDisplay(const CRect rectViewport, const CRect rectDisplay, std::vector<POINT>& p_rvecPoints)
{
	HRESULT hr = S_OK;
	
	std::vector<SVExtentPointStruct> vecPoints(p_rvecPoints.size());
	std::copy( p_rvecPoints.begin(), p_rvecPoints.end(), vecPoints.begin() );	// from POINT to ExtentPoint
	

	CRect rectImage = GetMaskImageRect();
	
	SVExtentFigureStruct figureSource(rectImage);  // mask image is the input
	SVExtentFigureStruct figureDest(rectViewport); // viewport is the output

	// translate mask image to viewport
	SVExtentPointStruct ptOffset = figureDest.m_svTopLeft - figureSource.m_svTopLeft;
	std::transform( vecPoints.begin(), vecPoints.end(), vecPoints.begin(),
	                std::bind2nd( std::minus<SVExtentPointStruct>(), ptOffset) );	// subtract the offset from each point

	if ( S_OK == hr )
	{
		figureSource = figureDest;	// viewport is now the input
		figureDest = rectDisplay;   // display is the output
		// translate viewport to display
		hr = TranslateCoordinates(rectViewport, rectDisplay, vecPoints);
	}

	std::copy( vecPoints.begin(), vecPoints.end(), p_rvecPoints.begin() );	// from ExtentPoint back to POINT

	return hr;
}// SVMaskShape::TranslateToDisplay

HRESULT SVMaskShape::TranslateToDisplay(const CRect rectViewport, const CRect rectDisplay, CRect& rectShape)
{
	HRESULT hr = S_OK;

	SVExtentFigureStruct figure(rectShape);

	CRect rectImage = GetMaskImageRect();

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
			rectShape = figure.Rect();
		}
	}

	return hr;
}// SVMaskShape::TranslateToDisplay

HRESULT SVMaskShape::TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, SVExtentFigureStruct& rRect)
{
	SVExtentPointStruct ptTopLeft = rRect.m_svTopLeft;
	SVExtentPointStruct ptBottomRight = rRect.m_svBottomRight;
	TranslateCoordinates(rectSource, rectDest, ptTopLeft);
	TranslateCoordinates(rectSource, rectDest, ptBottomRight);
	rRect.SetRect( ptTopLeft.m_dPositionY, ptTopLeft.m_dPositionX, ptBottomRight.m_dPositionY, ptBottomRight.m_dPositionX);
	return S_OK;
}

HRESULT SVMaskShape::TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, SVExtentPointStruct& rPoint)
{
	double dScaleX = rectDest.Size().m_dCX / rectSource.Size().m_dCX;
	double dScaleY = rectDest.Size().m_dCY / rectSource.Size().m_dCY;

	rPoint -= rectDest.m_svTopLeft;

	rPoint.m_dPositionX *= dScaleX;
	rPoint.m_dPositionY *= dScaleY;
	return S_OK;
}

HRESULT SVMaskShape::TranslateCoordinates(const SVExtentFigureStruct& rectSource, const SVExtentFigureStruct& rectDest, std::vector<SVExtentPointStruct>& rvecPoints)
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

//////////////////////////////////////////////////////////////////////////////////

SVMaskShapeRectangle::SVMaskShapeRectangle()
{
	// all properties in base
}

SVMaskShapeRectangle::~SVMaskShapeRectangle()
{
}

HRESULT SVMaskShapeRectangle::Render( CDC& dc, COLORREF rgbShape, COLORREF )
{
	HRESULT hr = S_OK;

	CRect rect(GetRect());

	dc.FillSolidRect(&rect, rgbShape );
	
	return hr;
}

HRESULT SVMaskShapeRectangle::RenderOutline( CDC& dc, const CRect rectViewport, const CRect rectDisplay, COLORREF rgb )
{
	HRESULT hr = S_OK;

	CRect rect(GetRect());

	TranslateToDisplay(rectViewport, rectDisplay, rect);

	if ( S_OK == hr )
	{
		CBrush brush;
		brush.CreateSolidBrush(rgb);
		CBrush* pOldBrush = dc.SelectObject(&brush);

		dc.FrameRect(&rect, &brush);

		dc.SelectObject( pOldBrush );
	}

	return hr;
}

//////////////////////////////////////////////////////////////////////////////////

SVMaskShapeOval::SVMaskShapeOval()
{
	// all properties in base
}

SVMaskShapeOval::~SVMaskShapeOval()
{
}

HRESULT SVMaskShapeOval::Render( CDC& dc, COLORREF rgbShape, COLORREF )
{
	HRESULT hr = S_OK;

	CRect rect(GetRect());

	dc.Ellipse(&rect);

	return hr;
}

HRESULT SVMaskShapeOval::RenderOutline( CDC& dc, const CRect rectViewport, const CRect rectDisplay, COLORREF rgb )
{
	HRESULT hr = S_OK;

	CRect rect(GetRect());

	TranslateToDisplay( rectViewport, rectDisplay, rect );

	dc.Arc(&rect, rect.TopLeft(), rect.BottomRight());
	dc.Arc(&rect, rect.BottomRight(), rect.TopLeft());

	return hr;
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

SVMaskShapeSymmetricTrapezoid::~SVMaskShapeSymmetricTrapezoid()
{
}

HRESULT SVMaskShapeSymmetricTrapezoid::Render( CDC& dc, COLORREF rgbShape, COLORREF )
{
	HRESULT hr = S_OK;

	std::vector<POINT> aPoints;
	
	hr = GetPoints(aPoints);

	BOOL bRet = dc.Polygon( &(aPoints[0]), static_cast<int>(aPoints.size()) );
	if ( !bRet )
	{
		DWORD dwError = ::GetLastError();
	}

	return hr;
}

HRESULT SVMaskShapeSymmetricTrapezoid::RenderOutline( CDC& dc, const CRect rectViewport, const CRect rectDisplay, COLORREF rgb )
{
	HRESULT hr = S_OK;

	std::vector<POINT> aPoints;
	
	hr = GetPoints(aPoints);

	TranslateToDisplay( rectViewport, rectDisplay, aPoints );

	dc.Polyline( &(aPoints[0]), static_cast<int>(aPoints.size()) );

	return hr;
}

HRESULT SVMaskShapeSymmetricTrapezoid::GetPoints(std::vector<POINT>& p_rvecPoints)
{
	CRect rect(GetRect());

	p_rvecPoints.push_back( rect.TopLeft() );                  // 0
	p_rvecPoints.push_back( CPoint(rect.left, rect.bottom) );  // 1
	p_rvecPoints.push_back( rect.BottomRight() );              // 2
	p_rvecPoints.push_back( CPoint(rect.right, rect.top) );    // 3

	long lOffset = m_mapProperties[ SVShapeMaskPropertyOffsetGuid ].value;
	SymmetryOrientation eSymmetryOrientation = static_cast <SymmetryOrientation> ( m_mapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ].value );

	// if offset is bigger than half the width, we will get a triangle with the top not really at the top.
	
	switch ( eSymmetryOrientation )
	{
		case VerticalAxisTop:
		{
			p_rvecPoints[0].x += lOffset;
			p_rvecPoints[3].x -= lOffset;
			CPoint ptIntersect;
			Intersect::LineResultEnum eIntersect = Intersect::Lines( p_rvecPoints[0], p_rvecPoints[1],
			                                                         p_rvecPoints[2], p_rvecPoints[3],
			                                                         ptIntersect );
			if ( eIntersect == Intersect::DO_INTERSECT )
			{
				p_rvecPoints[0] = ptIntersect;
				p_rvecPoints[3] = ptIntersect;
			}
			break;
		}
		case VerticalAxisBottom:
		{
			p_rvecPoints[1].x += lOffset;
			p_rvecPoints[2].x -= lOffset;
			CPoint ptIntersect;
			Intersect::LineResultEnum eIntersect = Intersect::Lines( p_rvecPoints[0], p_rvecPoints[1],
			                                                         p_rvecPoints[2], p_rvecPoints[3],
			                                                         ptIntersect );
			if ( eIntersect == Intersect::DO_INTERSECT )
			{
				p_rvecPoints[1] = ptIntersect;
				p_rvecPoints[2] = ptIntersect;
			}
			break;
		}
		case HorizontalAxisLeft:
		{
			p_rvecPoints[0].y += lOffset;
			p_rvecPoints[1].y -= lOffset;
			CPoint ptIntersect;
			Intersect::LineResultEnum eIntersect = Intersect::Lines( p_rvecPoints[0], p_rvecPoints[3],
			                                                         p_rvecPoints[1], p_rvecPoints[2],
			                                                         ptIntersect );
			if ( eIntersect == Intersect::DO_INTERSECT )
			{
				p_rvecPoints[0] = ptIntersect;
				p_rvecPoints[1] = ptIntersect;
			}
			break;
		}
		case HorizontalAxisRight:
		{
			p_rvecPoints[3].y += lOffset;
			p_rvecPoints[2].y -= lOffset;
			CPoint ptIntersect;
			Intersect::LineResultEnum eIntersect = Intersect::Lines( p_rvecPoints[0], p_rvecPoints[3],
			                                                         p_rvecPoints[1], p_rvecPoints[2],
			                                                         ptIntersect );
			if ( eIntersect == Intersect::DO_INTERSECT )
			{
				p_rvecPoints[3] = ptIntersect;
				p_rvecPoints[2] = ptIntersect;
			}
			break;
		}
	}// end switch ( eSymmetryOrientation )

	p_rvecPoints.push_back( p_rvecPoints[0] ); // 4; close the shape (back to original point)

	return S_OK;
}// SVMaskShapeSymmetricTrapezoid::GetPoints

HRESULT SVMaskShapeSymmetricTrapezoid::ValidateProperties(MapType& p_rmapProperties)
{
	HRESULT hrRet = S_OK;
	HRESULT hr = S_OK;

	if (   p_rmapProperties.find( SVShapeMaskPropertyOffsetGuid ) != p_rmapProperties.end()
		&& p_rmapProperties.find( SVShapeMaskPropertySymmetryOrientationGuid ) != p_rmapProperties.end() )
	{
		p_rmapProperties[ SVShapeMaskPropertyOffsetGuid ].value =  std::max< long >( 0, p_rmapProperties[ SVShapeMaskPropertyOffsetGuid ].value );
	
		SymmetryOrientation eSymmetryOrientation = static_cast <SymmetryOrientation> ( p_rmapProperties[ SVShapeMaskPropertySymmetryOrientationGuid ].value );
		switch ( eSymmetryOrientation )
		{
			case VerticalAxisTop:
			case VerticalAxisBottom:
				p_rmapProperties[ SVShapeMaskPropertyWidthGuid  ].value =  std::max< long >( p_rmapProperties[ SVShapeMaskPropertyWidthGuid  ].value, p_rmapProperties[ SVShapeMaskPropertyOffsetGuid ].value * 2 );
				break;
			case HorizontalAxisLeft:
			case HorizontalAxisRight:
				p_rmapProperties[ SVShapeMaskPropertyHeightGuid ].value =  std::max< long >( p_rmapProperties[ SVShapeMaskPropertyHeightGuid ].value, p_rmapProperties[ SVShapeMaskPropertyOffsetGuid ].value * 2 );
				break;
		}
		p_rmapProperties[ SVShapeMaskPropertyWidthGuid  ].value =  std::max< long >( 2, p_rmapProperties[ SVShapeMaskPropertyWidthGuid  ].value );
		p_rmapProperties[ SVShapeMaskPropertyHeightGuid ].value =  std::max< long >( 2, p_rmapProperties[ SVShapeMaskPropertyHeightGuid ].value );
	}

	hr = SVMaskShape::ValidateProperties( p_rmapProperties );
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

SVMaskShapeDoughnut::~SVMaskShapeDoughnut()
{
}

HRESULT SVMaskShapeDoughnut::Render( CDC& dc, COLORREF rgbShape, COLORREF rgbBackground )
{
	HRESULT hr = S_OK;

	CRect rect(GetRect());

	dc.Ellipse(&rect);

	CSize sizeThickness;
	sizeThickness.cx = m_mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value;
	sizeThickness.cy = m_mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value;

	CRect rectDoughnutHole(rect);
	rectDoughnutHole.DeflateRect(sizeThickness);

	CBrush brush;
	brush.CreateSolidBrush(rgbBackground);
	CBrush* pOldBrush = m_RenderDC.SelectObject(&brush);

	dc.Ellipse(&rectDoughnutHole);

	dc.SelectObject( pOldBrush );

	return hr;
}

HRESULT SVMaskShapeDoughnut::RenderOutline( CDC& dc, const CRect rectViewport, const CRect rectDisplay, COLORREF rgb )
{
	HRESULT hr = S_OK;

	CRect rect( GetRect() );

	TranslateToDisplay( rectViewport, rectDisplay, rect );

	dc.Arc(&rect, rect.TopLeft(), rect.BottomRight());
	dc.Arc(&rect, rect.BottomRight(), rect.TopLeft());

	CSize sizeThickness;
	sizeThickness.cx = m_mapProperties[ SVShapeMaskPropertySideThicknessGuid ].value;
	sizeThickness.cy = m_mapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value;

	CRect rectDoughnutHole( GetRect() );
	rectDoughnutHole.DeflateRect( sizeThickness );

	TranslateToDisplay( rectViewport, rectDisplay, rectDoughnutHole );

	dc.Arc(&rectDoughnutHole, rectDoughnutHole.TopLeft(), rectDoughnutHole.BottomRight());
	dc.Arc(&rectDoughnutHole, rectDoughnutHole.BottomRight(), rectDoughnutHole.TopLeft());

	return hr;
}

HRESULT SVMaskShapeDoughnut::ValidateProperties(MapType& p_rmapProperties)
{
	HRESULT hr = S_OK;

	if (   p_rmapProperties.find( SVShapeMaskPropertyTopBottomThicknessGuid ) != p_rmapProperties.end()
		&& p_rmapProperties.find( SVShapeMaskPropertySideThicknessGuid ) != p_rmapProperties.end() )
	{
		p_rmapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value =  std::max< long >( p_rmapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value, 1 );
		p_rmapProperties[ SVShapeMaskPropertySideThicknessGuid  ].value =  std::max< long >( p_rmapProperties[ SVShapeMaskPropertySideThicknessGuid  ].value, 1 );
	
		p_rmapProperties[ SVShapeMaskPropertyHeightGuid ].value =  std::max< long >( p_rmapProperties[ SVShapeMaskPropertyHeightGuid ].value, p_rmapProperties[ SVShapeMaskPropertyTopBottomThicknessGuid ].value * 2 );
		p_rmapProperties[ SVShapeMaskPropertyWidthGuid  ].value =  std::max< long >( p_rmapProperties[ SVShapeMaskPropertyWidthGuid  ].value, p_rmapProperties[ SVShapeMaskPropertySideThicknessGuid ].value * 2 );
	}

	hr = SVMaskShape::ValidateProperties( p_rmapProperties );

	return hr;
}

