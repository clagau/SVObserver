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
, m_FirstUnusedIndex( -1 )
, m_LastUnusedIndex( -1 )
{
	::InitializeCriticalSection( &m_Lock );
	m_IsLockCreated = true;

	m_ManagedIndexArrayVector.resize( 100 );
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
HRESULT SVDataManager::ReleaseAllIndexes(LONGLONG IndexArrayHandle)
{
	HRESULT Result( S_OK );
	SVManagedIndexArray* pManagedIndexArray;
	
	//--- Validate attributes ---------------------------------------------------
	Result = ValidateIndexArrayHandle ( IndexArrayHandle);
	if( S_OK == Result )
	{
		pManagedIndexArray = m_ManagedIndexArrayVector[IndexArrayHandle];
		if( nullptr != pManagedIndexArray )
		{
			Result = pManagedIndexArray->ReleaseAllIndexes();
		}
		else
		{
			Result = E_POINTER;
		}
	}
		
	return Result;
}// end ReleaseAllIndexes

HRESULT SVDataManager::CreateManagedIndexArray( SVSmartIndexArrayHandlePtr& rArrayHandle, BSTR Name, long Depth )
{
	HRESULT Result( S_OK ); 
	
	LONGLONG Handle = -1;
	
	Result = CreateManagedIndexArray( Name, Depth, &Handle );
	
	if( S_OK == Result )
	{
		rArrayHandle = new SVDataManagerIndexArrayHandle( *this, Handle );
	}
	else
	{
		rArrayHandle.clear();
	}
	
	return Result;
}

HRESULT SVDataManager::GetNextAvailableBufferIndex( const SVSmartIndexArrayHandlePtr& rArrayHandle, SVDataManagerLockTypeEnum aLockType, SVDataManagerHandle& rHandle )
{
	HRESULT Result( S_OK );
	
	rHandle.clear();
	
	if( !rArrayHandle.empty() )
	{
		long l_Index = -1;
		long l_TransactionId = -1;
		
		Result = GetNextAvailableBufferIndex( rArrayHandle->m_ArrayHandle, aLockType, true, &l_Index, &l_TransactionId );
		
		if( S_OK == Result )
		{
			rHandle.m_pArrayHandle = rArrayHandle;
			rHandle.m_LockType = aLockType;
			rHandle.m_Index = l_Index;
			rHandle.m_TransactionId = l_TransactionId;
		}
	}
	else
	{
		Result = E_POINTER;
	}
	
	return Result;
}

HRESULT SVDataManager::GetNextAvailableBufferIndexNoWait( const SVSmartIndexArrayHandlePtr& rArrayHandle, SVDataManagerLockTypeEnum aLockType, SVDataManagerHandle& rHandle )
{
	HRESULT Result = S_OK;
	
	rHandle.clear();
	
	if( !rArrayHandle.empty() )
	{
		long l_Index = -1;
		long l_TransactionId = -1;
		
		Result = GetNextAvailableBufferIndex( rArrayHandle->m_ArrayHandle, aLockType, false, &l_Index, &l_TransactionId );
		
		if( S_OK == Result )
		{
			rHandle.m_pArrayHandle = rArrayHandle;
			rHandle.m_LockType = aLockType;
			rHandle.m_Index = l_Index;
			rHandle.m_TransactionId = l_TransactionId;
		}
	}
	else
	{
		Result = E_POINTER;
	}
	
	return Result;
}

/*
This method creates managed index array elements.
*/
HRESULT SVDataManager::CreateManagedIndexArray (BSTR  aIndexArrayName, long  alIndexArrayDepth, LONGLONG* pIndexArrayHandle)
{
	HRESULT Result( S_OK );
	
	SVManagedIndexArray*  pNewManagedIndexArray( nullptr );
	
	if( nullptr != pIndexArrayHandle )
	{
		*pIndexArrayHandle = -1;
	
		if( MAXIMUM_DATAMANAGER_DEPTH >= alIndexArrayDepth  )
		{
			pNewManagedIndexArray = new SVManagedIndexArray;
		
			if (nullptr != pNewManagedIndexArray)
			{
				pNewManagedIndexArray->SetName( SvUl_SF::createSVString(aIndexArrayName) );
		
				Result = pNewManagedIndexArray->SetSize( alIndexArrayDepth );

				if( S_OK == Result )
				{
					//--- When making changes to SVDataManager elements it is necessary to lock
					//--- the mutex.
					if( Lock( 1000 ) )
					{
						if (m_FirstUnusedIndex == -1)
						{
							//------ There are no unused indexes. ---------------------------------------
							m_ManagedIndexArrayVector.push_back(pNewManagedIndexArray);
							*pIndexArrayHandle = static_cast<LONGLONG> (m_ManagedIndexArrayVector.size()-1);
						}
						else
						{
							LONGLONG  temp;
			
							temp = reinterpret_cast<LONGLONG> (m_ManagedIndexArrayVector [m_FirstUnusedIndex]);
			
							m_ManagedIndexArrayVector [m_FirstUnusedIndex] = pNewManagedIndexArray;
			
							*pIndexArrayHandle = m_FirstUnusedIndex;
							m_FirstUnusedIndex = temp;
			
							//------ If svmlFirstUnusedIndex equals -1, then it means we have just 
							//------ emptied the list. The svmlLastUnusedIndex value must be reset. -----
							if (m_FirstUnusedIndex == -1)
							{
								m_LastUnusedIndex = -1;
							}
						}
		
						Unlock ();
					}
					else
					{
						//------ If the mutex can not lock in 1 second, then we consider that a 
						//------ problem.
						Result = SVDM_1512COULDNOTLOCKDATAMANAGER_FATAL;
					}
				}
			}
			else
			{
				Result = SVDM_1509NOALLOCATE_FATAL;
			}
		}
		else
		{
			Result = SVDM_1510MAXIMUMDEPTH_ERROR;
		}
	}
	else
	{
		Result = E_POINTER;
	}
	
	if( FAILED( Result ) )
	{
		//--- No errors occur after the lock (accept for the lock) ------------------
		delete pNewManagedIndexArray;
		pNewManagedIndexArray = nullptr;
		
		Unlock (); // Just in case someone puts in an error
		//  break after locking and prior to 
		//  unlocking.
	}
	
	return Result;
}

/*
This method gets the next available buffer index based on the array index handle.
*/
HRESULT SVDataManager::GetNextAvailableBufferIndex( LONGLONG alIndexArrayHandle, SVDataManagerLockTypeEnum aLockType, bool p_WaitForLock, long* pAvailableBufferIndex, long* pTransactionId )
{

	HRESULT               Result( S_OK );
	long                  lAvailableBufferIndex = -1;
	long				  lTransactionId = 0;

	SVManagedIndex*       l_pAvailableIndex = nullptr;
	SVManagedIndex*       l_pNextAvailableIndex = nullptr;


	SVManagedIndexArray*  pManagedIndexArray;


	Result = ValidateIndexArrayHandle (alIndexArrayHandle);
	if( S_OK == Result )
	{
		Result = ValidateLockType (aLockType);
		if( S_OK == Result )
		{

			pManagedIndexArray = m_ManagedIndexArrayVector [alIndexArrayHandle];
			if( nullptr != pManagedIndexArray )
			{
				Result = pManagedIndexArray->GetNextIndex( lAvailableBufferIndex, lTransactionId, p_WaitForLock );
				if( lAvailableBufferIndex == -1 )
				{
					if( S_OK == Result )
					{
						Result = SVDM_1502NO_INDEXESAVAILABLE_ERROR;
					}

					SVString Message = pManagedIndexArray->GetReferenceCounts();

					SvStl::MessageMgrStd Exception( SvStl::LogOnly );
					Exception.setMessage( SVDM_1502NO_INDEXESAVAILABLE_ERROR, Message.c_str(), SvStl::SourceFileParams(StdMessageParams) );
				}
				else
				{
					Result = pManagedIndexArray->GetAt( lAvailableBufferIndex, l_pAvailableIndex );

					if( S_OK != Result || nullptr == l_pAvailableIndex )
					{
						if( S_OK == Result )
						{
							Result = SVDM_1513COULDNOTLOCKMANAGEDARRAY_FATAL;
						}
					}
					else
					{
						l_pAvailableIndex->svmlTransactionId = lTransactionId;

						Result = LockBufferIndex (alIndexArrayHandle, lAvailableBufferIndex, lTransactionId, aLockType);
					}
				}

				if (FAILED( Result ) )
				{
					if (lAvailableBufferIndex != -1)
					{
						pManagedIndexArray->ClearIndex( lAvailableBufferIndex, lTransactionId );
					}
				}
			}
			else
			{
				Result = E_POINTER;
			}
		}
	}

	if( SUCCEEDED(Result) )
	{
		*pAvailableBufferIndex = lAvailableBufferIndex;
		*pTransactionId = lTransactionId;
	}

	return Result;
}

/*
This method locks a buffer index based on the index value and the index array handle.
*/
HRESULT SVDataManager::LockBufferIndex (LONGLONG IndexArrayHandle, long    alBufferIndex, long	alTransactionId, SVDataManagerLockTypeEnum aLockType)
{
	HRESULT Result( S_OK );

	Result = ValidateIndexArrayHandle( IndexArrayHandle );

	if( S_OK == Result )
	{
		Result = ValidateLockType( aLockType );
	}
		
	if( S_OK == Result )
	{
		SVManagedIndexArray* pManagedIndexArray = m_ManagedIndexArrayVector[ IndexArrayHandle ];
		
		Result = pManagedIndexArray->AddReference( alBufferIndex, alTransactionId, aLockType );
	}
   
	return Result;
}

/*
This method locks a buffer index based on the index value and the index array handle.
*/
HRESULT SVDataManager::LockBufferIndexNoLock( LONGLONG IndexArrayHandle, long alBufferIndex, long alTransactionId,  SVDataManagerLockTypeEnum aLockType )
{
	HRESULT Result( S_OK );

	Result = ValidateIndexArrayHandle( IndexArrayHandle );

	if( S_OK == Result )
	{
		Result = ValidateLockType( aLockType );
	}
		
	if( S_OK == Result )
	{
		SVManagedIndexArray* pManagedIndexArray = m_ManagedIndexArrayVector[ IndexArrayHandle ];
		
		Result = pManagedIndexArray->AddReferenceNoLock( alBufferIndex, alTransactionId, aLockType );
	}
   
	return Result;
}

/*
This method releases a buffer index based on the index value and the index array handle.
*/
HRESULT SVDataManager::ReleaseBufferIndex( LONGLONG IndexArrayHandle, long alBufferIndex, long alTransactionId, SVDataManagerLockTypeEnum aLockType)
{
	HRESULT Result = S_OK;

	Result = ValidateIndexArrayHandle( IndexArrayHandle );

	if( S_OK == Result )
	{
		Result = ValidateLockType( aLockType );
	}

	if( S_OK == Result )
	{
		SVManagedIndexArray* pManagedIndexArray = m_ManagedIndexArrayVector[ IndexArrayHandle ];

		Result = pManagedIndexArray->ReleaseReference( alBufferIndex, alTransactionId, aLockType );
	}

	return Result;
}

/*
This method releases a buffer index based on the index value and the index array handle.
*/
HRESULT SVDataManager::ReleaseBufferIndexNoLock( LONGLONG IndexArrayHandle, long alBufferIndex, long alTransactionId, SVDataManagerLockTypeEnum aLockType)
{
	HRESULT Result = S_OK;

	Result = ValidateIndexArrayHandle( IndexArrayHandle );

	if( S_OK == Result )
	{
		Result = ValidateLockType( aLockType );
	}

	if( S_OK == Result )
	{
		SVManagedIndexArray* pManagedIndexArray = m_ManagedIndexArrayVector[ IndexArrayHandle ];

		Result = pManagedIndexArray->ReleaseReferenceNoLock( alBufferIndex, alTransactionId, aLockType );
	}

	return Result;
}


HRESULT SVDataManager::DestroyIndexArray( LONGLONG IndexArrayHandle)
{
	HRESULT Result( S_OK );
	SVManagedIndexArray*    pManagedIndexArray;
	
	Result = ValidateIndexArrayHandle (IndexArrayHandle);
	if( S_OK == Result )
	{
		pManagedIndexArray = m_ManagedIndexArrayVector [IndexArrayHandle];
		
		if( pManagedIndexArray->GetAvailableIndexCount() == pManagedIndexArray->GetSize() )
		{
			if( Lock( 1000 ) )
			{
				if (m_LastUnusedIndex == -1)
				{
					//------ There are no previous unused indexes. ------------------------------
					m_FirstUnusedIndex = IndexArrayHandle;
				}
				else
				{
					//------ There are other unused indexes. 
					//------ Please newly destroyed index at the end of the unused list. --------
					m_ManagedIndexArrayVector [m_LastUnusedIndex] = reinterpret_cast<SVManagedIndexArray*> (IndexArrayHandle);
				}
		
				m_ManagedIndexArrayVector [IndexArrayHandle] = reinterpret_cast<SVManagedIndexArray*> (-1);
				m_LastUnusedIndex = IndexArrayHandle;
		
				//--- The following delete, deletes the ManagedIndexArrayLock, so there is no
				//--- more need to free it.
				delete pManagedIndexArray;
				pManagedIndexArray = nullptr;
			}
			else
			{
				//------ If the mutex can not lock in 1 second, then we consider that a 
				//------ problem.
				Result = SVDM_1518COULDNOTLOCKDATAMANAGER_FATAL;
			}
		}
		else
		{
			//------ If the managed array is still being used (locks are still present). 
			Result = SVDM_1508INDEXESNOTFREED_FATAL;
		}
	}
	Unlock ();
	
	return Result;
}

/*
This method will validate the Index Array Handle.
*/
HRESULT SVDataManager::ValidateIndexArrayHandle( LONGLONG IndexArrayHandle ) const
{
	HRESULT  Result( S_OK );
	
	if( 0 <= IndexArrayHandle )
	{
		long lNbrOfElements = static_cast<long> (m_ManagedIndexArrayVector.size());

		if (IndexArrayHandle < lNbrOfElements)
		{
			if ( nullptr == m_ManagedIndexArrayVector [IndexArrayHandle] )
			{
				//------ This is a bit of an assumtion. If the handle is actually within 
				//------ acceptable boundaries, but the contents of the contained pointer are
				//------ also within the array boundries, then this is assumed to be part of 
				//------ the previously released, un-used linked list.
				Result = SVDM_1500INVALIDINDEXARRAYHANDLE_ERROR;
			}
		}
		else
		{
			Result = SVDM_1500INVALIDINDEXARRAYHANDLE_ERROR;
		}

	}
	else
	{
		Result = SVDM_1500INVALIDINDEXARRAYHANDLE_ERROR;
	}
		
	return Result;
}

/*
This method validate the lock type.
*/
HRESULT SVDataManager::ValidateLockType( SVDataManagerLockTypeEnum alLockType ) const
{
	HRESULT Result( S_OK );
	if (alLockType < SV_ACQUISITION || alLockType > SV_OTHER )
	{
		Result = SVDM_1501INVALIDLOCKTYPE_ERROR;
	}
		
	return Result;
}

/*
This method validate the Buffer Index based on a Index Array Handle.  alIndexArrayHandle must have already been validated separately.
*/
HRESULT SVDataManager::ValidateBufferIndex( LONGLONG IndexArrayHandle, long alBufferIndex, long alTransactionId ) const
{
	HRESULT Result = S_OK;

	SVManagedIndexArray* pManagedIndexArray = m_ManagedIndexArrayVector[ IndexArrayHandle ];
	if( nullptr != pManagedIndexArray )
	{
		if( -1 == alTransactionId )
		{
			Result = pManagedIndexArray->ValidateIndex( alBufferIndex );
		}
		else
		{
			Result = pManagedIndexArray->ValidateIndex( alBufferIndex, alTransactionId );
		}
	}
	else
	{
		Result = E_POINTER;
	}

	return Result;
}

HRESULT SVDataManager::ValidateBufferIndexNoLock( LONGLONG IndexArrayHandle, long alBufferIndex, long alTransactionId ) const
{
	HRESULT Result = S_OK;
	
	SVManagedIndexArray* pManagedIndexArray = m_ManagedIndexArrayVector[ IndexArrayHandle ];
	
	if( nullptr != pManagedIndexArray )
	{
		Result = pManagedIndexArray->ValidateIndexNoLock( alBufferIndex, alTransactionId );
	}
	else
	{
		Result = E_POINTER;
	}
	
	return Result;
}

HRESULT	SVDataManager::GetNbrOfAvailableIndexes (LONGLONG IndexArrayHandle, long* pNbrOfAvailableIndexes) const
{
	HRESULT Result( S_OK );
	
	if( nullptr != pNbrOfAvailableIndexes )
	{
		*pNbrOfAvailableIndexes = 0;
	
		Result = ValidateIndexArrayHandle( IndexArrayHandle );
		
		if( S_OK == Result )
		{
			SVManagedIndexArray* pManagedIndexArray = m_ManagedIndexArrayVector[ IndexArrayHandle ];
			if( nullptr != pManagedIndexArray )
			{
				*pNbrOfAvailableIndexes = static_cast<long> (pManagedIndexArray->GetAvailableIndexCount());
			}
			else
			{
				Result = E_POINTER;
			}
		}
	}
	else
	{
		Result = E_POINTER;
	}
	
	return Result;
}


// This function dumps all data associated with this handle to a file.
void SVDataManager::Dump (LONGLONG IndexArrayHandle, LPCSTR szSource ) const
{
	HRESULT Result( S_OK );
	SVManagedIndexArray*  pManagedIndexArray( nullptr );
	
	pManagedIndexArray = nullptr;

	Result = ValidateIndexArrayHandle( IndexArrayHandle);
	if( S_OK == Result )
	{
		pManagedIndexArray = m_ManagedIndexArrayVector [IndexArrayHandle];
		if(nullptr != pManagedIndexArray )
		{
			pManagedIndexArray->Dump( szSource );
		}
	}
}


// This function dumps all data associated with this handle to a file.
void SVDataManager::Dump_All ()
{
	for( long alIndexArrayHandle = 0 ; alIndexArrayHandle < m_ManagedIndexArrayVector.size() ; alIndexArrayHandle++ )
	{
		
		HRESULT Result = ValidateIndexArrayHandle (alIndexArrayHandle);
		
		if( S_OK != Result)
		{
			continue;
		}
		
		SVManagedIndexArray* pManagedIndexArray = m_ManagedIndexArrayVector [alIndexArrayHandle];
		
		if( nullptr == pManagedIndexArray )
		{
			continue;
		}

		pManagedIndexArray->Dump( _T( "Data Manager" ) );
	}
}


long SVDataManager::GetIndexLockCountByType( LONGLONG IndexArrayHandle, long Index, SVDataManagerLockTypeEnum LockType )
{
	long l_Count( 0 );
	
	if( S_OK == ValidateIndexArrayHandle( IndexArrayHandle ) )
	{
		if( S_OK == ValidateLockType( LockType ) )
		{
			SVManagedIndexArray* pManagedIndexArray = m_ManagedIndexArrayVector[ IndexArrayHandle ];
			
			if( nullptr != pManagedIndexArray && 0 <= Index && Index < static_cast< long >( pManagedIndexArray->GetSize() ) )
			{
				l_Count = pManagedIndexArray->GetReferenceCount( Index, LockType );
			}
		}
	}
	
	return l_Count;
}

long SVDataManager::GetLockCountByType( LONGLONG IndexArrayHandle, SVDataManagerLockTypeEnum LockType )
{
	long l_Count( 0 );
	
	if( S_OK == ValidateIndexArrayHandle( IndexArrayHandle ) )
	{
		if( S_OK == ValidateLockType( LockType ) )
		{
			SVManagedIndexArray* pManagedIndexArray = m_ManagedIndexArrayVector[ IndexArrayHandle ];
			
			if( nullptr != pManagedIndexArray )
			{
				for( size_t i = 0; i < pManagedIndexArray->GetSize(); ++i )
				{
					l_Count += pManagedIndexArray->GetReferenceCount( i, LockType );
				}
			}
		}
	}
	
	return l_Count;
}

bool SVDataManager::Lock( unsigned long Timeout ) const
{
	bool Result = m_IsLockCreated;
	
	if( Result )
	{
		if( Timeout == INFINITE )
		{
			::EnterCriticalSection( &m_Lock );
		}
		else
		{
			SvTl::SVTimeStamp l_CurrentTime = 0.0;
			SvTl::SVTimeStamp l_EndTime = 0.0;
			
			bool l_LockAcquired = false;
			
			l_CurrentTime = SvTl::GetTimeStamp();
			
			l_EndTime = l_CurrentTime + SvTl::ConvertFrom( SvTl::Milliseconds, Timeout );
			
			while( ! l_LockAcquired && l_CurrentTime <= l_EndTime )
			{
				l_LockAcquired = ::TryEnterCriticalSection(&m_Lock) ? true : false;
				
				if( ! l_LockAcquired && l_CurrentTime < l_EndTime)
				{
					::Sleep( 1 );
					l_CurrentTime = SvTl::GetTimeStamp();
				}
			}
			
			Result = l_LockAcquired;
		}
	}
	else
	{
		Result = false;
	}
	
	return Result;
}

bool SVDataManager::Unlock() const
{
	bool Result = m_IsLockCreated;
	
	if( Result )
	{
		::LeaveCriticalSection( &m_Lock );
	}
	
	return Result;
}

