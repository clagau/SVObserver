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

SVStateClass::SVStateClass()
: mdwState(0)
{
	
}

SVStateClass::~SVStateClass()
{
}

BOOL SVStateClass::GetState(DWORD * pdwState) const
{
	*pdwState = mdwState;
	return true;
}

BOOL SVStateClass::SetState(DWORD dwState)
{
	mdwState = dwState;
	return true;
}

BOOL SVStateClass::AddState(DWORD dwState)
{
	mdwState |= dwState;
	return true;
}

BOOL SVStateClass::RemoveState(DWORD dwState)
{
	mdwState &= ~dwState;
	return true;
}

BOOL SVStateClass::CheckState(DWORD dwState) const
{
	if (mdwState != dwState)
	{
		return false;
	}
	return true;
}
