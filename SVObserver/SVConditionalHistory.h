//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVConditionalHistory
//* .File Name       : $Workfile:   SVConditionalHistory.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:02:06  $
//******************************************************************************

#ifndef SVCONDITIONALHISTORY_H
#define SVCONDITIONALHISTORY_H

#include "SVObjectAppClass.h"
#include "SVScalarValue.h"
#include "SVImageBufferStruct.h"
#include "SVOLibrary/SVContainableCriticalSection.h"
#include "SVValueObjectImpl.h"
#include "SVLibrary/SVSimpleIndexQueue.h"
#include "SVImageBuffer.h"	// SVImageOverlayClass

extern LPCTSTR const PPQCH_Size;
extern LPCTSTR const PPQCH_Overwrite;

//!!!!!!
typedef SVReferenceCountingTemplate < SVImageOverlayClass > SVRefCountImageOverlayClass;
typedef std::vector <SVRefCountImageOverlayClass> SVRefCountImageOverlayStructVectorType;


struct SVConditionalHistoryEntry	// all the data for one product
{
	std::vector <SVScalarValue> vecValues;
	SVRefCountImageBufferStructVectorType vecImages;
	std::vector <SVScalarValue> vecConditionals;
	long lProcessCount;

	SVConditionalHistoryEntry() : lProcessCount(-1) {}
};

class SVInspectionProcess;

struct SVConditionalHistoryData
{
	// the actual history
	std::vector<SVConditionalHistoryEntry> vecHistory;

	// the history input specification
	std::vector <SVScalarValue>        vecConditionals;// list of conditional objects
	std::vector <SVScalarValue>        vecValues;      // list of values to store
	std::vector <SVImageBufferStruct>  vecImages;      // list of images (no buffers) and their infos 

	SVBoolValueObjectClass bvoOverwrite;
	SVLongValueObjectClass lvoSize;

	void swap( SVConditionalHistoryData& rhs );
};

class SVConditionalHistory : public SVObjectClass
{
public:
	SVConditionalHistory();

	HRESULT Validate( std::vector <SVScalarValue>& rvecValues, std::vector <SVScalarValue>& rvecImages, std::vector <SVScalarValue>& rvecConditionals );
	virtual HRESULT ResetObject();

	HRESULT SetProperties( SVScalarValueVector& rvecProperties, bool bResetObject = true );
	HRESULT GetProperties( SVScalarValueVector& rvecProperties );
	HRESULT SetList( std::vector <SVScalarValue>* pvecValues, std::vector <SVScalarValue>* pvecImages, std::vector <SVScalarValue>* pvecConditionals, bool bResetObject = true );	// vectors not const because we return status info
	HRESULT GetList( std::vector <SVScalarValue>& rvecValues, std::vector <SVScalarValue>& rvecImages, std::vector <SVScalarValue>& rvecConditionals );
	HRESULT GetHistoryAndClear( std::vector < std::vector <SVScalarValue> >& rvecValues, std::vector < std::vector <SVImageBufferStruct> >& rvecImages, std::vector < std::vector <SVScalarValue> >& rvecConditionals, std::vector<long>& rvecProcessCount );
	HRESULT GetMostRecentHistory( std::vector <SVScalarValue>& rvecValues, std::vector <SVImageBufferStruct>& rvecImages, std::vector <SVScalarValue>& rvecConditionals, long& rlProcessCount );

	HRESULT CheckConditionals() const;
	HRESULT CollectDataAndStore();
	HRESULT DeleteTool( LPCTSTR p_strToolName );

	void SetInspection( SVInspectionProcess* pInspection );
private:
	std::pair<SVObjectReference, SVInspectionProcess*> ParseName( const CString& strName );
	HRESULT Validate( std::vector <SVScalarValue>& rvec );
	HRESULT GetHistoryEntry( long lIndex, std::vector <SVScalarValue>& rvecValues, std::vector <SVImageBufferStruct>& rvecImages, std::vector <SVScalarValue>& rvecConditionals, long& rlProcessCount );

	SVInspectionProcess*            m_pInspection;
	//SVContainableCriticalSection  m_CritSec;//!!! UNNECESSARY NOW THAT CH IS TOTALLY CONTAINED WITHIN THE INSPECTION THREAD
	SVEmptySyncObject               m_CritSec;// does no locking; a placeholder so we can maintain the locking logic

	// the following serve as holders for the inputs between a Set and a ResetObject:
	std::vector <SVScalarValue>        m_vecConditionals;// list of conditional objects
	std::vector <SVScalarValue>        m_vecValues;      // list of values to store
	std::vector <SVImageBufferStruct>  m_vecImages;      // list of images and their infos 
	SVBoolValueObjectClass             m_bvoOverwrite;
	SVLongValueObjectClass             m_lvoSize;

	bool m_bClearQueue;	// a flag instructing the ResetObject to clear the queue (changing value or image inputs)

	SVConditionalHistoryData   m_Data;	// the actual history data used at runtime

	SVSimpleIndexQueue m_Queue;


	//SVTQueueObject<SVConditionalHistoryEntry*> m_Queue;
	// no need for locking (like SVTQueueObject) because all access
	// will be within m_CritSec
	//std::deque<SVConditionalHistoryEntry*> m_Queue;

						// but will deque do allocation of the chunks? can we preallocate?
	                    // we can pass in our own allocator, but how will that interact with freeing the memory?
	                    // we will make copies on the way out, but we need to allocate the vector to put them in
	                    // (do that outside the critsec)

						// preallocate everything on the Reset and use the Data Manager to get the next available
						// slot.   -  overkill

						
						// queue swap on rebuild


};



// SIAC function needs to have a crit sec?




#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVConditionalHistory.h_v  $
 * 
 *    Rev 1.1   08 May 2013 16:02:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Apr 2013 09:29:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  831
 * SCR Title:  Fix Conditional History crash when tools are deleted
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added DeleteTool function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:00:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Dec 2011 15:40:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the evaluation process of conditional history.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   28 Oct 2010 13:39:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Dec 2009 15:13:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   19 Jan 2006 08:54:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Process Count to Conditional History
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jan 2006 09:19:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first revision
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
