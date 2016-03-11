//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMemoryManager
//* .File Name       : $Workfile:   SVMemoryManager.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:36:16  $
//******************************************************************************

template <typename OWNERTYPE>
inline HRESULT SVMemoryManager<OWNERTYPE>::CreatePool( LPCTSTR strPoolName, __int64 lPoolSizeKBytes )
{
	HRESULT hr = S_FALSE;

	SVMemoryPoolMap::iterator iter = m_mapPools.find( SVString(strPoolName) );
	if ( iter == m_mapPools.end() )// ensure it doesn't already exist
	{
		//m_mapPools[ strPoolName ] = new SVMemoryPool;
		//hr = m_mapPools[ strPoolName ]->Create( lPoolSizeKBytes );
		TRACE(_T("SVMemoryManager::CreatePool(%s, %d)\n"), strPoolName, lPoolSizeKBytes*1024);
		hr = m_mapPools[ SVString(strPoolName) ].Create( lPoolSizeKBytes );
	}
	return hr;
}

template <typename OWNERTYPE>
inline HRESULT SVMemoryManager<OWNERTYPE>::ReservePoolMemory( LPCTSTR strPoolName, OWNERTYPE owner, __int64 lSizeInBytes )
{
	HRESULT hr = S_FALSE;

	SVMemoryPoolMap::iterator iter = m_mapPools.find( SVString(strPoolName) );
	if ( iter != m_mapPools.end() )// make sure it exists
	{
		TRACE(_T("SVMemoryManager::ReservePoolMemory(%s, %08X, %d)\n"), strPoolName, owner, lSizeInBytes);
		hr = iter->second.ReservePoolMemory( owner, lSizeInBytes );
	}

	return hr;
}

template <typename OWNERTYPE>
inline HRESULT SVMemoryManager<OWNERTYPE>::ReleasePoolMemory( LPCTSTR strPoolName, OWNERTYPE owner )
{
	HRESULT hr = S_FALSE;

	SVMemoryPoolMap::iterator iter = m_mapPools.find( SVString(strPoolName) );
	if ( iter != m_mapPools.end() )// make sure it exists
	{
		TRACE(_T("SVMemoryManager::ReleasePoolMemory(%s, %08X)\n"), strPoolName, owner);
		hr = iter->second.ReleasePoolMemory( owner );
	}

	return hr;
}

template <typename OWNERTYPE>
inline HRESULT SVMemoryManager<OWNERTYPE>::ReleasePoolMemory( LPCTSTR strPoolName, OWNERTYPE owner, long lSizeInBytes )
{
	HRESULT hr = S_FALSE;

	SVMemoryPoolMap::iterator iter = m_mapPools.find( SVString(strPoolName) );
	if ( iter != m_mapPools.end() )// make sure it exists
	{
		TRACE(_T("SVMemoryManager::ReleasePoolMemory(%s, %08X, %d)\n"), strPoolName, owner, lSizeInBytes);
		hr = iter->second.ReleasePoolMemory( owner, lSizeInBytes );
	}

	return hr;
}

template <typename OWNERTYPE>
bool SVMemoryManager<OWNERTYPE>::CanReservePoolMemory( LPCTSTR strPoolName, __int64 lReserveSizeInBytes )
{
	__int64 lFreeBytes = FreeBytes( strPoolName );
	return lFreeBytes - lReserveSizeInBytes > 0;
}


template <typename OWNERTYPE>
inline __int64 SVMemoryManager<OWNERTYPE>::FreeBytes( LPCTSTR strPoolName )
{
	__int64 lSize = 0;

	SVMemoryPoolMap::iterator iter = m_mapPools.find( SVString(strPoolName) );
	if ( iter != m_mapPools.end() )// make sure it exists
	{
		lSize = iter->second.FreeBytes( );
		TRACE(_T("SVMemoryManager::FreeBytes(%s, %d)\n"), strPoolName, lSize);
	}

	return lSize;
}

template <typename OWNERTYPE>
inline __int64 SVMemoryManager<OWNERTYPE>::SizeOfPoolBytes( LPCTSTR strPoolName )
{
	__int64 lSize = 0;

	SVMemoryPoolMap::iterator iter = m_mapPools.find( SVString(strPoolName) );
	if ( iter != m_mapPools.end() )// make sure it exists
	{
		lSize = iter->second.SizeOfPoolBytes();
	}

	return lSize;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMemoryManager.inl_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:36:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   18 Feb 2011 09:58:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   06 Oct 2005 07:45:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added DEBUG tracing
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Aug 2005 14:07:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented CanReservePoolMemory
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Jun 2005 10:11:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   filled out the stubs to actually implement the memory management
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   20 May 2005 12:07:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
