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

#ifndef SVCONTAINABLECRITICALSECTION_H_
#define SVCONTAINABLECRITICALSECTION_H_
#pragma once

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
	~SVContainableCriticalSection();

	const SVContainableCriticalSection& operator = ( const SVContainableCriticalSection& rhs );
	operator CRITICAL_SECTION*();

	void swap( SVContainableCriticalSection& rhs );

	// SVSyncObject
	virtual HRESULT Unlock();
	virtual HRESULT Lock();
	virtual HRESULT Lock(DWORD dwTimeout);

	DWORD SetSpinCount( DWORD dwSpinCount );

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

#endif	//#ifndef SVCONTAINABLECRITICALSECTION_H_

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLibrary\SVContainableCriticalSection.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 11:52:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Sep 2009 09:22:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Jan 2006 09:17:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed template<> from swap
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jun 2005 14:06:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved from SVOMFCLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   20 Jun 2005 09:28:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/