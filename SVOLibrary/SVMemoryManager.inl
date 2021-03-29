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

constexpr int UseLargerArchiveMemoryPool = 16000;
constexpr int AsyncDefault4GB = 50;
constexpr int AsyncDefault16GB = 200;
constexpr int GoOfflineDefault4GB = 300;
constexpr int GoOfflineDefault16GB = 2000;

template <typename OWNERTYPE>
inline void SVMemoryManager<OWNERTYPE>::InitializeMemoryManager(LPCTSTR strPoolName1, LPCTSTR strPoolName2, const SvLib::SVOINIClass& rSvimIni)
{
	//Get Amount of System Memory
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	DWORDLONG AmountOfRam = (statex.ullTotalPhys / 1024) / 1024;

	int iGoOfflineBufferSize = 0;
	int iAsyncBufferSize = 0;
	// allocate pools in the memory manager

	//Log amount of physical memory - may help in debugging issues in the future.
	SvDef::StringVector MessageList;
	MessageList.push_back(SvUl::Format(_T("%d"), AmountOfRam));
	SvStl::MessageManager Msg(SvStl::MsgType::Log);
	Msg.setMessage(SVMSG_SVO_54_EMPTY, SvStl::Tid_AmountOfSystemMemoryText, MessageList, SvStl::SourceFileParams(StdMessageParams), SvStl::Memory_Log_45001);

	//if amount of physical memory is around 16 GigE allocate the larger memory pools.
	if (AmountOfRam >= UseLargerArchiveMemoryPool)
	{
		iGoOfflineBufferSize = rSvimIni.GetValueInt(_T("Settings"), _T("ArchiveToolGoOfflineBufferSize"), GoOfflineDefault16GB);
		iAsyncBufferSize = rSvimIni.GetValueInt(_T("Settings"), _T("ArchiveToolAsyncBufferSize"), AsyncDefault16GB);
	}
	else
	{
		iGoOfflineBufferSize = rSvimIni.GetValueInt(_T("Settings"), _T("ArchiveToolGoOfflineBufferSize"), GoOfflineDefault4GB);
		iAsyncBufferSize = rSvimIni.GetValueInt(_T("Settings"), _T("ArchiveToolAsyncBufferSize"), AsyncDefault4GB);
	}
	CreatePool(strPoolName1, iGoOfflineBufferSize * 1024);
	CreatePool(strPoolName2, iAsyncBufferSize * 1024);
}

template <typename OWNERTYPE>
inline HRESULT SVMemoryManager<OWNERTYPE>::CreatePool( LPCTSTR strPoolName, __int64 lPoolSizeKBytes )
{
	HRESULT hr = S_FALSE;

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(strPoolName) );
	if ( iter == m_mapPools.end() )// ensure it doesn't already exist
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MEMORY)
		TRACE(_T("SVMemoryManager::CreatePool(%s, %d)\n"), strPoolName, lPoolSizeKBytes*1024);
#endif
		hr = m_mapPools[ std::string(strPoolName) ].Create( lPoolSizeKBytes );
	}
	return hr;
}

template <typename OWNERTYPE>
inline HRESULT SVMemoryManager<OWNERTYPE>::ReservePoolMemory( LPCTSTR strPoolName, OWNERTYPE owner, __int64 lSizeInBytes )
{
	HRESULT hr = S_FALSE;

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(strPoolName) );
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

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(strPoolName) );
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

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(strPoolName) );
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

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(strPoolName) );
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

	typename SVMemoryPoolMap::iterator iter = m_mapPools.find( std::string(strPoolName) );
	if ( iter != m_mapPools.end() )// make sure it exists
	{
		lSize = iter->second.SizeOfPoolBytes();
	}

	return lSize;
}

