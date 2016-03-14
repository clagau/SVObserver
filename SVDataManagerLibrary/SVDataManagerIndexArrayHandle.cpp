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
: m_pDataManager( NULL ),
	m_ArrayHandle( -1 )
{
}

SVDataManagerIndexArrayHandle::SVDataManagerIndexArrayHandle( SVDataManager& p_rDataManager, long p_ArrayHandle )
: m_pDataManager( &p_rDataManager ),
	m_ArrayHandle( p_ArrayHandle )
{
}

SVDataManagerIndexArrayHandle::~SVDataManagerIndexArrayHandle()
{
	if( m_pDataManager != NULL && 0 <= m_ArrayHandle )
	{
		m_pDataManager->ReleaseAllIndexes( m_ArrayHandle );
		m_pDataManager->DestroyIndexArray( m_ArrayHandle );
	}

	m_ArrayHandle = -1;
}

long SVDataManagerIndexArrayHandle::GetNbrOfAvailableIndexes() const
{
	long l_Size = 0;

	if( m_pDataManager != NULL && 0 <= m_ArrayHandle )
	{
		m_pDataManager->GetNbrOfAvailableIndexes( m_ArrayHandle, &l_Size );
	}

	return l_Size;
}

long SVDataManagerIndexArrayHandle::GetLockCountByType( SVDataManagerLockTypeEnum p_LockType ) const
{
	long l_Count = 0;

	if( m_pDataManager != NULL && 0 <= m_ArrayHandle )
	{
		l_Count = m_pDataManager->GetLockCountByType( m_ArrayHandle, p_LockType );
	}

	return l_Count;
}

void SVDataManagerIndexArrayHandle::Dump( LPCSTR p_szSource ) const
{
	if( m_pDataManager != NULL && 0 <= m_ArrayHandle )
	{
		m_pDataManager->Dump( m_ArrayHandle, p_szSource );
	}
}

HRESULT SVDataManagerIndexArrayHandle::LockBufferIndex( long p_Index, long p_TransactionId, SVDataManagerLockTypeEnum p_LockType )
{
	HRESULT l_Status = S_OK;

	if( m_pDataManager != NULL && 0 <= m_ArrayHandle )
	{
		l_Status = m_pDataManager->LockBufferIndexNoLock( m_ArrayHandle, p_Index, p_TransactionId, p_LockType );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVDataManagerIndexArrayHandle::ReleaseBufferIndex( const SVDataManagerHandle& p_rHandle )
{
	HRESULT l_Status = S_OK;

	if( m_pDataManager != NULL && 0 <= m_ArrayHandle )
	{
		l_Status = m_pDataManager->ReleaseBufferIndexNoLock( m_ArrayHandle, p_rHandle.m_Index, p_rHandle.m_TransactionId, p_rHandle.m_LockType );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVDataManagerIndexArrayHandle::ValidateBufferIndex( const SVDataManagerHandle& p_rHandle )
{
	HRESULT l_Status = S_OK;

	if( m_pDataManager != NULL && 0 <= m_ArrayHandle )
	{
		l_Status = m_pDataManager->ValidateBufferIndexNoLock( m_ArrayHandle, p_rHandle.m_Index, p_rHandle.m_TransactionId );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

long SVDataManagerIndexArrayHandle::GetIndexLockCountByType( const SVDataManagerHandle& p_rHandle )
{
	long l_Count = 0;

	if( m_pDataManager != NULL && 0 <= m_ArrayHandle )
	{
		l_Count = m_pDataManager->GetIndexLockCountByType( m_ArrayHandle, p_rHandle.m_Index, p_rHandle.m_LockType );
	}

	return l_Count;
}

