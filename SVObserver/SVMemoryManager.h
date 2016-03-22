//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMemoryManager
//* .File Name       : $Workfile:   SVMemoryManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   23 Jul 2014 11:36:32  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <map>
#include "SVOLibrary/SVContainableCriticalSection.h"
#include "SVOMFCLibrary/SVTemplate.h"	// for singleton
#include "SVMessage/SVMessage.h"
#include "SVUtilityLibrary/SVString.h"

// manages multiple virtual pools of memory
// no memory is actually allocated
// this just enables the app to put a cap on
// the amount of memory that function X / tool X uses
template <typename OWNERTYPE>
class SVMemoryManager
{
public:
	HRESULT CreatePool( LPCTSTR strPoolName, __int64 lPoolSizeKBytes );
	HRESULT ReservePoolMemory( LPCTSTR strPoolName, OWNERTYPE owner, __int64 lSizeInBytes );
	bool    CanReservePoolMemory( LPCTSTR strPoolName, __int64 lSizeInBytes );
	HRESULT ReleasePoolMemory( LPCTSTR strPoolName, OWNERTYPE owner );
	HRESULT ReleasePoolMemory( LPCTSTR strPoolName, OWNERTYPE owner, long lSizeInBytes );
	__int64 FreeBytes( LPCTSTR strPoolName );
	__int64 SizeOfPoolBytes( LPCTSTR strPoolName );
	__int64 ReservedBytes( LPCTSTR strPoolName ){ return SizeOfPoolBytes(strPoolName) - FreeBytes(strPoolName);}

private:

	// currently SVMemoryPool is thread-safe dealing with multiple owners,
	// but an individual owner is not thread-safe.
	// this may be changed if needed.
	// just make the iter->second.lSize operations use the InterlockedExchange APIs.
	class SVMemoryPool // manages a single pool of memory
	{
	public:
		SVMemoryPool() : m_lPoolSize(0), m_lUsed(0) {}

		//////////////////////////////////////////////////////////////
		// unless these functions are defined in the class definition,
		// MSVC6 produces linker errors 
		//////////////////////////////////////////////////////////////

		HRESULT Create( __int64 lPoolSizeKBytes )
		{
			m_lPoolSize = lPoolSizeKBytes * 1024;
			return S_OK;
		}
		
		HRESULT ReservePoolMemory( OWNERTYPE owner, __int64 lSizeInBytes )
		{
			HRESULT hr = SVMSG_SVO_32_ARCHIVE_TOOL_OUT_OF_MEMORY;

			if ( m_lUsed + lSizeInBytes < m_lPoolSize )
			{
				hr = S_OK;
			}
			
			//Add the amount of Memory even if you are over the limit.  The Archive Tool Dialog will handle negative memory properly
			SVMemoryPoolEntryMap::iterator iter = m_mapEntries.find(owner);
			if ( iter == m_mapEntries.end() )
			{
				SVSingleLock lock( m_critsec );
				iter = m_mapEntries.insert( SVMemoryPoolEntryPair(owner, SVMemoryPoolEntry()) ).first;
			}
#ifndef _WIN64
			::InterlockedExchangeAdd( const_cast <LPLONG> (&m_lUsed), static_cast<long>(lSizeInBytes) );
#else
			__int64 value = ::InterlockedExchangeAdd64( const_cast <PLONGLONG> (&m_lUsed), lSizeInBytes );
#endif
			iter->second.lSize += lSizeInBytes;

			return hr;
		}

		HRESULT ReleasePoolMemory( OWNERTYPE owner )
		{
			HRESULT hr = S_FALSE;
			
			//ASSERT( m_mapEntries.find(owner) != m_mapEntries.end() );

			SVMemoryPoolEntryMap::iterator iter = m_mapEntries.find(owner);
			if ( iter != m_mapEntries.end() )
			{
#ifndef _WIN64
			::InterlockedExchangeAdd(  const_cast <LPLONG> (&m_lUsed), static_cast<long>(-iter->second.lSize) ); 
#else
			__int64 value = ::InterlockedExchangeAdd64( const_cast <PLONGLONG> (&m_lUsed), -iter->second.lSize );
#endif
				TRACE(_T("SVMemoryPool::ReleasePoolMemory %08X - %d\n"), owner, iter->second.lSize);
				SVSingleLock lock( m_critsec );
				m_mapEntries.erase( m_mapEntries.find(owner) );
				hr = S_OK;
			}
			else
			{
				TRACE(_T("SVMemoryPool::ReleasePoolMemory %08X - NOT FOUND\n"), owner);
			}
			return hr;
		}

		HRESULT ReleasePoolMemory( OWNERTYPE owner, long lSizeInBytes )
		{
			HRESULT hr = S_FALSE;
			
			SVMemoryPoolEntryMap::iterator iter = m_mapEntries.find(owner);
			if ( iter != m_mapEntries.end() )
			{
				if( lSizeInBytes >= iter->second.lSize )
				{
					TRACE(_T("SVMemoryPool::ReleasePoolMemory %08X - %d >= %d\n"), owner, lSizeInBytes, iter->second.lSize);
				}

#ifndef _WIN64
			::InterlockedExchangeAdd(  const_cast <LPLONG> (&m_lUsed), -lSizeInBytes);
#else
			__int64 value = ::InterlockedExchangeAdd64( const_cast <PLONGLONG> (&m_lUsed), -lSizeInBytes );
#endif


				iter->second.lSize -= lSizeInBytes;
				TRACE(_T("SVMemoryPool::ReleasePoolMemory %08X - %d, remaining = %d\n"), owner, lSizeInBytes, iter->second.lSize);
				if ( iter->second.lSize <= 0 )	// check less than for safety
				{
					SVSingleLock lock( m_critsec );
					m_mapEntries.erase( iter );
					hr = S_OK;
				}
			}
			else
			{
				TRACE(_T("SVMemoryPool::ReleasePoolMemory %08X - NOT FOUND\n"), owner);
			}
			return hr;
		}

		__int64 FreeBytes( )
		{
			return m_lPoolSize - m_lUsed;
		}

		__int64 SizeOfPoolBytes()
		{
			return m_lPoolSize;
		}

	private:
		// manages memory in a pool for one owner
		struct SVMemoryPoolEntry
		{
			//OWNERTYPE owner;
			__int64   lSize;
			SVMemoryPoolEntry() : lSize(0) {}
		};// end class SVMemoryPoolEntry
		typedef std::pair <OWNERTYPE, SVMemoryPoolEntry> SVMemoryPoolEntryPair;
		typedef std::map <OWNERTYPE, SVMemoryPoolEntry> SVMemoryPoolEntryMap;

		SVMemoryPoolEntryMap m_mapEntries;
#ifndef _WIN64
		volatile long m_lPoolSize;
		volatile long m_lUsed;
#else
		volatile __int64 m_lPoolSize;
		volatile __int64 m_lUsed;
#endif 
		SVContainableCriticalSection m_critsec;
		//CCriticalSection m_critsec;

		// disable copying this object ( because of the CCriticalSection )
		//SVMemoryPool( const SVMemoryPool& );                // disable copy constructor
		//SVMemoryPool& operator = ( const SVMemoryPool& );   // disable operator =

		public:	// we are using SVContainableCriticalSection to allow copying
		SVMemoryPool( const SVMemoryPool& rhs ) : m_critsec( rhs.m_critsec ), m_mapEntries( rhs.m_mapEntries )
		{
			m_lPoolSize = rhs.m_lPoolSize;
			m_lUsed = rhs.m_lUsed;
		}

		SVMemoryPool& operator = ( const SVMemoryPool& rhs )
		{
			if ( this != &rhs )
			{
				m_critsec = rhs.m_critsec;
				m_mapEntries = rhs.m_mapEntries;
				m_lPoolSize = rhs.m_lPoolSize;
				m_lUsed = rhs.m_lUsed;
			}
		}

	};// end class SVMemoryPool

	typedef std::map <SVString, SVMemoryPool> SVMemoryPoolMap;
	SVMemoryPoolMap m_mapPools;

};// end class SVMemoryManager

#include "SVMemoryManager.inl"

typedef TBasicSingletonHolder < SVMemoryManager<void*> > SVMemoryManagerSingleton;

inline SVMemoryManager<void*>& TheSVMemoryManager() {return SVMemoryManagerSingleton::Instance();}

