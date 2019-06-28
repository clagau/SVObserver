//*****************************************************************************
/// \copyright (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// Class TreeToXml implement static functions to write an xml file from a SVtree 
//******************************************************************************
#pragma once

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <sstream>
#include "SVObjectXMLWriter.h"

#pragma endregion Includes

#pragma region Declarations
#pragma endregion Declarations

namespace SvXml
{
/// Class  implements  a static functions to write an xml file from a SVtree 
class TreeToXMl
{
private:
	TreeToXMl();
	~TreeToXMl();

public:

	/// Recursive Function which writes a node  of a SV_Tree to an XMLfile. 
	template<typename SVT_TREE>
	static bool  CopyTreeNodeToWriter(SVT_TREE& rTree, const typename SVT_TREE::SVBranchHandle& TreeNodeHandle, SVObjectXMLWriter& rWriter)
	{
		bool status(true);
		std::string  TreeNodeName;
		TreeNodeName = rTree.getBranchName(TreeNodeHandle);
		if (TreeNodeHandle == rTree.getRoot() && TreeNodeName == "Base")
		{
			rWriter.WriteStartOfBase();
		}
		else
		{
			rWriter.StartElement(TreeNodeName.c_str());
		}
		if (rTree.hasLeaves(TreeNodeHandle))
		{

			SVT_TREE::SVLeafHandle pLeaf;
			pLeaf = rTree.getFirstLeaf(TreeNodeHandle);
			while (rTree.isValidLeaf(TreeNodeHandle, pLeaf))
			{
				std::string  Leafname;
				_variant_t leafdata;
				Leafname = rTree.getLeafName(pLeaf);
				if (0 == Leafname.length())
				{
					break;
				}
				rTree.getLeafData(pLeaf, leafdata);
				if (VT_EMPTY == leafdata.vt)
				{
					break;
				}

				rWriter.WriteAttribute(Leafname.c_str(), leafdata);
				pLeaf = rTree.getNextLeaf(TreeNodeHandle, pLeaf);
			}

		}

		if (rTree.hasBranches(TreeNodeHandle))
		{
			SVT_TREE::SVBranchHandle Child(nullptr);
			SVT_TREE::SVBranchHandle Parent(TreeNodeHandle);
			Child = rTree.getFirstBranch(TreeNodeHandle);
			while (nullptr != Child)
			{
				CopyTreeNodeToWriter(rTree, Child, rWriter);
				Child = rTree.getNextBranch(Parent, Child);
			}
		}
		rWriter.EndElement();
		return true;
	}
};
} //namespace SvXml
