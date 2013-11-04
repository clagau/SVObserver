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

#ifndef SVNAVIGATETREECLASS_INL
#define SVNAVIGATETREECLASS_INL

#include "SVNavigateTreeClass.h"

template< typename SVTreeType >
bool SVNavigateTreeClass::AddItem( SVTreeType &rTree, typename SVTreeType::SVBranchHandle htiParent, LPCTSTR szName, _variant_t &rsvVariant, typename SVTreeType::SVLeafHandle* phtiItem )
{
	bool bOk = false;

	SVTreeType::SVLeafHandle l_Leaf;

	bOk = rTree.CreateLeaf( htiParent, _bstr_t( szName ), rsvVariant, &l_Leaf ) == S_OK;

	if( phtiItem != NULL )
	{
		*phtiItem = l_Leaf;
	}

	return bOk;
}

template< typename SVTreeType >
bool SVNavigateTreeClass::SetItem( SVTreeType &rTree, typename SVTreeType::SVBranchHandle htiParent, LPCTSTR szName, _variant_t &rsvVariant, typename SVTreeType::SVLeafHandle* phtiItem )
{
	bool bOk = false;

	SVTreeType::SVLeafHandle l_Leaf;

	bOk = GetItemLeaf( rTree, szName, htiParent, l_Leaf );

	if ( ! bOk )
	{
		bOk = AddItem( rTree, htiParent, szName, rsvVariant, &l_Leaf );
	}
	else
	{
		bOk = rTree.SetLeafData( l_Leaf, rsvVariant ) == S_OK;
	}

	if( phtiItem != NULL )
	{
		*phtiItem = l_Leaf;
	}

	return bOk;
}

template< typename SVTreeType >
bool SVNavigateTreeClass::AddBranch( SVTreeType &rTree, typename SVTreeType::SVBranchHandle htiParent, LPCTSTR szName, typename SVTreeType::SVBranchHandle* phtiItem )
{
	bool bOk = false;

	SVTreeType::SVBranchHandle l_Branch;

	bOk = ( rTree.CreateBranch( htiParent, _bstr_t( szName ), &l_Branch ) == S_OK );

	if( phtiItem != NULL )
	{
		*phtiItem = l_Branch;
	}

	return bOk;
}

template< typename SVTreeType >
bool SVNavigateTreeClass::SetBranch(SVTreeType &rTree, typename SVTreeType::SVBranchHandle htiParent, LPCTSTR szName, typename SVTreeType::SVBranchHandle *phtiItem)
{
	bool bOk = false;

	SVTreeType::SVBranchHandle l_Branch;

	bOk = GetItemBranch( rTree, szName, htiParent, l_Branch );

	if ( ! bOk )
	{
		bOk = AddBranch( rTree, htiParent, szName, &l_Branch );
	}

	if( phtiItem != NULL )
	{
		*phtiItem = l_Branch;
	}

	return bOk;
}

template< typename SVTreeType >
bool SVNavigateTreeClass::GetItemBranch( SVTreeType &rTree, LPCTSTR lpszName, typename SVTreeType::SVBranchHandle htiParent, typename SVTreeType::SVBranchHandle& htiItem )
{
	bool bOk = ( rTree.FindBranch( htiParent, _bstr_t( lpszName ), htiItem ) == S_OK );
	return bOk;
}

template< typename SVTreeType >
bool SVNavigateTreeClass::GetItemLeaf( SVTreeType &rTree, LPCTSTR lpszName, typename SVTreeType::SVBranchHandle htiParent, typename SVTreeType::SVLeafHandle& htiItem )
{
	bool bOk = ( rTree.FindLeaf( htiParent, _bstr_t( lpszName ), htiItem ) == S_OK );
	return bOk;
}

template< typename SVTreeType >
bool SVNavigateTreeClass::GetItem( SVTreeType &rTree, LPCTSTR lpszName, typename SVTreeType::SVBranchHandle htiParent, _variant_t &rsvVariant )
{
	SVTreeType::SVLeafHandle l_Leaf;

	rsvVariant.Clear();

	bool bOk = GetItemLeaf( rTree, lpszName, htiParent, l_Leaf );
	if ( bOk )
	{
		bOk = ( rTree.GetLeafData( l_Leaf, rsvVariant.GetVARIANT() ) == S_OK );
	}
	return bOk;
}

template< typename SVTreeType >
bool SVNavigateTreeClass::DeleteItem(SVTreeType &rTree, typename SVTreeType::SVBranchHandle htiItem)
{
	bool bOk = ( rTree.DeleteBranch( htiItem ) == S_OK );
	return bOk;
}

template< typename SVTreeType >
bool SVNavigateTreeClass::DeleteItem(SVTreeType &rTree, typename SVTreeType::SVBranchHandle htiParent, typename SVTreeType::SVLeafHandle htiItem)
{
	bool bOk = ( rTree.DeleteLeaf( htiParent, htiItem ) == S_OK );
	return bOk;
}

template< typename SVTreeType >
bool SVNavigateTreeClass::DeleteAllItems( SVTreeType &rTree )
{
	bool bOk = ( rTree.Clear() == S_OK );
	return bOk;
}

template< typename SVTreeType >
bool SVNavigateTreeClass::HasChildren(SVTreeType& rTree, typename SVTreeType::SVBranchHandle hItem)
{
	return ( rTree.DoesBranchHaveBranches( hItem ) == S_OK || rTree.DoesBranchHaveLeaves( hItem ) == S_OK );
}

template< typename SVTreeType, typename SVPredicate >
bool SVNavigateTreeClass::FindBranch( SVTreeType &rTree, typename SVTreeType::SVBranchHandle startBranch, const SVPredicate& rPredicate, typename SVTreeType::SVBranchHandle& rFoundItem )
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
			SVTreeType::SVBranchHandle childBranch;
			SVTreeType::SVBranchHandle localBranch = *l_Iter;

			l_ProcessHandles.erase( l_Iter );

			rTree.GetFirstBranch( localBranch, childBranch );

			while( ! status && ( rTree.IsValidBranch( childBranch ) == S_OK ) )
			{
				status = rPredicate( childBranch );

				if( status )
				{
					rFoundItem = childBranch;
				}
				else
				{
					l_ProcessHandles.push_back( childBranch );

					rTree.GetNextBranch( localBranch, childBranch );
				}
			}

			l_Iter = l_ProcessHandles.begin();
		}
	}

	return status;
}

template< typename SVToTreeType, typename SVFromTreeType >
bool SVNavigateTreeClass::Copy( SVToTreeType &rToTree, typename SVToTreeType::SVBranchHandle toParent, const SVFromTreeType &rFromTree, typename SVFromTreeType::SVBranchHandle fromBranch )
{
	bool ok = true;

	typename SVFromTreeType::SVBranchHandle fromChild;

	SVFromTreeType& fromTree( const_cast< SVFromTreeType& >( rFromTree ) );

	HRESULT status = fromTree.GetFirstBranch( fromBranch, fromChild );

	while( status == S_OK )
	{
		_bstr_t l_Name;

		status = fromTree.GetBranchName( fromChild, l_Name.GetBSTR() );

		if( status == S_OK )
		{
			typename SVToTreeType::SVBranchHandle toChild;

			status = rToTree.CreateBranch( toParent, l_Name, &toChild );

			if( status == S_OK )
			{
				status = ( Copy( rToTree, toChild, rFromTree, fromChild ) ) ? S_OK : S_FALSE;
			}
		}

		if( status == S_OK )
		{
			status = fromTree.GetNextBranch( fromBranch, fromChild );
		}
	}

	ok = SV_SUCCEEDED( status );

	if( ok )
	{
		typename SVFromTreeType::SVLeafHandle fromChild;

		status = fromTree.GetFirstLeaf( fromBranch, fromChild );

		while( status == S_OK )
		{
			_bstr_t l_Name;
			_variant_t l_Data;

			status = fromTree.GetLeafName( fromChild, l_Name.GetBSTR() );

			if( status == S_OK )
			{
				status = fromTree.GetLeafData( fromChild, l_Data );

				if( status == S_OK )
				{
					status = rToTree.CreateLeaf( toParent, l_Name, l_Data );
				}
			}

			if( status == S_OK )
			{
				status = fromTree.GetNextLeaf( fromBranch, fromChild );
			}
		}

		ok = SV_SUCCEEDED( status );
	}

	return ok;
}

#endif

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
