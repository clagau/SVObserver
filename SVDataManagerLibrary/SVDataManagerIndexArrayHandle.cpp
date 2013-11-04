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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVDataManagerLibrary\SVDataManagerIndexArrayHandle.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 07:54:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Jul 2011 08:01:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  729
 * SCR Title:  Adjust SVObserver to fix perfomance problems due to logging
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to remove logging and add a method to get lock count by type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   24 May 2010 13:16:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include a no lock path when locking is not required due to owner lock.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Sep 2009 16:47:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved code from SVLibrary to SVDataManagerLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Aug 2009 08:10:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated method to return count instead of errorous status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   13 Jul 2009 12:34:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the methodology for managing the indexes using the data manager.  The new methodology uses the smart pointer to externally reference count the index usage and release the array handle when all indexes are released.  Atransaction id was added to prevent unintended index reuse.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
