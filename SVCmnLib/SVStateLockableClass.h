// ******************************************************************************
// * COPYRIGHT (c) 2001 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVStateLockableClass
// * .File Name       : $Workfile:   SVStateLockableClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:10:08  $
// ******************************************************************************

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_SVSTATELOCKABLECLASS_3B24DD3301E4_INCLUDED
#define _INC_SVSTATELOCKABLECLASS_3B24DD3301E4_INCLUDED

#include "SVStateClass.h"

//This class holds and maintains information about the 
//current application state.
//##ModelId=3B24DD3301E4
class SVStateLockableClass : public SVStateClass
{
public:
	//This operation tries to create a mutex.  It checks the 
	//mutex to see if it was already created.  If it was not 
	//created, this operator locks and initializes the state 
	//value.
	//##ModelId=3B2622AA032C
	SVStateLockableClass();

	//This operation closes the mutex handle.
	//##ModelId=3B2622A90290
	virtual ~SVStateLockableClass();

	//This operation will explose the current Focus state 
	//value.
	//##ModelId=3B24E2CB004E
	virtual BOOL GetState(DWORD * pdwState);

	//This operation will try to set the state attribute.  
	//This operation will wait for the mutex with ten 
	//millisecond time-out.  If the mutex fails the status 
	//parameter is set to false.  Otherwise, the Focus state 
	//attribute is asigned the value of the state parameter.
	//##ModelId=3B24E2CE00DA
	virtual BOOL SetState(DWORD dwState);

	//This operation adds a substate to the existing state 
	//value.  The value passed in as a parameter is ORed to 
	//the existing value.  This operation will wait for the 
	//mutex with ten millisecond time-out.  If the mutex 
	//fails the status parameter is set to false.  Otherwise, 
	//the state attribute is asigned the value of the state 
	//parameter.
	//##ModelId=3B24E2CA008C
	virtual BOOL AddState(DWORD dwState);

	//This operation removes a particular substate from the 
	//existing state value.  This process takes the substate 
	//value and inverts it and ANDs it to the existing state 
	//value.  This operation will wait for the mutex with ten 
	//millisecond time-out.  If the mutex fails the status 
	//parameter is set to false.  Otherwise, the state 
	//attribute is asigned the value of the state parameter.
	//##ModelId=3B24E2CB037A
	virtual BOOL RemoveState(DWORD dwState);

	//This operation checks the parameter state value against 
	//the interal value and outputs in the result parameter 
	//whether they are equal on not.
	//##ModelId=3B2629AD002E
	virtual BOOL CheckState(DWORD dwState);

private:
	//This attribute contains a handle to the lock used to 
	//control processes or threads trying to change the state.
	//##ModelId=3B24E3F801E4
	HANDLE mhStateLock;

};

#endif /* _INC_SVSTATELOCKABLECLASS_3B24DD3301E4_INCLUDED */

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCmnLib\SVStateLockableClass.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:10:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Jun 2001 10:09:58   Joe
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  92
 * SCR Title:  Do not clear current inspection configuration when returning to Logo Screen
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Empty classes created by Rational Rose.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/