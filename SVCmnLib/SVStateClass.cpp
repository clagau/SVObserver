// ******************************************************************************
// * COPYRIGHT (c) 2001 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVStateClass
// * .File Name       : $Workfile:   SVStateClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:10:06  $
// ******************************************************************************

#include "stdafx.h"
#include "SVStateClass.h"



//##ModelId=3AC0E6A6003F
SVStateClass::SVStateClass()
{
	mdwState = 0;
}

//##ModelId=3AC0E69D00FA
SVStateClass::~SVStateClass()
{
}

//##ModelId=3AC0E47C009C
BOOL SVStateClass::GetState(DWORD * pdwState)
{
	*pdwState = mdwState;
	return TRUE;
}

//##ModelId=3AC0E50D00FA
BOOL SVStateClass::SetState(DWORD dwState)
{
	mdwState = dwState;
	return TRUE;
}

//##ModelId=3AC0ED3D02AF
BOOL SVStateClass::AddState(DWORD dwState)
{
	mdwState |= dwState;
	return TRUE;
}

//##ModelId=3AC0ED850157
BOOL SVStateClass::RemoveState(DWORD dwState)
{
	mdwState &= ~dwState;
	return TRUE;
}

//##ModelId=3AB7CF850167
BOOL SVStateClass::CheckState(DWORD dwState)
{
	if (mdwState != dwState)
		return FALSE;
	return TRUE;
}

