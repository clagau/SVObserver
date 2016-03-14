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

#ifndef SVPOINT_H
#define SVPOINT_H

#include <comdef.h>

#include "SVString.h"

/**
@SVObjectName Point

@SVObjectOverview

@SVObjectOperations

*/
class SVPOINT
{
public:
	SVPOINT();
	SVPOINT( const SVPOINT& p_rObject );
	SVPOINT( const POINT& p_rPoint );
	SVPOINT( const VARIANT& p_rVariant );
	SVPOINT( long x, long y );

	virtual ~SVPOINT();

	operator const POINT & () const;
	operator POINT & ();

	operator _variant_t () const;

	SVString ToString() const;
	
	const SVPOINT& operator=( const SVPOINT& p_rObject );
	const SVPOINT& operator=( const POINT& p_rPoint );
	const SVPOINT& operator=( const VARIANT& p_rVariant );

	BOOL GetValue( POINT &Point ) const;

	long x() const;
	long y() const;

protected:
	POINT m_Point;

};

#endif // #ifndef SVPOINT_H

