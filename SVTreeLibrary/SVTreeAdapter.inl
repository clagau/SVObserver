//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTreeAdapter
//* .File Name       : $Workfile:   SVTreeAdapter.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Jul 2014 16:54:28  $
//******************************************************************************

#include "SVCompareKeys.h"

///////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTreeAdapter( SVTreeContainer& p_rTree, bool bUseUniqueKey )
: m_rTree( p_rTree ), m_bUseUniqueKey(bUseUniqueKey) 
{
}

///////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::~SVTreeAdapter() 
{ 
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
void SVTreeAdapter<ElementKey, ElementType, InvalidKey>::Assign( SVTreeContainer& p_rTree, bool bUseUniqueKey = false )
{
	m_rTree = p_rTree;
	m_bUseUniqueKey = bUseUniqueKey;
}

///////////////////////////////////////////////////////////////////////
// Clear the tree
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
void SVTreeAdapter<ElementKey, ElementType, InvalidKey>::Clear() 
{ 
	m_rTree.clear(); 
}

///////////////////////////////////////////////////////////////////////
// Insert a Node (parent or child)
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
HRESULT SVTreeAdapter<ElementKey, ElementType, InvalidKey>::Insert(const SVTreeElement& element, const ElementKey& parentKey, const ElementKey& insertionKey, SVTreeInsertionFlags::InsertionFlags flags)
{
	HRESULT hr = S_FALSE;

	// ensure Key doesn't exist
	if (m_bUseUniqueKey)
	{
		SVTree_pre_order_iterator itNode = FindNode(element.first);
		if (itNode != m_rTree.pre_order_end())
			return S_FALSE;
	}
	
	// How to specify the Invalid state?
	if (parentKey != InvalidKey())
	{
		SVTree_pre_order_iterator itParent = FindNode(parentKey);
		if (itParent != m_rTree.pre_order_end())
		{
 			if (insertionKey != InvalidKey())
			{
				if (flags == SVTreeInsertionFlags::InsertBefore)
				{
					// insert before Node (Sibling)
					SVTree_iterator it = FindNode(itParent, insertionKey);
					
					// if sibling found
					if (it != m_rTree.end())
					{
						itParent.node()->insert(it, element);
						hr = S_OK;
					}
				}
				else if (flags == SVTreeInsertionFlags::InsertAfter)
				{
					// insert after Node (Sibling)
					SVTree_iterator it = FindNode(itParent, insertionKey);
					itParent.node()->insert(++it, element);
					hr = S_OK;
				}
			}
			else // insert at the end of the sibling trunk
			{
				itParent.node()->insert(element);
				hr = S_OK;
			}
		}
		else // Parent Node not found
		{
			// error
		}
	}
	else
	{
		if (insertionKey != InvalidKey())
		{
			// insert after Node (Sibling)
			SVTree_pre_order_iterator it = FindNode(insertionKey);
			it.node()->insert(element);
			hr = S_OK;
		}
		else // insert at the end of the root trunk
		{
			m_rTree.insert(element);
			hr = S_OK;
		}
	}
	return hr;
}

///////////////////////////////////////////////////////////////////////
// Remove the Node
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
HRESULT SVTreeAdapter<ElementKey, ElementType, InvalidKey>::Remove(const ElementKey& key)
{
	HRESULT hr = S_FALSE;

	// find the node to remove
	// removing the node will remove the children automatically

	SVTree_pre_order_iterator it = FindNode(key);
	if (it != m_rTree.pre_order_end())
	{
		// back up to the parent
		SVTreeContainer* pTree = it.node()->parent();
		
		// silly logic cause pre_order_iterator has no conversion to iterator
		SVTree_iterator pIt;
		for (pIt = pTree->begin();pIt != pTree->end();++pIt)
		{
			if (pIt->first == key)
			{
				pTree->erase(pIt);
				hr = S_OK;
				break;
			}
		}
	}
	return hr;
}

///////////////////////////////////////////////////////////////////////
// Get the Node
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
const std::pair<ElementKey, ElementType>* SVTreeAdapter<ElementKey, ElementType, InvalidKey>::Get(const ElementKey& key) const
{
	SVTree_const_pre_order_iterator it = FindNode(key);
	if (it != m_rTree.pre_order_end())
		return it.node()->get();

	return NULL;
}

///////////////////////////////////////////////////////////////////////
// Set the Eelment Type based on the key
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
HRESULT SVTreeAdapter<ElementKey, ElementType, InvalidKey>::Set(const ElementKey& key, const ElementType& type)
{
	HRESULT hr = S_FALSE;

	SVTree_pre_order_iterator it = FindNode(key);
	if (it != m_rTree.pre_order_end())
	{
		SVTreeElement element(key, type);
		it.node()->set(element);
		hr = S_OK;
	}
	return hr;
}

///////////////////////////////////////////////////////////////////////
// Get the first Node
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
const std::pair<ElementKey, ElementType>* SVTreeAdapter<ElementKey, ElementType, InvalidKey>::GetFirst() const
{
	SVTree_const_iterator it = m_rTree.begin();
	if (it != m_rTree.end())
		return it.node()->get();

	return NULL;
}

///////////////////////////////////////////////////////////////////////
// Get the next Node
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
const std::pair<ElementKey, ElementType>* SVTreeAdapter<ElementKey, ElementType, InvalidKey>::GetNext(const ElementKey& key) const
{
	SVTree_const_iterator it;
	it = std::find_if(m_rTree.begin(), m_rTree.end(), SVCompareKeys<ElementKey, ElementType>(key));
	if (it != m_rTree.end())
	{
		++it;
		if (it != m_rTree.end())
			return it.node()->get();
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////
// Get the first Child Node of a Parent Node
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
const std::pair<ElementKey, ElementType>* SVTreeAdapter<ElementKey, ElementType, InvalidKey>::GetFirstChild(const ElementKey& parentKey) const
{
	SVTree_const_pre_order_iterator itParent = FindNode(parentKey);
	if (itParent != m_rTree.pre_order_end())
	{
		SVTree_const_iterator it = itParent.node()->begin();
		{
			if (it != m_rTree.end())
				return it.node()->get();
				//return *(it.node()->get());
		}
	}
	//return SVObjectReferencePtr(InvalidKey);
	return NULL;
}

///////////////////////////////////////////////////////////////////////
// Get the next Child Node of a Parent Node
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
const std::pair<ElementKey, ElementType>* SVTreeAdapter<ElementKey, ElementType, InvalidKey>::GetNextChild(const ElementKey& parentKey, const ElementKey& key) const
{
	SVTree_const_pre_order_iterator itParent = FindNode(parentKey);
	if (itParent != m_rTree.pre_order_end())
	{
		const SVTreeContainer* pParent = itParent.node();
		SVTree_const_iterator itChild = FindNode(itParent, key);
		if (itChild != pParent->end())
		{
			++itChild;
			if (itChild != pParent->end())
				return itChild.node()->get();
		}
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////
// Check if a Node has Children
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
const std::pair<ElementKey, ElementType>* SVTreeAdapter<ElementKey, ElementType, InvalidKey>::GetParent(const ElementKey& key) const
{
	SVTree_const_pre_order_iterator it = FindNode(key);
	if (it != m_rTree.pre_order_end())
	{
		if (it.node()->parent())
			return it.node()->parent()->get();
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////
// Check if a Node has Children
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
bool SVTreeAdapter<ElementKey, ElementType, InvalidKey>::HasChildren(const ElementKey& key) const
{
	bool bHasChildren = false;
	SVTree_const_pre_order_iterator it = FindNode(key);
	if (it != m_rTree.pre_order_end())
	{
		bHasChildren = (it.node()->size()) ? true : false;
	}
	return bHasChildren;
}

///////////////////////////////////////////////////////////////////////
// Find a Node
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTree_const_pre_order_iterator
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::FindNode(const ElementKey& key) const
{
	SVTree_const_pre_order_iterator it;
	it = std::find_if(m_rTree.pre_order_begin(), m_rTree.pre_order_end(), SVCompareKeys<ElementKey, ElementType>(key));
	return it;
}

///////////////////////////////////////////////////////////////////////
// Find a Child Node of a Parent
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTree_const_iterator
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::FindNode(SVTree_const_pre_order_iterator& itParent, const ElementKey& key) const
{
	SVTree_const_iterator it;
	it = std::find_if(itParent.node()->begin(), itParent.node()->end(), SVCompareKeys<ElementKey, ElementType>(key));
	return it;
}

///////////////////////////////////////////////////////////////////////
// Find a Node
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTree_pre_order_iterator
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::FindNode(const ElementKey& key)
{
	SVTree_pre_order_iterator it;
	it = std::find_if(m_rTree.pre_order_begin(), m_rTree.pre_order_end(), SVCompareKeys<ElementKey, ElementType>(key));
	return it;
}

///////////////////////////////////////////////////////////////////////
// Find a Child Node of a Parent
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTree_iterator
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::FindNode(SVTree_pre_order_iterator& itParent, const ElementKey& key)
{
	SVTree_iterator it;
	it = std::find_if(itParent.node()->begin(), itParent.node()->end(), SVCompareKeys<ElementKey, ElementType>(key));
	return it;
}

///////////////////////////////////////////////////////////////////////
// Get the SubTree
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTreeContainer 
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::GetSubTreeContainer(const ElementKey& key) const
{
	SVTree_const_pre_order_iterator it = FindNode(key);
	if (it != m_rTree.pre_order_end())
	{
		SVTreeContainer tree;
		// pre_order_iterator has no conversion to iterator
		// back up to the parent
		const SVTreeContainer* pTree = it.node()->parent();
		for (SVTree_const_iterator pIt = pTree->begin();pIt != pTree->end();++pIt)
		{
			if (pIt->first == key)
			{
				// insert the parent Node and children
				const SVTreeContainer* pChildren = it.node();
				tree.insert(*pChildren);
				break;
			}
		}
		return SVTreeContainer(tree);
	}
	return SVTreeContainer();
}

///////////////////////////////////////////////////////////////////////
// Check if a Node has Children
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
bool SVTreeAdapter<ElementKey, ElementType, InvalidKey>::HasChildren(SVTree_const_iterator& it) const
{
	bool bHasChildren = false;
	if (it != m_rTree.end())
	{
		bHasChildren = (it.node()->size()) ? true : false;
	}
	return bHasChildren;
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
SVTreeIterator<ElementKey, ElementType, InvalidKey> SVTreeAdapter<ElementKey, ElementType, InvalidKey>::insert(const ElementKey& key, const sv_tree_iterator& insertionIt, SVTreeInsertionFlags::InsertionFlags flags)
{
	SVTreeElement l_Element( key, ElementType() );

	return insert( l_Element, insertionIt, flags );
}
///////////////////////////////////////////////////////////////////////
// Insert a Node into the tree
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
SVTreeIterator<ElementKey, ElementType, InvalidKey> SVTreeAdapter<ElementKey, ElementType, InvalidKey>::insert(const SVTreeElement& element, const sv_tree_iterator& insertionIt, SVTreeInsertionFlags::InsertionFlags flags)
{
	sv_tree_iterator it = m_rTree.end();

	SVTree_iterator curIt(static_cast<SVTree_iterator>(sv_tree_iterator(insertionIt)));

	if (flags == SVTreeInsertionFlags::InsertBefore)
	{
		// if sibling found
		if (curIt != m_rTree.end())
		{
			it = curIt.node()->parent()->insert(curIt, element);
		}
	}
	else if (flags == SVTreeInsertionFlags::InsertAfter)
	{
		// insert after Node (Sibling)
		it = curIt.node()->parent()->insert(++curIt, element);
	}
	else 
	{
		if (curIt != m_rTree.end())
		{
			it = curIt.node()->insert(element);
		}
		else
		{
			it = m_rTree.insert(element);
		}
	}
	return it;
}

///////////////////////////////////////////////////////////////////////
// Insert a Node into the tree
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
SVTreeIterator<ElementKey, ElementType, InvalidKey> SVTreeAdapter<ElementKey, ElementType, InvalidKey>::insert(const SVTreeContainer& tree, const sv_tree_iterator& insertionIt, SVTreeInsertionFlags::InsertionFlags flags=SVTreeInsertionFlags::InsertAtEnd)
{
	sv_tree_iterator it = m_rTree.end();

	SVTree_iterator curIt(static_cast<SVTree_iterator>(sv_tree_iterator(insertionIt)));

	if (flags == SVTreeInsertionFlags::InsertBefore)
	{
		// if sibling found
		if (curIt != m_rTree.end())
		{
			it = curIt.node()->parent()->insert(curIt, tree);
		}
	}
	else if (flags == SVTreeInsertionFlags::InsertAfter)
	{
		// insert after Node (Sibling)
		it = curIt.node()->parent()->insert(++curIt, tree);
	}
	else 
	{
		if (curIt != m_rTree.end())
		{
			it = curIt.node()->insert(tree);
		}
		else
		{
			it = m_rTree.insert(tree);
		}
	}
	return it;
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
bool SVTreeAdapter<ElementKey, ElementType, InvalidKey>::empty() const
{
	return m_rTree.empty();
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
void SVTreeAdapter<ElementKey, ElementType, InvalidKey>::swap(
	typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTreeContainer & rhs
	)
{
	m_rTree.swap(rhs);
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::sv_tree_iterator
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::begin() 
{ 
	return sv_tree_iterator( m_rTree.begin() ); 
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::sv_tree_const_iterator
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::begin() const
{
	return sv_tree_const_iterator( m_rTree.begin() ); 
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTree_pre_order_iterator
	SVTreeAdapter<ElementKey, ElementType, InvalidKey>::pre_order_begin() 
{ 
	return SVTree_pre_order_iterator( m_rTree.pre_order_begin() ); 
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTree_const_pre_order_iterator
	SVTreeAdapter<ElementKey, ElementType, InvalidKey>::pre_order_begin() const
{ 
	return SVTree_const_pre_order_iterator( m_rTree.pre_order_begin() ); 
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTree_post_order_iterator
	SVTreeAdapter<ElementKey, ElementType, InvalidKey>::post_order_begin() 
{ 
	return SVTree_post_order_iterator( m_rTree.post_order_begin() ); 
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTree_const_post_order_iterator
	SVTreeAdapter<ElementKey, ElementType, InvalidKey>::post_order_begin() const
{ 
	return SVTree_const_post_order_iterator( m_rTree.post_order_begin() ); 
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTree_level_order_iterator
	SVTreeAdapter<ElementKey, ElementType, InvalidKey>::level_order_begin() 
{ 
	return SVTree_level_order_iterator( m_rTree.level_order_begin() ); 
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTree_const_level_order_iterator
	SVTreeAdapter<ElementKey, ElementType, InvalidKey>::level_order_begin() const
{ 
	return SVTree_const_level_order_iterator( m_rTree.level_order_begin() ); 
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::sv_tree_iterator
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::end() 
{ 
	return sv_tree_iterator( m_rTree.end() ); 
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::sv_tree_const_iterator
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::end() const
{ 
	return sv_tree_const_iterator( m_rTree.end() ); 
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTree_pre_order_iterator
	SVTreeAdapter<ElementKey, ElementType, InvalidKey>::pre_order_end() 
{ 
	return SVTree_pre_order_iterator( m_rTree.pre_order_end() ); 
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTree_const_pre_order_iterator
	SVTreeAdapter<ElementKey, ElementType, InvalidKey>::pre_order_end() const
{ 
	return SVTree_const_pre_order_iterator( m_rTree.pre_order_end() ); 
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTree_post_order_iterator
	SVTreeAdapter<ElementKey, ElementType, InvalidKey>::post_order_end() 
{ 
	return SVTree_post_order_iterator( m_rTree.post_order_end() ); 
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTree_const_post_order_iterator
	SVTreeAdapter<ElementKey, ElementType, InvalidKey>::post_order_end() const
{ 
	return SVTree_const_post_order_iterator( m_rTree.post_order_end() ); 
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTree_level_order_iterator
	SVTreeAdapter<ElementKey, ElementType, InvalidKey>::level_order_end() 
{ 
	return SVTree_level_order_iterator( m_rTree.level_order_end() ); 
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTree_const_level_order_iterator
	SVTreeAdapter<ElementKey, ElementType, InvalidKey>::level_order_end() const
{ 
	return SVTree_const_level_order_iterator( m_rTree.level_order_end() ); 
}

///////////////////////////////////////////////////////////////////////
// Erase a node from the tree
// Note: end iterators for child nodes use both the Tree and iterator check
///////////////////////////////////////////////////////////////////////
template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::sv_tree_iterator
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::erase(sv_tree_iterator& it) 
{ 
	SVTree_iterator treeIt(it); 
	SVTreeContainer* pTree = treeIt.node()->parent();
	return sv_tree_iterator( pTree->erase(treeIt) ); 
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::sv_tree_iterator
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::find( SVTreeContainer* p_pSubTree )
{
	sv_tree_iterator l_Iter( m_rTree.end() );

	if( p_pSubTree != NULL )
	{
		for( l_Iter = m_rTree.begin(); l_Iter != m_rTree.end(); ++l_Iter )
		{
			if( l_Iter.GetChildTree() == p_pSubTree )
			{
				break;
			}
		}
	}

	return l_Iter;
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::sv_tree_iterator
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::find( const ElementKey& key, bool bRecursive )
{
	SVTree_iterator l_Iter( m_rTree.end() );

	if (bRecursive == false)
	{
		for( l_Iter = m_rTree.begin(); l_Iter != m_rTree.end(); ++l_Iter )
		{
			if( l_Iter->first == key )
			{
				break;
			}
		}
	}
	else
	{
		SVTree_pre_order_iterator descentIt = FindNode(key);
		if (descentIt != m_rTree.pre_order_end())
		{
			//l_Iter = descentIt.node()->begin();
			l_Iter = descentIt.base();
		}
	}
	return l_Iter;
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::sv_tree_const_iterator
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::find( const ElementKey& key, bool bRecursive ) const
{
	SVTree_const_iterator l_Iter( m_rTree.end() );

	if (bRecursive == false)
	{
		for( l_Iter = m_rTree.begin(); l_Iter != m_rTree.end(); ++l_Iter )
		{
			if( l_Iter->first == key )
			{
				break;
			}
		}
	}
	else
	{
		SVTree_const_pre_order_iterator descentIt = FindNode(key);
		if (descentIt != m_rTree.pre_order_end())
		{
			//l_Iter = descentIt.node()->begin();
			l_Iter = descentIt.base();
		}
	}
	return l_Iter;
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::operator SVTreeContainer * ()
{
	return &m_rTree;
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::operator SVTreeContainer & ()
{
	return m_rTree;
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::operator SVTreeContainer const & () const
{
	return m_rTree;
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTreeElement* 
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::get()
{
	return m_rTree.get();
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
const typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTreeElement* 
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::get() const
{
	return m_rTree.get();
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
void SVTreeAdapter<ElementKey, ElementType, InvalidKey>::set( const SVTreeElement& p_rElement )
{
	m_rTree.set( p_rElement );
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTreeContainer*
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::parent()
{
	return m_rTree.parent();
}

template<typename ElementKey, typename ElementType, typename InvalidKey>
const typename SVTreeAdapter<ElementKey, ElementType, InvalidKey>::SVTreeContainer*
SVTreeAdapter<ElementKey, ElementType, InvalidKey>::parent() const
{
	return m_rTree.parent();
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVTreeLibrary\SVTreeAdapter.inl_v  $
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
 *    Rev 1.0   25 Apr 2013 18:50:10   bWalter
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
