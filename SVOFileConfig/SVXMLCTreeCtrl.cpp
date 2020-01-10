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
//Moved to precompiled header: #include <comdef.h>
#include "SVXMLCTreeCtrl.h"


SVXMLCTreeCtrl::SVXMLCTreeCtrl( CTreeCtrl& rTree )
: m_rTree( rTree ), m_Root( TVI_ROOT )
{
}

SVXMLCTreeCtrl::SVXMLCTreeCtrl( CTreeCtrl& rTree, SVBranchHandle pBranch )
: m_rTree( rTree ), m_Root( pBranch )
{
}

SVXMLCTreeCtrl::SVXMLCTreeCtrl( SVXMLCTreeCtrl& rTree )
: m_rTree( rTree.m_rTree ), m_Root( rTree.m_Root )
{
}

SVXMLCTreeCtrl::SVXMLCTreeCtrl( SVXMLCTreeCtrl& rTree, SVBranchHandle pBranch )
: m_rTree( rTree.m_rTree ), m_Root( pBranch )
{
}

SVXMLCTreeCtrl::~SVXMLCTreeCtrl()
{
}

size_t SVXMLCTreeCtrl::getCount() const
{
	return m_rTree.GetCount();
}

SVXMLCTreeCtrl::SVBranchHandle SVXMLCTreeCtrl::getRoot()
{
	return m_Root;
}

bool SVXMLCTreeCtrl::isRoot( const SVBranchHandle pBranch ) const
{
	return pBranch == m_Root;
}

// cppcheck-suppress unusedFunction
bool SVXMLCTreeCtrl::hasBranches( const SVBranchHandle pBranch )
{
	SVBranchHandle pChild( nullptr );

	pChild = getFirstBranch( pBranch );

	return (nullptr != pChild);
}

// cppcheck-suppress unusedFunction
SVXMLCTreeCtrl::SVBranchHandle SVXMLCTreeCtrl::getParentBranch( const SVBranchHandle pChild )
{
	SVBranchHandle pResult( nullptr );

	if( nullptr != pChild )
	{
		pResult = m_rTree.GetParentItem( pChild );
	}

	return pResult;
}

SVXMLCTreeCtrl::SVBranchHandle SVXMLCTreeCtrl::getFirstBranch( const SVBranchHandle pParent )
{
	SVBranchHandle pResult( nullptr );

	pResult = m_rTree.GetChildItem( pParent );

	while( nullptr != pResult && m_rTree.GetItemData( pResult ) )
	{
		pResult = m_rTree.GetNextItem( pResult, TVGN_NEXT );
	}

	return pResult;
}

SVXMLCTreeCtrl::SVBranchHandle SVXMLCTreeCtrl::getNextBranch( const SVBranchHandle pParent, const SVBranchHandle pBranch )
{
	SVBranchHandle pResult( pBranch );

	if( nullptr != pResult )
	{
		do
		{
			pResult = m_rTree.GetNextItem( pResult, TVGN_NEXT );
		}
		while( nullptr != pResult && m_rTree.GetItemData( pResult ) );

	}

	return pResult;
}

SVXMLCTreeCtrl::SVBranchHandle SVXMLCTreeCtrl::findBranch( const SVBranchHandle pParent, LPCTSTR Name )
{
	SVBranchHandle pResult( nullptr );

	SVBranchHandle pBranch = m_rTree.GetChildItem( pParent );

	while( nullptr == pResult && nullptr != pBranch )
	{
		std::string BranchName = m_rTree.GetItemText( pBranch );

		if( 0 == m_rTree.GetItemData( pBranch ) && Name == BranchName )
		{
			pResult = pBranch;
		}
		else
		{
			pBranch = m_rTree.GetNextItem( pBranch, TVGN_NEXT );
		}
	}

	return pResult;
}

bool SVXMLCTreeCtrl::isValidBranch( const SVBranchHandle pParent )
{
	return (nullptr != pParent);
}

// cppcheck-suppress unusedFunction
HRESULT SVXMLCTreeCtrl::createBranch( const SVBranchHandle pParent, LPCTSTR Name, SVBranchHandle* ppBranch )
{
	HRESULT Result( S_OK );

	SVBranchHandle pNewBranch( m_rTree.InsertItem( Name, pParent, TVI_LAST ) );

	if( nullptr != pNewBranch )
	{
		m_rTree.SetItemData( pNewBranch, 0 );
	}

	if( nullptr != ppBranch)
	{
		*ppBranch = pNewBranch;
	}

	return Result;
}

HRESULT SVXMLCTreeCtrl::deleteBranch( SVBranchHandle& rpBranch )
{
	HRESULT l_Status( S_OK );

	HTREEITEM pChild = m_rTree.GetChildItem( rpBranch );

	while( nullptr != pChild )
	{
		HTREEITEM pDeleteChild( pChild );

		pChild = m_rTree.GetNextItem( pChild, TVGN_NEXT );

		if( 0 != m_rTree.GetItemData( pDeleteChild )  )
		{
			if( S_OK != deleteLeaf( pDeleteChild ) )
			{
				l_Status = S_FALSE;
			}
		}
		else
		{
			if( S_OK != deleteBranch( pDeleteChild ) )
			{
				l_Status = S_FALSE;
			}
		}
	}

	if( isRoot( rpBranch ) )
	{
		if( ! m_rTree.DeleteItem( rpBranch ) )
		{
			l_Status = S_FALSE;
		}

		rpBranch = nullptr;
	}

	return l_Status;
}

std::string SVXMLCTreeCtrl::getBranchName( const SVBranchHandle pBranch ) const
{
	std::string Result;

	if( isRoot( pBranch ) )
	{
		Result = _T("Base");
	}
	else
	{
		Result = m_rTree.GetItemText( pBranch );
	}

	return Result;
}

// cppcheck-suppress unusedFunction
bool SVXMLCTreeCtrl::hasLeaves( const SVBranchHandle pBranch )
{
	SVLeafHandle pChild( nullptr );

	pChild = getFirstLeaf( pBranch );

	return (nullptr != pChild);
}

SVXMLCTreeCtrl::SVLeafHandle SVXMLCTreeCtrl::getFirstLeaf( const SVBranchHandle pParent )
{
	SVLeafHandle pResult( nullptr );

	pResult = m_rTree.GetChildItem( pParent );

	while( nullptr != pResult && !m_rTree.GetItemData( pResult ) )
	{
		pResult = m_rTree.GetNextItem( pResult, TVGN_NEXT );
	}

	return pResult;
}

SVXMLCTreeCtrl::SVLeafHandle SVXMLCTreeCtrl::getNextLeaf( const SVBranchHandle pParent, const SVLeafHandle pLeaf )
{
	SVLeafHandle pResult( pLeaf );

	if( nullptr != pResult )
	{
		do
		{
			pResult = m_rTree.GetNextItem( pResult, TVGN_NEXT );
		}
		while( nullptr != pResult && !m_rTree.GetItemData( pResult ) );
	}

	return pResult;
}

SVXMLCTreeCtrl::SVLeafHandle SVXMLCTreeCtrl::findLeaf( const SVBranchHandle pParent, LPCTSTR Name )
{
	SVLeafHandle pResult( nullptr );

	SVBranchHandle pBranch = m_rTree.GetChildItem( pParent );

	while( nullptr == pResult && nullptr != pBranch )
	{
		std::string BranchName = m_rTree.GetItemText( pBranch );

		if( m_rTree.GetItemData( pBranch ) && Name == BranchName )
		{
			pResult = pBranch;
		}
		else
		{
			pBranch = m_rTree.GetNextItem( pBranch, TVGN_NEXT );
		}
	}
	return pResult;
}

bool SVXMLCTreeCtrl::isValidLeaf( const SVBranchHandle pParent, const SVLeafHandle pLeaf )
{
	bool Result( false );

	if( nullptr != pParent )
	{
		SVLeafHandle pTestLeaf;
		pTestLeaf = m_rTree.GetChildItem( pParent );

		while( nullptr != pTestLeaf && !Result )
		{
			if( pLeaf == pTestLeaf )
			{
				Result = true;
			}
			pTestLeaf = m_rTree.GetNextItem( pTestLeaf, TVGN_NEXT );
		}
	}

	return Result;
}

// cppcheck-suppress unusedFunction
HRESULT SVXMLCTreeCtrl::createLeaf( const SVBranchHandle pParent, LPCTSTR Name, const _variant_t& rData, SVLeafHandle* ppLeaf )
{
	HRESULT Result( S_OK );

	SVLeafHandle pNewLeaf( nullptr );
	VARIANT* pVariant( new VARIANT );

	if( nullptr != pVariant )
	{
		::VariantInit( pVariant );
		VARIANT Value( rData );
		if( S_OK == ::VariantCopy( pVariant, &Value ) )
		{
			pNewLeaf = m_rTree.InsertItem( Name, pParent, TVI_LAST );

			if( nullptr != pNewLeaf )
			{
				DWORD_PTR pData( reinterpret_cast< DWORD_PTR >( pVariant ) );
				if( !m_rTree.SetItemData( pNewLeaf, pData ) )
				{
					Result = E_FAIL;
				}
			}
			else
			{
				Result = E_FAIL;
			}
		}
	}
	else
	{
		Result = E_FAIL;
	}

	if( nullptr != ppLeaf )
	{
		*ppLeaf = pNewLeaf;
	}

	return Result;
}

HRESULT SVXMLCTreeCtrl::deleteLeaf( const SVLeafHandle pLeaf )
{
	HRESULT l_Status( S_OK );

	if( nullptr != pLeaf )
	{
		DWORD_PTR pData( m_rTree.GetItemData( pLeaf ) );
		
		if( ! m_rTree.DeleteItem( pLeaf ) )
		{
			l_Status = E_FAIL;
		}

		if( 0 != pData )
		{
			VARIANT* pVariant( reinterpret_cast< VARIANT* >( pData ) );

			::VariantClear( pVariant );

			delete pVariant;
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

std::string SVXMLCTreeCtrl::getLeafName( const SVLeafHandle pLeaf ) const
{
	std::string Result;

	Result = m_rTree.GetItemText( pLeaf );

	return Result;
}

HRESULT SVXMLCTreeCtrl::getLeafData( const SVLeafHandle pLeaf, _variant_t& rData ) const
{
	HRESULT Result( E_FAIL );

	rData.Clear();

	if( nullptr != pLeaf )
	{
		DWORD_PTR pData( m_rTree.GetItemData( pLeaf ) );
		
		if( 0 != pData )
		{
			VARIANT* pVariant( reinterpret_cast< VARIANT* >( pData ) );

			::VariantInit( &rData.GetVARIANT() );
			::VariantCopy( &rData.GetVARIANT(), pVariant );
		}
	}

	return Result;
}

HRESULT SVXMLCTreeCtrl::getLeafData( const SVBranchHandle pParent, LPCTSTR Name, _variant_t& rData )
{
	return 	getLeafData( findLeaf( pParent, Name ), rData );
}

// cppcheck-suppress unusedFunction
HRESULT SVXMLCTreeCtrl::setLeafData( const SVLeafHandle pLeaf, const _variant_t& rData )
{
	HRESULT l_Status( S_OK );

	if( nullptr != pLeaf )
	{
		DWORD_PTR pData( m_rTree.GetItemData( pLeaf ) );
		
		if( 0 != pData )
		{
			VARIANT* pVariant = reinterpret_cast< VARIANT* >( pData );

			l_Status = ::VariantCopy( pVariant, &rData );
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


