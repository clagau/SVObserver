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

//##ModelId=3B2622AA032C
SVStateLockableClass::SVStateLockableClass()
{
	mhStateLock = CreateMutex( NULL, FALSE, NULL );
}

//##ModelId=3B2622A90290
SVStateLockableClass::~SVStateLockableClass()
{
	if( mhStateLock )
		if( WaitForSingleObject( mhStateLock, dwWaitTime ) == WAIT_OBJECT_0 )
			CloseHandle( mhStateLock );
}

//##ModelId=3B24E2CB004E
BOOL SVStateLockableClass::GetState(DWORD * pdwState)
{
	if( mhStateLock )
		if( WaitForSingleObject( mhStateLock, dwWaitTime ) == WAIT_OBJECT_0 )
			return SVStateClass::GetState(pdwState);

	return FALSE;
}

//##ModelId=3B24E2CE00DA
BOOL SVStateLockableClass::SetState(DWORD dwState)
{
	if( mhStateLock )
		if( WaitForSingleObject( mhStateLock, dwWaitTime ) == WAIT_OBJECT_0 )
			return SVStateClass::SetState(dwState);

	return FALSE;
}

//##ModelId=3B24E2CA008C
BOOL SVStateLockableClass::AddState(DWORD dwState)
{
	if( mhStateLock )
		if( WaitForSingleObject( mhStateLock, dwWaitTime ) == WAIT_OBJECT_0 )
			return SVStateClass::AddState(dwState);

	return FALSE;
}

//##ModelId=3B24E2CB037A
BOOL SVStateLockableClass::RemoveState(DWORD dwState)
{
	if( mhStateLock )
		if( WaitForSingleObject( mhStateLock, dwWaitTime ) == WAIT_OBJECT_0 )
			return SVStateClass::RemoveState(dwState);

	return FALSE;
}

//##ModelId=3B2629AD002E
BOOL SVStateLockableClass::CheckState(DWORD dwState)
{
	if( mhStateLock )
		if( WaitForSingleObject( mhStateLock, dwWaitTime ) == WAIT_OBJECT_0 )
			return SVStateClass::CheckState(dwState);

	return FALSE;
}

