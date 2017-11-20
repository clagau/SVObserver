//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPOINT
//* .File Name       : $Workfile:   SVPOINT.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:23:26  $
//******************************************************************************

#include "stdafx.h"
#include "StringHelper.h"
#include "SVPOINT.h"

SVPoint::SVPoint()
: m_Point()
{
	m_Point.x = 0;
	m_Point.y = 0;
}

SVPoint::SVPoint( const SVPoint& p_rObject )
: m_Point()
{
	m_Point.x = 0;
	m_Point.y = 0;

	*this = p_rObject;
}

SVPoint::SVPoint( const POINT& p_rPoint )
: m_Point()
{
	m_Point.x = 0;
	m_Point.y = 0;

	*this = p_rPoint;
}

SVPoint::SVPoint( const VARIANT& p_rVariant )
: m_Point()
{
	m_Point.x = 0;
	m_Point.y = 0;

	*this = p_rVariant;
}

SVPoint::SVPoint( long x, long y )
{
	m_Point.x = x;
	m_Point.y = y;
}

SVPoint::~SVPoint()
{
	m_Point.x = 0;
	m_Point.y = 0;
}

SVPoint::operator const POINT & () const
{
	return m_Point;
}

SVPoint::operator POINT & ()
{
	return m_Point;
}

SVPoint::operator _variant_t () const
{
	return _variant_t( ToString().c_str());
}

void SVPoint::GetValue(POINT &Point) const
{
	Point = m_Point;
}

std::string SVPoint::ToString() const
{
	return SvUl::Format(_T( "( %ld, %ld )" ), m_Point.x, m_Point.y );
}

const SVPoint& SVPoint::operator=( const SVPoint& p_rObject )
{
	if( this != &p_rObject )
	{
		m_Point = p_rObject.m_Point;
	}

	return *this;
}

const SVPoint& SVPoint::operator=( const POINT& p_rPoint )
{
	if( &m_Point != &p_rPoint )
	{
		m_Point = p_rPoint;
	}

	return *this;
}

const SVPoint& SVPoint::operator=( const VARIANT& p_rVariant )
{
	POINT l_Point;

	std::string l_String = SvUl::createStdString( p_rVariant );

	if( 2 == _stscanf( l_String.c_str(), _T( "( %ld, %ld )" ), &( l_Point.x ), &( l_Point.y ) ) )
	{
		m_Point = l_Point;
	}
	else
	{
		m_Point.x = 0;
		m_Point.y = 0;
	}

	return *this;
}

long SVPoint::x() const
{
	return m_Point.x;
}

long SVPoint::y() const
{
	return m_Point.y;
}

