// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDataManager
// * .File Name       : $Workfile:   DataManager.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 07:53:36  $
// ******************************************************************************

#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "ManagedIndexArray.h"
#include "ManagedIndexArrayList.h"
#include "SVDataManagerIndexArrayHandle.h"
#include "SVDataManagerEnums.h"

class ManagedIndexArray;

/*
This object manages object element resources in the system.
*/
class SVDataManager 
{
public:
	friend class SVDataManagerHandle;
	friend class SVDataManagerIndexArrayHandle;
	friend class SVImageCompressionClass;
	friend class StaticImageCompressionPool;
	
	SVDataManager();

	virtual ~SVDataManager();
	
	HRESULT CreateManagedIndexArray( SVSmartIndexArrayHandlePtr& p_pArrayHandle, BSTR p_Name, long p_Depth );
	
	HRESULT GetNextAvailableBufferIndex( const SVSmartIndexArrayHandlePtr& p_pArrayHandle, 
		SVDataManagerLockTypeEnum aLockType, SVDataManagerHandle& p_rHandle );
	
	HRESULT GetNextAvailableBufferIndexNoWait( const SVSmartIndexArrayHandlePtr& p_pArrayHandle, 
		SVDataManagerLockTypeEnum aLockType, SVDataManagerHandle& p_rHandle );

	void Dump_All();
	
protected:
	HRESULT  ReleaseAllIndexes( long alIndexArrayHandle );
	
	HRESULT  CreateManagedIndexArray( BSTR aIndexArrayName, long alIndexArrayDepth, long* alpIndexArrayHandle );
	
	HRESULT  GetNextAvailableBufferIndex( long alIndexArrayHandle, 
		SVDataManagerLockTypeEnum aLockType, bool p_WaitForLock,
		long* alBufferIndex, long* alTransactionId );
	
	HRESULT  LockBufferIndex( long alIndexArrayHandle, long alBufferIndex,
		long alTransactionId, SVDataManagerLockTypeEnum aLockType);
	
	HRESULT  LockBufferIndexNoLock( long alIndexArrayHandle, long alBufferIndex,
		long alTransactionId, SVDataManagerLockTypeEnum aLockType);
	
	HRESULT  ReleaseBufferIndex( long alIndexArrayHandle, long alBufferIndex,
		long alTransactionId, SVDataManagerLockTypeEnum aLockType);
	
	HRESULT  ReleaseBufferIndexNoLock( long alIndexArrayHandle, long alBufferIndex,
		long alTransactionId, SVDataManagerLockTypeEnum aLockType);
	
	HRESULT  DestroyIndexArray( long alIndexArrayHandle );
	
	HRESULT  StringCopy( TCHAR* apszDestString, const TCHAR* apszSourceString, long alNbrOfBytes );
	
	HRESULT  ValidateIndexArrayHandle( long alIndexArrayHandle ) const;
	
	HRESULT  ValidateLockType( long alLockType );
	
	HRESULT  ValidateBufferIndex( long alIndexArrayHandle, long alBufferIndex );
	
	HRESULT	GetNbrOfAvailableIndexes( long alIndexArrayHandle, long* alpNbrOfAvailableIndexes );
	
	void Dump( long alIndexArrayHandle, LPCSTR p_szSource ) const;
		
	HRESULT  ValidateBufferIndex( long alIndexArrayHandle, long alBufferIndex, long alTransactionId );

	HRESULT  ValidateBufferIndexNoLock( long alIndexArrayHandle, long alBufferIndex, long alTransactionId );
	
	long GetIndexLockCountByType( long p_ArrayHandle, long p_Index, SVDataManagerLockTypeEnum p_LockType );
	
	long GetLockCountByType( long p_ArrayHandle, SVDataManagerLockTypeEnum p_LockType );
	
	// This element holds the array of managed index containers.
	SVManagedIndexArrayList svmManagedIndexArrayList;
	
	// This references the first position in the svmManagedIndexArrayList which have been destroyed.
	long svmlFirstUnusedIndex;

	// This references the last position in the svmManagedIndexArrayList which have been destroyed.
	long svmlLastUnusedIndex;
	
	BOOL Lock( unsigned long p_Timeout ) const;
	BOOL Unlock() const;

	bool m_IsLockCreated;

	// This attribute manages the lock for multi-threaded access to this container.
	mutable CRITICAL_SECTION m_Lock;
	
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVDataManagerLibrary\DataManager.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 07:53:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   14 Jul 2011 08:01:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  729
 * SCR Title:  Adjust SVObserver to fix perfomance problems due to logging
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to remove logging and add a method to get lock count by type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 May 2010 13:16:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include a no lock path when locking is not required due to owner lock.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Feb 2010 09:07:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Sep 2009 16:47:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved code from SVLibrary to SVDataManagerLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   13 Jul 2009 12:34:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the methodology for managing the indexes using the data manager.  The new methodology uses the smart pointer to externally reference count the index usage and release the array handle when all indexes are released.  Atransaction id was added to prevent unintended index reuse.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Nov 2005 14:36:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SV_DISPLAY and SV_LAST_INSPECTED
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Feb 2005 12:37:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added DataManger member
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Jan 2003 13:10:38   cschmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Code was moved  to SVLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Dec 2002 17:03:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  304
 * SCR Title:  Fix crash after deleting a PPQ and its attached inspection(s)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added DumpAllLocks method to SVDataManager class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
