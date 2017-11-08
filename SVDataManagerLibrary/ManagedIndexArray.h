// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : ManagedIndexArray.h
// * .File Name       : $Workfile:   ManagedIndexArray.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   07 May 2013 19:59:28  $
// ******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVDataManagerGlobals.h"
#include "SVDataManagerEnums.h"
#pragma endregion Includes

class SVManagedIndex
{
public:
	SVManagedIndex();

	//	A TransactionId of 0 should mean unassigned.
	long  svmlTransactionId;

	std::string GetReferenceCounts() const;

	long GetReferenceCount() const;
	long GetReferenceCount( SVDataManagerLockTypeEnum p_eType ) const;

	HRESULT AddReference( SVDataManagerLockTypeEnum p_eType );
	HRESULT ReleaseReference( SVDataManagerLockTypeEnum p_eType );
	HRESULT ReleaseAllReferences();

protected:
	// Holder for acquisition reference count.
	long  svmlAcquisition;

	// Holder for inspection reference count.
	long  svmlInspection;

	// Holder for product position queue (PPQ) reference count.
	long  svmlPPQ;

	// Holder for archive reference count.
	long  svmlArchive;

	// Holder for display reference count.
	long  svmlDisplay;

	// Holder for last inspection reference count.
	long  svmlLastInspected;

	// Holder for DCOM reference count.
	long  svmlDCOM;

	// Holder for unknown reference count.
	long  svmlOther;

};

/*
This container is used to track the index usage of a particular managed element.
*/
class SVManagedIndexArray 
{
public:
	friend class SVDataManager;

	SVManagedIndexArray();
	
	virtual ~SVManagedIndexArray();
	 
	HRESULT RemoveAll();

	size_t GetSize() const;
	HRESULT SetSize( size_t p_Size );

	HRESULT GetAt( size_t p_Index, SVManagedIndex*& p_rpManagedIndex ) const;

	HRESULT ValidateIndex( size_t p_Index ) const;
	HRESULT ValidateIndex( size_t p_Index, long p_TransactionId ) const;
	HRESULT ReleaseAllIndexes();

	std::string GetReferenceCounts() const;

	long GetReferenceCount( size_t p_Index ) const;
	long GetReferenceCount( size_t p_Index, SVDataManagerLockTypeEnum p_eType ) const;

	HRESULT GetNextIndex( long& p_rIndex, long& p_rTransactionID, bool p_WaitForLock );
	HRESULT ClearIndex( size_t p_Index, long p_TransactionId );

	HRESULT AddReference( size_t p_Index, long p_TransactionId, SVDataManagerLockTypeEnum p_eType );
	HRESULT ReleaseReference( size_t p_Index, long p_TransactionId, SVDataManagerLockTypeEnum p_eType );

	size_t GetAvailableIndexCount() const;

	bool LogEventMessage();

	const std::string& GetName() const;
	void SetName( const std::string& p_rName );

	long GetMessageCounter() const;

	void Dump( const std::string& p_rSourceName ) const;

protected:
	typedef std::vector< SVManagedIndex* > SVManagedIndexPtrVector;

	HRESULT GetAtNoLock( size_t p_Index, SVManagedIndex*& p_rpManagedIndex ) const;

	HRESULT AddReferenceNoLock( size_t p_Index, long p_TransactionId, SVDataManagerLockTypeEnum p_eType );
	HRESULT ReleaseReferenceNoLock( size_t p_Index, long p_TransactionId, SVDataManagerLockTypeEnum p_eType );

	HRESULT ValidateIndexNoLock( size_t p_Index, long p_TransactionId ) const;

	// This is the name of the managed array
	std::string m_Name;

	// svmlTransactionCounter - this will assigned to each index when the index 
	// is first retrieved.  This value will be increamented for each retrieved 
	// index.  This counter be maintianed per array, and it will be acceptable 
	// for this to rollover. ----------------------------------------------------
	long m_TransactionCounter;

	long m_MessageCounter;

	// This is the size of the managed array.
	size_t m_ArrayDepth;

	long m_LastIndex;

	// This is the array of reference counting index elements.
	SVManagedIndexPtrVector m_IndexArray;

	// This attribute manages the lock for multi-threaded access to this container.
	mutable SVCriticalSection m_Lock;
	
	SvTl::SVTimeStamp m_LastMessageTimeStamp;

private:
	SVManagedIndexArray( const SVManagedIndexArray& p_rObject );

	const SVManagedIndexArray& operator=( const SVManagedIndexArray& p_rObject );

};

typedef std::vector<SVManagedIndexArray*> SVManagedIndexArrayVector;