//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAutoLockAndReleaseTemplate
//* .File Name       : $Workfile:   SVAutoLockAndReleaseTemplate.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:49:08  $
//******************************************************************************

#ifndef SVAUTOLOCKANDRELEASETEMPLATE_INL
#define SVAUTOLOCKANDRELEASETEMPLATE_INL

#include "SVAutoLockAndReleaseTemplate.h"

template< typename SVLockType >
SVAutoLockAndReleaseTemplate< SVLockType >::SVAutoLockAndReleaseTemplate()
: m_pLock( nullptr )
{
}

template< typename SVLockType >
BOOL SVAutoLockAndReleaseTemplate< SVLockType >::Assign( SVLockType* p_pLock, DWORD p_WaitTime )
{
	BOOL l_Status = true;

	if( nullptr != m_pLock )
	{
		m_pLock->Unlock();
	}

	m_pLock = p_pLock;

	if( nullptr != m_pLock )
	{
		l_Status = m_pLock->Lock( p_WaitTime );

		if( !l_Status )
		{
			m_pLock = nullptr;
		}
	}

	return l_Status;
}

template< typename SVLockType >
SVAutoLockAndReleaseTemplate< SVLockType >::~SVAutoLockAndReleaseTemplate()
{
	if( nullptr != m_pLock )
	{
		m_pLock->Unlock();
	}
}

#endif

