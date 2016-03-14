//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLockableClass
//* .File Name       : $Workfile:   SVLockableClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:37:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVLockableClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace Seidenader
{
	namespace SVSystemLibrary
	{
		SVLockableClass::SVLockableClass()
			: hProtectionMutex( NULL )
		{
			hProtectionMutex = CreateMutex( NULL, FALSE, NULL );
		}

		SVLockableClass::~SVLockableClass()
		{
			if( hProtectionMutex != NULL )
			{
				CloseHandle( hProtectionMutex );
			}
		}


		BOOL SVLockableClass::Lock( DWORD DWWaitTime /* = INFINITE */ ) const
		{
			BOOL l_Status = ( hProtectionMutex != NULL );

			if( l_Status )
			{
				l_Status = ( WaitForSingleObject( hProtectionMutex, DWWaitTime ) == WAIT_OBJECT_0 );
			}

			return l_Status;
		}

		BOOL SVLockableClass::Unlock() const
		{
			BOOL l_Status = ( hProtectionMutex != NULL );

			if( l_Status )
			{
				l_Status = ReleaseMutex( hProtectionMutex );
			}

			return l_Status;
		}

		HANDLE SVLockableClass::GetLockHandle()
		{
			return hProtectionMutex;
		}
	} //SVSystemLibrary
} //Seidenader

