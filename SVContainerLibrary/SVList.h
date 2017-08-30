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

#pragma once

//Moved to precompiled header: #include <list>
//Moved to precompiled header: #include <iterator>

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

	~SVList();

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
	INT_PTR GetCount() const;
	bool IsEmpty() const;

	// add before head or after tail
	iterator AddHead( SVArgType newElement );
	iterator AddTail( SVArgType newElement );

	// add another list of elements before head or after tail
	void AddTail( SVList* pNewList );

	// remove all elements
	void RemoveAll();

	// getting/modifying an element at a given position
	void SetAt( iterator pos, SVArgType newElement );
	void RemoveAt( iterator& position );

	iterator NotFound();

	// helper functions (note: O(n) speed)
	// defaults to starting at the HEAD, returns Position equal to NotFound() if not found
	iterator Find( SVArgType searchValue ) const;
	iterator FindIndex( int nIndex ) const;

	// iteration
	iterator GetHeadPosition() const;
	SVType& GetNext( iterator& rPosition ); // return *Position++
	const SVType& GetNext( iterator& rPosition ) const; // return *Position++

	// getting/modifying an element at a given position
	SVType& GetAt( iterator position );
	const SVType& GetAt( const_iterator position ) const;

protected:
	SVListBase m_List;

};

#include "SVList.inl"

