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
BOOL __stdcall InitializeCriticalSectionAndSpinCount_(
  LPCRITICAL_SECTION lpCriticalSection,  // critical section
  DWORD dwSpinCount                      // spin count
);

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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLibrary\SVTQueueObject.inl_v  $
 * 
 *    Rev 1.0   25 Apr 2013 12:23:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   19 Dec 2011 14:17:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to check if container is not empty before clearing or popping elements.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   14 Jul 2011 08:33:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated queue container to include a pop head method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   19 Jan 2010 14:55:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   24 Sep 2009 09:26:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed compiler warning about type comparision mismatch.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Sep 2009 17:56:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added two new methods to get and remove elements at indexes in the queue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Jul 2009 08:56:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   14 Jul 2009 10:40:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the code to handle buffer transfer and signal handling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Jan 2006 09:16:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added crit sec locking object
 * modified RemoveHead to compile with T not being a pointer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Oct 2005 09:18:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change SVTQueue to use smallObj to allocate memory instead of using new and delete because they cause a lock on the system.  The SVTQueue class has a member variable that allocates memory from a pool that is owned by the queue object.  The initial size of this pool is set to 4096.  This may need changed in the future depending on how much is wasted.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Aug 2003 13:04:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
