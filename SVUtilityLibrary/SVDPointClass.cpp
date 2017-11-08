//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointClass
//* .File Name       : $Workfile:   SVDPointClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:20:40  $
//******************************************************************************

#include "stdafx.h"
#include "StringHelper.h"
#include "SVDPointClass.h"

SVDPointClass::SVDPointClass()
{
	local_initialize();
}

SVDPointClass::SVDPointClass(const POINT &Point)
{
	local_initialize();

	*this = Point;
}

SVDPointClass::SVDPointClass(const SVDPointClass &Point)
{
	local_initialize();

	*this = Point;
}

SVDPointClass::SVDPointClass( double dX, double dY )
{
	x = dX;
	y = dY;
}

SVDPointClass::SVDPointClass( const VARIANT& p_rVariant )
{
	local_initialize();

	*this = p_rVariant;
}

SVDPointClass::~SVDPointClass()
{
}

void SVDPointClass::GetValue(POINT &Point) const
{
	Point.x = static_cast< long >( x );
	Point.y = static_cast< long >( y );
}

SVDPointClass::operator POINT () const
{
	POINT l_Point;

	l_Point.x = static_cast< long >( x );
	l_Point.y = static_cast< long >( y );

	return l_Point;
}

SVDPointClass::operator _variant_t () const
{
	return _variant_t( ToString().c_str());
}

std::string SVDPointClass::ToString() const
{
	return SvUl::Format( _T( "( %lf, %lf )" ), x, y );
}

SVDPointClass& SVDPointClass::operator=( const POINT& Point )
{
	x = static_cast< double >( Point.x );
	y = static_cast< double >( Point.y );

	return (*this);
}

SVDPointClass& SVDPointClass::operator=( const SVDPointClass& Point )
{
	x = Point.x;
	y = Point.y;

	return (*this);
}

SVDPointClass& SVDPointClass::operator=( const VARIANT& p_rVariant )
{
	double l_x;
	double l_y;

	std::string l_String = SvUl::createStdString( p_rVariant );

	if( 2 == _stscanf( l_String.c_str(), _T( "( %lf, %lf )" ), &l_x, &l_y ) )
	{
		x = l_x;
		y = l_y;
	}
	else
	{
		x = 0.0;
		y = 0.0;
	}

	return (*this);
}

bool SVDPointClass::operator==( const SVDPointClass &Point ) const
{
	return ( x == Point.x && y == Point.y );
}

bool SVDPointClass::operator!=( const SVDPointClass &Point ) const
{
	return ( x != Point.x || y != Point.y );
}

void SVDPointClass::local_initialize()
{
	x = 0.0;
	y = 0.0;
}

