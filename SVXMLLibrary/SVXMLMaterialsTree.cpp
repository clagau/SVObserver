//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLMaterialsTree
//* .File Name       : $Workfile:   SVXMLMaterialsTree.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Dec 2014 04:46:16  $
//******************************************************************************

#include "stdafx.h"
#include "SVXMLMaterialsTree.h"

SVXMLMaterialsTree::SVXMLMaterialsTree( SVMaterialsTreeAdapter& p_rTree )
: m_rTree( p_rTree )
{
}

SVXMLMaterialsTree::SVXMLMaterialsTree( SVMaterialsTreeAdapter& p_rTree, SVMaterialsTreeAdapter::SVTreeContainer*& p_rBranch )
: m_rTree( *p_rBranch )
{
}

SVXMLMaterialsTree::SVXMLMaterialsTree( SVXMLMaterialsTree& p_rTree )
: m_rTree( p_rTree.m_rTree )
{
}

SVXMLMaterialsTree::SVXMLMaterialsTree( SVXMLMaterialsTree& p_rTree, SVXMLMaterialsTree::SVBranchHandle& p_rBranch )
: m_rTree( *p_rBranch )
{
}

SVXMLMaterialsTree::~SVXMLMaterialsTree()
{
}

size_t SVXMLMaterialsTree::GetCount() const
{
	return SVMaterialsTreeAdapter::GetCount( m_rTree );
}

// GetRootNode () -----------------------------------------------------------
//  If this function is called, and the root doesn't yet exist, then it 
//	 should be created.  The root element of the tree MUST be a node that does
//  not contain data. DoesNodeHaveData () must return false for the root 
//  node.
HRESULT SVXMLMaterialsTree::GetRoot( SVBranchHandle& p_rRoot )
{
	HRESULT l_Status( S_OK );

	p_rRoot = m_rTree;

	return l_Status;
}

HRESULT SVXMLMaterialsTree::IsRoot( const SVBranchHandle& p_rBranch )
{
	HRESULT l_Status( S_OK );

	if( p_rBranch != m_rTree )
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

// DoesNodeHaveChildren () --------------------------------------------------
//		0 - Has children
//    1 - No children
HRESULT SVXMLMaterialsTree::DoesBranchHaveBranches( const SVBranchHandle& p_rBranch )
{
	HRESULT l_Status( S_OK );

	SVBranchHandle l_Parent = p_rBranch;

	if( l_Parent == NULL )
	{
		GetRoot( l_Parent );
	}

	if( l_Parent == NULL || l_Parent->begin() == l_Parent->end() )
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

HRESULT SVXMLMaterialsTree::GetParentBranch( const SVBranchHandle& p_rChild, SVBranchHandle& p_rParent )
{
	HRESULT l_Status( S_OK );

	if( p_rChild != NULL )
	{
		p_rParent = p_rChild->parent();
	}
	else
	{
		p_rParent = NULL;

		l_Status = E_FAIL;
	}

	return l_Status;
}

// GetFirstBranch () -----------------------------------------------------
//  This function must return S_OK if there are children.
HRESULT SVXMLMaterialsTree::GetFirstBranch( const SVBranchHandle& p_rParent, SVBranchHandle& p_rChild )
{
	HRESULT l_Status = S_OK;

	SVBranchHandle l_Parent = p_rParent;

	if( l_Parent == NULL )
	{
		GetRoot( l_Parent );
	}

	l_Status = DoesBranchHaveBranches( l_Parent );

	if( SUCCEEDED( l_Status ) )
	{
		if( l_Status == S_OK )
		{
			SVBranchHandle l_pBranch( const_cast< SVBranchHandle >( p_rParent ) );

			SVMaterialsTreeAdapter::SVTreeContainer::iterator l_Iter( l_pBranch->begin() );

			if( l_Iter != l_pBranch->end() )
			{
				p_rChild = l_Iter.node();
			}
			else
			{
				p_rChild = NULL;

				l_Status = S_FALSE;
			}
		}
		else
		{
			p_rChild = NULL;
		}
	}
	else
	{
		p_rChild = NULL;
	}

	return l_Status;
}

// GetNextBranch () ----------------------------------------------------
//  This function must return S_OK if there are more siblings.
HRESULT SVXMLMaterialsTree::GetNextBranch( const SVBranchHandle& p_rParent, SVBranchHandle& p_rChild )
{
	HRESULT l_Status = S_OK;

	SVBranchHandle l_Parent = p_rParent;

	if( l_Parent == NULL )
	{
		GetRoot( l_Parent );
	}

	l_Status = DoesBranchHaveBranches( l_Parent );

	if( l_Status == S_OK )
	{
		SVBranchHandle l_pBranch( const_cast< SVBranchHandle >( l_Parent ) );

		SVMaterialsTreeAdapter::SVTreeContainer::iterator l_Iter( l_pBranch->end() );

		if( p_rChild != NULL )
		{
			for( l_Iter = l_pBranch->begin(); l_Iter != l_pBranch->end(); ++l_Iter )
			{
				if( l_Iter.node() == p_rChild )
				{
					++l_Iter;

					break;
				}
			}
		}

		if( l_Iter != l_pBranch->end() )
		{
			p_rChild = l_Iter.node();
		}
		else
		{
			p_rChild = NULL;

			l_Status = S_FALSE;
		}
	}
	else
	{
		p_rChild = NULL;

		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVXMLMaterialsTree::FindBranch( const SVBranchHandle& p_rParent, const BSTR& p_rName, SVBranchHandle& p_rChild )
{
	HRESULT l_Status( S_OK );

	p_rChild = NULL;

	SVBranchHandle l_Parent = p_rParent;

	if( l_Parent == NULL )
	{
		GetRoot( l_Parent );
	}

	if( l_Parent != NULL )
	{
		SVBranchHandle l_pBranch( const_cast< SVBranchHandle >( l_Parent ) );

		SVMaterialsTreeAdapter::sv_tree_iterator l_Iter;

		for( l_Iter = l_pBranch->begin(); p_rChild == NULL && l_Iter != l_pBranch->end(); ++l_Iter )
		{
			if( l_Iter->first == p_rName )
			{
				p_rChild = l_Iter.GetChildTree();
			}
		}

		if( p_rChild == NULL )
		{
			l_Status = S_FALSE;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT	SVXMLMaterialsTree::IsValidBranch( const SVBranchHandle& p_rParent )
{
	HRESULT l_Status( S_OK );

	if( p_rParent == NULL )
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

HRESULT SVXMLMaterialsTree::CreateBranch( const SVBranchHandle& p_rParent, const BSTR& p_rName, SVBranchHandle* p_pChild )
{
	HRESULT l_Status( S_OK );
	SVBranchHandle l_Handle( NULL );

	SVBranchHandle l_Parent = p_rParent;

	if( l_Parent == NULL )
	{
		GetRoot( l_Parent );
	}

	if( l_Parent != NULL )
	{
		SVMaterials l_Data;
		SVMaterialsTreeAdapter::SVTreeElement l_Element( p_rName, l_Data );
		SVMaterialsTreeAdapter::iterator l_Iter( l_Parent->insert( l_Element ) );

		if( l_Iter != l_Parent->end() )
		{
			l_Handle = l_Iter.GetChildTree();
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	if( p_pChild != NULL )
	{
		( *p_pChild ) = l_Handle;
	}

	return l_Status;
}

HRESULT SVXMLMaterialsTree::DeleteBranch( SVBranchHandle& p_rBranch )
{
	HRESULT l_Status( S_OK );

	if( p_rBranch != NULL )
	{
		if( IsRoot( p_rBranch ) == S_OK )
		{
			m_rTree.Clear();
		}
		else
		{
			SVBranchHandle l_pParent( p_rBranch->parent() );

			if( l_pParent != NULL )
			{
				SVMaterialsTreeAdapter::SVTreeContainer::iterator l_Iter( l_pParent->end() );

				for( l_Iter = l_pParent->begin(); l_Iter != l_pParent->end(); ++l_Iter )
				{
					if( l_Iter.node() == p_rBranch )
					{
						break;
					}
				}

				if( l_Iter != l_pParent->end() )
				{
					l_pParent->erase( l_Iter );
				}
			}
		}

		p_rBranch = NULL;
	}

	return l_Status;
}

// GetBranchName () -----------------------------------------------------------
//
// 	GetBranchName () function needs to understand that the calling function 
//    will destroy the returned BSTR at its whim.  Usually, the derived 
//    GetNodeName () function will need to create the BSTR with SysAlloc () 
//    (or similar function), and will copy the node name from the actual tree
//    location.
HRESULT SVXMLMaterialsTree::GetBranchName( const SVBranchHandle& p_rBranch, BSTR& p_rName )
{
	HRESULT l_Status( S_OK );

	if( p_rBranch != NULL )
	{
		_bstr_t l_String;

		l_String.Attach( p_rName );

		if( IsRoot( p_rBranch ) == S_OK )
		{
			l_String = "Base";
		}
		else
		{
			const SVMaterialsTreeAdapter::SVTreeElement* l_pElement( p_rBranch->get() );

			if( l_pElement != NULL )
			{
				l_String = l_pElement->first.ToBSTR();
			}
			else
			{
				l_String = "";

				l_Status = E_FAIL;
			}
		}

		p_rName = l_String.Detach();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

// DoesNodeHaveData () ------------------------------------------------------
//		0 - Has Data
//    1 - No Data
HRESULT SVXMLMaterialsTree::DoesBranchHaveLeaves( const SVBranchHandle& p_rBranch )
{
	HRESULT l_Status( S_OK );

	SVBranchHandle l_Parent = p_rBranch;

	if( l_Parent == NULL )
	{
		GetRoot( l_Parent );
	}

	if( l_Parent != NULL )
	{
		const SVMaterialsTreeAdapter::SVTreeElement* l_pElement( l_Parent->get() );

		if( l_pElement != NULL )
		{
			if( l_pElement->second.empty() )
			{
				l_Status = S_FALSE;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

// GetFirstLeaf () -----------------------------------------------------
//  This function must return S_OK if there are children.
HRESULT SVXMLMaterialsTree::GetFirstLeaf( const SVBranchHandle& p_rParent, SVLeafHandle& p_rChild )
{
	HRESULT l_Status( S_OK );

	SVBranchHandle l_Parent = p_rParent;

	if( l_Parent == NULL )
	{
		GetRoot( l_Parent );
	}

	if( l_Parent != NULL )
	{
		SVMaterialsTreeAdapter::SVTreeElement* l_pElement( NULL );

		l_pElement = const_cast< SVMaterialsTreeAdapter::SVTreeElement* >( l_Parent->get() );

		if( l_pElement != NULL )
		{
			p_rChild = l_pElement->second.begin();

			if(	p_rChild == l_pElement->second.end() )
			{
				l_Status = S_FALSE;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

// GetNextLeaf () -----------------------------------------------------
//  This function must return S_OK if there is another child.
HRESULT SVXMLMaterialsTree::GetNextLeaf( const SVBranchHandle& p_rParent, SVLeafHandle& p_rChild )
{
	HRESULT l_Status( S_OK );

	SVBranchHandle l_Parent = p_rParent;

	if( l_Parent == NULL )
	{
		GetRoot( l_Parent );
	}

	if( l_Parent != NULL )
	{
		SVMaterialsTreeAdapter::SVTreeElement* l_pElement( NULL );

		l_pElement = const_cast< SVMaterialsTreeAdapter::SVTreeElement* >( l_Parent->get() );

		if( l_pElement != NULL )
		{
			if( p_rChild != l_pElement->second.end() )
			{
				++p_rChild;

				if( p_rChild == l_pElement->second.end() )
				{
					l_Status = S_FALSE;
				}
			}
			else
			{
				l_Status = S_FALSE;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVXMLMaterialsTree::FindLeaf( const SVBranchHandle& p_rParent, const BSTR& p_rName, SVLeafHandle& p_rChild )
{
	HRESULT l_Status = S_OK;

	SVBranchHandle l_Parent = p_rParent;

	if( l_Parent == NULL )
	{
		GetRoot( l_Parent );
	}

	if( l_Parent != NULL )
	{
		SVMaterialsTreeAdapter::SVTreeElement* l_pElement( NULL );

		l_pElement = const_cast< SVMaterialsTreeAdapter::SVTreeElement* >( l_Parent->get() );

		if( l_pElement != NULL )
		{
			p_rChild = l_pElement->second.find( p_rName );

			if( p_rChild == l_pElement->second.end() )
			{
				l_Status = S_FALSE;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVXMLMaterialsTree::IsValidLeaf( const SVBranchHandle& p_rParent, const SVLeafHandle& p_rLeaf )
{
	HRESULT l_Status = S_OK;

	SVBranchHandle l_Parent = p_rParent;

	if( l_Parent == NULL )
	{
		GetRoot( l_Parent );
	}

	if( l_Parent != NULL )
	{
		const SVMaterialsTreeAdapter::SVTreeElement* l_pElement( NULL );

		l_pElement = l_Parent->get();

		if( l_pElement != NULL )
		{
			if( p_rLeaf == l_pElement->second.end() )
			{
				l_Status = S_FALSE;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVXMLMaterialsTree::CreateLeaf( const SVBranchHandle& p_rParent, const BSTR& p_rName, const VARIANT& p_rData, SVLeafHandle* p_pChild )
{
	HRESULT l_Status( S_OK );

	SVBranchHandle l_Parent = p_rParent;

	if( l_Parent == NULL )
	{
		GetRoot( l_Parent );
	}

	if( l_Parent != NULL )
	{
		SVMaterialsTreeAdapter::SVTreeElement* l_pElement( NULL );

		l_pElement = const_cast< SVMaterialsTreeAdapter::SVTreeElement* >( l_Parent->get() );

		if( l_pElement != NULL )
		{
			l_Status = l_pElement->second.AddMaterial( p_rName, p_rData );

			if( l_Status == S_OK )
			{
				if( p_pChild != NULL )
				{
					( *p_pChild ) = l_pElement->second.find( p_rName );

					if( ( *p_pChild ) == l_pElement->second.end() )
					{
						l_Status = E_FAIL;
					}
				}
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVXMLMaterialsTree::DeleteLeaf( const SVBranchHandle& p_rParent, SVLeafHandle& p_rChild )
{
	HRESULT l_Status( S_OK );

	SVBranchHandle l_Parent = p_rParent;

	if( l_Parent == NULL )
	{
		GetRoot( l_Parent );
	}

	if( l_Parent != NULL )
	{
		SVMaterialsTreeAdapter::SVTreeElement* l_pElement( NULL );

		l_pElement = const_cast< SVMaterialsTreeAdapter::SVTreeElement* >( l_Parent->get() );

		if( l_pElement != NULL )
		{
			SVMaterials::iterator l_Iter( p_rChild );

			p_rChild = l_pElement->second.erase( p_rChild );
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVXMLMaterialsTree::GetLeafName( const SVLeafHandle& p_rLeaf, BSTR& p_rName )
{
	HRESULT l_Status( S_OK );

	_bstr_t l_String;

	l_String.Attach( p_rName );

	l_String = p_rLeaf->first.ToBSTR();

	p_rName = l_String.Detach();

	return l_Status;
}

HRESULT SVXMLMaterialsTree::GetLeafData( const SVLeafHandle& p_rLeaf, VARIANT& p_rData )
{
	HRESULT l_Status( S_OK );

	_variant_t l_Variant;

	l_Variant.Attach( p_rData );

	l_Variant = static_cast< const _variant_t & >( p_rLeaf->second );

	p_rData = l_Variant.Detach();

	return l_Status;
}

// SetLeafData () -----------------------------------------------------------
//  All data will be Copied for the tree.  The original VARIANT and any 
//  associated (BSTR) data will not be alterred.
HRESULT SVXMLMaterialsTree::SetLeafData( const SVLeafHandle& p_rLeaf, const VARIANT& p_rData )
{
	HRESULT l_Status( S_OK );

	p_rLeaf->second = p_rData;

	return l_Status;
}

HRESULT SVXMLMaterialsTree::ReplaceName( const SVBranchHandle& rParent, const VARIANT& rSearchName, const VARIANT& rReplaceName )
{
	HRESULT Result( S_OK );
	SVLeafHandle Leaf;

	GetFirstLeaf( rParent, Leaf);
	while( S_OK == IsValidLeaf( rParent, Leaf ) )
	{
		_variant_t Value;
		GetLeafData( Leaf, Value );
		if( Value == rSearchName )
		{
			Value = rReplaceName;
			SetLeafData( Leaf, Value );
		}

		GetNextLeaf( rParent, Leaf);
	}

	SVBranchHandle ChildBranch;
	GetFirstBranch( rParent, ChildBranch );
	while( S_OK == IsValidBranch( ChildBranch ) )
	{
		ReplaceName( ChildBranch, rSearchName, rReplaceName );
		GetNextBranch( rParent, ChildBranch );
	}

	return Result;
}

HRESULT SVXMLMaterialsTree::getLeafValues( const SVBranchHandle& rParent, const SVString& rSearchName, SVStringSet& rLeafValues )
{
	HRESULT Result( S_OK );
	SVLeafHandle Leaf;

	GetFirstLeaf( rParent, Leaf);
	while( S_OK == IsValidLeaf( rParent, Leaf ) )
	{
		_bstr_t LeafName;
		GetLeafName( Leaf, LeafName.GetBSTR() );
		if( rSearchName == SVString( LeafName) )
		{
			_variant_t Value;
			GetLeafData( Leaf, Value );
			rLeafValues.insert( SVString(Value) );
		}

		GetNextLeaf( rParent, Leaf);
	}

	SVBranchHandle ChildBranch;
	GetFirstBranch( rParent, ChildBranch );
	while( S_OK == IsValidBranch( ChildBranch ) )
	{
		getLeafValues( ChildBranch, rSearchName, rLeafValues );
		GetNextBranch( rParent, ChildBranch );
	}

	return Result;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVXMLLibrary\SVXMLMaterialsTree.cpp_v  $
 * 
 *    Rev 1.1   19 Dec 2014 04:46:16   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Method added: ReplaceName and getLeafValues
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 19:56:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Sep 2012 18:25:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated XML Materials Tree to allow a NULL branch to represent the root node.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Sep 2012 14:57:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added the count method to the tree classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 18:12:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * This code is the refacting of the items from the SVLibrary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
