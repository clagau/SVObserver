//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataManagerLockHost
//* .File Name       : $Workfile:   SVDataManagerIndexArrayHandle.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:54:28  $
//******************************************************************************

#include "stdafx.h"
#include "SVDataManagerIndexArrayHandle.h"
#include "DataManager.h"
#include "SVDataManagerHandle.h"

SVDataManagerIndexArrayHandle::SVDataManagerIndexArrayHandle()
: m_pDataManager( nullptr ),
	m_ArrayHandle( -1 )
{
}

SVDataManagerIndexArrayHandle::SVDataManagerIndexArrayHandle( SVDataManager& rDataManager, LONGLONG ArrayHandle )
: m_pDataManager( &rDataManager ),
	m_ArrayHandle( ArrayHandle )
{
}

SVDataManagerIndexArrayHandle::~SVDataManagerIndexArrayHandle()
{
	if( nullptr != m_pDataManager && 0 <= m_ArrayHandle )
	{
		m_pDataManager->ReleaseAllIndexes( m_ArrayHandle );
		m_pDataManager->DestroyIndexArray( m_ArrayHandle );
	}

	m_ArrayHandle = -1;
}

long SVDataManagerIndexArrayHandle::GetNbrOfAvailableIndexes() const
{
	long l_Size = 0;

	if( nullptr != m_pDataManager && 0 <= m_ArrayHandle )
	{
		m_pDataManager->GetNbrOfAvailableIndexes( m_ArrayHandle, &l_Size );
	}

	return l_Size;
}

long SVDataManagerIndexArrayHandle::GetLockCountByType( SVDataManagerLockTypeEnum LockType ) const
{
	long l_Count = 0;

	if( nullptr != m_pDataManager && 0 <= m_ArrayHandle )
	{
		l_Count = m_pDataManager->GetLockCountByType( m_ArrayHandle, LockType );
	}

	return l_Count;
}

void SVDataManagerIndexArrayHandle::Dump( LPCSTR szSource ) const
{
	if( nullptr != m_pDataManager && 0 <= m_ArrayHandle )
	{
		m_pDataManager->Dump( m_ArrayHandle, szSource );
	}
}

HRESULT SVDataManagerIndexArrayHandle::LockBufferIndex( long Index, long TransactionId, SVDataManagerLockTypeEnum LockType )
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pDataManager && 0 <= m_ArrayHandle )
	{
		l_Status = m_pDataManager->LockBufferIndexNoLock( m_ArrayHandle, Index, TransactionId, LockType );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVDataManagerIndexArrayHandle::ReleaseBufferIndex( const SVDataManagerHandle& rHandle )
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pDataManager && 0 <= m_ArrayHandle )
	{
		l_Status = m_pDataManager->ReleaseBufferIndexNoLock( m_ArrayHandle, rHandle.m_Index, rHandle.m_TransactionId, rHandle.m_LockType );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVDataManagerIndexArrayHandle::ValidateBufferIndex( const SVDataManagerHandle& rHandle )
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pDataManager && 0 <= m_ArrayHandle )
	{
		l_Status = m_pDataManager->ValidateBufferIndexNoLock( m_ArrayHandle, rHandle.m_Index, rHandle.m_TransactionId );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

long SVDataManagerIndexArrayHandle::GetIndexLockCountByType( const SVDataManagerHandle& rHandle )
{
	long l_Count = 0;

	if( nullptr != m_pDataManager && 0 <= m_ArrayHandle )
	{
		l_Count = m_pDataManager->GetIndexLockCountByType( m_ArrayHandle, rHandle.m_Index, rHandle.m_LockType );
	}

	return l_Count;
}

