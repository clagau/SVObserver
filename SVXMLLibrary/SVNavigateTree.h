//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVNavigateTreeClass
//* .File Name       : $Workfile:   SVNavigateTreeClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:47:32  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVXMLMaterialsTree.h"
#pragma endregion Includes


namespace  SvXml
{
using FindPredicate = std::function<bool(const SVXMLMaterialsTree::SVBranchHandle&)>;

class SVNavigateTree
{
private:
	SVNavigateTree();
	~SVNavigateTree();

public:
	static bool AddItem( SVXMLMaterialsTree &rTree, SVXMLMaterialsTree::SVBranchHandle pParent, LPCTSTR Name, const _variant_t &rVariant, SVXMLMaterialsTree::SVLeafHandle* ppItem = nullptr );
	static bool SetItem( SVXMLMaterialsTree &rTree, SVXMLMaterialsTree::SVBranchHandle pParent, LPCTSTR Name, const _variant_t &rVariant, SVXMLMaterialsTree::SVLeafHandle* ppItem = nullptr );
	static bool AddBranch( SVXMLMaterialsTree &rTree, SVXMLMaterialsTree::SVBranchHandle pParent, LPCTSTR Name, SVXMLMaterialsTree::SVBranchHandle* ppItem = nullptr );
	static bool SetBranch( SVXMLMaterialsTree &rTree, SVXMLMaterialsTree::SVBranchHandle pParent, LPCTSTR Name, SVXMLMaterialsTree::SVBranchHandle* ppItem = nullptr );
	static bool GetItemBranch( SVXMLMaterialsTree &rTree, LPCTSTR Name, SVXMLMaterialsTree::SVBranchHandle pParent, SVXMLMaterialsTree::SVBranchHandle& rpItem );
	static std::vector<SVXMLMaterialsTree::SVBranchHandle> findSubbranches(SVXMLMaterialsTree& rTree, SVXMLMaterialsTree::SVBranchHandle pParent);
	static bool GetItemLeaf( SVXMLMaterialsTree &rTree, LPCTSTR Name, SVXMLMaterialsTree::SVBranchHandle pParent, SVXMLMaterialsTree::SVLeafHandle& rpItem );
	static bool GetItem( SVXMLMaterialsTree &rTree, LPCTSTR Name, SVXMLMaterialsTree::SVBranchHandle pParent, _variant_t& rData );
	static bool DeleteItem( SVXMLMaterialsTree &rTree, SVXMLMaterialsTree::SVBranchHandle pItem );
	static bool DeleteItem( SVXMLMaterialsTree &rTree, SVXMLMaterialsTree::SVBranchHandle pParent, SVXMLMaterialsTree::SVLeafHandle pItem );
	static bool DeleteAllItems( SVXMLMaterialsTree &rTree );
	static bool HasChildren(SVXMLMaterialsTree& rTree, SVXMLMaterialsTree::SVBranchHandle pItem);
	static bool FindBranch( SVXMLMaterialsTree &rTree, SVXMLMaterialsTree::SVBranchHandle startBranch, FindPredicate findPredicate, SVXMLMaterialsTree::SVBranchHandle& rFoundItem );
};
} //namespace SvXml

