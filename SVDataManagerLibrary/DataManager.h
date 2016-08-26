// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDataManager
// * .File Name       : $Workfile:   DataManager.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 07:53:36  $
// ******************************************************************************

#pragma once

#include "ManagedIndexArray.h"
#include "ManagedIndexArrayList.h"
#include "SVDataManagerIndexArrayHandle.h"
#include "SVDataManagerEnums.h"

class ManagedIndexArray;

/*
This object manages object element resources in the system.
*/
class SVDataManager 
{
public:
	friend class SVDataManagerHandle;
	friend class SVDataManagerIndexArrayHandle;

	SVDataManager();

	virtual ~SVDataManager();
	
	HRESULT CreateManagedIndexArray( SVSmartIndexArrayHandlePtr& rArrayHandle, BSTR Name, long Depth );
	
	HRESULT GetNextAvailableBufferIndex( const SVSmartIndexArrayHandlePtr& rArrayHandle, SVDataManagerLockTypeEnum aLockType, SVDataManagerHandle& rHandle );
	
	HRESULT GetNextAvailableBufferIndexNoWait( const SVSmartIndexArrayHandlePtr& rArrayHandle, SVDataManagerLockTypeEnum aLockType, SVDataManagerHandle& rHandle );

	void Dump_All();
	
protected:
	HRESULT  ReleaseAllIndexes( LONGLONG IndexArrayHandle );
	
	HRESULT  CreateManagedIndexArray( BSTR aIndexArrayName, long alIndexArrayDepth, LONGLONG* alpIndexArrayHandle );
	
	HRESULT  GetNextAvailableBufferIndex( LONGLONG IndexArrayHandle, SVDataManagerLockTypeEnum aLockType, bool WaitForLock, long* alBufferIndex, long* alTransactionId );
	
	HRESULT  LockBufferIndex( LONGLONG IndexArrayHandle, long alBufferIndex, long alTransactionId, SVDataManagerLockTypeEnum aLockType);
	
	HRESULT  LockBufferIndexNoLock( LONGLONG IndexArrayHandle, long alBufferIndex, long alTransactionId, SVDataManagerLockTypeEnum aLockType);
	
	HRESULT  ReleaseBufferIndex( LONGLONG IndexArrayHandle, long alBufferIndex, long alTransactionId, SVDataManagerLockTypeEnum aLockType);
	
	HRESULT  ReleaseBufferIndexNoLock( LONGLONG IndexArrayHandle, long alBufferIndex, long alTransactionId, SVDataManagerLockTypeEnum aLockType);
	
	HRESULT  DestroyIndexArray( LONGLONG IndexArrayHandle );

	HRESULT  ValidateIndexArrayHandle( LONGLONG IndexArrayHandle ) const;
	
	HRESULT  ValidateLockType( SVDataManagerLockTypeEnum LockType ) const;
	
	
	HRESULT	GetNbrOfAvailableIndexes( LONGLONG IndexArrayHandle, long* alpNbrOfAvailableIndexes ) const;
	
	void Dump( LONGLONG IndexArrayHandle, LPCSTR szSource ) const;
		
	HRESULT  ValidateBufferIndex( LONGLONG IndexArrayHandle, long alBufferIndex, long alTransactionId = -1 ) const;

	HRESULT  ValidateBufferIndexNoLock( LONGLONG IndexArrayHandle, long alBufferIndex, long alTransactionId ) const;
	
	long GetIndexLockCountByType( LONGLONG IndexArrayHandle, long Index, SVDataManagerLockTypeEnum LockType );
	
	long GetLockCountByType( LONGLONG IndexArrayHandle, SVDataManagerLockTypeEnum LockType );
	
	// This element holds the array of managed index containers.
	SVManagedIndexArrayList svmManagedIndexArrayList;
	
	// This references the first position in the svmManagedIndexArrayList which have been destroyed.
	LONGLONG m_FirstUnusedIndex;

	// This references the last position in the svmManagedIndexArrayList which have been destroyed.
	LONGLONG m_LastUnusedIndex;
	
	BOOL Lock( unsigned long Timeout ) const;
	BOOL Unlock() const;

	bool m_IsLockCreated;

	// This attribute manages the lock for multi-threaded access to this container.
	mutable CRITICAL_SECTION m_Lock;
};
