//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCriticalSection
//* .File Name       : $Workfile:   SVCriticalSection.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:50:06  $
//******************************************************************************

#pragma once

#include "SVUtilityLibrary/SVSharedPtr.h"

class SVCriticalSection
{
public:
	SVCriticalSection( DWORD p_SpinCount = 4000 );

	virtual ~SVCriticalSection();

	BOOL Lock( DWORD p_TimeOutMilliseconds = INFINITE ) const;
	BOOL Unlock() const;

protected:
	BOOL EnterLock() const;
	BOOL TryEnterLock() const;

	DWORD m_SpinCount;

	// This attribute denotes whether the lock is created.
	mutable bool m_IsLockCreated;

	// This attribute manages the lock for multi-threaded access to this container.
	mutable CRITICAL_SECTION m_Lock;

private:
	// Do Not Implement
	SVCriticalSection( const SVCriticalSection& p_rObject );

	// Do Not Implement
	const SVCriticalSection& operator=( const SVCriticalSection& p_rObject );

};

typedef SVSharedPtr< SVCriticalSection > SVCriticalSectionPtr;

