//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataManagerLockHost
//* .File Name       : $Workfile:   SVDataManagerHandle.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:53:32  $
//******************************************************************************

#ifndef SVDATAMANAGERHANDLE_H
#define SVDATAMANAGERHANDLE_H

#include "SVDataManagerEnums.h"
#include "SVDataManagerIndexArrayHandle.h"

class SVDataManagerHandle
{
public:
	friend class SVDataManager;
	friend class SVDataManagerIndexArrayHandle;

	SVDataManagerHandle();
	SVDataManagerHandle( const SVDataManagerHandle& p_rObject );
	SVDataManagerHandle( const SVDataManagerHandle& p_rObject, SVDataManagerLockTypeEnum p_LockType );

	const SVDataManagerHandle& operator=( const SVDataManagerHandle& p_rObject );

	virtual ~SVDataManagerHandle();

	HRESULT Assign( const SVDataManagerHandle& p_rObject, SVDataManagerLockTypeEnum p_LockType );

	bool empty() const;
	void clear();

	SVDataManagerLockTypeEnum GetLockType() const;

	long GetIndex() const;

	long GetIndexLockCountByType( SVDataManagerLockTypeEnum p_LockType );

	long GetNbrOfAvailableIndexes() const;

protected:
	SVSmartIndexArrayHandlePtr m_pArrayHandle;

	SVDataManagerLockTypeEnum	m_LockType;
	long m_Index;
	long m_TransactionId;

};

#endif

