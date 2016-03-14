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
#include "SVPOINT.h"

SVPOINT::SVPOINT()
: m_Point()
{
	m_Point.x = 0;
	m_Point.y = 0;
}

SVPOINT::SVPOINT( const SVPOINT& p_rObject )
: m_Point()
{
	m_Point.x = 0;
	m_Point.y = 0;

	*this = p_rObject;
}

SVPOINT::SVPOINT( const POINT& p_rPoint )
: m_Point()
{
	m_Point.x = 0;
	m_Point.y = 0;

	*this = p_rPoint;
}

SVPOINT::SVPOINT( const VARIANT& p_rVariant )
: m_Point()
{
	m_Point.x = 0;
	m_Point.y = 0;

	*this = p_rVariant;
}

SVPOINT::SVPOINT( long x, long y )
{
	m_Point.x = x;
	m_Point.y = y;
}

SVPOINT::~SVPOINT()
{
	m_Point.x = 0;
	m_Point.y = 0;
}

SVPOINT::operator const POINT & () const
{
	return m_Point;
}

SVPOINT::operator POINT & ()
{
	return m_Point;
}

SVPOINT::operator _variant_t () const
{
	return _variant_t( ToString().c_str());
}

BOOL SVPOINT::GetValue(POINT &Point) const
{
	Point = m_Point;
	return TRUE;
}

SVString SVPOINT::ToString() const
{
	return SvUl_SF::Format(_T( "( %ld, %ld )" ), m_Point.x, m_Point.y );
}

const SVPOINT& SVPOINT::operator=( const SVPOINT& p_rObject )
{
	if( this != &p_rObject )
	{
		m_Point = p_rObject.m_Point;
	}

	return *this;
}

const SVPOINT& SVPOINT::operator=( const POINT& p_rPoint )
{
	if( &m_Point != &p_rPoint )
	{
		m_Point = p_rPoint;
	}

	return *this;
}

const SVPOINT& SVPOINT::operator=( const VARIANT& p_rVariant )
{
	POINT l_Point;

	SVString l_String = SvUl_SF::createSVString( p_rVariant );

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

long SVPOINT::x() const
{
	return m_Point.x;
}

long SVPOINT::y() const
{
	return m_Point.y;
}

