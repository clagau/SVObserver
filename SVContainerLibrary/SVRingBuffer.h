//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRingBuffer
//* .File Name       : $Workfile:   SVRingBuffer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:44:16  $
//******************************************************************************

#ifndef SVRINGBUFFER_H
#define SVRINGBUFFER_H

#include <vector>

struct SVElementDoNothing
{
	template< typename SVElement >
	void operator()( SVElement& p_rElement ) const
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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVContainerLibrary\SVRingBuffer.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 07:44:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Oct 2011 14:13:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upated Ring Buffer to handle clearing popped elements based on a supplied stratagy.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 May 2011 15:32:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new Ring Buffer container to library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

