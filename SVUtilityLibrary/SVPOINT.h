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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVPOINT.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:23:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Feb 2012 13:22:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated code to fix loading issue associated with POINT.
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
