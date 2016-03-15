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

