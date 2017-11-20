//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPOINT
//* .File Name       : $Workfile:   SVPOINT.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:23:42  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#pragma endregion Includes

/**
@SVObjectName Point

@SVObjectOverview

@SVObjectOperations

*/
class SVPoint
{
public:
	SVPoint();
	SVPoint( const SVPoint& p_rObject );
	SVPoint( const POINT& p_rPoint );
	SVPoint( const VARIANT& p_rVariant );
	SVPoint( long x, long y );

	virtual ~SVPoint();

	operator const POINT & () const;
	operator POINT & ();

	operator _variant_t () const;

	std::string ToString() const;
	
	const SVPoint& operator=( const SVPoint& p_rObject );
	const SVPoint& operator=( const POINT& p_rPoint );
	const SVPoint& operator=( const VARIANT& p_rVariant );

	void GetValue( POINT &Point ) const;

	long x() const;
	long y() const;

protected:
	POINT m_Point;

};

