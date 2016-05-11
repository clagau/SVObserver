// ******************************************************************************
// * COPYRIGHT (c) 2001 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVStateLockableClass
// * .File Name       : $Workfile:   SVStateLockableClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:10:06  $
// ******************************************************************************

#include "stdafx.h"
#include "SVStateLockableClass.h"

const DWORD dwWaitTime = 10;

SVStateLockableClass::SVStateLockableClass()
{
	mhStateLock = CreateMutex( nullptr, false, nullptr );
}

SVStateLockableClass::~SVStateLockableClass()
{
	if( mhStateLock )
	{
		if( WAIT_OBJECT_0 == WaitForSingleObject( mhStateLock, dwWaitTime ) )
		{
			CloseHandle( mhStateLock );
		}
	}
}

BOOL SVStateLockableClass::GetState(DWORD * pdwState) const
{
	if( mhStateLock )
	{
		if( WAIT_OBJECT_0 == WaitForSingleObject( mhStateLock, dwWaitTime ) )
		{
			return SVStateClass::GetState(pdwState);
		}
	}
	return false;
}

BOOL SVStateLockableClass::SetState(DWORD dwState)
{
	if( mhStateLock )
	{
		if( WAIT_OBJECT_0 == WaitForSingleObject( mhStateLock, dwWaitTime ) )
		{
			return SVStateClass::SetState(dwState);
		}
	}
	return false;
}

BOOL SVStateLockableClass::AddState(DWORD dwState)
{
	if( mhStateLock )
	{
		if( WAIT_OBJECT_0 == WaitForSingleObject( mhStateLock, dwWaitTime ) )
		{
			return SVStateClass::AddState(dwState);
		}
	}
	return false;
}

BOOL SVStateLockableClass::RemoveState(DWORD dwState)
{
	if( mhStateLock )
	{
		if( WAIT_OBJECT_0 == WaitForSingleObject( mhStateLock, dwWaitTime ) )
		{
			return SVStateClass::RemoveState(dwState);
		}
	}
	return false;
}

BOOL SVStateLockableClass::CheckState(DWORD dwState) const
{
	if( mhStateLock )
	{
		if( WAIT_OBJECT_0 == WaitForSingleObject( mhStateLock, dwWaitTime ) )
		{
			return SVStateClass::CheckState(dwState);
		}
	}
	return false;
}

