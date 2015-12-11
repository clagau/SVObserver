//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLCTreeCtrl
//* .File Name       : $Workfile:   SVXMLCTreeCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:50:52  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVXMLTree.h"
#pragma endregion Includes

namespace Seidenader { namespace SVXMLLibrary
{
	class SVXMLCTreeCtrl : public SVXMLTree< HTREEITEM, HTREEITEM >  
	//Note in this case the Template arguments HTREEITEM = SVBranchHandle and HTREEITEM = SVLeafHandle
	{
	#pragma region Constructor
	public:
		//************************************
		//! The class constructor
		//! \param rTree [in] reference to a copy of the tree to create
		//************************************
		SVXMLCTreeCtrl( CTreeCtrl& rTree );

		//************************************
		//! The class constructor
		//! \param rTree [in] reference to a copy of the tree to create
		//! \param pBranch [in] handle to the root branch
		//************************************
		SVXMLCTreeCtrl( CTreeCtrl& rTree, SVBranchHandle pBranch );

		//************************************
		//! The class constructor
		//! \param rTree [in] reference to a copy of the tree to create
		//************************************
		SVXMLCTreeCtrl( SVXMLCTreeCtrl& rTree );

		//************************************
		//! The class constructor
		//! \param rTree [in] reference to a copy of the tree to create
		//! \param pBranch [in] handle to the root branch
		//************************************
		SVXMLCTreeCtrl( SVXMLCTreeCtrl& rTree, SVBranchHandle pBranch );

		virtual ~SVXMLCTreeCtrl();
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		//************************************
		//! Method to get the total count of leafs and nodes
		//! \returns the count of the tree elements
		//************************************
		virtual size_t getCount() const;

		//************************************
		//! Method to get main parent branch
		//! \returns a handle to the main branch
		//************************************
		virtual SVBranchHandle getRoot();

		//************************************
		//! Method to determine if the branch is the root branch
		//! \param pBranch [in] handle to the branch to check
		//! \returns true if branch is root
		//************************************
		virtual bool isRoot( const SVBranchHandle pBranch ) const;

		//************************************
		//! Method to determine if the branch has branch children
		//! \param pBranch [in] handle to the branch to check
		//! \returns true if branch has children branches
		//************************************
		virtual bool hasBranches( const SVBranchHandle pBranch );

		//************************************
		//! Method to get the parent branch
		//! \param pBranch [in] handle to the branch to check
		//! \returns handle to the parent branch
		//************************************
		virtual SVBranchHandle getParentBranch( const SVBranchHandle pChild );

		//************************************
		//! Method to get the first child branch
		//! \param pBranch [in] handle to the branch to check
		//! \returns handle to the corresponding branch
		//************************************
		virtual SVBranchHandle getFirstBranch( const SVBranchHandle pParent );

		//************************************
		//! Method to get the next child branch
		//! \param pParent [in] handle to the parent branch
		//! \param pBranch [in] handle to the current branch
		//! \returns handle to the next branch
		//************************************
		virtual SVBranchHandle getNextBranch( const SVBranchHandle pParent, const SVBranchHandle pChild );

		//************************************
		//! Method to find a child branch
		//! \param pParent [in] handle to the parent branch
		//! \param Name [in] name of child branch to search for
		//! \returns handle to the found branch
		//************************************
		virtual SVBranchHandle findBranch( const SVBranchHandle pParent, LPCTSTR Name );

		//************************************
		//! Method to check if branch is valid
		//! \param pParent [in] handle to the parent branch
		//! \returns true if valid
		//************************************
		virtual bool isValidBranch( const SVBranchHandle pBranch );

		//************************************
		//! Method to create a new child branch
		//! \param pParent [in] handle to the parent branch
		//! \param Name [in] name of child branch to create
		//! \param ppBranch [out] pointer to the resulting child handle (default is nullptr)
		//! \returns S_OK on success
		//************************************
		virtual HRESULT createBranch( const SVBranchHandle pParent, LPCTSTR Name, SVBranchHandle* ppBranch=nullptr  );

		//************************************
		//! Method to delete a branch
		//! \param pBranch [in] handle to the branch to delete
		//! \returns S_OK on success
		//************************************
		virtual HRESULT deleteBranch( SVBranchHandle& rpBranch );

		//************************************
		//! Method to get the branch name
		//! \param pBranch [in] handle to the branch
		//! \returns the branch name
		//************************************
		virtual std::string	getBranchName( const SVBranchHandle pBranch ) const;

		//************************************
		//! Method to determine if the branch has child leaves
		//! \param pBranch [in] handle to the branch to check
		//! \returns true if branch has leaves
		//************************************
		virtual bool hasLeaves( const SVBranchHandle pBranch );

		//************************************
		//! Method to get the first child leaf
		//! \param pBranch [in] handle to the branch to check
		//! \returns handle to the corresponding leaf
		//************************************
		virtual SVLeafHandle getFirstLeaf( const SVBranchHandle pParent );

		//************************************
		//! Method to get the next child leaf
		//! \param pParent [in] handle to the parent branch
		//! \param pLeaf [in] handle to the current leaf
		//! \returns handle to the next leaf
		//************************************
		virtual SVLeafHandle getNextLeaf( const SVBranchHandle pParent, const SVLeafHandle pLeaf );

		//************************************
		//! Method to find a child leaf
		//! \param pParent [in] handle to the parent branch
		//! \param Name [in] name of child leaf to search for
		//! \returns handle to the found leaf
		//************************************
		virtual SVLeafHandle findLeaf( const SVBranchHandle pParent, LPCTSTR Name );

		//************************************
		//! Method to check if leaf is valid
		//! \param pParent [in] handle to the parent branch
		//! \param pLeaf [in] handle to leaf to check
		//! \returns true if valid
		//************************************
		virtual bool isValidLeaf( const SVBranchHandle pParent, const SVLeafHandle pLeaf );

		//************************************
		//! Method to create a new child leaf
		//! \param pParent [in] handle to the parent branch
		//! \param Name [in] name of child leaf to create
		//! \param ppLeaf [out] pointer to the resulting child leaf (default is nullptr)
		//! \returns S_OK on success
		//************************************
		virtual HRESULT createLeaf( const SVBranchHandle pParent, LPCTSTR Name, const VARIANT& rData, SVLeafHandle* ppLeaf=nullptr );

		//************************************
		//! Method to delete a leaf
		//! \param pLeaf [in] handle to the leaf
		//! \returns S_OK on success
		//************************************
		virtual HRESULT deleteLeaf( const SVLeafHandle pLeaf );

		//************************************
		//! Method to get the leaf name
		//! \param pLeaf [in] handle to the leaf
		//! \returns the leaf name
		//************************************
		virtual std::string	getLeafName( const SVLeafHandle pLeaf ) const;

		//************************************
		//! Method to get the leaf data
		//! \param pLeaf [in] handle to the leaf
		//! \returns the leaf data as a variant
		//************************************
		virtual VARIANT	getLeafData( const SVLeafHandle pLeaf ) const;

		//************************************
		//! Method to get the leaf data
		//! \param pParent [in] handle to the parent branch
		//! \param Name [in] name of child leaf to get the data
		//! \returns the leaf data as a variant
		//************************************
		virtual VARIANT	getLeafData( const SVBranchHandle pParent, LPCTSTR Name );

		//************************************
		//! Method to set the leaf data
		//! \param pLeaf [in] handle to the leaf
		//! \param rData [in] reference to the data to set
		//! \returns S_OK on success
		//************************************
		virtual HRESULT	setLeafData( const SVLeafHandle pLeaf, const VARIANT& rData );
	#pragma endregion Public Methods

	#pragma region Member variables
	private:
		CTreeCtrl& m_rTree;		//The tree control
		HTREEITEM m_Root;		//Handle to the root item
	#pragma endregion Member variables
	};
} /* namespace SVXMLLibrary */ } /* namespace Seidenader */

namespace SvXml = Seidenader::SVXMLLibrary;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVXMLLibrary\SVXMLCTreeCtrl.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:50:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Oct 2012 10:40:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code and projects to fix whitespace issues in the XML file and consolidate functionality for load and saving to this library.
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
