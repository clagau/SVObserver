//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVXMLMaterialsTree
//* .File Name       : $Workfile:   SVXMLMaterialsTree.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Dec 2014 04:46:14  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVXMLTree.h"
#include "SVMaterialData.h"
#include "SVContainerLibrary/SVTree.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

typedef SvCl::SVTree<std::string, SvXml::SVMaterialData> SVMaterialsTree;

namespace SvXml
{
	class SVXMLMaterialsTree : public SVXMLTree< SVMaterialsTree::SVTreeContainer*, SVMaterialsTree::iterator >
	//Note in this case the Template arguments SVMaterialsTree::SVTreeContainer* = SVBranchHandle and SVMaterialsTree::iterator = SVLeafHandle
	{
	#pragma region Constructor
	public:
		SVXMLMaterialsTree();

		//************************************
		//! The class constructor
		//! \param rTree [in] reference to a copy of the tree to create
		//************************************
		explicit SVXMLMaterialsTree(const SVMaterialsTree::SVTreeContainer& rTree);

		virtual ~SVXMLMaterialsTree();
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		//************************************
		//! Method to get the total count of leafs and nodes
		//! \returns the count of the tree elements
		//************************************
		virtual size_t getCount() const override;

		//************************************
		//! Method to get main parent branch
		//! \returns a handle to the main branch
		//************************************
		virtual SVBranchHandle getRoot() override;

		//************************************
		//! Method to determine if the branch is the root branch
		//! \param pBranch [in] handle to the branch to check
		//! \returns true if branch is root
		//************************************
		virtual bool isRoot( const SVBranchHandle pBranch ) const override;

		//************************************
		//! Method to determine if the branch has branch children
		//! \param pBranch [in] handle to the branch to check
		//! \returns true if branch has children branches
		//************************************
		virtual bool hasBranches( const SVBranchHandle pBranch ) override;

		//************************************
		//! Method to get the parent branch
		//! \param pBranch [in] handle to the branch to check
		//! \returns handle to the parent branch
		//************************************
		virtual SVBranchHandle getParentBranch( const SVBranchHandle pChild ) override;

		//************************************
		//! Method to get the first child branch
		//! \param pBranch [in] handle to the branch to check
		//! \returns handle to the corresponding branch
		//************************************
		virtual SVBranchHandle getFirstBranch( const SVBranchHandle pParent ) override;

		//************************************
		//! Method to get the next child branch
		//! \param pParent [in] handle to the parent branch
		//! \param pBranch [in] handle to the current branch
		//! \returns handle to the next branch
		//************************************
		virtual SVBranchHandle getNextBranch( const SVBranchHandle pParent, const SVBranchHandle pChild ) override;
		virtual SVBranchHandle getPreviousBranch(const SVBranchHandle pParent, const SVBranchHandle pBranch) override;

		//************************************
		//! Method to find a child branch
		//! \param pParent [in] handle to the parent branch
		//! \param Name [in] name of child branch to search for
		//! \returns handle to the found branch
		//************************************
		virtual SVBranchHandle findBranch( const SVBranchHandle pParent, LPCTSTR Name ) override;

		std::vector<SVBranchHandle> getSubbranches(const SVBranchHandle pParent);

		//************************************
		//! Method to check if branch is valid
		//! \param pParent [in] handle to the parent branch
		//! \returns true if valid
		//************************************
		virtual bool isValidBranch( const SVBranchHandle pBranch ) override;

		//************************************
		//! Method to create a new child branch
		//! \param pParent [in] handle to the parent branch
		//! \param Name [in] name of child branch to create
		//! \param ppBranch [out] pointer to the resulting child handle (default is nullptr)
		//! \returns S_OK on success
		//************************************
		virtual HRESULT createBranch( const SVBranchHandle pParent, LPCTSTR Name, SVBranchHandle* ppBranch=nullptr  ) override;

		//************************************
		//! Method to delete a branch
		//! \param pBranch [in] handle to the branch to delete
		//! \returns S_OK on success
		//************************************
		virtual HRESULT deleteBranch( SVBranchHandle& rpBranch ) override;

		//************************************
		//! Method to get the branch name
		//! \param pBranch [in] handle to the branch
		//! \returns the branch name
		//************************************
		virtual std::string getBranchName( const SVBranchHandle pBranch ) const override;

		//************************************
		//! Method to determine if the branch has child leaves
		//! \param pBranch [in] handle to the branch to check
		//! \returns true if branch has leaves
		//************************************
		virtual bool hasLeaves( const SVBranchHandle pBranch ) override;

		//************************************
		//! Method to get the first child leaf
		//! \param pBranch [in] handle to the branch to check
		//! \returns handle to the corresponding leaf
		//************************************
		virtual SVLeafHandle getFirstLeaf( const SVBranchHandle pParent ) override;

		//************************************
		//! Method to get the next child leaf
		//! \param pParent [in] handle to the parent branch
		//! \param pLeaf [in] handle to the current leaf
		//! \returns handle to the next leaf
		//************************************
		virtual SVLeafHandle getNextLeaf( const SVBranchHandle pParent, const SVLeafHandle pLeaf ) override;

		//************************************
		//! Method to find a child leaf
		//! \param pParent [in] handle to the parent branch
		//! \param Name [in] name of child leaf to search for
		//! \returns handle to the found leaf
		//************************************
		virtual SVLeafHandle findLeaf( const SVBranchHandle pParent, LPCTSTR Name ) override;

		//************************************
		//! Method to check if leaf is valid
		//! \param pParent [in] handle to the parent branch
		//! \param pLeaf [in] handle to leaf to check
		//! \returns true if valid
		//************************************
		virtual bool isValidLeaf( const SVBranchHandle pParent, const SVLeafHandle pLeaf ) override;

		//************************************
		//! Method to create a new child leaf
		//! \param pParent [in] handle to the parent branch
		//! \param Name [in] name of child leaf to create
		//! \param ppLeaf [out] pointer to the resulting child leaf (default is nullptr)
		//! \returns S_OK on success
		//************************************
		virtual HRESULT createLeaf( const SVBranchHandle pParent, LPCTSTR Name, const _variant_t& rData, SVLeafHandle* ppLeaf=nullptr ) override;

		//************************************
		//! Method to delete a leaf
		//! \param pLeaf [in] handle to the leaf
		//! \returns S_OK on success
		//************************************
		virtual HRESULT deleteLeaf( const SVLeafHandle pLeaf ) override;

		//************************************
		//! Method to get the leaf name
		//! \param pLeaf [in] handle to the leaf
		//! \returns the leaf name
		//************************************
		virtual std::string getLeafName( const SVLeafHandle pLeaf ) const override;

		//************************************
		//! Method to get the leaf data
		//! \param pLeaf [in] handle to the leaf
		//! \param rData [out] reference to a variant for the data
		//! \returns S_OK on success
		//************************************
		virtual HRESULT	getLeafData( const SVLeafHandle pLeaf, _variant_t& rData ) const override;

		//************************************
		//! Method to get the leaf data
		//! \param pParent [in] handle to the parent branch
		//! \param Name [in] name of child leaf to get the data
		//! \param rData [out] reference to a variant for the data
		//! \returns S_OK on success
		//************************************
		virtual HRESULT	getLeafData( const SVBranchHandle pParent, LPCTSTR Name, _variant_t& rData ) override;

		//************************************
		//! Method to set the leaf data
		//! \param pLeaf [in] handle to the leaf
		//! \param rData [in] reference to the data to set
		//! \returns S_OK on success
		//************************************
		virtual HRESULT	setLeafData( const SVLeafHandle pLeaf, const _variant_t& rData ) override;

		//************************************
		//! Method to get a list of leaf values
		//! \param pParent [in] handle to the parent branch
		//! \param rSearchName [in] reference to the name to search 
		//! \returns the list of values found
		//************************************
		SvDef::StringVector getLeafValues( const SVBranchHandle pParent, const std::string& rSearchName);

		//************************************
		//! Method to get the root tree
		//! \returns the root tree
		//************************************
		SVMaterialsTree& getTree() { return m_Tree; };
	#pragma endregion Public Methods

	#pragma region Private Methods
	private:
		//************************************
		//! Method to set the children's list for quicker search
		//! \returns true on success
		//************************************
		bool setChildren( const SVBranchHandle pParent ) const;
	#pragma endregion Private Methods

	#pragma region Member variables
	private:
		SVMaterialsTree m_Tree;						//The materials tree container
		mutable SVBranchHandle m_pCurrentParent;	//Handle to the current branch
		mutable SVLeafHandle m_pCurrentLeaf;		//Handle to the current Leaf
		mutable SVBranches m_ChildBranches;			//Handle to the current child branches
		mutable SVLeaves m_ChildLeaves;				//Handle to the current child leaves
	#pragma endregion Member variables
	};
} //namespace SvXml

typedef SvXml::SVXMLMaterialsTree SVTreeType;
