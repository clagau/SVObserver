//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLTree
//* .File Name       : $Workfile:   SVXMLTree.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:58:00  $
//******************************************************************************


namespace SvXml
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
} //namespace SvXml

