//******************************************************************************
//* COPYRIGHT (c) 2006 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDataManagerLockHost
//* .File Name       : $Workfile:   SVDataManagerHandle.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:53:48  $
//******************************************************************************

#include "stdafx.h"
//Moved to precompiled header#include <memory>
#include "SVDataManagerHandle.h"
#include "DataManager.h"

SVDataManagerHandle::SVDataManagerHandle()
: m_LockType( SV_INVALID_TYPE ),
	m_Index( -1 ),
	m_TransactionId( 0 )
{
}

SVDataManagerHandle::SVDataManagerHandle( const SVDataManagerHandle& p_rObject, SVDataManagerLockTypeEnum p_LockType )
: m_LockType( SV_INVALID_TYPE ),
	m_Index( -1 ),
	m_TransactionId( 0 )
{
	Assign( p_rObject, p_LockType );
}

SVDataManagerHandle::~SVDataManagerHandle()
{
	clear();
}

HRESULT SVDataManagerHandle::Assign( const SVDataManagerHandle& p_rObject, SVDataManagerLockTypeEnum p_LockType )
{
	HRESULT l_Status( S_OK );

	if( this != &p_rObject )
	{
		clear();

		if( nullptr != p_rObject.m_pArrayHandle && 0 <= p_rObject.m_Index )
		{
			l_Status = p_rObject.m_pArrayHandle->LockBufferIndex( p_rObject.m_Index, p_rObject.m_TransactionId, p_LockType );

			if( S_OK == l_Status )
			{
				m_pArrayHandle = p_rObject.m_pArrayHandle;
				m_LockType = p_LockType;
				m_Index = p_rObject.m_Index;
				m_TransactionId = p_rObject.m_TransactionId;
			}
		}
	}

	return l_Status;
}

bool SVDataManagerHandle::empty() const
{
	bool l_Status( m_Index == -1 );

	return l_Status;
}

void SVDataManagerHandle::clear()
{
	if(nullptr != m_pArrayHandle)
	{
		m_pArrayHandle->ReleaseBufferIndex( *this );
	}

	m_pArrayHandle.reset();
	m_LockType = SV_INVALID_TYPE;
	m_Index = -1;
	m_TransactionId = 0;
}

long SVDataManagerHandle::GetIndex() const
{
	long l_Index( -1 );

	if(nullptr != m_pArrayHandle)
	{
		if( S_OK == m_pArrayHandle->ValidateBufferIndex( *this ) )
		{
			l_Index = m_Index;
		}
	}

	return l_Index;
}

SVDataManagerLockTypeEnum SVDataManagerHandle::GetLockType() const
{
	return m_LockType;
}

long SVDataManagerHandle::GetIndexLockCountByType( SVDataManagerLockTypeEnum p_LockType )
{
	long l_Count = 0;

	if(nullptr != m_pArrayHandle)
	{
		l_Count = m_pArrayHandle->GetIndexLockCountByType( *this ); 
	}

	return l_Count;
}

long SVDataManagerHandle::GetNbrOfAvailableIndexes() const
{
	long l_Count = 0;

	if(nullptr != m_pArrayHandle)
	{
		l_Count = m_pArrayHandle->GetNbrOfAvailableIndexes();
	}

	return l_Count;
}

SVDataManagerHandle::SVDataManagerHandle( const SVDataManagerHandle& p_rObject )
: m_LockType( SV_INVALID_TYPE ),
	m_Index( -1 ),
	m_TransactionId( 0 )
{
	Assign( p_rObject, p_rObject.m_LockType );
}

const SVDataManagerHandle& SVDataManagerHandle::operator=( const SVDataManagerHandle& p_rObject )
{
	if( this != &p_rObject )
	{
		Assign( p_rObject, p_rObject.m_LockType );
	}

	return *this;
}

