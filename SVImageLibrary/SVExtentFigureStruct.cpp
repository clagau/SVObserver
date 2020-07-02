//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExtentFigureStruct
//* .File Name       : $Workfile:   SVExtentFigureStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:39:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVExtentFigureStruct.h"
#include "SVLibrary/Intersect.h"
#pragma endregion Includes

SVExtentFigureStruct::SVExtentFigureStruct()
{
	Initialize();
}

SVExtentFigureStruct::SVExtentFigureStruct( const SVExtentFigureStruct &p_rsvValue )
{
	*this = p_rsvValue;
}

SVExtentFigureStruct::SVExtentFigureStruct( RECT l_oRect )
{
	*this = l_oRect;
}

SVExtentFigureStruct::~SVExtentFigureStruct()
{
	Initialize();
}

const SVExtentFigureStruct& SVExtentFigureStruct::operator = ( const SVExtentFigureStruct &p_rsvValue )
{
	if ( this != &p_rsvValue )
	{
		Initialize();

		m_eShape = p_rsvValue.m_eShape;

		m_svTopLeft = p_rsvValue.m_svTopLeft;
		m_svTopRight = p_rsvValue.m_svTopRight;
		m_svBottomRight = p_rsvValue.m_svBottomRight;
		m_svBottomLeft = p_rsvValue.m_svBottomLeft;
		m_svCenterLeft = p_rsvValue.m_svCenterLeft;
		m_svCenterRight = p_rsvValue.m_svCenterRight;
		m_svTopCenter = p_rsvValue.m_svTopCenter;
		m_svBottomCenter = p_rsvValue.m_svBottomCenter;
		m_svCenter = p_rsvValue.m_svCenter;
	}
	return *this;
}

const SVExtentFigureStruct& SVExtentFigureStruct::operator += ( const SVPoint<double>& rOffset )
{
	m_svTopLeft += rOffset;
	m_svTopRight += rOffset;
	m_svBottomLeft += rOffset;
	m_svBottomRight += rOffset;
	m_svCenterLeft += rOffset;
	m_svCenterRight += rOffset;
	m_svTopCenter += rOffset;
	m_svBottomCenter += rOffset;
	m_svCenter += rOffset;
	return *this;
}

const SVExtentFigureStruct& SVExtentFigureStruct::operator -= ( const SVPoint<double>& rOffset )
{
	m_svTopLeft -= rOffset;
	m_svTopRight -= rOffset;
	m_svBottomLeft -= rOffset;
	m_svBottomRight -= rOffset;
	m_svCenterLeft -= rOffset;
	m_svCenterRight -= rOffset;
	m_svTopCenter -= rOffset;
	m_svBottomCenter -= rOffset;
	m_svCenter -= rOffset;
	return *this;
}

const SVExtentFigureStruct& SVExtentFigureStruct::operator = ( RECT oRect )
{
	//SetRect(p_oRect.top, p_oRect.left, p_oRect.bottom, p_oRect.right);
	// leave original behavior in place (width/2 truncates, etc.)
	POINT l_oPoint;

	long width = oRect.right - oRect.left;
	long height = oRect.bottom - oRect.top;

	l_oPoint.x = oRect.left;
	l_oPoint.y = oRect.top;

	Initialize();

	m_eShape = SvDef::SVExtentShapeRectangle;

	m_svTopLeft = l_oPoint;
	m_svTopRight = l_oPoint;
	m_svBottomRight = l_oPoint;
	m_svBottomLeft = l_oPoint;
	m_svCenterLeft = l_oPoint;
	m_svCenterRight = l_oPoint;
	m_svTopCenter = l_oPoint;
	m_svBottomCenter = l_oPoint;
	m_svCenter = l_oPoint;

	m_svTopCenter.m_x += width / 2;

	m_svTopRight.m_x += width;

	m_svCenterLeft.m_y += height / 2;

	m_svCenterRight.m_y += height / 2;
	m_svCenterRight.m_x += width;

	m_svBottomLeft.m_y += height;

	m_svBottomCenter.m_y += height;
	m_svBottomCenter.m_x += width / 2;

	m_svBottomRight.m_y += height;
	m_svBottomRight.m_x += width;

	m_svCenter.m_x += width / 2;
	m_svCenter.m_y += height / 2;

	return *this;
}

HRESULT SVExtentFigureStruct::GetRect(RECT& p_rRect) const
{
	HRESULT hr = (m_eShape == SvDef::SVExtentShapeRectangle) ? S_OK : E_FAIL;
	if ( S_OK == hr )
	{
		p_rRect.top = static_cast<long> (m_svTopLeft.m_y);
		p_rRect.left = static_cast<long> (m_svTopLeft.m_x);
		p_rRect.bottom = static_cast<long> (m_svBottomRight.m_y);
		p_rRect.right = static_cast<long> (m_svBottomRight.m_x);
	}
	return hr;
}

RECT SVExtentFigureStruct::Rect() const
{
	RECT rect;
	
	rect.top = static_cast<long> (m_svTopLeft.m_y);
	rect.left = static_cast<long> (m_svTopLeft.m_x);
	rect.bottom = static_cast<long> (m_svBottomRight.m_y);
	rect.right = static_cast<long> (m_svBottomRight.m_x);

	return rect;
}

SVExtentSizeStruct SVExtentFigureStruct::Size() const
{
	return SVExtentSizeStruct( m_svBottomRight.m_x - m_svTopLeft.m_x,  m_svBottomRight.m_y - m_svTopLeft.m_y );
}

// cppcheck-suppress unusedFunction
double SVExtentFigureStruct::Top() const
{
	return m_svTopLeft.m_y;
}

double SVExtentFigureStruct::Left() const
{
	return m_svTopLeft.m_x;
}

double SVExtentFigureStruct::Right() const
{
	return m_svBottomRight.m_x;
}

HRESULT SVExtentFigureStruct::SetRect( double dTop, double dLeft, double dBottom, double dRight )
{
	SVPoint<double> point(dLeft, dTop);

	double dWidth = dRight - dLeft;
	double dHeight = dBottom - dTop;

	Initialize();

	m_eShape = SvDef::SVExtentShapeRectangle;

	m_svTopLeft = point;
	m_svTopRight = point;
	m_svBottomRight = point;
	m_svBottomLeft = point;
	m_svCenterLeft = point;
	m_svCenterRight = point;
	m_svTopCenter = point;
	m_svBottomCenter = point;
	m_svCenter = point;

	m_svTopCenter.m_x += (dWidth / 2.0);

	m_svTopRight.m_x += dWidth;

	m_svCenterLeft.m_y += ( dHeight / 2.0 );

	m_svCenterRight.m_y += ( dHeight / 2.0 );
	m_svCenterRight.m_x += ( dWidth );

	m_svBottomLeft.m_y += ( dHeight );

	m_svBottomCenter.m_y += ( dHeight );
	m_svBottomCenter.m_x += ( dWidth / 2.0 );

	m_svBottomRight.m_y += ( dHeight );
	m_svBottomRight.m_x += ( dWidth );

	m_svCenter.m_x += ( dWidth / 2.0 );
	m_svCenter.m_y += ( dHeight / 2.0 );

	return S_OK;
}

HRESULT SVExtentFigureStruct::Initialize()
{
	m_eShape = SvDef::SVExtentShapeUnknown;

	m_svTopLeft.clear();
	m_svTopRight.clear();
	m_svBottomRight.clear();
	m_svBottomLeft.clear();
	m_svCenterLeft.clear();
	m_svCenterRight.clear();
	m_svTopCenter.clear();
	m_svBottomCenter.clear();
	m_svCenter.clear();

	return S_OK;
}

HRESULT SVExtentFigureStruct::IsPointOverFigure( const SVPoint<double>& rPoint ) const
{
	HRESULT l_hrOk = S_OK;

	if( m_eShape != SvDef::SVExtentShapeUnknown )
	{
		POINT intersectPoint = { 0, 0 };

		SVPoint<double> center = ( m_svTopCenter + m_svBottomCenter ) / 2.0;

		if( center != rPoint )
		{
			bool bIntersect = ( center == m_svTopCenter ) || ( center == m_svBottomCenter );

			bIntersect = bIntersect || Intersect::Lines( center.getPoint(), rPoint.getPoint(), m_svTopCenter.getPoint(), m_svTopRight.getPoint(), intersectPoint) == Intersect::DO_INTERSECT;
			bIntersect = bIntersect || Intersect::Lines( center.getPoint(), rPoint.getPoint(), m_svTopRight.getPoint(), m_svCenterRight.getPoint(), intersectPoint) == Intersect::DO_INTERSECT;
			bIntersect = bIntersect || Intersect::Lines( center.getPoint(), rPoint.getPoint(), m_svCenterRight.getPoint(), m_svBottomRight.getPoint(), intersectPoint) == Intersect::DO_INTERSECT;
			bIntersect = bIntersect || Intersect::Lines( center.getPoint(), rPoint.getPoint(), m_svBottomRight.getPoint(), m_svBottomCenter.getPoint(), intersectPoint) == Intersect::DO_INTERSECT;
			bIntersect = bIntersect || Intersect::Lines( center.getPoint(), rPoint.getPoint(), m_svBottomCenter.getPoint(), m_svBottomLeft.getPoint(), intersectPoint) == Intersect::DO_INTERSECT;
			bIntersect = bIntersect || Intersect::Lines( center.getPoint(), rPoint.getPoint(), m_svBottomLeft.getPoint(), m_svCenterLeft.getPoint(), intersectPoint) == Intersect::DO_INTERSECT;
			bIntersect = bIntersect || Intersect::Lines( center.getPoint(), rPoint.getPoint(), m_svCenterLeft.getPoint(), m_svTopLeft.getPoint(), intersectPoint) == Intersect::DO_INTERSECT;
			bIntersect = bIntersect || Intersect::Lines( center.getPoint(), rPoint.getPoint(), m_svTopLeft.getPoint(), m_svTopCenter.getPoint(), intersectPoint ) == Intersect::DO_INTERSECT;

			if( bIntersect )
			{
				l_hrOk = S_FALSE;
			}
		}
	}
	else
	{
		l_hrOk = E_FAIL;
	}

	return l_hrOk;
}

