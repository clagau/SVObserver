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


BOOL SVDPointClass::GetValue(POINT &Point)
{
	Point.x = static_cast< long >( x );
	Point.y = static_cast< long >( y );

	return TRUE;
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
	return ToString().ToVARIANT();
}

SVString SVDPointClass::ToString() const
{
	SVString l_String;

	l_String.Format( _T( "( %lf, %lf )" ), x, y );

	return l_String;
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

	SVString l_String( p_rVariant );

	if( 2 == _stscanf( l_String.ToString(), _T( "( %lf, %lf )" ), &l_x, &l_y ) )
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVDPointClass.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:20:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Mar 2011 07:16:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix performance issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 09:40:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:28:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
