//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTreeIterator
//* .File Name       : $Workfile:   SVTreeIterator.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:50:24  $
//******************************************************************************

#ifndef INCL_SVTREEITERATOR_H
#define INCL_SVTREEITERATOR_H

// Add $(ProgramFiles)\TCL_5.3.0 to include directories as part of VS Options for sequential_tree.h
#include <sequential_tree.h>

#include <utility>

/**
@SVObjectName Tree Iterator

@SVObjectOverview This defines the types related to Tree Iteratation and the interface to the Tree Iterator class.

@SVObjectOperations Available operations include multiple constructors, comparison operators, pointer and smart pointer operators, increment and decrement operators, relationship operations, interator operations, and a find operation.

*/
template <typename ElementKey, typename ElementType, typename InvalidKey>
class SVTreeIterator
{
public:
	typedef std::pair<ElementKey, ElementType> SVTreeElement;
	typedef tcl::sequential_tree<SVTreeElement> SVTreeContainer;
	typedef typename SVTreeContainer::iterator SVTree_iterator;
	typedef typename SVTreeContainer::const_iterator SVTree_const_iterator;
	typedef typename SVTreeContainer::pre_order_iterator SVTree_pre_order_iterator;
	typedef typename SVTreeContainer::const_pre_order_iterator SVTree_const_pre_order_iterator;

	typedef SVTreeIterator<ElementKey, ElementType, InvalidKey> sv_tree_iterator;

	SVTreeIterator() : m_it() {}
	SVTreeIterator( const SVTree_iterator& it ) : m_it( it ) {}

	typedef typename SVTree_const_iterator::difference_type difference_type;
	typedef size_t size_type;

private:
	SVTree_iterator m_it;

public:
	// overloaded operators
	bool operator == (const SVTreeIterator& rhs ) const { return m_it == rhs.m_it; }
	bool operator != (const SVTreeIterator& rhs ) const { return !( *this == rhs ); }
	bool operator < (const SVTreeIterator& rhs) const { return m_it < rhs.m_it; }
	bool operator <= (const SVTreeIterator& rhs) const { return m_it <= rhs.m_it; }
	bool operator > (const SVTreeIterator& rhs) const { return m_it > rhs.m_it; }
	bool operator >= (const SVTreeIterator& rhs) const { return m_it >= rhs.m_it; }

	SVTreeElement& operator*() { return *m_it; }
	SVTreeElement* operator->() { return &( *m_it ); }

	const SVTreeElement& operator*() const { return *( SVTree_const_iterator( m_it ) ); }
	const SVTreeElement* operator->() const { return &( *( SVTree_const_iterator( m_it ) ) ); }
	
	sv_tree_iterator& operator ++() { ++m_it; return *this; }
	sv_tree_iterator operator ++(int) { SVTreeIterator old( *this ); ++*this; return old; }
	sv_tree_iterator& operator --() { --m_it; return *this; }
	sv_tree_iterator operator --(int) { SVTreeIterator old( *this ); --*this; return old; }
	sv_tree_iterator& operator +=(size_type n) { m_it += n; return *this; }
	sv_tree_iterator& operator -=(size_type n) { m_it -= n; return *this; }
	difference_type operator -(const SVTreeIterator& rhs) const { return m_it - rhs.m_it; }

	bool HasChildren() const { return ( m_it.node()->size() ) ? true : false; }

	SVTreeContainer* GetChildTree() { return m_it.node(); }
	const SVTreeContainer* GetChildTree() const { return m_it.node(); }
	SVTreeContainer* parent() { return m_it.node()->parent(); }
	const SVTreeContainer* parent() const { return m_it.node()->parent(); }

	operator SVTree_iterator() { return m_it; } 
	operator SVTree_const_iterator() const { return SVTree_const_iterator( m_it ); } 

	sv_tree_iterator find( const ElementKey& key ) 
	{
		SVTree_iterator l_Iter;
		SVTreeContainer* l_pTree( m_it.node() );

		if( l_pTree != NULL )
		{
			for( l_Iter = l_pTree->begin(); l_Iter != l_pTree->end(); ++l_Iter )
			{
				if( l_Iter->first == key )
				{
					break;
				}
			}
		}

		return sv_tree_iterator( l_Iter );
	}
};

/**
@SVObjectName Tree Const Iterator

@SVObjectOverview This defines the types related to Tree Const Iteratation and the interface to the Tree Const Iterator class.

@SVObjectOperations Available operations include multiple constructors, comparison operators, pointer and smart pointer operators, increment and decrement operators, relationship operations, and an interator operation.

*/
template <typename ElementKey, typename ElementType, typename InvalidKey>
class SVTreeConstIterator
{
public:
	typedef std::pair<ElementKey, ElementType> SVTreeElement;
	typedef tcl::sequential_tree<SVTreeElement> SVTreeContainer;
	typedef typename SVTreeContainer::iterator SVTree_iterator;
	typedef typename SVTreeContainer::const_iterator SVTree_const_iterator;
	typedef typename SVTreeContainer::pre_order_iterator SVTree_pre_order_iterator;
	typedef typename SVTreeContainer::const_pre_order_iterator SVTree_const_pre_order_iterator;

	typedef SVTreeConstIterator<ElementKey, ElementType, InvalidKey> sv_tree_const_iterator;

	SVTreeConstIterator() : m_it() {}
	SVTreeConstIterator( const SVTree_const_iterator& it ) : m_it( it ) {}

	typedef typename SVTree_const_iterator::difference_type difference_type;
	typedef size_t size_type;

private:
	SVTree_const_iterator m_it;

public:
	// overloaded operators
	bool operator == (const SVTreeConstIterator& rhs ) const { return m_it == rhs.m_it; }
	bool operator != (const SVTreeConstIterator& rhs ) const { return !( *this == rhs ); }
	bool operator < (const SVTreeConstIterator& rhs) const { return m_it < rhs.m_it; }
	bool operator <= (const SVTreeConstIterator& rhs) const { return m_it <= rhs.m_it; }
	bool operator > (const SVTreeConstIterator& rhs) const { return m_it > rhs.m_it; }
	bool operator >= (const SVTreeConstIterator& rhs) const { return m_it >= rhs.m_it; }
	
	const SVTreeElement& operator*() const { return *( m_it ); }
	const SVTreeElement* operator->() const { return &( *( m_it ) ); }

	sv_tree_const_iterator& operator ++() { ++m_it; return *this; }
	sv_tree_const_iterator operator ++(int) { SVTreeConstIterator old( *this ); ++*this; return old; }
	sv_tree_const_iterator& operator --() { --m_it; return *this; }
	sv_tree_const_iterator operator --(int) { SVTreeConstIterator old( *this ); --*this; return old; }
	sv_tree_const_iterator& operator +=(size_type n) { m_it += n; return *this; }
	sv_tree_const_iterator& operator -=(size_type n) { m_it -= n; return *this; }
	difference_type operator -(const SVTreeConstIterator& rhs) const { return m_it - rhs.m_it; }

	bool HasChildren() const { return ( m_it.node()->size() ) ? true : false; }

	const SVTreeContainer* GetContainer() const { return m_it._Mycont; }
	const SVTreeContainer* GetChildTree() const { return m_it.node(); }
	const SVTreeContainer* parent() const { return m_it.node()->parent(); }

	operator SVTree_const_iterator() const { return m_it; } 
};
#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTreeLibrary\SVTreeIterator.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:50:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/