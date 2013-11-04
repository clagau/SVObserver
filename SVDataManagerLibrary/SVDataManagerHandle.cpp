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

		if( ! p_rObject.m_pArrayHandle.empty() && 0 <= p_rObject.m_Index )
		{
			l_Status = p_rObject.m_pArrayHandle->LockBufferIndex( p_rObject.m_Index, p_rObject.m_TransactionId, p_LockType );

			if( l_Status == S_OK )
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
	if( ! m_pArrayHandle.empty() )
	{
		m_pArrayHandle->ReleaseBufferIndex( *this );
	}

	m_pArrayHandle.clear();
	m_LockType = SV_INVALID_TYPE;
	m_Index = -1;
	m_TransactionId = 0;
}

long SVDataManagerHandle::GetIndex() const
{
	long l_Index( -1 );

	if( ! m_pArrayHandle.empty() )
	{
		if( m_pArrayHandle->ValidateBufferIndex( *this ) == S_OK )
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

	if( ! m_pArrayHandle.empty() )
	{
		l_Count = m_pArrayHandle->GetIndexLockCountByType( *this ); 
	}

	return l_Count;
}

long SVDataManagerHandle::GetNbrOfAvailableIndexes() const
{
	long l_Count = 0;

	if( ! m_pArrayHandle.empty() )
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVDataManagerLibrary\SVDataManagerHandle.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 07:53:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Jul 2011 14:35:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  724
 * SCR Title:  Add Resource Logging to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to create a method to get the number of availiable indexes and to manage logging when resources are consumed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Nov 2009 16:19:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated to have copy constructor and operator= methods available for general use.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Sep 2009 16:47:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved code from SVLibrary to SVDataManagerLibrary
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
