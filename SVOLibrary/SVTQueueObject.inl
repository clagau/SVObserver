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
bool __stdcall InitializeCriticalSectionAndSpinCount_(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount);

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
bool SVTQueueObject<T>::Create()
{
	if( !m_bCreated )
	{
		m_bCreated = InitializeCriticalSectionAndSpinCount_( &m_hCriticalSection, 4000 );
	}
	return m_bCreated;
}

template <typename T>
void SVTQueueObject<T>::Destroy()
{
	if( m_bCreated )
	{
		::DeleteCriticalSection( &m_hCriticalSection );

		m_bCreated = false;
	}
}

template <typename T>
bool SVTQueueObject<T>::IsCreated() const
{
	return m_bCreated;
}// end IsCreated

template <typename T>
bool SVTQueueObject<T>::Lock()
{
	if( m_bCreated )
	{
		::EnterCriticalSection( &m_hCriticalSection );

		return true;
	}// end if

	return false;
}// end Lock

template <typename T>
bool SVTQueueObject<T>::Unlock()
{
	if( m_bCreated )
	{
		::LeaveCriticalSection( &m_hCriticalSection );

		return true;
	}// end if

	return false;
}// end Unlock

template <typename T>
bool SVTQueueObject<T>::AddHead( T pParam )
{
	if( m_bCreated )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		m_Queue.push_front( pParam );

		return true;
	}// end if

	return false;
}// end AddHead

template <typename T>
bool SVTQueueObject<T>::AddTail( T pParam )
{
	if( m_bCreated )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		m_Queue.push_back( pParam );

		return true;
	}// end if

	return false;
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
bool SVTQueueObject<T>::GetSize( long &lSize )
{
	if( m_bCreated )
	{
		lSize = static_cast< long >( m_Queue.size() );

		return true;
	}// end if

	return false;
}// end GetSize

template <typename T>
bool SVTQueueObject<T>::GetHead( T* pParam )
{
	if( m_bCreated && nullptr != pParam )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		if( 0 < m_Queue.size() )
		{
			*pParam = m_Queue.front();

			return true;
		}
	}

	return false;
}

template <typename T>
bool SVTQueueObject<T>::GetTail( T* pParam )
{
	if( m_bCreated && nullptr != pParam )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		if( 0 < m_Queue.size() )
		{
			*pParam = m_Queue.back();

			return true;
		}
	}

	return false;
}

template <typename T>
bool SVTQueueObject<T>::GetAt( long lPosition, T* pParam )
{
	if( m_bCreated )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		if( 0 <= lPosition && lPosition < static_cast< long >( m_Queue.size() ) )
		{
			*pParam = m_Queue[ lPosition ];

			return true;
		}
	}

	return false;
}

template <typename T>
bool SVTQueueObject<T>::GetHeadPosition( QUEUEPOS& pos )
{
	if( m_bCreated )
	{
		pos = 0;

		return true;
	}

	return false;
}// end GetHeadPosition

template <typename T>
bool SVTQueueObject<T>::GetNextItem( QUEUEPOS& pos, T* pParam )
{
	if( m_bCreated )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		if( 0 <= pos && static_cast< size_t>( pos ) < m_Queue.size() )
		{
			*pParam = m_Queue[ pos++ ];

			return true;
		}
	}

	return false;
}// end GetNextItem

template <typename T>
bool SVTQueueObject<T>::RemoveHead( T* pParam )
{
	if( m_bCreated )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		if( GetHead( pParam ) )
		{
			m_Queue.pop_front();

			return true;
		}
	}

	return false;
}

template <typename T>
bool SVTQueueObject<T>::RemoveAt( long lPosition )
{
	if( m_bCreated )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		auto l_Iter = m_Queue.begin();

		std::advance( l_Iter, lPosition );

		if( l_Iter != m_Queue.end() )
		{
			m_Queue.erase( l_Iter );

			return true;
		}
	}

	return false;
}

template <typename T>
bool SVTQueueObject<T>::RemoveAll()
{
	if( m_bCreated )
	{
		SVCriticalSectionSingleLock lock( m_critsec );

		m_Queue.clear();

		return true;
	}

	return false;
}

