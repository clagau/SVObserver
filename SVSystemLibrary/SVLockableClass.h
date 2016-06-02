//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLockableClass
//* .File Name       : $Workfile:   SVLockableClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:38:00  $
//******************************************************************************

#pragma once

namespace Seidenader { namespace SVSystemLibrary
{
	class SVLockableClass
	{
	public:
		SVLockableClass();

		virtual ~SVLockableClass();

		BOOL Lock( DWORD DWWaitTime = INFINITE ) const;
		BOOL Unlock() const;

	protected:
		HANDLE hProtectionMutex; 
	};
} /*SVSystemLibrary*/ } /*Seidenader*/

namespace SvSyl = Seidenader::SVSystemLibrary;

