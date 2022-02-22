//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLockableClass
//* .File Name       : $Workfile:   SVLockableClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:38:00  $
//******************************************************************************

#pragma once

namespace SvSyl
{
	class SVLockableClass
	{
	public:
		SVLockableClass();

		virtual ~SVLockableClass();

		bool Lock( DWORD DWWaitTime = INFINITE ) const;
		bool Unlock() const;

	protected:
		HANDLE hProtectionMutex; 
	};
} //namespace SvSyl

