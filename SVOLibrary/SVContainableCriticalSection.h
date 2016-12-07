// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVContainableCriticalSection
// * .File Name       : $Workfile:   SVContainableCriticalSection.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:52:36  $
// ******************************************************************************

#pragma once

//Moved to precompiled header: #include <assert.h>
#include "SVMultithread.h"

// this class should look mostly like CCriticalSection
// except that this class has correct copying semantics
// to enable it to be used in containers.
// Only when the ref count goes to 0 on destruction will the CRITICAL_SECTION
// be deleted. When a ContainableCriticalSection is copied, it increments the ref count.
// When the destructor happens, it decrements the ref count.

class SVContainableCriticalSection : public SVSyncObject
{
public:
	SVContainableCriticalSection( DWORD dwSpinCount = 0 );
	SVContainableCriticalSection( const SVContainableCriticalSection& rhs );
	virtual ~SVContainableCriticalSection();

	const SVContainableCriticalSection& operator = ( const SVContainableCriticalSection& rhs );
	operator CRITICAL_SECTION*();

	void swap( SVContainableCriticalSection& rhs );

	// SVSyncObject
	virtual HRESULT Unlock() override;
	virtual HRESULT Lock() override;
	virtual HRESULT Lock(DWORD dwTimeout) override;

	CRITICAL_SECTION* m_pCritSec;
	DWORD             m_dwSpinCount;

private:
	void Acquire( const SVContainableCriticalSection& rhs );
	void Release();

	mutable volatile long* m_plRefCount;	// mutable to allow this class to be const-friendly
};

// we don't want to use the default std::swap because it will invoke copy constructors and destructors.
// Make the efficient member function available to algorithms:

inline void swap ( SVContainableCriticalSection& lhs, SVContainableCriticalSection& rhs )
{
	lhs.swap(rhs);
}

namespace std	// yes this is legal and good practice; see Effective C++ 3rd ed. Item 25
{
	//template<>
	inline void swap( SVContainableCriticalSection& lhs, SVContainableCriticalSection& rhs )
	{
		lhs.swap(rhs);
	}
};

#include "SVContainableCriticalSection.inl"

