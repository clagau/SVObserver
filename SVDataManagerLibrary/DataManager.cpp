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
#include "SVStatusLibrary\MessageManager.h"
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
	
	if( S_OK == l_Status )
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
		
		if( S_OK == l_Status )
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
		
		if( S_OK == l_Status )
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
	pNewManagedIndexArray = nullptr;
	*alpIndexArrayHandle = -1;
	
	while (1)
	{
		if (alIndexArrayDepth > MAXIMUM_DATAMANAGER_DEPTH)
		{
			lErr = SVDM_1510MAXIMUMDEPTH_ERROR;
			break;
		}
		
		pNewManagedIndexArray = new SVManagedIndexArray;
		
		if (nullptr == pNewManagedIndexArray)
		{
			lErr = SVDM_1509NOALLOCATE_FATAL;
			break;
		}
		
		pNewManagedIndexArray->SetName( SvUl_SF::createSVString(aIndexArrayName) );
		
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
		pNewManagedIndexArray = nullptr;
		
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

	SVManagedIndex*       l_pAvailableIndex = nullptr;
	SVManagedIndex*       l_pNextAvailableIndex = nullptr;


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
				if( S_OK == lErr )
				{
					lErr = SVDM_1502NO_INDEXESAVAILABLE_ERROR;
				}

				SVString l_Message = pManagedIndexArray->GetReferenceCounts();

				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVDM_1502NO_INDEXESAVAILABLE_ERROR, l_Message.c_str(), SvStl::SourceFileParams(StdMessageParams) );

				break;
			}

			do
			{
				lErr = pManagedIndexArray->GetAt( lAvailableBufferIndex, l_pAvailableIndex );

				if( S_OK != lErr || nullptr == l_pAvailableIndex )
				{
					if( S_OK == lErr )
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

	if( S_OK == l_Status )
	{
		l_Status = ValidateLockType( aLockType );
	}
		
	if( S_OK == l_Status )
	{
		SVManagedIndexArray* pManagedIndexArray = svmManagedIndexArrayList[ alIndexArrayHandle ];
		
		l_Status = pManagedIndexArray->AddReference( alBufferIndex, alTransactionId, aLockType );
	}
   
	return l_Status;
}

/*
This method locks a buffer index based on the index value and the index array handle.
*/
HRESULT SVDataManager::LockBufferIndexNoLock (long alIndexArrayHandle,
											long alBufferIndex,
											long alTransactionId,
											SVDataManagerLockTypeEnum aLockType)
{
	HRESULT l_Status = S_OK;

	l_Status = ValidateIndexArrayHandle( alIndexArrayHandle );

	if( S_OK == l_Status )
	{
		l_Status = ValidateLockType( aLockType );
	}
		
	if( S_OK == l_Status )
	{
		SVManagedIndexArray* pManagedIndexArray = svmManagedIndexArrayList[ alIndexArrayHandle ];
		
		l_Status = pManagedIndexArray->AddReferenceNoLock( alBufferIndex, alTransactionId, aLockType );
	}
   
	return l_Status;
}

/*
This method releases a buffer index based on the index value and the index array handle.
*/
HRESULT SVDataManager::ReleaseBufferIndex (long alIndexArrayHandle,
                                           long alBufferIndex,
										   long alTransactionId,
                                           SVDataManagerLockTypeEnum aLockType)
{
	HRESULT l_Status = S_OK;

	l_Status = ValidateIndexArrayHandle( alIndexArrayHandle );

	if( S_OK == l_Status )
	{
		l_Status = ValidateLockType( aLockType );
	}

	if( S_OK == l_Status )
	{
		SVManagedIndexArray* pManagedIndexArray = svmManagedIndexArrayList[ alIndexArrayHandle ];

		l_Status = pManagedIndexArray->ReleaseReference( alBufferIndex, alTransactionId, aLockType );
	}

	return l_Status;
}

/*
This method releases a buffer index based on the index value and the index array handle.
*/
HRESULT SVDataManager::ReleaseBufferIndexNoLock (long alIndexArrayHandle,
											long alBufferIndex,
											long alTransactionId,
											SVDataManagerLockTypeEnum aLockType)
{
	HRESULT l_Status = S_OK;

	l_Status = ValidateIndexArrayHandle( alIndexArrayHandle );

	if( S_OK == l_Status )
	{
		l_Status = ValidateLockType( aLockType );
	}

	if( S_OK == l_Status )
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
		pManagedIndexArray = nullptr;
		
		break;
	}
	
	Unlock ();
	
	return lErr;
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
	
		if ( nullptr == svmManagedIndexArrayList [alIndexArrayHandle] )
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
		
		if( S_OK != lErr )
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
	pManagedIndexArray = nullptr;

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
	pManagedIndexArray = nullptr;
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
		
		if( nullptr == pManagedIndexArray )
		{
			continue;
		}

		pManagedIndexArray->Dump( _T( "Data Manager" ) );
	}
}


long SVDataManager::GetIndexLockCountByType( long p_ArrayHandle, long p_Index, SVDataManagerLockTypeEnum p_LockType )
{
	long l_Count( 0 );
	
	if( S_OK == ValidateIndexArrayHandle( p_ArrayHandle ) )
	{
		if( S_OK == ValidateLockType( p_LockType ) )
		{
			SVManagedIndexArray* l_pManagedIndexArray = svmManagedIndexArrayList[ p_ArrayHandle ];
			
			if( nullptr != l_pManagedIndexArray && 0 <= p_Index && p_Index < static_cast< long >( l_pManagedIndexArray->GetSize() ) )
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
	
	if( S_OK == ValidateIndexArrayHandle( p_ArrayHandle ) )
	{
		if( S_OK == ValidateLockType( p_LockType ) )
		{
			SVManagedIndexArray* l_pManagedIndexArray = svmManagedIndexArrayList[ p_ArrayHandle ];
			
			if( nullptr != l_pManagedIndexArray )
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

