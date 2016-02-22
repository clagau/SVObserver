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

template< typename SVToTreeType, typename SVFromTreeType >
bool SVNavigateTree::Copy( SVToTreeType &rToTree, typename SVToTreeType::SVBranchHandle toParent, const SVFromTreeType &rFromTree, typename SVFromTreeType::SVBranchHandle fromBranch )
{
	bool ok = true;

	typename SVFromTreeType::SVBranchHandle fromChildBranch( nullptr );

	SVFromTreeType& fromTree( const_cast< SVFromTreeType& >( rFromTree ) );

	fromChildBranch = fromTree.getFirstBranch( fromBranch );

	while( nullptr != fromChildBranch )
	{
		SVString Name;

		Name = fromTree.getBranchName( fromChildBranch );

		typename SVToTreeType::SVBranchHandle toChildBranch;

		 rToTree.createBranch( toParent, Name.c_str(), &toChildBranch );

		if( nullptr != toChildBranch )
		{
			Copy( rToTree, toChildBranch, rFromTree, fromChildBranch );
		}

		fromChildBranch = fromTree.getNextBranch( fromBranch, fromChildBranch );
	}

	typename SVFromTreeType::SVLeafHandle fromChildLeaf;

	fromChildLeaf = fromTree.getFirstLeaf( fromBranch );

	while( fromTree.isValidLeaf( fromBranch, fromChildLeaf ) )
	{
		SVString Name;

		_variant_t Data;

		Name = fromTree.getLeafName( fromChildLeaf );

		Data = fromTree.getLeafData( fromChildLeaf );

		rToTree.createLeaf( toParent, Name.c_str(), Data );

		fromChildLeaf = fromTree.getNextLeaf( fromBranch, fromChildLeaf );
	}

	return ok;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVNavigateTreeClass.inl_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:47:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Mar 2013 13:22:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Copy method to report correct status and to fix recursion issue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Oct 2012 10:40:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code and projects to fix whitespace issues in the XML file and consolidate functionality for load and saving to this library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   27 Sep 2012 18:34:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated type definitions from HTREEITEM to generic branch handle type and fixed initialization problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Sep 2012 15:13:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Templated class and move code to source include file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Sep 2012 18:24:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Sep 2012 15:27:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Navigate Tree class to not force the programmer to create an instance of the class to use it.  Converted all functionality to static.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Nov 2010 11:08:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Oct 2005 08:58:42   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  514
 * SCR Title:  Fix Memory leak in Save Configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with the VARIANTs in the ItemData were not being cleaned up.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 11:24:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 12:57:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
