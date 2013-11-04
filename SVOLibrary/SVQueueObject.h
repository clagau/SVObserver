// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVQueueObject
// * .File Name       : $Workfile:   SVQueueObject.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 12:23:08  $
// ******************************************************************************

#ifndef _INC_SVQUEUEOBJECT_INCLUDED
#define _INC_SVQUEUEOBJECT_INCLUDED

#pragma warning( disable: 4786 )	// identifier truncation 255 chars

#include <comdef.h>
#include <deque>

#include "SVUtilityLibrary/SVString.h"
#include "SVMultithread.h"

typedef long QUEUEPOS;

/*
This template creates a generic multi-threaded queue.
*/
template <typename T>
class SVTQueueObject
{
public:
	typedef std::deque< T > SVQueueBase;

	//This constructor initializes the local attibutes.
	SVTQueueObject();
	//This destructor calls the Destroy method and initializes the local attibutes.
	virtual ~SVTQueueObject();

	//This method clears all objects from this container.
	void clear();
	//This method removes the head element from the queue.
	void pop_front();
	//This method places the supplied object on the end of the queue.
	void push_back( const T& p_rValue );

	//This method creates the internal objects to allow the queue to operate.
	BOOL Create();
	//This method destroys the internal objects.
	BOOL Destroy();
	//This method returns the creation state of the object.
	BOOL IsCreated() const;

	//This method locks the internal lock object.
	BOOL Lock();
	//This method unlocks the internal lock object.
	BOOL Unlock();

	//This method adds a new element to the head of the queue. This method uses svnew instead of new. svnew uses a member variable m_Allocator that is derived from Loki::smallObject allocator.  This maintains a memory pool for each Queue object. The reason for this memory pool is to prevent contention between different tasks using new and delete.
	BOOL AddHead( T pParam );
	//This method adds a new element to the tail of the queue.  This method uses svnew instead of new. svnew uses a member variable m_Allocator that is derived from Loki::smallObject allocator.  This maintains a memory pool for each Queue object. The reason for this memory pool is to prevent contention between different tasks using new and delete.
	BOOL AddTail( T pParam );

	// Get information about the Queue
	BOOL IsEmpty() { return m_Queue.empty(); };
	//This method returns the size of the queue.
	BOOL GetSize( long& lSize );
	//This method returns the head element of the queue.
	BOOL GetHead( T* pParam );
	//This method returns the tail element of the queue.
	BOOL GetTail( T* pParam );

	//This method receives the object at the supplied location in the queue into the provided object pointer.
	BOOL GetAt( long lPosition, T* pParam );

	//This method returns the iterator to the head element of the queue.
	BOOL GetHeadPosition( QUEUEPOS& pos );
	//This method returns the element the iterator is pointing to and moves the iterator to the next element of the queue.
	BOOL GetNextItem( QUEUEPOS& pos, T* pParam );

	//This method returns the element at the head of the queue and remove the element from the queue.
	BOOL RemoveHead( T* pParam );
	//This method removes the object at the provided position in the queue.
	BOOL RemoveAt( long lPosition );
	//This method removes all element in the queue.
	BOOL RemoveAll();

	//This method returns the size of the queue.
	int GetCount();

private:
	//This attribute holds the creation state of this object.
	BOOL m_bCreated;

	//This attribute manages the multi-threaded lock.
	SVCriticalSectionAdapter m_critsec;
	//This attribute is the multi-threaded lock.
	CRITICAL_SECTION    m_hCriticalSection;

	//This attribute represents the queue container for maintaining the queue data.
	SVQueueBase m_Queue;

};

//This object is the specialization of the queue object for void pointers.
typedef SVTQueueObject<void*> SVQueueObject;

//This object is the specialization for the queue lock.
typedef SVSingleLockT<SVQueueObject> SVQueueObjectLock;

//#define SV_LOG_STATUS_INFO
#ifdef SV_LOG_STATUS_INFO
	typedef SVTQueueObject< SVString > SVStatusDeque;
#endif

#include "SVTQueueObject.inl"

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLibrary\SVQueueObject.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 12:23:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   14 Jul 2011 08:33:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated queue container to include a pop head method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   15 Feb 2010 10:58:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   19 Jan 2010 14:55:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   22 Sep 2009 17:56:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added two new methods to get and remove elements at indexes in the queue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   14 Jul 2009 10:40:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the code to handle buffer transfer and signal handling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Jan 2006 09:16:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added crit sec locking object
 * modified RemoveHead to compile with T not being a pointer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Oct 2005 12:06:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new method to check if the queue is empty.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Oct 2005 09:18:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change SVTQueue to use smallObj to allocate memory instead of using new and delete because they cause a lock on the system.  The SVTQueue class has a member variable that allocates memory from a pool that is owned by the queue object.  The initial size of this pool is set to 4096.  This may need changed in the future depending on how much is wasted.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Aug 2003 13:09:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Template-ized SVQueueClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 10:17:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
