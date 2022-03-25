//******************************************************************************
//* COPYRIGHT (c) 2011 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRingBuffer
//* .File Name       : $Workfile:   SVRingBuffer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:44:16  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <vector>

struct SVElementDoNothing
{
	template< typename SVElement >
	void operator()( SVElement&  ) const
	{
	}

};

struct SVElementClear
{
	template< typename SVElement >
	void operator()( SVElement& p_rElement ) const
	{
		p_rElement.clear();
	}

};

/*
This template object is a fixed size lock free ring buffer.  The size of the buffer is set at construction and cannot be changed during its life.  The object cannot be copied. The PushTail method will return an invalid state when the buffer is full.  The GetHead and PopHead methods will return an invalid state when buffer is empty.
*/
template< typename SVElement, typename SVClearElement = SVElementDoNothing >
class SVRingBuffer
{
public:
	SVRingBuffer( size_t p_Capacity );

	virtual ~SVRingBuffer();

	bool empty() const;
	void clear();

	size_t size() const;
	size_t capacity() const;

	HRESULT GetHead( SVElement& p_rElement ) const;

	HRESULT PopHead();
	HRESULT PopHead( SVElement& p_rElement );

	HRESULT PushTail( const SVElement& p_rElement );

protected:
	struct SVRingElement
	{
		SVRingElement();
		SVRingElement( const SVRingElement& p_rObject );

		long m_Index;
		SVElement m_Element;
	};

	typedef std::vector< SVRingElement > SVBufferVector;

	HRESULT RemoveHead();

	long distance( long p_rStart, long p_rEnd ) const;

	long m_Active;

	long m_Head;
	long m_PopHead;
	long m_SizeHead;

	long m_Tail;
	long m_PushTail;

	SVBufferVector m_Buffer;
	SVClearElement m_Clear;

private:
	SVRingBuffer();
	SVRingBuffer( const SVRingBuffer& p_rObject );
	const SVRingBuffer& operator=( const SVRingBuffer& p_rObject );

};

#include "SVRingBuffer.inl"

