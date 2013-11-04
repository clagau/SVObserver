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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVDataManagerLibrary\SVDataManagerHandle.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 07:53:32   bWalter
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
 *    Rev 1.0   01 Sep 2009 16:47:34   jspila
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
