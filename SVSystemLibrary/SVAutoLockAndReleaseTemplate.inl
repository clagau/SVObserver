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

template< typename SVLockType >
SVAutoLockAndReleaseTemplate< SVLockType >::SVAutoLockAndReleaseTemplate()
: m_pLock( nullptr )
{
}

template< typename SVLockType >
bool SVAutoLockAndReleaseTemplate< SVLockType >::Assign( SVLockType* pLock, DWORD WaitTime )
{
	bool Result = true;

	if( nullptr != m_pLock )
	{
		m_pLock->Unlock();
	}

	m_pLock = pLock;

	if( nullptr != m_pLock )
	{
		Result = m_pLock->Lock( WaitTime ) ? true : false;

		if( !Result )
		{
			m_pLock = nullptr;
		}
	}

	return Result;
}

template< typename SVLockType >
SVAutoLockAndReleaseTemplate< SVLockType >::~SVAutoLockAndReleaseTemplate()
{
	if( nullptr != m_pLock )
	{
		m_pLock->Unlock();
	}
}

