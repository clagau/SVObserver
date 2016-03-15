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

