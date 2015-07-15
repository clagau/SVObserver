//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVList
//* .File Name       : $Workfile:   SVList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 07:44:12  $
//******************************************************************************

#ifndef SVLIST_H
#define SVLIST_H

#include <list>

/**
@SVObjectName Seidenader Vision Inc. List

@SVObjectOverview

@SVObjectOperations

*/
template<typename SVT_TYPE>
class SVList
{
public:
	typedef std::list< SVT_TYPE > SVListBase;
	typedef typename SVListBase::const_iterator const_iterator;
	typedef typename SVListBase::iterator iterator;
	typedef typename SVListBase::value_type SVType;
	typedef const SVType& SVArgType;

	SVList();
	SVList( const SVList& p_rObject );
	SVList( const SVListBase& p_rObject );

	virtual ~SVList();

	operator SVListBase const & () const;
	operator SVListBase & ();

	const SVList& operator=( const SVList& p_rObject );
	const SVList& operator=( const SVListBase& p_rObject );

	inline const_iterator begin() const { return m_List.begin(); }
	inline const_iterator end() const { return m_List.end(); }
	inline iterator begin() { return m_List.begin(); }
	inline iterator end() { return m_List.end(); }

	inline iterator erase( iterator p_Where ) { return m_List.erase( p_Where ); }

	// MFC Style Operators

	// Attributes (head and tail)
	// count of elements
	virtual INT_PTR GetCount() const;
	virtual BOOL IsEmpty() const;

	// add before head or after tail
	virtual iterator AddHead( SVArgType newElement );
	virtual iterator AddTail( SVArgType newElement );

	// add another list of elements before head or after tail
	virtual void AddTail( SVList* pNewList );

	// remove all elements
	virtual void RemoveAll();

	// getting/modifying an element at a given position
	virtual void SetAt( iterator pos, SVArgType newElement );
	virtual void RemoveAt( iterator& position );

	virtual iterator NotFound();

	// helper functions (note: O(n) speed)
	// defaults to starting at the HEAD, returns Position equal to NotFound() if not found
	virtual iterator Find( SVArgType searchValue ) const;
	virtual iterator FindIndex( int nIndex ) const;

	// iteration
	virtual iterator GetHeadPosition() const;
	virtual SVType& GetNext( iterator& rPosition ); // return *Position++
	virtual const SVType& GetNext( iterator& rPosition ) const; // return *Position++

	// getting/modifying an element at a given position
	virtual SVType& GetAt( iterator position );
	virtual const SVType& GetAt( iterator position ) const;

	/*
	// Attributes (head and tail)
	// count of elements
	virtual INT_PTR GetSize() const;

	// peek at head or tail
	virtual SVType& GetHead();
	virtual const SVType& GetHead() const;
	virtual SVType& GetTail();
	virtual const SVType& GetTail() const;

	// Operations
	// get head or tail (and remove it) - don't call on empty list !
	virtual SVType RemoveHead();
	virtual SVType RemoveTail();

	// add before head or after tail
	virtual iterator AddHead( SVArgType newElement );

	// add another list of elements before head or after tail
	virtual void AddHead( SVList* pNewList );
	virtual void AddTail( SVList* pNewList );

	// iteration
	virtual iterator GetTailPosition() const;
	virtual SVType& GetPrev( iterator& rPosition ); // return *Position--
	virtual const SVType& GetPrev( iterator& rPosition ) const; // return *Position--

	// inserting before or after a given position
	virtual iterator InsertBefore( iterator position, SVArgType newElement );
	virtual iterator InsertAfter( iterator position, SVArgType newElement );

	// helper functions (note: O(n) speed)
	// defaults to starting at the HEAD, returns Position equal to NotFound() if not found
	virtual iterator Find( SVArgType searchValue, iterator startAfter = NULL ) const;
	*/

protected:
	SVListBase m_List;

};

#include "SVList.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVContainerLibrary\SVList.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 07:44:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
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
 *    Rev 1.0   17 Nov 2008 07:21:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

