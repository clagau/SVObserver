//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Maschinenbau GmbH, München
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLCTreeCtrl
//* .File Name       : $Workfile:   SVXMLCTreeCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:50:36  $
//******************************************************************************

#include "stdafx.h"
#include <comdef.h>
#include "SVXMLCTreeCtrl.h"

SVXMLCTreeCtrl::SVXMLCTreeCtrl( CTreeCtrl& p_rTree )
: m_rTree( p_rTree ), m_Root( TVI_ROOT )
{
}

SVXMLCTreeCtrl::SVXMLCTreeCtrl( CTreeCtrl& p_rTree, HTREEITEM& p_rBranch )
: m_rTree( p_rTree ), m_Root( p_rBranch )
{
}

SVXMLCTreeCtrl::SVXMLCTreeCtrl( SVXMLCTreeCtrl& p_rTree )
: m_rTree( p_rTree.m_rTree ), m_Root( p_rTree.m_Root )
{
}

SVXMLCTreeCtrl::SVXMLCTreeCtrl( SVXMLCTreeCtrl& p_rTree, SVXMLCTreeCtrl::SVBranchHandle& p_rBranch )
: m_rTree( p_rTree.m_rTree ), m_Root( p_rBranch )
{
}

SVXMLCTreeCtrl::~SVXMLCTreeCtrl()
{
}

size_t SVXMLCTreeCtrl::GetCount() const
{
	return m_rTree.GetCount();
}

// GetRootNode () -----------------------------------------------------------
//  If this function is called, and the root doesn't yet exist, then it 
//	 should be created.  The root element of the tree MUST be a node that does
//  not contain data. DoesNodeHaveData () must return false for the root 
//  node.
HRESULT SVXMLCTreeCtrl::GetRoot( SVBranchHandle& p_rRoot )
{
	HRESULT l_Status( S_OK );

	p_rRoot = m_Root;

	return l_Status;
}

HRESULT SVXMLCTreeCtrl::IsRoot( const SVBranchHandle& p_rBranch )
{
	HRESULT l_Status( S_OK );

	if( p_rBranch != m_Root )
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

// DoesNodeHaveChildren () --------------------------------------------------
//		0 - Has children
//    1 - No children
HRESULT SVXMLCTreeCtrl::DoesBranchHaveBranches( const SVBranchHandle& p_rBranch )
{
	HRESULT l_Status( S_OK );

	SVBranchHandle l_Child;

	l_Status = GetFirstBranch( p_rBranch, l_Child );

	return l_Status;
}

HRESULT SVXMLCTreeCtrl::GetParentBranch( const SVBranchHandle& p_rChild, SVBranchHandle& p_rParent )
{
	HRESULT l_Status( S_OK );

	if( p_rChild != NULL )
	{
		p_rParent = m_rTree.GetParentItem( p_rChild );
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
HRESULT SVXMLCTreeCtrl::GetFirstBranch( const SVBranchHandle& p_rParent, SVBranchHandle& p_rChild )
{
	HRESULT l_Status( S_OK );

	p_rChild = m_rTree.GetChildItem( p_rParent );

	while( p_rChild != NULL && m_rTree.GetItemData( p_rChild ) != NULL )
	{
		p_rChild = m_rTree.GetNextItem( p_rChild, TVGN_NEXT );
	}

	if( p_rChild == NULL )
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

// GetNextBranch () ----------------------------------------------------
//  This function must return S_OK if there are more siblings.
HRESULT SVXMLCTreeCtrl::GetNextBranch( const SVBranchHandle& p_rParent, SVBranchHandle& p_rChild )
{
	HRESULT l_Status( S_OK );

	if( p_rChild != NULL )
	{
		do
		{
			p_rChild = m_rTree.GetNextItem( p_rChild, TVGN_NEXT );
		}
		while( p_rChild != NULL && m_rTree.GetItemData( p_rChild ) != NULL );

		if( p_rChild == NULL )
		{
			l_Status = S_FALSE;
		}
	}
	else
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

HRESULT SVXMLCTreeCtrl::FindBranch( const SVBranchHandle& p_rParent, const BSTR& p_rName, SVBranchHandle& p_rChild )
{
	HRESULT hrOk = S_OK;

	p_rChild = NULL;

	_bstr_t l_Name = p_rName;

	SVBranchHandle l_Branch = m_rTree.GetChildItem( p_rParent );

	while( p_rChild == NULL && l_Branch != NULL )
	{
		CString csName = m_rTree.GetItemText( l_Branch );

		if( m_rTree.GetItemData( l_Branch ) == NULL && csName.Compare( l_Name ) == 0 )
		{
			p_rChild = l_Branch;
		}
		else
		{
			l_Branch = m_rTree.GetNextItem( l_Branch, TVGN_NEXT );
		}
	}

	if( p_rChild == NULL )
	{
		hrOk = S_FALSE;
	}

	return hrOk;
}

HRESULT	SVXMLCTreeCtrl::IsValidBranch( const SVBranchHandle& p_rParent )
{
	HRESULT l_Status( S_OK );

	if( p_rParent == NULL )
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

HRESULT SVXMLCTreeCtrl::CreateBranch( const SVBranchHandle& p_rParent, const BSTR& p_rName, SVBranchHandle* p_pChild )
{
	HRESULT l_Status( S_OK );

	_bstr_t l_Name( p_rName );
	
	SVBranchHandle l_Branch( m_rTree.InsertItem( l_Name, p_rParent, TVI_LAST ) );

	if( l_Branch != NULL )
	{
		if( ! m_rTree.SetItemData( l_Branch, NULL ) )
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
		( *p_pChild ) = l_Branch;
	}

	return l_Status;
}

HRESULT SVXMLCTreeCtrl::DeleteBranch( SVBranchHandle& p_rBranch )
{
	HRESULT l_Status( S_OK );

	HTREEITEM l_Child = m_rTree.GetChildItem( p_rBranch );

	while( l_Child != NULL )
	{
		HTREEITEM l_DeleteChild( l_Child );

		l_Child = m_rTree.GetNextItem( l_Child, TVGN_NEXT );

		if( m_rTree.GetItemData( l_DeleteChild ) != NULL )
		{
			if( DeleteLeaf( p_rBranch, l_DeleteChild ) != S_OK )
			{
				l_Status = S_FALSE;
			}
		}
		else
		{
			if( DeleteBranch( l_DeleteChild ) != S_OK )
			{
				l_Status = S_FALSE;
			}
		}
	}

	if( IsRoot( p_rBranch ) != S_OK )
	{
		if( ! m_rTree.DeleteItem( p_rBranch ) )
		{
			l_Status = S_FALSE;
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
HRESULT SVXMLCTreeCtrl::GetBranchName( const SVBranchHandle& p_rBranch, BSTR& p_rName )
{
	HRESULT l_Status( S_OK );

	_bstr_t l_String;

	l_String.Attach( p_rName );

	if( IsRoot( p_rBranch ) == S_OK )
	{
		l_String = "Base";
	}
	else
	{
		l_String = m_rTree.GetItemText( p_rBranch );
	}

	p_rName = l_String.Detach();

	return l_Status;
}

// DoesNodeHaveData () ------------------------------------------------------
//		0 - Has Data
//    1 - No Data
HRESULT SVXMLCTreeCtrl::DoesBranchHaveLeaves( const SVBranchHandle& p_rBranch )
{
	HRESULT l_Status( S_OK );

	SVLeafHandle l_Child;

	l_Status = GetFirstLeaf( p_rBranch, l_Child );

	return l_Status;
}

// GetFirstLeaf () -----------------------------------------------------
//  This function must return S_OK if there are children.
HRESULT SVXMLCTreeCtrl::GetFirstLeaf( const SVBranchHandle& p_rParent, SVLeafHandle& p_rChild )
{
	HRESULT l_Status( S_OK );

	p_rChild = m_rTree.GetChildItem( p_rParent );

	while( p_rChild != NULL && m_rTree.GetItemData( p_rChild ) == NULL )
	{
		p_rChild = m_rTree.GetNextItem( p_rChild, TVGN_NEXT );
	}

	if( p_rChild == NULL )
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

// GetNextLeaf () -----------------------------------------------------
//  This function must return S_OK if there is another child.
HRESULT SVXMLCTreeCtrl::GetNextLeaf( const SVBranchHandle& p_rParent, SVLeafHandle& p_rChild )
{
	HRESULT l_Status( S_OK );

	if( p_rChild != NULL )
	{
		do
		{
			p_rChild = m_rTree.GetNextItem( p_rChild, TVGN_NEXT );
		}
		while( p_rChild != NULL && m_rTree.GetItemData( p_rChild ) == NULL );

		if( p_rChild == NULL )
		{
			l_Status = S_FALSE;
		}
	}
	else
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

HRESULT SVXMLCTreeCtrl::FindLeaf( const SVBranchHandle& p_rParent, const BSTR& p_rName, SVLeafHandle& p_rChild )
{
	HRESULT hrOk = S_OK;

	p_rChild = NULL;

	_bstr_t l_Name = p_rName;

	SVBranchHandle l_Branch = m_rTree.GetChildItem( p_rParent );

	while( p_rChild == NULL && l_Branch != NULL )
	{
		CString csName = m_rTree.GetItemText( l_Branch );

		if( m_rTree.GetItemData( l_Branch ) != NULL && csName.Compare( l_Name ) == 0 )
		{
			p_rChild = l_Branch;
		}
		else
		{
			l_Branch = m_rTree.GetNextItem( l_Branch, TVGN_NEXT );
		}
	}

	if( p_rChild == NULL )
	{
		hrOk = S_FALSE;
	}

	return hrOk;
}

HRESULT	SVXMLCTreeCtrl::IsValidLeaf( const SVBranchHandle& p_rParent, const SVLeafHandle& p_rLeaf )
{
	HRESULT l_Status( S_OK );

	if( p_rParent != NULL )
	{
		if( p_rLeaf == NULL )
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

HRESULT SVXMLCTreeCtrl::CreateLeaf( const SVBranchHandle& p_rParent, const BSTR& p_rName, const VARIANT& p_rData, SVLeafHandle* p_pChild )
{
	HRESULT l_Status( S_OK );

	_bstr_t l_Name( p_rName );
	
	SVBranchHandle l_Branch( m_rTree.InsertItem( l_Name, p_rParent, TVI_LAST ) );

	if( l_Branch != NULL )
	{
		VARIANT* l_pVariant( new VARIANT );
		
		if( l_pVariant != NULL )
		{
			::VariantInit( l_pVariant );

			l_Status = ::VariantCopy( l_pVariant, const_cast< VARIANT * >( &p_rData ) );

			if( l_Status == S_OK )
			{
				DWORD_PTR l_pData( reinterpret_cast< DWORD_PTR >( l_pVariant ) );

				if( ! m_rTree.SetItemData( l_Branch, l_pData ) )
				{
					l_Status = E_FAIL;
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

	if( p_pChild != NULL )
	{
		( *p_pChild ) = l_Branch;
	}

	return l_Status;
}

HRESULT SVXMLCTreeCtrl::DeleteLeaf( const SVBranchHandle& p_rParent, SVLeafHandle& p_rChild )
{
	HRESULT l_Status( S_OK );

	if( p_rChild != NULL )
	{
		DWORD_PTR l_pData( m_rTree.GetItemData( p_rChild ) );
		
		if( ! m_rTree.DeleteItem( p_rChild ) )
		{
			l_Status = S_FALSE;
		}

		if( l_pData != NULL )
		{
			VARIANT* l_pVariant( reinterpret_cast< VARIANT* >( l_pData ) );

			::VariantClear( l_pVariant );

			delete l_pVariant;
		}
		else
		{
			l_Status = S_FALSE;
		}
	}
	else
	{
		l_Status = S_FALSE;
	}

	p_rChild = NULL;

	return l_Status;
}

HRESULT SVXMLCTreeCtrl::GetLeafName( const SVLeafHandle& p_rLeaf, BSTR& p_rName )
{
	HRESULT l_Status( S_OK );

	_bstr_t l_String( m_rTree.GetItemText( p_rLeaf ) );

	p_rName = l_String.Detach();

	return l_Status;
}

HRESULT SVXMLCTreeCtrl::GetLeafData( const SVLeafHandle& p_rLeaf, VARIANT& p_rData )
{
	HRESULT l_Status( S_OK );

	if( p_rLeaf != NULL )
	{
		DWORD_PTR l_pData( m_rTree.GetItemData( p_rLeaf ) );
		
		if( l_pData != NULL )
		{
			VARIANT* l_pVariant( reinterpret_cast< VARIANT* >( l_pData ) );

			l_Status = ::VariantCopy( &p_rData, l_pVariant );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

// SetLeafData () -----------------------------------------------------------
//  All data will be Copied for the tree.  The original VARIANT and any 
//  associated (BSTR) data will not be alterred.
HRESULT SVXMLCTreeCtrl::SetLeafData( const SVLeafHandle& p_rLeaf, const VARIANT& p_rData )
{
	HRESULT l_Status( S_OK );

	if( p_rLeaf != NULL )
	{
		VARIANT* l_pVariant( NULL );

		DWORD_PTR l_pData( m_rTree.GetItemData( p_rLeaf ) );
		
		if( l_pData != NULL )
		{
			l_pVariant = reinterpret_cast< VARIANT* >( l_pData );

			l_Status = ::VariantCopy( l_pVariant, const_cast< VARIANT * >( &p_rData ) );
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVXMLCTreeCtrl.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:50:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Sep 2012 14:57:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added the count method to the tree classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 18:12:44   jspila
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
