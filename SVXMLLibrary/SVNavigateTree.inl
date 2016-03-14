//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVNavigateTreeClass
//* .File Name       : $Workfile:   SVNavigateTreeClass.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:47:48  $
//******************************************************************************

#include "SVNavigateTree.h"

template< typename SVTreeType >
bool SVNavigateTree::AddItem( SVTreeType &rTree, typename SVTreeType::SVBranchHandle pParent, LPCTSTR Name, const _variant_t &rVariant, typename SVTreeType::SVLeafHandle* ppItem )
{
	bool Result( false );

	SVTreeType::SVLeafHandle pLeaf;

	Result = rTree.createLeaf( pParent, Name, rVariant, &pLeaf ) == S_OK;

	if( nullptr != ppItem )
	{
		*ppItem = pLeaf;
	}

	return Result;
}

template< typename SVTreeType >
bool SVNavigateTree::SetItem( SVTreeType &rTree, typename SVTreeType::SVBranchHandle pParent, LPCTSTR Name, const _variant_t &rVariant, typename SVTreeType::SVLeafHandle* ppItem )
{
	bool Result( false );

	SVTreeType::SVLeafHandle pLeaf;

	Result = GetItemLeaf( rTree, Name, pParent, pLeaf );

	if ( !Result )
	{
		Result = AddItem( rTree, pParent, szName, rVariant, &l_Leaf );
	}
	else
	{
		Result = rTree.setLeafData( l_Leaf, rVariant ) == S_OK;
	}

	if( nullptr != ppItem )
	{
		*ppItem = l_Leaf;
	}

	return Result;
}

template< typename SVTreeType >
bool SVNavigateTree::AddBranch( SVTreeType &rTree, typename SVTreeType::SVBranchHandle pParent, LPCTSTR Name, typename SVTreeType::SVBranchHandle* ppItem )
{
	bool Result( false );

	SVTreeType::SVBranchHandle pBranch( nullptr );

	Result = rTree.createBranch( pParent, Name, &pBranch ) == S_OK;

	if( nullptr != ppItem )
	{
		*ppItem = pBranch;
	}

	return Result;
}

template< typename SVTreeType >
bool SVNavigateTree::SetBranch(SVTreeType &rTree, typename SVTreeType::SVBranchHandle pParent, LPCTSTR Name, typename SVTreeType::SVBranchHandle *ppItem)
{
	bool bOk = false;

	SVTreeType::SVBranchHandle pBranch( nullptr );

	bOk = GetItemBranch( rTree, Name, pParent, pBranch );

	if ( ! bOk )
	{
		bOk = AddBranch( rTree, pParent, Name, pBranch );
	}

	if( nullptr != ppItem )
	{
		*ppItem = pBranch;
	}

	return bOk;
}

template< typename SVTreeType >
bool SVNavigateTree::GetItemBranch( SVTreeType &rTree, LPCTSTR Name, typename SVTreeType::SVBranchHandle pParent, typename SVTreeType::SVBranchHandle& rpBranch )
{
	rpBranch = rTree.findBranch( pParent, Name );
	return (nullptr != rpBranch);
}

template< typename SVTreeType >
bool SVNavigateTree::GetItemLeaf( SVTreeType &rTree, LPCTSTR Name, typename SVTreeType::SVBranchHandle pParent, typename SVTreeType::SVLeafHandle& rpItem )
{
	rpItem = rTree.findLeaf( pParent, Name );
	return rTree.isValidLeaf(pParent, rpItem);
}

template< typename SVTreeType >
bool SVNavigateTree::GetItem( SVTreeType &rTree, LPCTSTR Name, typename SVTreeType::SVBranchHandle pParent, _variant_t &rVariant )
{
	bool Result( false );
	SVTreeType::SVLeafHandle pLeaf;

	rVariant.Clear();

	Result = GetItemLeaf( rTree, Name, pParent, pLeaf );
	if ( Result )
	{
		rVariant = rTree.getLeafData( pLeaf );
	}

	return Result;
}

template< typename SVTreeType >
bool SVNavigateTree::DeleteItem(SVTreeType &rTree, typename SVTreeType::SVBranchHandle pItem)
{
	bool bOk = ( rTree.deleteBranch( pItem ) == S_OK );
	return bOk;
}

template< typename SVTreeType >
bool SVNavigateTree::DeleteItem(SVTreeType &rTree, typename SVTreeType::SVBranchHandle pParent, typename SVTreeType::SVLeafHandle pItem)
{
	bool bOk = ( rTree.deleteLeaf( pParent, pItem ) == S_OK );
	return bOk;
}

template< typename SVTreeType >
bool SVNavigateTree::DeleteAllItems( SVTreeType &rTree )
{
	bool bOk = ( rTree.Clear() == S_OK );
	return bOk;
}

template< typename SVTreeType >
bool SVNavigateTree::HasChildren(SVTreeType& rTree, typename SVTreeType::SVBranchHandle pItem)
{
	return ( rTree.hasBranches( pItem ) || rTree.hasLeaves( pItem ) );
}

template< typename SVTreeType, typename SVPredicate >
bool SVNavigateTree::FindBranch( SVTreeType &rTree, typename SVTreeType::SVBranchHandle startBranch, const SVPredicate& rPredicate, typename SVTreeType::SVBranchHandle& rFoundItem )
{
	bool status = rPredicate( startBranch );

	if( status )
	{
		rFoundItem = startBranch;
	}
	else
	{
		typedef std::deque< typename SVTreeType::SVBranchHandle > SVProcessChildHandleDeque;

		SVProcessChildHandleDeque l_ProcessHandles;

		l_ProcessHandles.push_back( startBranch );

		SVProcessChildHandleDeque::iterator l_Iter = l_ProcessHandles.begin();

		while( ! status && l_Iter != l_ProcessHandles.end() )
		{
			SVTreeType::SVBranchHandle localBranch = *l_Iter;

			l_ProcessHandles.erase( l_Iter );

			SVTreeType::SVBranchHandle childBranch( rTree.getFirstBranch( localBranch ) );

			while( ! status && rTree.isValidBranch( childBranch ) )
			{
				status = rPredicate( childBranch );

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


