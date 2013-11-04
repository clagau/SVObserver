// ******************************************************************************
// * COPYRIGHT (c) 2001 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVStateClass
// * .File Name       : $Workfile:   SVStateClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:10:06  $
// ******************************************************************************

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_SVSTATECLASS_3AC0E41A034B_INCLUDED
#define _INC_SVSTATECLASS_3AC0E41A034B_INCLUDED

//##ModelId=3AC0E41A034B
class SVStateClass 
{
public:
	//##ModelId=3AC0E6A6003F
	SVStateClass();

	//##ModelId=3AC0E69D00FA
	virtual ~SVStateClass();

	//This operation returns the value in contained in the 
	//state attribute.
	//##ModelId=3AC0E47C009C
	virtual BOOL GetState(DWORD * pdwState);

	//This operation will try to set the state attribute.  
	//This operation will wait for the mutex with ten 
	//millisecond time-out.  If the mutex fails the status 
	//parameter is set to false.  Otherwise, the SVIM state 
	//attribute is asigned the value of the state parameter.
	//##ModelId=3AC0E50D00FA
	virtual BOOL SetState(DWORD dwState);

	//This operation adds a substate to the existing state 
	//value.  The value passed in as a parameter is ORed to 
	//the existing value.
	//##ModelId=3AC0ED3D02AF
	virtual BOOL AddState(DWORD dwState);

	//This operation removes a particular substate from the 
	//existing state value.  This process takes the substate 
	//value and inverts it and ANDs it to the existing state 
	//value.
	//##ModelId=3AC0ED850157
	virtual BOOL RemoveState(DWORD dwState);

	//This operation checks the parameter state value against 
	//the interal value and outputs in the result parameter 
	//whether they are equal on not.
	//##ModelId=3AB7CF850167
	virtual BOOL CheckState(DWORD dwState);

private:
	//This attribute contain the state value.
	//##ModelId=3AC0E4CC0186
	DWORD mdwState;

};

#endif /* _INC_SVSTATECLASS_3AC0E41A034B_INCLUDED */

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVCmnLib\SVStateClass.h_v  $
 * 
 *    Rev 1.0   18 Apr 2013 18:10:06   bWalter
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