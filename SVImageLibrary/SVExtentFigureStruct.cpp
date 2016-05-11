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

#include "stdafx.h"
#include "SVExtentFigureStruct.h"
#include "SVLibrary/Intersect.h"
#include "SVOMFCLibrary/SVOMFCLibraryGlobals.h"

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

const SVExtentFigureStruct& SVExtentFigureStruct::operator += ( const SVExtentPointStruct& p_rsvOffset )
{
	m_svTopLeft += p_rsvOffset;
	m_svTopRight += p_rsvOffset;
	m_svBottomLeft += p_rsvOffset;
	m_svBottomRight += p_rsvOffset;
	m_svCenterLeft += p_rsvOffset;
	m_svCenterRight += p_rsvOffset;
	m_svTopCenter += p_rsvOffset;
	m_svBottomCenter += p_rsvOffset;
	m_svCenter += p_rsvOffset;
	return *this;
}

const SVExtentFigureStruct& SVExtentFigureStruct::operator -= ( const SVExtentPointStruct& p_rsvOffset )
{
	m_svTopLeft -= p_rsvOffset;
	m_svTopRight -= p_rsvOffset;
	m_svBottomLeft -= p_rsvOffset;
	m_svBottomRight -= p_rsvOffset;
	m_svCenterLeft -= p_rsvOffset;
	m_svCenterRight -= p_rsvOffset;
	m_svTopCenter -= p_rsvOffset;
	m_svBottomCenter -= p_rsvOffset;
	m_svCenter -= p_rsvOffset;
	return *this;
}

const SVExtentFigureStruct& SVExtentFigureStruct::operator = ( RECT p_oRect )
{
	//SetRect(p_oRect.top, p_oRect.left, p_oRect.bottom, p_oRect.right);
	// leave original behavior in place (width/2 truncates, etc.)
	POINT l_oPoint;

	long l_lWidth = p_oRect.right - p_oRect.left;
	long l_lHeight = p_oRect.bottom - p_oRect.top;

	l_oPoint.x = p_oRect.left;
	l_oPoint.y = p_oRect.top;

	Initialize();

	m_eShape = SVExtentShapeRectangle;

	m_svTopLeft = l_oPoint;
	m_svTopRight = l_oPoint;
	m_svBottomRight = l_oPoint;
	m_svBottomLeft = l_oPoint;
	m_svCenterLeft = l_oPoint;
	m_svCenterRight = l_oPoint;
	m_svTopCenter = l_oPoint;
	m_svBottomCenter = l_oPoint;
	m_svCenter = l_oPoint;

	m_svTopCenter.m_dPositionX += (long) ( l_lWidth / 2 );

	m_svTopRight.m_dPositionX += (long) ( l_lWidth );

	m_svCenterLeft.m_dPositionY += (long) ( l_lHeight / 2 );

	m_svCenterRight.m_dPositionY += (long) ( l_lHeight / 2 );
	m_svCenterRight.m_dPositionX += (long) ( l_lWidth );

	m_svBottomLeft.m_dPositionY += (long) ( l_lHeight );

	m_svBottomCenter.m_dPositionY += (long) ( l_lHeight );
	m_svBottomCenter.m_dPositionX += (long) ( l_lWidth / 2 );

	m_svBottomRight.m_dPositionY += (long) ( l_lHeight );
	m_svBottomRight.m_dPositionX += (long) ( l_lWidth );

	m_svCenter.m_dPositionX += (long) ( l_lWidth / 2 );
	m_svCenter.m_dPositionY += (long) ( l_lHeight / 2 );

	return *this;
}

HRESULT SVExtentFigureStruct::GetRect(RECT& p_rRect) const
{
	HRESULT hr = (m_eShape == SVExtentShapeRectangle) ? S_OK : SV_FALSE;
	if ( S_OK == hr )
	{
		p_rRect.top = (long) m_svTopLeft.m_dPositionY;
		p_rRect.left = (long) m_svTopLeft.m_dPositionX;
		p_rRect.bottom = (long) m_svBottomRight.m_dPositionY;
		p_rRect.right = (long) m_svBottomRight.m_dPositionX;
	}
	return hr;
}

RECT SVExtentFigureStruct::Rect() const
{
	RECT rect;
	
	rect.top = (long) m_svTopLeft.m_dPositionY;
	rect.left = (long) m_svTopLeft.m_dPositionX;
	rect.bottom = (long) m_svBottomRight.m_dPositionY;
	rect.right = (long) m_svBottomRight.m_dPositionX;

	return rect;
}

SVExtentSizeStruct SVExtentFigureStruct::Size() const
{
	return SVExtentSizeStruct( m_svBottomRight.m_dPositionX - m_svTopLeft.m_dPositionX,  m_svBottomRight.m_dPositionY - m_svTopLeft.m_dPositionY );
}

double SVExtentFigureStruct::Top() const
{
	return m_svTopLeft.m_dPositionY;
}

double SVExtentFigureStruct::Left() const
{
	return m_svTopLeft.m_dPositionX;
}

double SVExtentFigureStruct::Right() const
{
	return m_svBottomRight.m_dPositionX;
}

HRESULT SVExtentFigureStruct::SetRect( double p_dTop, double p_dLeft, double p_dBottom, double p_dRight )
{
	SVExtentPointStruct l_svPoint;

	double l_dWidth = p_dRight - p_dLeft;
	double l_dHeight = p_dBottom - p_dTop;

	l_svPoint.m_dPositionX = p_dLeft;
	l_svPoint.m_dPositionY = p_dTop;

	Initialize();

	m_eShape = SVExtentShapeRectangle;

	m_svTopLeft = l_svPoint;
	m_svTopRight = l_svPoint;
	m_svBottomRight = l_svPoint;
	m_svBottomLeft = l_svPoint;
	m_svCenterLeft = l_svPoint;
	m_svCenterRight = l_svPoint;
	m_svTopCenter = l_svPoint;
	m_svBottomCenter = l_svPoint;
	m_svCenter = l_svPoint;

	m_svTopCenter.m_dPositionX += (l_dWidth / 2.0);

	m_svTopRight.m_dPositionX += l_dWidth;

	m_svCenterLeft.m_dPositionY += ( l_dHeight / 2.0 );

	m_svCenterRight.m_dPositionY += ( l_dHeight / 2.0 );
	m_svCenterRight.m_dPositionX += ( l_dWidth );

	m_svBottomLeft.m_dPositionY += ( l_dHeight );

	m_svBottomCenter.m_dPositionY += ( l_dHeight );
	m_svBottomCenter.m_dPositionX += ( l_dWidth / 2.0 );

	m_svBottomRight.m_dPositionY += ( l_dHeight );
	m_svBottomRight.m_dPositionX += ( l_dWidth );

	m_svCenter.m_dPositionX += ( l_dWidth / 2.0 );
	m_svCenter.m_dPositionY += ( l_dHeight / 2.0 );

	return S_OK;
}

HRESULT SVExtentFigureStruct::Initialize()
{
	HRESULT l_hrOk = S_OK;

	m_eShape = SVExtentShapeUnknown;

	if ( S_OK != m_svTopLeft.Initialize() )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svTopRight.Initialize() )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svBottomRight.Initialize() )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svBottomLeft.Initialize() )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svCenterLeft.Initialize() )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svCenterRight.Initialize() )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svTopCenter.Initialize() )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svBottomCenter.Initialize() )
	{
		l_hrOk = S_FALSE;
	}

	if ( S_OK != m_svCenter.Initialize() )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVExtentFigureStruct::IsPointOverFigure( const SVExtentPointStruct& p_rPoint ) const
{
	HRESULT l_hrOk = S_OK;

	if( m_eShape != SVExtentShapeUnknown )
	{
		POINT l_IntersectPt = { 0, 0 };

		SVExtentPointStruct l_Center = ( m_svTopCenter + m_svBottomCenter ) / 2.0;

		if( l_Center != p_rPoint )
		{
			bool l_Intersect = ( l_Center == m_svTopCenter ) || ( l_Center == m_svBottomCenter );

			l_Intersect = l_Intersect || Intersect::Lines( l_Center, p_rPoint, m_svTopCenter, m_svTopRight, l_IntersectPt ) == Intersect::DO_INTERSECT;
			l_Intersect = l_Intersect || Intersect::Lines( l_Center, p_rPoint, m_svTopRight, m_svCenterRight, l_IntersectPt ) == Intersect::DO_INTERSECT;
			l_Intersect = l_Intersect || Intersect::Lines( l_Center, p_rPoint, m_svCenterRight, m_svBottomRight, l_IntersectPt ) == Intersect::DO_INTERSECT;
			l_Intersect = l_Intersect || Intersect::Lines( l_Center, p_rPoint, m_svBottomRight, m_svBottomCenter, l_IntersectPt ) == Intersect::DO_INTERSECT;
			l_Intersect = l_Intersect || Intersect::Lines( l_Center, p_rPoint, m_svBottomCenter, m_svBottomLeft, l_IntersectPt ) == Intersect::DO_INTERSECT;
			l_Intersect = l_Intersect || Intersect::Lines( l_Center, p_rPoint, m_svBottomLeft, m_svCenterLeft, l_IntersectPt ) == Intersect::DO_INTERSECT;
			l_Intersect = l_Intersect || Intersect::Lines( l_Center, p_rPoint, m_svCenterLeft, m_svTopLeft, l_IntersectPt ) == Intersect::DO_INTERSECT;
			l_Intersect = l_Intersect || Intersect::Lines( l_Center, p_rPoint, m_svTopLeft, m_svTopCenter, l_IntersectPt ) == Intersect::DO_INTERSECT;

			if( l_Intersect )
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

