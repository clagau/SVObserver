// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVContainableCriticalSection
// * .File Name       : $Workfile:   SVContainableCriticalSection.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:52:52  $
// ******************************************************************************
#include <assert.h>

inline SVContainableCriticalSection::SVContainableCriticalSection( const SVContainableCriticalSection& rhs )
: m_dwSpinCount(0)
{
	Acquire( rhs );
}

inline const SVContainableCriticalSection& SVContainableCriticalSection::operator = ( const SVContainableCriticalSection& rhs )
{
	if ( this != &rhs )
	{
		Release();
		Acquire( rhs );
	}
	return *this;
}

inline SVContainableCriticalSection::~SVContainableCriticalSection()
{
	Release();
}

inline void SVContainableCriticalSection::Acquire( const SVContainableCriticalSection& rhs )
{
	m_plRefCount = rhs.m_plRefCount;
	m_pCritSec = rhs.m_pCritSec;
	m_dwSpinCount = rhs.m_dwSpinCount;
	long lRefCount = ::InterlockedIncrement( const_cast <long*> (m_plRefCount) );	// cast away volatile
	assert( lRefCount > 1 );
}

inline void SVContainableCriticalSection::Release()
{
	long lRefCount = ::InterlockedDecrement( const_cast <long*> (m_plRefCount) );	// cast away volatile
	assert( lRefCount >= 0 );
	if ( lRefCount == 0 )
	{
		delete (const_cast<long*> (m_plRefCount));
		::DeleteCriticalSection(m_pCritSec);
		delete m_pCritSec;
	}
	m_plRefCount = 0;
	m_pCritSec = NULL;
}

inline SVContainableCriticalSection::operator CRITICAL_SECTION*()
{
	return static_cast <CRITICAL_SECTION*> (m_pCritSec);
}

inline void SVContainableCriticalSection::swap( SVContainableCriticalSection& rhs )
{
	if ( this != &rhs )
	{
		std::swap( m_plRefCount, rhs.m_plRefCount );
		std::swap( m_dwSpinCount, rhs.m_dwSpinCount );
		std::swap( m_pCritSec, rhs.m_pCritSec );
	}
}

inline HRESULT SVContainableCriticalSection::Unlock()
{
	::LeaveCriticalSection( m_pCritSec );
	return S_OK;
}

inline HRESULT SVContainableCriticalSection::Lock()
{
	::EnterCriticalSection( m_pCritSec );
	return S_OK;
}

inline HRESULT SVContainableCriticalSection::Lock(DWORD dwTimeout)
{
	return Lock();
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLibrary\SVContainableCriticalSection.inl_v  $
 * 
 *    Rev 1.0   25 Apr 2013 11:52:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Oct 2012 09:14:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with construction and assignment operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Sep 2009 09:22:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
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
