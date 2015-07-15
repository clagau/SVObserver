//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDeque
//* .File Name       : $Workfile:   SVDeque.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:44:08  $
//******************************************************************************

#ifndef SVDEQUE_H
#define SVDEQUE_H

#include <deque>
#include <BaseTsd.h>

/**
@SVObjectName Seidenader Vision Inc. List

@SVObjectOverview This file defines the interface for the SVDeque class.

@SVObjectOperations This file defines the types SVDequeBase, interator, SVType, and SVArgType.  Available operations include multiple constructors, a virtual destructor, multiple address of operators, multiple assignment operators, an empty function, and a push_back function.

*/
template<typename SVT_TYPE>
class SVDeque
{
public:
	typedef std::deque< SVT_TYPE > SVDequeBase;
	typedef typename SVDequeBase::size_type size_type;
	typedef typename SVDequeBase::iterator iterator;
	typedef typename SVDequeBase::const_iterator const_iterator;
	typedef typename SVDequeBase::value_type SVType;
	typedef const SVType& SVArgType;

	SVDeque();
	SVDeque( const SVDeque& p_rObject );
	SVDeque( const SVDequeBase& p_rObject );

	virtual ~SVDeque();

	operator SVDequeBase const & () const;
	operator SVDequeBase & ();

	const SVDeque& operator=( const SVDeque& p_rObject );
	const SVDeque& operator=( const SVDequeBase& p_rObject );

	inline const_iterator begin() const { return m_Deque.begin(); }
	inline const_iterator end() const { return m_Deque.end(); }
	inline iterator begin() { return m_Deque.begin(); }
	inline iterator end() { return m_Deque.end(); }

	bool empty() const;
	void clear();

	iterator erase( iterator p_Where ) { return m_Deque.erase( p_Where ); }

	void push_back( const SVType& p_Val );

	iterator insert( iterator p_Where, const SVType& p_rVal );

	size_t size() const { return m_Deque.size(); }

	SVType& operator[](size_t index) { return m_Deque[index]; }
	const SVType& operator[](size_t index) const { return m_Deque[index]; }

	// MFC Style Operators
	// count of elements
	INT_PTR GetCount() const;
	INT_PTR GetSize() const;
	INT_PTR GetUpperBound() const;

	INT_PTR Add(const SVType& newElement);
	void SetAt(INT_PTR nIndex, const SVType& newElement);

	// getting/modifying an element at a given position
	SVType& GetAt( iterator position );
	const SVType& GetAt(INT_PTR nIndex) const;

	void RemoveAll();

protected:
	SVDequeBase m_Deque;

};

#include "SVDeque.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVContainerLibrary\SVDeque.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 07:44:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Mar 2012 15:41:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  756
 * SCR Title:  Fix Variant Array Object Script Issue
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added new functions to act like CStringArray.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Dec 2010 15:35:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Nov 2010 11:49:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to add missing type identifiers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Nov 2008 07:20:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

