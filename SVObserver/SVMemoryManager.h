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

#ifndef SVMEMORYMANAGER_H
#define SVMEMORYMANAGER_H

#include <string>
#include <map>
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


#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVMemoryManager.h_v  $
 * 
 *    Rev 1.1   23 Jul 2014 11:36:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  916
 * SCR Title:  Fix issue with available memory calculation with Archive Tool (SV0-350)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed ReserverPoolMemory to add the memory allocation even if it goes over.  the error will still be out of memory
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:36:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Sep 2011 16:09:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated object to use the new resource functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   14 Jul 2011 08:16:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  729
 * SCR Title:  Adjust SVObserver to fix perfomance problems due to logging
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to remove logging in places that cause performance issues and added a registry controlled variable to handle the go offline data manager dumps to files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   18 Feb 2011 09:58:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   16 Dec 2009 12:00:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   06 Oct 2005 07:45:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added DEBUG tracing
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Sep 2005 09:09:14   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added method ReservedBytes
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Aug 2005 14:07:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented CanReservePoolMemory
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jun 2005 14:23:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to use SVSingleLock instead of CSingleLock
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
