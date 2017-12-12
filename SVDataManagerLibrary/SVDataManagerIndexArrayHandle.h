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

#pragma once

#pragma region Includes
#include "SVDataManagerEnums.h"
#pragma endregion Includes

class SVDataManagerIndexArrayHandle  
{
public:
	friend class SVDataManager;
	friend class SVDataManagerHandle;

	SVDataManagerIndexArrayHandle();
	SVDataManagerIndexArrayHandle( SVDataManager& rDataManager, LONGLONG ArrayHandle );

	virtual ~SVDataManagerIndexArrayHandle();

	long GetNbrOfAvailableIndexes() const;

	long GetLockCountByType( SVDataManagerLockTypeEnum LockType ) const;

	void Dump( LPCSTR szSource ) const;

protected:
	HRESULT LockBufferIndex( long Index, long TransactionId, SVDataManagerLockTypeEnum LockType );

	HRESULT ReleaseBufferIndex( const SVDataManagerHandle& rHandle );

	HRESULT ValidateBufferIndex( const SVDataManagerHandle& rHandle );

	long GetIndexLockCountByType( const SVDataManagerHandle& rHandle );

	SVDataManager* m_pDataManager;
	LONGLONG m_ArrayHandle;

private:
	SVDataManagerIndexArrayHandle( const SVDataManagerIndexArrayHandle& p_rObject );

	const SVDataManagerIndexArrayHandle& operator=( const SVDataManagerIndexArrayHandle& p_rObject );

};

typedef std::shared_ptr< SVDataManagerIndexArrayHandle > SVSmartIndexArrayHandlePtr;


