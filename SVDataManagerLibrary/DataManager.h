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
	
	HRESULT CreateManagedIndexArray( SVSmartIndexArrayHandlePtr& p_pArrayHandle, BSTR p_Name, long p_Depth );
	
	HRESULT GetNextAvailableBufferIndex( const SVSmartIndexArrayHandlePtr& p_pArrayHandle, 
		SVDataManagerLockTypeEnum aLockType, SVDataManagerHandle& p_rHandle );
	
	HRESULT GetNextAvailableBufferIndexNoWait( const SVSmartIndexArrayHandlePtr& p_pArrayHandle, 
		SVDataManagerLockTypeEnum aLockType, SVDataManagerHandle& p_rHandle );

	void Dump_All();
	
protected:
	HRESULT  ReleaseAllIndexes( long alIndexArrayHandle );
	
	HRESULT  CreateManagedIndexArray( BSTR aIndexArrayName, long alIndexArrayDepth, long* alpIndexArrayHandle );
	
	HRESULT  GetNextAvailableBufferIndex( long alIndexArrayHandle, 
		SVDataManagerLockTypeEnum aLockType, bool p_WaitForLock,
		long* alBufferIndex, long* alTransactionId );
	
	HRESULT  LockBufferIndex( long alIndexArrayHandle, long alBufferIndex,
		long alTransactionId, SVDataManagerLockTypeEnum aLockType);
	
	HRESULT  LockBufferIndexNoLock( long alIndexArrayHandle, long alBufferIndex,
		long alTransactionId, SVDataManagerLockTypeEnum aLockType);
	
	HRESULT  ReleaseBufferIndex( long alIndexArrayHandle, long alBufferIndex,
		long alTransactionId, SVDataManagerLockTypeEnum aLockType);
	
	HRESULT  ReleaseBufferIndexNoLock( long alIndexArrayHandle, long alBufferIndex,
		long alTransactionId, SVDataManagerLockTypeEnum aLockType);
	
	HRESULT  DestroyIndexArray( long alIndexArrayHandle );

	HRESULT  ValidateIndexArrayHandle( long alIndexArrayHandle ) const;
	
	HRESULT  ValidateLockType( long alLockType );
	
	HRESULT  ValidateBufferIndex( long alIndexArrayHandle, long alBufferIndex );
	
	HRESULT	GetNbrOfAvailableIndexes( long alIndexArrayHandle, long* alpNbrOfAvailableIndexes );
	
	void Dump( long alIndexArrayHandle, LPCSTR p_szSource ) const;
		
	HRESULT  ValidateBufferIndex( long alIndexArrayHandle, long alBufferIndex, long alTransactionId );

	HRESULT  ValidateBufferIndexNoLock( long alIndexArrayHandle, long alBufferIndex, long alTransactionId );
	
	long GetIndexLockCountByType( long p_ArrayHandle, long p_Index, SVDataManagerLockTypeEnum p_LockType );
	
	long GetLockCountByType( long p_ArrayHandle, SVDataManagerLockTypeEnum p_LockType );
	
	// This element holds the array of managed index containers.
	SVManagedIndexArrayList svmManagedIndexArrayList;
	
	// This references the first position in the svmManagedIndexArrayList which have been destroyed.
	long svmlFirstUnusedIndex;

	// This references the last position in the svmManagedIndexArrayList which have been destroyed.
	long svmlLastUnusedIndex;
	
	BOOL Lock( unsigned long p_Timeout ) const;
	BOOL Unlock() const;

	bool m_IsLockCreated;

	// This attribute manages the lock for multi-threaded access to this container.
	mutable CRITICAL_SECTION m_Lock;
};
