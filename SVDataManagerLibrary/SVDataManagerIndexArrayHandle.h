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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVDataManagerLibrary\SVDataManagerIndexArrayHandle.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 07:54:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Jul 2011 08:01:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  729
 * SCR Title:  Adjust SVObserver to fix perfomance problems due to logging
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to remove logging and add a method to get lock count by type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Sep 2009 16:47:40   jspila
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
