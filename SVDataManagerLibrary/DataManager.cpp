// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDataManager
// * .File Name       : $Workfile:   DataManager.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.4  $
// * .Check In Date   : $Date:   23 May 2014 07:12:58  $
// ******************************************************************************

#include "stdafx.h"
#include "DataManager.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/SVException.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVDataManagerHandle.h"

/*
This constructor initializes all of the internal attributes.
*/
SVDataManager::SVDataManager ()
: m_IsLockCreated( false )
{
	::InitializeCriticalSection( &m_Lock );
	m_IsLockCreated = true;

	svmlFirstUnusedIndex = -1;
	svmlLastUnusedIndex = -1;
	svmManagedIndexArrayList.SetSize( 100 );
}

SVDataManager::~SVDataManager ()
{
	if( m_IsLockCreated )
	{
		::DeleteCriticalSection( &m_Lock );
	}
}
/*
This method is responsible for releasing all of the indexes associated with a particular index array handle.
*/
HRESULT SVDataManager::ReleaseAllIndexes(long alIndexArrayHandle)
{
	SVManagedIndexArray* pManagedIndexArray;
	long lErr;
	
	while( 1 )
	{
		//--- Validate attributes ---------------------------------------------------
		lErr = ValidateIndexArrayHandle (alIndexArrayHandle);
		//--- EventID 1500
		if (lErr & SVDM_ERROR_CONDITION)
		{
			break;
		}
		
		pManagedIndexArray = svmManagedIndexArrayList[ alIndexArrayHandle ];

		lErr = pManagedIndexArray->ReleaseAllIndexes();
		
		break;
	}// end while
	
	return lErr;
}// end ReleaseAllIndexes

HRESULT SVDataManager::CreateManagedIndexArray( SVSmartIndexArrayHandlePtr& p_pArrayHandle, BSTR p_Name, long p_Depth )
{
	HRESULT l_Status = S_OK;
	
	long l_Handle = -1;
	
	l_Status = CreateManagedIndexArray( p_Name, p_Depth, &( l_Handle ) );
	
	if( l_Status == S_OK )
	{
		p_pArrayHandle = new SVDataManagerIndexArrayHandle( *this, l_Handle );
	}
	else
	{
		p_pArrayHandle.clear();
	}
	
	return l_Status;
}

HRESULT SVDataManager::GetNextAvailableBufferIndex( const SVSmartIndexArrayHandlePtr& p_pArrayHandle, 
																									 SVDataManagerLockTypeEnum aLockType, 
																									 SVDataManagerHandle& p_rHandle )
{
	HRESULT l_Status = S_OK;
	
	p_rHandle.clear();
	
	if( ! p_pArrayHandle.empty() )
	{
		long l_Index = -1;
		long l_TransactionId = -1;
		
		l_Status = GetNextAvailableBufferIndex( p_pArrayHandle->m_ArrayHandle, aLockType, true, &l_Index, &l_TransactionId );
		
		if( l_Status == S_OK )
		{
			p_rHandle.m_pArrayHandle = p_pArrayHandle;
			p_rHandle.m_LockType = aLockType;
			p_rHandle.m_Index = l_Index;
			p_rHandle.m_TransactionId = l_TransactionId;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}
	
	return l_Status;
}

HRESULT SVDataManager::GetNextAvailableBufferIndexNoWait( const SVSmartIndexArrayHandlePtr& p_pArrayHandle, 
																												 SVDataManagerLockTypeEnum aLockType, 
																												 SVDataManagerHandle& p_rHandle )
{
	HRESULT l_Status = S_OK;
	
	p_rHandle.clear();
	
	if( ! p_pArrayHandle.empty() )
	{
		long l_Index = -1;
		long l_TransactionId = -1;
		
		l_Status = GetNextAvailableBufferIndex( p_pArrayHandle->m_ArrayHandle, aLockType, false, &l_Index, &l_TransactionId );
		
		if( l_Status == S_OK )
		{
			p_rHandle.m_pArrayHandle = p_pArrayHandle;
			p_rHandle.m_LockType = aLockType;
			p_rHandle.m_Index = l_Index;
			p_rHandle.m_TransactionId = l_TransactionId;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}
	
	return l_Status;
}

/*
This method creates managed index array elements.
*/
HRESULT SVDataManager::CreateManagedIndexArray (BSTR  aIndexArrayName,
                                                long  alIndexArrayDepth,
                                                long* alpIndexArrayHandle)
{
	long                 lErr;
	
	SVManagedIndexArray*  pNewManagedIndexArray;
	
	lErr = 0;
	pNewManagedIndexArray = NULL;
	*alpIndexArrayHandle = -1;
	
	while (1)
	{
		if (alIndexArrayDepth > MAXIMUM_DATAMANAGER_DEPTH)
		{
			lErr = SVDM_1510MAXIMUMDEPTH_ERROR;
			break;
		}
		
		pNewManagedIndexArray = new SVManagedIndexArray;
		
		if (pNewManagedIndexArray == NULL)
		{
			lErr = SVDM_1509NOALLOCATE_FATAL;
			break;
		}
		
		pNewManagedIndexArray->SetName( aIndexArrayName );
		
		lErr = pNewManagedIndexArray->SetSize( alIndexArrayDepth );
		
		//--- When making changes to SVDataManager elements it is necessary to lock
		//--- the mutex.
		lErr = Lock( 1000 );
		
		if (lErr == 0)
		{
			//------ If the mutex can not lock in 1 second, then we consider that a 
			//------ problem.
			lErr = SVDM_1512COULDNOTLOCKDATAMANAGER_FATAL;
			break;
		}
		else
			lErr = 0;
		
		if (svmlFirstUnusedIndex == -1)
		{
			//------ There are no unused indexes. ---------------------------------------
			*alpIndexArrayHandle = static_cast< long >( 
				svmManagedIndexArrayList.Add( pNewManagedIndexArray ) );
			
		}
		else
		{
			long  temp;
			
			temp = (long) svmManagedIndexArrayList [svmlFirstUnusedIndex];
			
			svmManagedIndexArrayList [svmlFirstUnusedIndex] = 
				pNewManagedIndexArray;
			
			*alpIndexArrayHandle = svmlFirstUnusedIndex;
			svmlFirstUnusedIndex = temp;
			
			//------ If svmlFirstUnusedIndex equals -1, then it means we have just 
			//------ emptied the list. The svmlLastUnusedIndex value must be reset. -----
			if (svmlFirstUnusedIndex == -1)
			{
				svmlLastUnusedIndex = -1;
			}
		}
		
		Unlock ();
		//--- We are finished making changes to SVDataManager elements.  The mutex 
		//--- may now be unlocked.
		
		break;
	}
	
	if (lErr & SVDM_ERROR_CONDITION)
	{
		//--- No errors occur after the lock (accept for the lock) ------------------
		delete pNewManagedIndexArray;
		pNewManagedIndexArray = NULL;
		
		Unlock (); // Just in case someone puts in an error
		//  break after locking and prior to 
		//  unlocking.
	}
	
	return lErr;
}

/*
This method gets the next available buffer index based on the array index handle.
*/
HRESULT SVDataManager::GetNextAvailableBufferIndex( long alIndexArrayHandle,
												   SVDataManagerLockTypeEnum aLockType,
												   bool p_WaitForLock,
												   long* aplAvailableBufferIndex,
												   long* alTransactionId)
{

	long                  lErr;
	long                  lAvailableBufferIndex = -1;
	long					 lTransactionId = 0;

	SVManagedIndex*       l_pAvailableIndex = NULL;
	SVManagedIndex*       l_pNextAvailableIndex = NULL;


	SVManagedIndexArray*  pManagedIndexArray;

	lErr = 0;

	while (1)
	{

		//--- Validate attributes ---------------------------------------------------
		lErr = ValidateIndexArrayHandle (alIndexArrayHandle);
		//--- EventID 1500
		if (lErr & SVDM_ERROR_CONDITION)
		{
			break;
		}

		lErr = ValidateLockType (aLockType);
		//--- EventID 1501
		if (lErr & SVDM_ERROR_CONDITION)
		{
			break;
		}

		//--- End of Validate attributes --------------------------------------------

		pManagedIndexArray = svmManagedIndexArrayList [alIndexArrayHandle];

		do
		{
			lErr = pManagedIndexArray->GetNextIndex( lAvailableBufferIndex, lTransactionId, p_WaitForLock );
			if (lAvailableBufferIndex == -1)
			{
				if( lErr == S_OK )
				{
					lErr = SVDM_1502NO_INDEXESAVAILABLE_ERROR;
				}

				SVException l_Log;
				SVString l_Message = pManagedIndexArray->GetReferenceCounts();

				SETEXCEPTION1( l_Log, SVDM_1502NO_INDEXESAVAILABLE_ERROR, l_Message.ToString() );
				l_Log.LogException( l_Message.ToString() );

				break;
			}

			do
			{
				lErr = pManagedIndexArray->GetAt( lAvailableBufferIndex, l_pAvailableIndex );

				if( lErr != S_OK || l_pAvailableIndex == NULL )
				{
					if( lErr == S_OK )
					{
						lErr = SVDM_1513COULDNOTLOCKMANAGEDARRAY_FATAL;
					}

					break;
				}

				l_pAvailableIndex->svmlTransactionId = lTransactionId;

				lErr = LockBufferIndex (alIndexArrayHandle,
					lAvailableBufferIndex,
					lTransactionId,
					aLockType);

				if (lErr & SVDM_ERROR_CONDITION)
					break;

				break;
			} 
			while( 0 );

			if (lErr & SVDM_ERROR_CONDITION)
			{
				if (lAvailableBufferIndex != -1)
				{
					pManagedIndexArray->ClearIndex( lAvailableBufferIndex, lTransactionId );
				}

				break;
			}
		} while ( 0 );

		break;
	}

	if (lErr & SVDM_ERROR_CONDITION)
	{

	}
	else
	{
		*aplAvailableBufferIndex = lAvailableBufferIndex;
		*alTransactionId = lTransactionId;
	}

	return lErr;
}

/*
This method locks a buffer index based on the index value and the index array handle.
*/
HRESULT SVDataManager::LockBufferIndex (long    alIndexArrayHandle,
                                        long    alBufferIndex,
																				long	alTransactionId,
                                        SVDataManagerLockTypeEnum aLockType)
{
	HRESULT l_Status = S_OK;

	l_Status = ValidateIndexArrayHandle( alIndexArrayHandle );

	if( l_Status == S_OK )
	{
		l_Status = ValidateLockType( aLockType );
	}
		
	if( l_Status == S_OK )
	{
		SVManagedIndexArray* pManagedIndexArray = svmManagedIndexArrayList[ alIndexArrayHandle ];
		
		l_Status = pManagedIndexArray->AddReference( alBufferIndex, alTransactionId, aLockType );
	}
   
	return l_Status;
}

/*
This method locks a buffer index based on the index value and the index array handle.
*/
HRESULT SVDataManager::LockBufferIndexNoLock (long    alIndexArrayHandle,
                                        long    alBufferIndex,
																				long	alTransactionId,
                                        SVDataManagerLockTypeEnum aLockType)
{
	HRESULT l_Status = S_OK;

	l_Status = ValidateIndexArrayHandle( alIndexArrayHandle );

	if( l_Status == S_OK )
	{
		l_Status = ValidateLockType( aLockType );
	}
		
	if( l_Status == S_OK )
	{
		SVManagedIndexArray* pManagedIndexArray = svmManagedIndexArrayList[ alIndexArrayHandle ];
		
		l_Status = pManagedIndexArray->AddReferenceNoLock( alBufferIndex, alTransactionId, aLockType );
	}
   
	return l_Status;
}

/*
This method releases a buffer index based on the index value and the index array handle.
*/
HRESULT SVDataManager::ReleaseBufferIndex (long    alIndexArrayHandle,
                                           long    alBufferIndex,
																					 long	   alTransactionId,
                                           SVDataManagerLockTypeEnum aLockType)
{
	HRESULT l_Status = S_OK;

	l_Status = ValidateIndexArrayHandle( alIndexArrayHandle );

	if( l_Status == S_OK )
	{
		l_Status = ValidateLockType( aLockType );
	}

	if( l_Status == S_OK )
	{
		SVManagedIndexArray* pManagedIndexArray = svmManagedIndexArrayList[ alIndexArrayHandle ];

		l_Status = pManagedIndexArray->ReleaseReference( alBufferIndex, alTransactionId, aLockType );
	}

	return l_Status;
}

/*
This method releases a buffer index based on the index value and the index array handle.
*/
HRESULT SVDataManager::ReleaseBufferIndexNoLock (long    alIndexArrayHandle,
                                           long    alBufferIndex,
																					 long	   alTransactionId,
                                           SVDataManagerLockTypeEnum aLockType)
{
	HRESULT l_Status = S_OK;

	l_Status = ValidateIndexArrayHandle( alIndexArrayHandle );

	if( l_Status == S_OK )
	{
		l_Status = ValidateLockType( aLockType );
	}

	if( l_Status == S_OK )
	{
		SVManagedIndexArray* pManagedIndexArray = svmManagedIndexArrayList[ alIndexArrayHandle ];

		l_Status = pManagedIndexArray->ReleaseReferenceNoLock( alBufferIndex, alTransactionId, aLockType );
	}

	return l_Status;
}


HRESULT SVDataManager::DestroyIndexArray (long    alIndexArrayHandle)
{
	SVManagedIndexArray*    pManagedIndexArray;
	
	long  lErr;
	
	lErr = 0;
	
	while (1)
	{
		//--- Validate attributes ---------------------------------------------------
		lErr = ValidateIndexArrayHandle (alIndexArrayHandle);
		//--- EventID 1500
		if (lErr & SVDM_ERROR_CONDITION)
		{
			break;
		}
		//--- End of Validate attributes --------------------------------------------
		
		pManagedIndexArray = svmManagedIndexArrayList [alIndexArrayHandle];
		
		if( pManagedIndexArray->GetAvailableIndexCount() != pManagedIndexArray->GetSize() )
		{
			//------ If the managed array is still being used (locks are still present). 
			lErr = SVDM_1508INDEXESNOTFREED_FATAL;
			break;
		}
		
		lErr = Lock( 1000 );
		
		if (lErr == 0)
		{
			//------ If the mutex can not lock in 1 second, then we consider that a 
			//------ problem.
			lErr = SVDM_1518COULDNOTLOCKDATAMANAGER_FATAL;
			break;
		}
		else
		{
			lErr = 0;
		}
		
		if (svmlLastUnusedIndex == -1)
		{
			//------ There are no previous unused indexes. ------------------------------
			svmlFirstUnusedIndex = alIndexArrayHandle;
		}
		else
		{
			//------ There are other unused indexes. 
			//------ Please newly destroyed index at the end of the unused list. --------
			svmManagedIndexArrayList [svmlLastUnusedIndex] = 
				(SVManagedIndexArray*) alIndexArrayHandle;
		}
		
		svmManagedIndexArrayList [alIndexArrayHandle] = (SVManagedIndexArray*) -1;
		svmlLastUnusedIndex = alIndexArrayHandle;
		
		//--- The following delete, deletes the ManagedIndexArrayLock, so there is no
		//--- more need to free it.
		delete pManagedIndexArray;
		pManagedIndexArray = NULL;
		
		break;
	}
	
	Unlock ();
	
	return lErr;
}

/*
This method handles string copies as desired by this class.
*/
HRESULT SVDataManager::StringCopy (TCHAR*		   apczDestString,
																	 const TCHAR*	apczSourceString,
																	 long		   alNbrOfCharacters)
{
	_tcsncpy (apczDestString, apczSourceString, alNbrOfCharacters);
	
	apczDestString [alNbrOfCharacters - 1] = 0;
	
	return 0;
}

/*
This method will validate the Index Array Handle.
*/
HRESULT SVDataManager::ValidateIndexArrayHandle (long   alIndexArrayHandle) const
{
	long  lErr;
	long  lNbrOfElements;
	
	lErr = 0;
	
	while (1)
	{
		if (alIndexArrayHandle < 0)
		{
			lErr = SVDM_1500INVALIDINDEXARRAYHANDLE_ERROR;
			break;
		}
		
		lNbrOfElements = static_cast< long >( svmManagedIndexArrayList.GetSize() );
		
		if (alIndexArrayHandle >= lNbrOfElements)
		{
			lErr = SVDM_1500INVALIDINDEXARRAYHANDLE_ERROR;
			break;
		}
	
		if ( svmManagedIndexArrayList [alIndexArrayHandle] == nullptr )
		{
			//------ This is a bit of an assumtion. If the handle is actually within 
			//------ acceptable boundaries, but the contents of the contained pointer are
			//------ also within the array boundries, then this is assumed to be part of 
			//------ the previously released, un-used linked list.
			lErr = SVDM_1500INVALIDINDEXARRAYHANDLE_ERROR;
			break;
		}
		
		break;
	}
	
	return lErr;
}

/*
This method validate the lock type.
*/
HRESULT SVDataManager::ValidateLockType (long alLockType)
{
	long  lErr;
	
	lErr = 0;
	
	while (1)
	{
		if (alLockType < 0)
		{
			lErr = SVDM_1501INVALIDLOCKTYPE_ERROR;
			break;
		}
		
		if (alLockType > SV_OTHER)
		{
			lErr = SVDM_1501INVALIDLOCKTYPE_ERROR;
			break;
		}
		
		break;
	}
	
	return lErr;
}

/*
This method validate the Buffer Index based on a Index Array Handle.  alIndexArrayHandle must have already been validated separately.
*/
HRESULT SVDataManager::ValidateBufferIndex (long alIndexArrayHandle,
																						long alBufferIndex)
{
	HRESULT l_Status = S_OK;
	
	SVManagedIndexArray* pManagedIndexArray = svmManagedIndexArrayList[ alIndexArrayHandle ];
		
	l_Status = pManagedIndexArray->ValidateIndex( alBufferIndex );
	
	return l_Status;
}

HRESULT SVDataManager::ValidateBufferIndex (long alIndexArrayHandle, 
																						long alBufferIndex,
																						long alTransactionId )
{
	HRESULT l_Status = S_OK;
	
	SVManagedIndexArray* pManagedIndexArray = svmManagedIndexArrayList[ alIndexArrayHandle ];
		
	l_Status = pManagedIndexArray->ValidateIndex( alBufferIndex, alTransactionId );
	
	return l_Status;
}

HRESULT SVDataManager::ValidateBufferIndexNoLock (long alIndexArrayHandle, 
																						long alBufferIndex,
																						long alTransactionId )
{
	HRESULT l_Status = S_OK;
	
	SVManagedIndexArray* pManagedIndexArray = svmManagedIndexArrayList[ alIndexArrayHandle ];
		
	l_Status = pManagedIndexArray->ValidateIndexNoLock( alBufferIndex, alTransactionId );
	
	return l_Status;
}

HRESULT	SVDataManager::GetNbrOfAvailableIndexes (long alIndexArrayHandle, long* alpNbrOfAvailableIndexes)
{
	long  lErr;
	long  lCounter = 0;
	
	SVManagedIndexArray*    pManagedIndexArray;
	
	lErr = 0;
	*alpNbrOfAvailableIndexes = 0;
	
	while (1)
	{
		lErr = ValidateIndexArrayHandle( alIndexArrayHandle );
		
		if( lErr != S_OK )
		{
			break;
		}
		
		pManagedIndexArray = svmManagedIndexArrayList[ alIndexArrayHandle ];
		
		lCounter = static_cast< long >( pManagedIndexArray->GetAvailableIndexCount() );
		
		break;
	}
	
	*alpNbrOfAvailableIndexes = lCounter;
	
	return lErr;
}


// This function dumps all data associated with this handle to a file.
void SVDataManager::Dump (long    alIndexArrayHandle, LPCSTR p_szSource ) const
{
	
	
	
	SVManagedIndexArray*  pManagedIndexArray;
	
	long  lErr;
	
	lErr = 0;
	pManagedIndexArray = NULL;

	do
	{
		//--- Validate attributes ---------------------------------------------------
		lErr = ValidateIndexArrayHandle (alIndexArrayHandle);
		
		//--- EventID 1500
		if (lErr & SVDM_ERROR_CONDITION)
		{
			break;
		}
		
		pManagedIndexArray = svmManagedIndexArrayList [alIndexArrayHandle];

		pManagedIndexArray->Dump( p_szSource );
		
	}while( false );
}


// This function dumps all data associated with this handle to a file.
void SVDataManager::Dump_All ()
{
	
	
	SVManagedIndexArray*  pManagedIndexArray;
	
	long  lErr;
	
	lErr = 0;
	pManagedIndexArray = NULL;
	for( long alIndexArrayHandle = 0 ; alIndexArrayHandle < svmManagedIndexArrayList.GetSize() ; alIndexArrayHandle++ )
	{
		
		//--- Validate attributes ---------------------------------------------------
		lErr = ValidateIndexArrayHandle (alIndexArrayHandle);
		
		//--- EventID 1500
		if (lErr & SVDM_ERROR_CONDITION)
		{
			continue;
		}
		
		pManagedIndexArray = svmManagedIndexArrayList [alIndexArrayHandle];
		
		if( pManagedIndexArray == NULL )
		{
			continue;
		}

		pManagedIndexArray->Dump( _T( "Data Manager" ) );
	}
}


long SVDataManager::GetIndexLockCountByType( long p_ArrayHandle, long p_Index, SVDataManagerLockTypeEnum p_LockType )
{
	long l_Count( 0 );
	
	if( ValidateIndexArrayHandle( p_ArrayHandle ) == S_OK )
	{
		if( ValidateLockType( p_LockType ) == S_OK )
		{
			SVManagedIndexArray* l_pManagedIndexArray = svmManagedIndexArrayList[ p_ArrayHandle ];
			
			if( l_pManagedIndexArray != NULL && 0 <= p_Index && p_Index < static_cast< long >( l_pManagedIndexArray->GetSize() ) )
			{
				l_Count = l_pManagedIndexArray->GetReferenceCount( p_Index, p_LockType );
			}
		}
	}
	
	return l_Count;
}

long SVDataManager::GetLockCountByType( long p_ArrayHandle, SVDataManagerLockTypeEnum p_LockType )
{
	long l_Count( 0 );
	
	if( ValidateIndexArrayHandle( p_ArrayHandle ) == S_OK )
	{
		if( ValidateLockType( p_LockType ) == S_OK )
		{
			SVManagedIndexArray* l_pManagedIndexArray = svmManagedIndexArrayList[ p_ArrayHandle ];
			
			if( l_pManagedIndexArray != NULL )
			{
				for( size_t i = 0; i < l_pManagedIndexArray->GetSize(); ++i )
				{
					l_Count += l_pManagedIndexArray->GetReferenceCount( i, p_LockType );
				}
			}
		}
	}
	
	return l_Count;
}

BOOL SVDataManager::Lock( unsigned long p_Timeout ) const
{
	BOOL l_Status = m_IsLockCreated;
	
	if( l_Status )
	{
		if( p_Timeout == INFINITE )
		{
			::EnterCriticalSection( &m_Lock );
		}
		else
		{
			SVClock::SVTimeStamp l_CurrentTime = 0.0;
			SVClock::SVTimeStamp l_EndTime = 0.0;
			
			BOOL l_LockAcquired = false;
			
			l_CurrentTime = SVClock::GetTimeStamp();
			
			l_EndTime = l_CurrentTime + SVClock::ConvertFrom( SVClock::Milliseconds, p_Timeout );
			
			while( ! l_LockAcquired && l_CurrentTime <= l_EndTime )
			{
				l_LockAcquired = ::TryEnterCriticalSection( &m_Lock );
				
				if( ! l_LockAcquired && l_CurrentTime < l_EndTime)
				{
					::Sleep( 1 );
					l_CurrentTime = SVClock::GetTimeStamp();
				}
			}
			
			l_Status = l_LockAcquired;
		}
	}
	else
	{
		l_Status = false;
	}
	
	return l_Status;
}

BOOL SVDataManager::Unlock() const
{
	BOOL l_Status = m_IsLockCreated;
	
	if( l_Status )
	{
		::LeaveCriticalSection( &m_Lock );
	}
	
	return l_Status;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVDataManagerLibrary\DataManager.cpp_v  $
 * 
 *    Rev 1.4   23 May 2014 07:12:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified ValidateIndexArrayHandle array check changed old code that did not work with the current vector container.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Sep 2013 14:50:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 May 2013 10:55:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 May 2013 13:00:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use new SVClock conversions functions, which will clarify timing tolerance checks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 19:59:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 012.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Apr 2013 12:27:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 07:53:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Sep 2011 14:10:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Data Manager functionality to remove locking issues and resource issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   14 Jul 2011 08:01:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  729
 * SCR Title:  Adjust SVObserver to fix perfomance problems due to logging
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to remove logging and add a method to get lock count by type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Jul 2011 14:35:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  724
 * SCR Title:  Add Resource Logging to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to create a method to get the number of availiable indexes and to manage logging when resources are consumed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 May 2010 13:16:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include a no lock path when locking is not required due to owner lock.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Feb 2010 09:07:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Sep 2009 16:47:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved code from SVLibrary to SVDataManagerLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   13 Jul 2009 12:34:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the methodology for managing the indexes using the data manager.  The new methodology uses the smart pointer to externally reference count the index usage and release the array handle when all indexes are released.  Atransaction id was added to prevent unintended index reuse.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   27 Jan 2009 14:53:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  647
 * SCR Title:  Add Information to Event Log
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified error testing to use a binary and instead of a logical and.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   01 Nov 2005 14:36:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SV_DISPLAY and SV_LAST_INSPECTED
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   01 Nov 2005 14:36:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SV_DISPLAY and SV_LAST_INSPECTED
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   16 Feb 2005 12:37:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added DataManger member
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   27 Feb 2003 17:11:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  325
 * SCR Title:  Error retrieving last element out of a Data Manager list
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Updated LockBufferIndex and GetNextAvailableBufferIndex to fix a mutex locking problem and fix a the last availiable index pointer problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   10 Jan 2003 13:10:38   cschmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Code was moved  to SVLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Dec 2002 17:03:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  304
 * SCR Title:  Fix crash after deleting a PPQ and its attached inspection(s)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added DumpAllLocks method to SVDataManager class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
