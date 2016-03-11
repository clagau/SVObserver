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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVPOINT.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:23:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Feb 2012 13:22:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated code to fix loading issue associated with POINT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 09:40:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:29:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
