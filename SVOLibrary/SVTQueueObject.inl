// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTQueueObject
// * .File Name       : $Workfile:   SVTQueueObject.inl  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 12:23:26  $
// ******************************************************************************

// the following proxy functions are implemented in SVQueueObject.cpp
BOOL __stdcall InitializeCriticalSectionAndSpinCount_(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount);

template <typename T>
SVTQueueObject<T>::SVTQueueObject()
: m_critsec(m_hCriticalSection), m_bCreated( false )
{
}

template <typename T>
SVTQueueObject<T>::~SVTQueueObject()
{
	Destroy();
}

template <typename T>
void SVTQueueObject<T>::clear()
{
	if( m_bCreated )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		if( !( m_Queue.empty() ) )
		{
			m_Queue.clear();
		}
	}
}

template <typename T>
void SVTQueueObject<T>::pop_front()
{
	if( m_bCreated )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		if( !( m_Queue.empty() ) )
		{
			m_Queue.pop_front();
		}
	}
}

template <typename T>
void SVTQueueObject<T>::push_back( const T& p_rValue )
{
	if( m_bCreated )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		m_Queue.push_back( p_rValue );
	}
}

template <typename T>
BOOL SVTQueueObject<T>::Create()
{
	if( !m_bCreated )
	{
		m_bCreated = InitializeCriticalSectionAndSpinCount_( &m_hCriticalSection, 4000 );
	}
	return m_bCreated;
}

template <typename T>
BOOL SVTQueueObject<T>::Destroy()
{
	if( m_bCreated )
	{
		::DeleteCriticalSection( &m_hCriticalSection );

		m_bCreated = false;
	}

	return TRUE;
}

template <typename T>
BOOL SVTQueueObject<T>::IsCreated() const
{
	return m_bCreated;
}// end IsCreated

template <typename T>
BOOL SVTQueueObject<T>::Lock()
{
	if( m_bCreated )
	{
		::EnterCriticalSection( &m_hCriticalSection );

		return TRUE;
	}// end if

	return FALSE;
}// end Lock

template <typename T>
BOOL SVTQueueObject<T>::Unlock()
{
	if( m_bCreated )
	{
		::LeaveCriticalSection( &m_hCriticalSection );

		return TRUE;
	}// end if

	return FALSE;
}// end Unlock

template <typename T>
BOOL SVTQueueObject<T>::AddHead( T pParam )
{
	if( m_bCreated )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		m_Queue.push_front( pParam );

		return TRUE;
	}// end if

	return FALSE;
}// end AddHead

template <typename T>
BOOL SVTQueueObject<T>::AddTail( T pParam )
{
	if( m_bCreated )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		m_Queue.push_back( pParam );

		return TRUE;
	}// end if

	return FALSE;
}// end AddTail

template <typename T>
int SVTQueueObject<T>::GetCount()
{
	int iSize = 0;

	if( m_bCreated )
	{
		iSize = static_cast< int >( m_Queue.size() );
	}

	return iSize;
}

template <typename T>
BOOL SVTQueueObject<T>::GetSize( long &lSize )
{
	if( m_bCreated )
	{
		lSize = static_cast< long >( m_Queue.size() );

		return TRUE;
	}// end if

	return FALSE;
}// end GetSize

template <typename T>
BOOL SVTQueueObject<T>::GetHead( T* pParam )
{
	if( m_bCreated && pParam != NULL )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		if( 0 < m_Queue.size() )
		{
			*pParam = m_Queue.front();

			return TRUE;
		}
	}

	return FALSE;
}

template <typename T>
BOOL SVTQueueObject<T>::GetTail( T* pParam )
{
	if( m_bCreated && pParam != NULL )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		if( 0 < m_Queue.size() )
		{
			*pParam = m_Queue.back();

			return TRUE;
		}
	}

	return FALSE;
}

template <typename T>
BOOL SVTQueueObject<T>::GetAt( long lPosition, T* pParam )
{
	if( m_bCreated )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		if( 0 <= lPosition && lPosition < static_cast< long >( m_Queue.size() ) )
		{
			*pParam = m_Queue[ lPosition ];

			return TRUE;
		}
	}

	return FALSE;
}

template <typename T>
BOOL SVTQueueObject<T>::GetHeadPosition( QUEUEPOS& pos )
{
	if( m_bCreated )
	{
		pos = 0;

		return TRUE;
	}

	return FALSE;
}// end GetHeadPosition

template <typename T>
BOOL SVTQueueObject<T>::GetNextItem( QUEUEPOS& pos, T* pParam )
{
	if( m_bCreated )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		if( 0 <= pos && static_cast< size_t>( pos ) < m_Queue.size() )
		{
			*pParam = m_Queue[ pos++ ];

			return TRUE;
		}
	}

	return FALSE;
}// end GetNextItem

template <typename T>
BOOL SVTQueueObject<T>::RemoveHead( T* pParam )
{
	if( m_bCreated )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		if( GetHead( pParam ) )
		{
			m_Queue.pop_front();

			return TRUE;
		}
	}

	return FALSE;
}

template <typename T>
BOOL SVTQueueObject<T>::RemoveAt( long lPosition )
{
	if( m_bCreated )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		SVQueueBase::iterator l_Iter = m_Queue.begin();

		std::advance( l_Iter, lPosition );

		if( l_Iter != m_Queue.end() )
		{
			m_Queue.erase( l_Iter );

			return TRUE;
		}
	}

	return FALSE;
}

template <typename T>
BOOL SVTQueueObject<T>::RemoveAll()
{
	if( m_bCreated )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		m_Queue.clear();

		return TRUE;
	}

	return FALSE;
}

