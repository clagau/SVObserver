// ******************************************************************************
// * COPYRIGHT (c) 2002 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	SVTQueueObject() = default;
	//This destructor calls the Destroy method and initializes the local attibutes.
	virtual ~SVTQueueObject() = default;

	//This method clears all objects from this container.
	void clear();
	//This method removes the head element from the queue.
	void pop_front();
	//This method places the supplied object on the end of the queue.
	void push_back(const T& p_rValue);

	void emplace_back(T&& Value);

	//This method adds a new element to the head of the queue. This method uses svnew instead of new. svnew uses a member variable m_Allocator that is derived from Loki::smallObject allocator.  This maintains a memory pool for each Queue object. The reason for this memory pool is to prevent contention between different tasks using new and delete.
	bool AddHead(const T& rValue);
	//This method adds a new element to the tail of the queue.  This method uses svnew instead of new. svnew uses a member variable m_Allocator that is derived from Loki::smallObject allocator.  This maintains a memory pool for each Queue object. The reason for this memory pool is to prevent contention between different tasks using new and delete.
	bool AddTail(const T&  rValue);

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
	std::mutex m_queueMutex;

	//This attribute represents the queue container for maintaining the queue data.
	SVQueueBase m_Queue;

};

//This object is the specialization of the queue object for void pointers.
typedef SVTQueueObject<void*> SVQueueObject;

#include "SVTQueueObject.inl"

