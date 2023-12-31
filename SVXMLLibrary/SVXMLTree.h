//******************************************************************************
//* COPYRIGHT (c) 2008 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLTree
//* .File Name       : $Workfile:   SVXMLTree.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:57:44  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#pragma endregion Includes
/**
@SVObjectName XML Tree Base

@SVObjectOverview This class is a base class from which to derive tree classes that will interact with the SVR XML classes (SVXMLClass, and SVDOMClass).  The derived class and associated tree structure must have a root node defined (same type as other nodes, not a list).  This root node may contain children, but may not contain data. Zero is not allowed to be a legitimate tree handle.  A handle of zero will represent no handle.... as in could not be found or allocated.

@SVObjectOperations

*/
namespace SvXml
{
	template<typename SVT_BRANCH_HANDLE, typename SVT_LEAF_HANDLE>
	class SVXMLTree  
	{
	#pragma region Declarations
	public:
		typedef SVT_BRANCH_HANDLE SVBranchHandle;
		typedef SVT_LEAF_HANDLE SVLeafHandle;

		typedef std::vector< SVBranchHandle > SVBranches;
		typedef std::vector< SVLeafHandle > SVLeaves;

	#pragma endregion Declarations

	#pragma region Constructor
	public:
		SVXMLTree();

		virtual ~SVXMLTree();
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		//************************************
		//! Method to clear the tree
		//! \returns S_OK on success
		//************************************
		virtual HRESULT Clear();

		//************************************
		//! Method to get the total count of leafs and nodes
		//! \returns the count of the tree elements
		//************************************
		virtual size_t getCount() const = 0;

		//************************************
		//! Method to get main parent branch
		//! \returns a handle to the main branch
		//************************************
		virtual SVBranchHandle getRoot() = 0;

		//************************************
		//! Method to determine if the branch is the root branch
		//! \param pBranch [in] handle to the branch to check
		//! \returns true if branch is root
		//************************************
		virtual bool isRoot( const SVBranchHandle pBranch ) const = 0;

		//************************************
		//! Method to determine if the branch has branch children
		//! \param pBranch [in] handle to the branch to check
		//! \returns true if branch has children branches
		//************************************
		virtual bool hasBranches( const SVBranchHandle pBranch ) = 0;

		//************************************
		//! Method to get the parent branch
		//! \param pBranch [in] handle to the branch to check
		//! \returns handle to the parent branch
		//************************************
		virtual SVBranchHandle getParentBranch( const SVBranchHandle pChild ) const = 0;

		//************************************
		//! Method to get the first child branch
		//! \param pBranch [in] handle to the branch to check
		//! \returns handle to the corresponding branch
		//************************************
		virtual SVBranchHandle	getFirstBranch( const SVBranchHandle pParent ) = 0;

		//************************************
		//! Method to get the next child branch
		//! \param pParent [in] handle to the parent branch
		//! \param pBranch [in] handle to the current branch
		//! \returns handle to the next branch
		//************************************
		virtual SVBranchHandle getNextBranch( const SVBranchHandle pParent, const SVBranchHandle pBranch ) = 0;
		virtual SVBranchHandle getPreviousBranch(const SVBranchHandle pParent, const SVBranchHandle pBranch) = 0;

		//************************************
		//! Method to find a child branch
		//! \param pParent [in] handle to the parent branch
		//! \param Name [in] name of child branch to search for
		//! \returns handle to the found branch
		//************************************
		virtual SVBranchHandle findBranch( const SVBranchHandle pParent, LPCTSTR Name ) = 0;

		//************************************
		//! Method to check if branch is valid
		//! \param pParent [in] handle to the parent branch
		//! \returns true if valid
		//************************************
		virtual bool isValidBranch( const SVBranchHandle pParent ) const = 0;

		//************************************
		//! Method to create a new child branch
		//! \param pParent [in] handle to the parent branch
		//! \param Name [in] name of child branch to create
		//! \param ppBranch [out] pointer to the resulting child handle (default is nullptr)
		//! \returns S_OK on success
		//************************************
		virtual HRESULT createBranch( const SVBranchHandle pParent, LPCTSTR Name, SVBranchHandle* ppBranch=nullptr ) = 0;

		//************************************
		//! Method to delete a branch
		//! \param pBranch [in] handle to the branch to delete
		//! \returns S_OK on success
		//************************************
		virtual HRESULT deleteBranch( SVBranchHandle& rpBranch ) = 0;

		//************************************
		//! Method to get the branch name
		//! \param pBranch [in] handle to the branch
		//! \returns the branch name
		//************************************
		virtual std::string getBranchName( const SVBranchHandle pBranch ) const = 0;

		//************************************
		//! Method to determine if the branch has child leaves
		//! \param pBranch [in] handle to the branch to check
		//! \returns true if branch has leaves
		//************************************
		virtual bool hasLeaves( const SVBranchHandle pBranch ) = 0;

		//************************************
		//! Method to get the first child leaf
		//! \param pBranch [in] handle to the branch to check
		//! \returns handle to the corresponding leaf
		//************************************
		virtual SVLeafHandle getFirstLeaf( const SVBranchHandle pParent ) = 0;

		//************************************
		//! Method to get the next child leaf
		//! \param pParent [in] handle to the parent branch
		//! \param pLeaf [in] handle to the current leaf
		//! \returns handle to the next leaf
		//************************************
		virtual SVLeafHandle getNextLeaf( const SVBranchHandle pParent, const SVLeafHandle pLeaf ) = 0;

		//************************************
		//! Method to find a child leaf
		//! \param pParent [in] handle to the parent branch
		//! \param Name [in] name of child leaf to search for
		//! \returns handle to the found leaf
		//************************************
		virtual SVLeafHandle findLeaf( const SVBranchHandle pParent, LPCTSTR Name ) = 0;

		//************************************
		//! Method to check if leaf is valid
		//! \param pParent [in] handle to the parent branch
		//! \param pLeaf [in] handle to leaf to check
		//! \returns true if valid
		//************************************
		virtual bool isValidLeaf( const SVBranchHandle pParent, const SVLeafHandle pLeaf ) = 0;

		//************************************
		//! Method to create a new child leaf
		//! \param pParent [in] handle to the parent branch
		//! \param Name [in] name of child leaf to create
		//! \param rData [in] reference to the data
		//! \param ppLeaf [out] pointer to the resulting child leaf (default is nullptr)
		//! \returns S_OK on success
		//************************************
		virtual HRESULT createLeaf( const SVBranchHandle pParent, LPCTSTR Name, const _variant_t& rData, SVLeafHandle* ppLeaf=nullptr ) = 0;

		//************************************
		//! Method to delete a leaf
		//! \param pLeaf [in] handle to the leaf to delete
		//! \returns S_OK on success
		//************************************
		virtual HRESULT deleteLeaf( const SVLeafHandle pLeaf ) = 0;

		//************************************
		//! Method to get the leaf name
		//! \param pLeaf [in] handle to the leaf
		//! \returns the leaf name
		//************************************
		virtual std::string getLeafName( const SVLeafHandle pLeaf ) const = 0;

		//************************************
		//! Method to get the leaf data
		//! \param pLeaf [in] handle to the leaf
		//! \param rData [out] reference to a variant for the data
		//! \returns S_OK on success
		//************************************
		virtual HRESULT	getLeafData( const SVLeafHandle pLeaf, _variant_t& rData ) const = 0;

		//************************************
		//! Method to get the leaf data
		//! \param pParent [in] handle to the parent branch
		//! \param Name [in] name of child leaf to get the data
		//! \param rData [out] reference to a variant for the data
		//! \returns S_OK on success
		//************************************
		virtual HRESULT	getLeafData( const SVBranchHandle pParent, LPCTSTR Name, _variant_t& rData ) = 0;

		//************************************
		//! Method to set the leaf data
		//! \param pLeaf [in] handle to the leaf
		//! \param rData [in] reference to the data to set
		//! \returns S_OK on success
		//************************************
		virtual HRESULT	setLeafData( const SVLeafHandle pLeaf, const _variant_t& rData ) = 0;
	#pragma endregion Public Methods
	};
} //namespace SvXml

#include "SVXMLTree.inl"

