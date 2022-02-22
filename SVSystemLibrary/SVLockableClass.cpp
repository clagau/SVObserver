//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#endif

namespace SvSyl
{
	SVLockableClass::SVLockableClass()
		: hProtectionMutex( nullptr )
	{
		hProtectionMutex = CreateMutex( nullptr, false, nullptr );
	}

	SVLockableClass::~SVLockableClass()
	{
		if( nullptr != hProtectionMutex )
		{
			CloseHandle( hProtectionMutex );
		}
	}

	bool SVLockableClass::Lock( DWORD DWWaitTime /* = INFINITE */ ) const
	{
		bool l_Status = ( nullptr != hProtectionMutex );

		if( l_Status )
		{
			l_Status = ( WaitForSingleObject( hProtectionMutex, DWWaitTime ) == WAIT_OBJECT_0 );
		}

		return l_Status;
	}

	bool SVLockableClass::Unlock() const
	{
		bool l_Status = ( nullptr != hProtectionMutex );

		if( l_Status )
		{
			l_Status = (ReleaseMutex( hProtectionMutex )?true:false);
		}

		return l_Status;
	}
} //namespace SvSyl

