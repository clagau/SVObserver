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
	m_pCritSec = nullptr;
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

