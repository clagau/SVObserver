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

#pragma once

#pragma warning( disable: 4786 )	// identifier truncation 255 chars

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <deque>


#include "SVMultithread.h"
#pragma endregion Includes

typedef long QUEUEPOS;

/*
This template creates a generic multi-threaded queue.
*/
template <typename T>
class SVTQueueObject
{
public:
	typedef std::deque<typename T> SVQueueBase;

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
	bool Create();
	//This method destroys the internal objects.
	void Destroy();
	//This method returns the creation state of the object.
	bool IsCreated() const;

	//This method locks the internal lock object.
	bool Lock();
	//This method unlocks the internal lock object.
	bool Unlock();

	//This method adds a new element to the head of the queue. This method uses svnew instead of new. svnew uses a member variable m_Allocator that is derived from Loki::smallObject allocator.  This maintains a memory pool for each Queue object. The reason for this memory pool is to prevent contention between different tasks using new and delete.
	bool AddHead( T pParam );
	//This method adds a new element to the tail of the queue.  This method uses svnew instead of new. svnew uses a member variable m_Allocator that is derived from Loki::smallObject allocator.  This maintains a memory pool for each Queue object. The reason for this memory pool is to prevent contention between different tasks using new and delete.
	bool AddTail( T pParam );

	// Get information about the Queue
	bool IsEmpty() { return m_Queue.empty(); };
	//This method returns the size of the queue.
	bool GetSize( long& lSize );
	//This method returns the head element of the queue.
	bool GetHead( T* pParam );
	//This method returns the tail element of the queue.
	bool GetTail( T* pParam );

	//This method receives the object at the supplied location in the queue into the provided object pointer.
	bool GetAt( long lPosition, T* pParam );

	//This method returns the iterator to the head element of the queue.
	bool GetHeadPosition( QUEUEPOS& pos );
	//This method returns the element the iterator is pointing to and moves the iterator to the next element of the queue.
	bool GetNextItem( QUEUEPOS& pos, T* pParam );

	//This method returns the element at the head of the queue and remove the element from the queue.
	bool RemoveHead( T* pParam );
	//This method removes the object at the provided position in the queue.
	bool RemoveAt( long lPosition );
	//This method removes all element in the queue.
	bool RemoveAll();

	//This method returns the size of the queue.
	int GetCount();

private:
	//This attribute holds the creation state of this object.
	bool m_bCreated;

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

#include "SVTQueueObject.inl"

