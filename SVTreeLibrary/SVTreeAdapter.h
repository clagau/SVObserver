//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTreeAdapter
//* .File Name       : $Workfile:   SVTreeAdapter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Jul 2014 16:54:28  $
//******************************************************************************

#pragma once

// Add $(ProgramFiles)\TCL_5.3.0 to include directories as part of VS Options for sequential_tree.h
#include <sequential_tree.h>

#include <utility>
#include <algorithm>
#include <functional>

#include "SVTreeIterator.h"
#include "SVTreeLibraryEnums.h"

/**
@SVObjectName Tree Adapter

@SVObjectOverview This file defines the types related to Tree Adapters and the interface to the Tree Adapter class.

@SVObjectOperations Available operations include a virtual destructor; multiple insert operations; assign, clear, set, and remove operations; multiple get operations; opertions to support interators; a pointer operator; and a parent function.

*/

template <typename ElementKey, typename ElementType, typename InvalidKey>
class SVTreeAdapter: private tcl::sequential_tree<std::pair<ElementKey, ElementType> >
{
public:
	typedef std::pair<ElementKey, ElementType> SVTreeElement;
	typedef tcl::sequential_tree<SVTreeElement> SVTreeContainer;
	typedef typename SVTreeContainer::iterator SVTree_iterator;
	typedef typename SVTreeContainer::const_iterator SVTree_const_iterator;
	typedef typename SVTreeContainer::pre_order_iterator SVTree_pre_order_iterator;
	typedef typename SVTreeContainer::const_pre_order_iterator SVTree_const_pre_order_iterator;
	typedef typename SVTreeContainer::post_order_iterator SVTree_post_order_iterator;
	typedef typename SVTreeContainer::const_post_order_iterator SVTree_const_post_order_iterator;
	typedef typename SVTreeContainer::level_order_iterator SVTree_level_order_iterator;
	typedef typename SVTreeContainer::const_level_order_iterator SVTree_const_level_order_iterator;
	typedef SVTreeIterator<ElementKey, ElementType, InvalidKey> sv_tree_iterator;
	typedef SVTreeConstIterator<ElementKey, ElementType, InvalidKey> sv_tree_const_iterator;
	typedef SVTreeContainer::container_type ChildContainer;

protected:
	SVTreeContainer& m_rTree;
	bool m_bUseUniqueKey;

public:
	virtual ~SVTreeAdapter();

	void Assign( SVTreeContainer& p_rTree, bool bUseUniqueKey = false );

	void Clear();

	virtual HRESULT Insert(const SVTreeElement& element, const ElementKey& parentKey, const ElementKey& insertionKey, SVTreeInsertionFlags::InsertionFlags flags=SVTreeInsertionFlags::InsertAtEnd);
	HRESULT Remove(const ElementKey& key);

	// for unique keys
	virtual HRESULT Set(const ElementKey& key, const ElementType& type);
	const SVTreeElement* Get(const ElementKey& key) const;

	const SVTreeElement* GetFirst() const;
	const SVTreeElement* GetNext(const ElementKey& key) const;
	const SVTreeElement* GetFirstChild(const ElementKey& parentKey) const;
	const SVTreeElement* GetNextChild(const ElementKey& parentKey, const ElementKey& key) const;
	const SVTreeElement* GetParent(const ElementKey& key) const;
	bool HasChildren(const ElementKey& key) const;

	// for non-unique keys
	virtual sv_tree_iterator insert(const ElementKey& key, const sv_tree_iterator& insertionIt, SVTreeInsertionFlags::InsertionFlags flags=SVTreeInsertionFlags::InsertAtEnd);
	virtual sv_tree_iterator insert(const SVTreeElement& element, const sv_tree_iterator& insertionIt, SVTreeInsertionFlags::InsertionFlags flags=SVTreeInsertionFlags::InsertAtEnd);
	virtual sv_tree_iterator insert(const SVTreeContainer& tree, const sv_tree_iterator& insertionIt, SVTreeInsertionFlags::InsertionFlags flags=SVTreeInsertionFlags::InsertAtEnd);

	bool empty() const;
	void swap(SVTreeContainer &);

	sv_tree_iterator begin();
	sv_tree_const_iterator begin() const;
	SVTree_pre_order_iterator pre_order_begin();
	SVTree_const_pre_order_iterator pre_order_begin() const;
	SVTree_post_order_iterator post_order_begin();
	SVTree_const_post_order_iterator post_order_begin() const;
	SVTree_level_order_iterator level_order_begin();
	SVTree_const_level_order_iterator level_order_begin() const;

	sv_tree_iterator end();
	sv_tree_const_iterator end() const;
	SVTree_pre_order_iterator pre_order_end();
	SVTree_const_pre_order_iterator pre_order_end() const;
	SVTree_post_order_iterator post_order_end();
	SVTree_const_post_order_iterator post_order_end() const;
	SVTree_level_order_iterator level_order_end();
	SVTree_const_level_order_iterator level_order_end() const;

	sv_tree_iterator erase(sv_tree_iterator& it);

	sv_tree_iterator find( SVTreeContainer* p_pSubTree );
	sv_tree_iterator find( const ElementKey& key, bool bRecursive=false );
	sv_tree_const_iterator find( const ElementKey& key, bool bRecursive=false ) const;

	operator SVTreeContainer * ();
	operator SVTreeContainer & ();
	operator SVTreeContainer const & () const;

	SVTreeElement* get();
	const SVTreeElement* get() const;
	void set( const SVTreeElement& p_rElement );

	SVTreeContainer* parent();
	const SVTreeContainer* parent() const;

	SVTreeContainer::size_type size() const { return m_rTree.size(); }

protected:
	explicit SVTreeAdapter( SVTreeContainer& p_rTree, bool bUseUniqueKey = false );

	bool HasChildren(SVTree_const_iterator& it) const;

	SVTree_const_pre_order_iterator FindNode(const ElementKey& key) const;
	SVTree_const_iterator FindNode(SVTree_const_pre_order_iterator& itParent, const ElementKey& key) const;
	SVTree_pre_order_iterator FindNode(const ElementKey& key);
	SVTree_iterator FindNode(SVTree_pre_order_iterator& itParent, const ElementKey& key);
	
	SVTreeContainer GetSubTreeContainer(const ElementKey& key) const;

};

#include "SVTreeAdapter.inl"

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVTreeLibrary\SVTreeAdapter.h_v  $
 * 
 *    Rev 1.1   17 Jul 2014 16:54:28   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added post and level iterators and const iterators
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 18:49:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Dec 2008 15:27:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to upgrade to TCL 5.3.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Nov 2008 07:24:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/