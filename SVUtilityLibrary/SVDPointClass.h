//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointClass
//* .File Name       : $Workfile:   SVDPointClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:20:56  $
//******************************************************************************

#ifndef _SVDPOINTCLASS_H
#define _SVDPOINTCLASS_H

#include "SVUtilityLibrary/SVString.h"

/**
@SVObjectName Double Point

@SVObjectOverview

@SVObjectOperations

*/
class SVDPointClass  
{
public:
	SVDPointClass();
	SVDPointClass( const POINT &Point );
	SVDPointClass( const SVDPointClass &Point );
	SVDPointClass( double dX, double dY );
	SVDPointClass( const VARIANT& p_rVariant );

	BOOL GetValue( POINT &Point );

	~SVDPointClass();

	operator POINT () const;
	operator _variant_t () const;

	SVString ToString() const;
	
	SVDPointClass& operator=( const POINT &Point );
	SVDPointClass& operator=( const SVDPointClass &Point );
	SVDPointClass& operator=( const VARIANT& p_rVariant );

	double x;
	double y;

	bool operator==( const SVDPointClass &Point ) const;
	bool operator!=( const SVDPointClass &Point ) const;

private:
	void local_initialize();
};

#endif // _SVDPOINTCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVUtilityLibrary\SVDPointClass.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:20:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Mar 2011 07:16:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix performance issues.
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
