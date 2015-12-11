//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLTree
//* .File Name       : $Workfile:   SVXMLTree.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:58:00  $
//******************************************************************************


namespace Seidenader { namespace SVXMLLibrary
{
	template<typename SVT_BRANCH_HANDLE, typename SVT_LEAF_HANDLE>
	SVXMLTree< SVT_BRANCH_HANDLE, SVT_LEAF_HANDLE >::SVXMLTree()
	{
	}

	template<typename SVT_BRANCH_HANDLE, typename SVT_LEAF_HANDLE>
	SVXMLTree< SVT_BRANCH_HANDLE, SVT_LEAF_HANDLE >::~SVXMLTree()
	{
	}

	// Clear () -----------------------------------------------------------------
	//  This function incorporates a framework that will clear the tree as long 
	//  as the required virtual functions have been developed.  If there is 
	//  already a function in the supported derived tree to clear itself, then 
	//  this function should be overridden in the derived class, and the virtual 
	//  functions required only to implement this function are not neccessary.
	//  
	//  Virtual functions used only through calls to Clear ():
	//    DeleteNode ()
	template<typename SVT_BRANCH_HANDLE, typename SVT_LEAF_HANDLE>
	HRESULT SVXMLTree< SVT_BRANCH_HANDLE, SVT_LEAF_HANDLE >::Clear()
	{
		HRESULT	l_Status( S_OK );

		SVBranchHandle pBranch( getRoot() );

		if( nullptr != pBranch )
		{
			l_Status = deleteBranch( pBranch );
		}
		return l_Status;
	}
} /* namespace SVXMLLibrary */ } /* namespace Seidenader */

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVXMLTree.inl_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:58:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Oct 2012 10:39:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code and projects to fix whitespace issues in the XML file and consolidate functionality for load and saving to this library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 18:12:40   jspila
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