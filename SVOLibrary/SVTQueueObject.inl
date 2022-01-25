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

template <typename T>
void SVTQueueObject<T>::clear()
{
	{
		std::lock_guard<std::mutex> lock {m_queueMutex};
		m_Queue.clear();
	}
}

template <typename T>
void SVTQueueObject<T>::pop_front()
{
	std::lock_guard<std::mutex> lock {m_queueMutex};

	if( !( m_Queue.empty() ) )
	{
		m_Queue.pop_front();
	}
}

template <typename T>
void SVTQueueObject<T>::push_back( const T& p_rValue )
{
	std::lock_guard<std::mutex> lock {m_queueMutex};
	m_Queue.push_back( p_rValue );
}


template <typename T>
bool SVTQueueObject<T>::AddHead( T pParam )
{
	std::lock_guard<std::mutex> lock {m_queueMutex};
	m_Queue.push_front( pParam );
	
	return true;
}

template <typename T>
bool SVTQueueObject<T>::AddTail( T pParam )
{
	std::lock_guard<std::mutex> lock {m_queueMutex};
	m_Queue.push_back( pParam );

	return true;
}

template <typename T>
int SVTQueueObject<T>::GetCount()
{
	std::lock_guard<std::mutex> lock {m_queueMutex};
	return static_cast< int >( m_Queue.size() );
}

template <typename T>
bool SVTQueueObject<T>::GetSize( long &lSize )
{
	std::lock_guard<std::mutex> lock {m_queueMutex};
	lSize = static_cast< long >( m_Queue.size() );
	return true;
}

template <typename T>
bool SVTQueueObject<T>::GetHead( T* pParam )
{
	if(nullptr != pParam )
	{
		std::lock_guard<std::mutex> lock {m_queueMutex};

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
	if(nullptr != pParam )
	{
		std::lock_guard<std::mutex> lock {m_queueMutex};

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
	std::lock_guard<std::mutex> lock {m_queueMutex};

	if( 0 <= lPosition && lPosition < static_cast<long> (m_Queue.size()))
	{
		*pParam = m_Queue[ lPosition ];

		return true;
	}

	return false;
}

template <typename T>
bool SVTQueueObject<T>::GetHeadPosition( QUEUEPOS& pos )
{
	pos = 0;
	return true;
}

template <typename T>
bool SVTQueueObject<T>::GetNextItem( QUEUEPOS& pos, T* pParam )
{
	std::lock_guard<std::mutex> lock {m_queueMutex};

	if( 0 <= pos && static_cast< size_t>( pos ) < m_Queue.size() )
	{
		*pParam = m_Queue[ pos++ ];
		return true;
	}

	return false;
}// end GetNextItem

template <typename T>
bool SVTQueueObject<T>::RemoveHead( T* pParam )
{
	std::lock_guard<std::mutex> lock {m_queueMutex};
	if (0 < m_Queue.size())
	{
		*pParam = std::move(m_Queue.front());
		m_Queue.pop_front();
		return true;
	}

	return false;
}

template <typename T>
bool SVTQueueObject<T>::RemoveAt( long lPosition )
{
	std::lock_guard<std::mutex> lock {m_queueMutex};

	auto l_Iter = m_Queue.begin();

	std::advance( l_Iter, lPosition );

	if( l_Iter != m_Queue.end() )
	{
		m_Queue.erase( l_Iter );
		return true;
	}

	return false;
}

template <typename T>
bool SVTQueueObject<T>::RemoveAll()
{
	std::lock_guard<std::mutex> lock {m_queueMutex};
	m_Queue.clear();
	return true;
}

