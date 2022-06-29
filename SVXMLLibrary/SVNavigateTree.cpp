//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVNavigateTreeClass
//* .File Name       : $Workfile:   SVNavigateTreeClass.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:47:48  $
//******************************************************************************
#pragma region Includes
#include "Stdafx.h"
#include "SVNavigateTree.h"
#pragma endregion Includes

namespace  SvXml
{
bool SVNavigateTree::AddItem(SVXMLMaterialsTree &rTree, SVXMLMaterialsTree::SVBranchHandle pParent, LPCTSTR Name, const _variant_t &rVariant, SVXMLMaterialsTree::SVLeafHandle* ppItem)
{
	bool Result( false );

	SVXMLMaterialsTree::SVLeafHandle pLeaf;

	Result = S_OK == rTree.createLeaf( pParent, Name, rVariant, &pLeaf );

	if( nullptr != ppItem )
	{
		*ppItem = pLeaf;
	}

	return Result;
}

bool SVNavigateTree::SetItem(SVXMLMaterialsTree &rTree, SVXMLMaterialsTree::SVBranchHandle pParent, LPCTSTR Name, const _variant_t &rVariant, SVXMLMaterialsTree::SVLeafHandle* ppItem)
{
	bool Result( false );

	SVXMLMaterialsTree::SVLeafHandle pLeaf;

	Result = GetItemLeaf( rTree, Name, pParent, pLeaf );

	if ( !Result )
	{
		Result = AddItem( rTree, pParent, Name, rVariant, &pLeaf );
	}
	else
	{
		Result = S_OK == rTree.setLeafData( pLeaf, rVariant );
	}

	if( nullptr != ppItem )
	{
		*ppItem = pLeaf;
	}

	return Result;
}

bool SVNavigateTree::AddBranch(SVXMLMaterialsTree &rTree, SVXMLMaterialsTree::SVBranchHandle pParent, LPCTSTR Name, SVXMLMaterialsTree::SVBranchHandle* ppItem)
{
	bool Result( false );

	SVXMLMaterialsTree::SVBranchHandle pBranch( nullptr );

	Result = S_OK == rTree.createBranch( pParent, Name, &pBranch );

	if( nullptr != ppItem )
	{
		*ppItem = pBranch;
	}

	return Result;
}

bool SVNavigateTree::SetBranch(SVXMLMaterialsTree &rTree, SVXMLMaterialsTree::SVBranchHandle pParent, LPCTSTR Name, SVXMLMaterialsTree::SVBranchHandle *ppItem)
{
	bool bOk = false;

	SVXMLMaterialsTree::SVBranchHandle pBranch( nullptr );

	bOk = GetItemBranch( rTree, Name, pParent, pBranch );

	if ( ! bOk )
	{
		bOk = AddBranch( rTree, pParent, Name, &pBranch );
	}

	if( nullptr != ppItem )
	{
		*ppItem = pBranch;
	}

	return bOk;
}

bool SVNavigateTree::GetItemBranch(SVXMLMaterialsTree& rTree, LPCTSTR Name, SVXMLMaterialsTree::SVBranchHandle pParent, SVXMLMaterialsTree::SVBranchHandle& rpBranch)
{
	rpBranch = rTree.findBranch(pParent, Name);
	return (nullptr != rpBranch);
}

std::vector<SVXMLMaterialsTree::SVBranchHandle> SVNavigateTree::findSubbranches(SVXMLMaterialsTree& rTree, SVXMLMaterialsTree::SVBranchHandle pParent)
{
	return rTree.getSubbranches(pParent);
}

bool SVNavigateTree::GetItemLeaf(SVXMLMaterialsTree &rTree, LPCTSTR Name, SVXMLMaterialsTree::SVBranchHandle pParent, SVXMLMaterialsTree::SVLeafHandle& rpItem)
{
	rpItem = rTree.findLeaf( pParent, Name );
	return rTree.isValidLeaf(pParent, rpItem);
}

bool SVNavigateTree::GetItem(SVXMLMaterialsTree &rTree, LPCTSTR Name, SVXMLMaterialsTree::SVBranchHandle pParent, _variant_t &rData)
{
	bool Result( false );
	SVXMLMaterialsTree::SVLeafHandle pLeaf;

	Result = GetItemLeaf( rTree, Name, pParent, pLeaf );
	if ( Result )
	{
		rTree.getLeafData( pLeaf, rData );
	}

	return Result;
}

bool SVNavigateTree::DeleteItem(SVXMLMaterialsTree &rTree, SVXMLMaterialsTree::SVBranchHandle pItem)
{
	bool bOk = ( S_OK == rTree.deleteBranch( pItem ) );
	return bOk;
}

bool SVNavigateTree::DeleteItem(SVXMLMaterialsTree &rTree, SVXMLMaterialsTree::SVBranchHandle , SVXMLMaterialsTree::SVLeafHandle pItem)
{
	bool bOk = ( S_OK == rTree.deleteLeaf(pItem) );
	return bOk;
}

bool SVNavigateTree::DeleteAllItems(SVXMLMaterialsTree &rTree)
{
	bool bOk = ( S_OK == rTree.Clear() );
	return bOk;
}

bool SVNavigateTree::HasChildren(SVXMLMaterialsTree& rTree, SVXMLMaterialsTree::SVBranchHandle pItem)
{
	return ( rTree.hasBranches( pItem ) || rTree.hasLeaves( pItem ) );
}

bool SVNavigateTree::FindBranch( SVXMLMaterialsTree &rTree, SVXMLMaterialsTree::SVBranchHandle startBranch, FindPredicate findPredicate, SVXMLMaterialsTree::SVBranchHandle& rFoundItem )
{
	bool status = findPredicate( startBranch );

	if( status )
	{
		rFoundItem = startBranch;
	}
	else
	{
		typedef std::deque< SVXMLMaterialsTree::SVBranchHandle > SVProcessChildHandleDeque;

		SVProcessChildHandleDeque l_ProcessHandles;

		l_ProcessHandles.push_back( startBranch );

		auto l_Iter = l_ProcessHandles.begin();

		while( ! status && l_Iter != l_ProcessHandles.end() )
		{
			SVXMLMaterialsTree::SVBranchHandle localBranch = *l_Iter;

			l_ProcessHandles.erase( l_Iter );

			SVXMLMaterialsTree::SVBranchHandle childBranch( rTree.getFirstBranch( localBranch ) );

			while( ! status && rTree.isValidBranch( childBranch ) )
			{
				status = findPredicate( childBranch );

				if( status )
				{
					rFoundItem = childBranch;
				}
				else
				{
					l_ProcessHandles.push_back( childBranch );

					childBranch = rTree.getNextBranch( localBranch, childBranch );
				}
			}

			l_Iter = l_ProcessHandles.begin();
		}
	}

	return status;
}
} //namespace SvXml
