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
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		TRACE(_T("SVMemoryManager::CreatePool(%s, %d)\n"), strPoolName, lPoolSizeKBytes*1024);
#endif
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
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		TRACE(_T("SVMemoryManager::ReservePoolMemory(%s, %08X, %d)\n"), strPoolName, owner, lSizeInBytes);
#endif
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
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		TRACE(_T("SVMemoryManager::ReleasePoolMemory(%s, %08X)\n"), strPoolName, owner);
#endif
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
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		TRACE(_T("SVMemoryManager::ReleasePoolMemory(%s, %08X, %d)\n"), strPoolName, owner, lSizeInBytes);
#endif
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
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		TRACE(_T("SVMemoryManager::FreeBytes(%s, %d)\n"), strPoolName, lSize);
#endif
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

