// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : ManagedIndexArray.h
// * .File Name       : $Workfile:   ManagedIndexArray.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   07 May 2013 19:59:28  $
// ******************************************************************************

#ifndef _INC_MANAGEDINDEXARRAY_3C8E8C340067_INCLUDED
#define _INC_MANAGEDINDEXARRAY_3C8E8C340067_INCLUDED

#include <set>
#include <vector>
#include "SVSystemLibrary/SVCriticalSection.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVDataManagerGlobals.h"
#include "SVDataManagerEnums.h"
/*
This class is used to manage the reference counts associated with a particular index.
*/
class SVManagedIndex
{
public:
	SVManagedIndex();

	//	A TransactionId of 0 should mean unassigned.
	long  svmlTransactionId;

	SVString GetReferenceCounts() const;

	long GetReferenceCount() const;
	long GetReferenceCount( SVDataManagerLockTypeEnum p_eType ) const;

	HRESULT AddReference( SVDataManagerLockTypeEnum p_eType );
	HRESULT ReleaseReference( SVDataManagerLockTypeEnum p_eType );
	HRESULT ReleaseAllReferences();

protected:
	// Holder for acquisition reference count.
	long  svmlAcquisition;

	// Holder for inspection reference count.
	long  svmlInspection;

	// Holder for product position queue (PPQ) reference count.
	long  svmlPPQ;

	// Holder for archive reference count.
	long  svmlArchive;

	// Holder for display reference count.
	long  svmlDisplay;

	// Holder for last inspection reference count.
	long  svmlLastInspected;

	// Holder for DCOM reference count.
	long  svmlDCOM;

	// Holder for unknown reference count.
	long  svmlOther;

};

/*
This container is used to track the index usage of a particular managed element.
*/
class SVManagedIndexArray 
{
public:
	friend class SVDataManager;

	SVManagedIndexArray();
	
	virtual ~SVManagedIndexArray();
	 
	HRESULT RemoveAll();

	size_t GetSize() const;
	HRESULT SetSize( size_t p_Size );

	HRESULT GetAt( size_t p_Index, SVManagedIndex*& p_rpManagedIndex ) const;

	HRESULT ValidateIndex( size_t p_Index ) const;
	HRESULT ValidateIndex( size_t p_Index, long p_TransactionId ) const;
	HRESULT ReleaseAllIndexes();

	SVString GetReferenceCounts() const;

	long GetReferenceCount( size_t p_Index ) const;
	long GetReferenceCount( size_t p_Index, SVDataManagerLockTypeEnum p_eType ) const;

	HRESULT GetNextIndex( long& p_rIndex, long& p_rTransactionID, bool p_WaitForLock );
	HRESULT ClearIndex( size_t p_Index, long p_TransactionId );

	HRESULT AddReference( size_t p_Index, long p_TransactionId, SVDataManagerLockTypeEnum p_eType );
	HRESULT ReleaseReference( size_t p_Index, long p_TransactionId, SVDataManagerLockTypeEnum p_eType );

	size_t GetAvailableIndexCount() const;

	bool LogEventMessage();

	const SVString& GetName() const;
	void SetName( const SVString& p_rName );

	long GetMessageCounter() const;

	void Dump( const SVString& p_rSourceName ) const;

protected:
	typedef std::vector< SVManagedIndex* > SVManagedIndexPtrVector;

	HRESULT GetAtNoLock( size_t p_Index, SVManagedIndex*& p_rpManagedIndex ) const;

	HRESULT AddReferenceNoLock( size_t p_Index, long p_TransactionId, SVDataManagerLockTypeEnum p_eType );
	HRESULT ReleaseReferenceNoLock( size_t p_Index, long p_TransactionId, SVDataManagerLockTypeEnum p_eType );

	HRESULT ValidateIndexNoLock( size_t p_Index, long p_TransactionId ) const;

	// This is the name of the managed array
	SVString m_Name;

	// svmlTransactionCounter - this will assigned to each index when the index 
	// is first retrieved.  This value will be increamented for each retrieved 
	// index.  This counter be maintianed per array, and it will be acceptable 
	// for this to rollover. ----------------------------------------------------
	long m_TransactionCounter;

	long m_MessageCounter;

	// This is the size of the managed array.
	size_t m_ArrayDepth;

	long m_LastIndex;

	// This is the array of reference counting index elements.
	SVManagedIndexPtrVector m_IndexArray;

	// This attribute manages the lock for multi-threaded access to this container.
	mutable SVCriticalSection m_Lock;
	
	SVClock::SVTimeStamp m_LastMessageTimeStamp;

private:
	SVManagedIndexArray( const SVManagedIndexArray& p_rObject );

	const SVManagedIndexArray& operator=( const SVManagedIndexArray& p_rObject );

};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVDataManagerLibrary\ManagedIndexArray.h_v  $
 * 
 *    Rev 1.1   07 May 2013 19:59:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 012.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Apr 2013 12:27:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 07:53:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Sep 2011 14:10:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated Data Manager functionality to remove locking issues and resource issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Jul 2011 14:35:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  724
 * SCR Title:  Add Resource Logging to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to create a method to get the number of availiable indexes and to manage logging when resources are consumed.
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
 *    Rev 1.0   01 Sep 2009 16:47:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved code from SVLibrary to SVDataManagerLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 Jul 2009 12:34:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the methodology for managing the indexes using the data manager.  The new methodology uses the smart pointer to externally reference count the index usage and release the array handle when all indexes are released.  Atransaction id was added to prevent unintended index reuse.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Nov 2005 09:24:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   corrected check-in directory
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Nov 2005 09:24:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   corrected check-in directory
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   01 Nov 2005 14:35:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added svmlDisplay and svmlLastInspected
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   10 Jan 2003 13:31:48   cschmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Code was moved to the SVLibrary Project
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
