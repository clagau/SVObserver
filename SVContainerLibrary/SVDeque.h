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

#pragma once

//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <BaseTsd.h>

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

