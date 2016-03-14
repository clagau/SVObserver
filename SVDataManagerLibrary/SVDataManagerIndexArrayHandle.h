//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataManagerIndexArrayHandle
//* .File Name       : $Workfile:   SVDataManagerIndexArrayHandle.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:54:30  $
//******************************************************************************

#ifndef SVDATAMANAGERINDEXARRAYHANDLE_H
#define SVDATAMANAGERINDEXARRAYHANDLE_H

#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVDataManagerEnums.h"

class SVDataManagerIndexArrayHandle  
{
public:
	friend class SVDataManager;
	friend class SVDataManagerHandle;

	SVDataManagerIndexArrayHandle();
	SVDataManagerIndexArrayHandle( SVDataManager& p_rDataManager, long p_ArrayHandle );

	virtual ~SVDataManagerIndexArrayHandle();

	long GetNbrOfAvailableIndexes() const;

	long GetLockCountByType( SVDataManagerLockTypeEnum p_LockType ) const;

	void Dump( LPCSTR p_szSource ) const;

protected:
	HRESULT LockBufferIndex( long p_Index, long p_TransactionId, SVDataManagerLockTypeEnum p_LockType );

	HRESULT ReleaseBufferIndex( const SVDataManagerHandle& p_rHandle );

	HRESULT ValidateBufferIndex( const SVDataManagerHandle& p_rHandle );

	long GetIndexLockCountByType( const SVDataManagerHandle& p_rHandle );

	SVDataManager* m_pDataManager;
	long m_ArrayHandle;

private:
	SVDataManagerIndexArrayHandle( const SVDataManagerIndexArrayHandle& p_rObject );

	const SVDataManagerIndexArrayHandle& operator=( const SVDataManagerIndexArrayHandle& p_rObject );

};

typedef SVSharedPtr< SVDataManagerIndexArrayHandle > SVSmartIndexArrayHandlePtr;

#endif

