//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionTreeParser
//* .File Name       : $Workfile:   SVInspectionTreeParser.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   19 Dec 2014 04:10:42  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectScriptParserBase.h"
#include "SVObjectLibrary/SVObjectScriptEnums.h"
#include "SVXMLLibrary/SVXMLMaterialsTree.h"
#pragma endregion Includes

class SVObjectClass;
class SVObjectScriptParser;
namespace SvXml
{
class SVXMLMaterialsTree;
}

class SVInspectionTreeParser : public SVObjectScriptParserBase
{
private:
	SvXml::SVXMLMaterialsTree& m_rTree;
	SvXml::SVXMLMaterialsTree::SVBranchHandle m_rootItem;
	size_t m_count;
	size_t m_totalSize;
	bool m_ReplaceUniqueID;

public:
	SVInspectionTreeParser(SvXml::SVXMLMaterialsTree& rTreeCtrl, SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, unsigned long parserHandle, uint32_t OwnerId, SVObjectClass* pOwnerObject, CWnd* pWnd);
	virtual ~SVInspectionTreeParser();
	virtual HRESULT DoParse() override;
	virtual size_t GetTotal() const override;

	static HRESULT CreateInspectionObject(uint32_t& rInspectionId, SVTreeType& p_rTree, SvXml::SVXMLMaterialsTree::SVBranchHandle hItem);

private:
	HRESULT Process(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t ownerID);
	
	HRESULT ProcessChildren(SvXml::SVXMLMaterialsTree::SVBranchHandle hParentItem, uint32_t ownerID);

	HRESULT ProcessFriend(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t ownerID);
	HRESULT ProcessFriends(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t ownerID);
	
	HRESULT ProcessEmbeddeds(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t ownerID);
	HRESULT ProcessEmbedded(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t ownerID);
	HRESULT ProcessEmbeddedChilds(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t ownerID);
	HRESULT ProcessEmbeddedValues(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t ownerID, uint32_t objectID, SVObjectScriptDataObjectTypeEnum dataType);

	HRESULT ProcessBranchObjectValues(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t ownerID, uint32_t objectID, bool excludeDefaultAndArray);
	HRESULT ProcessLeafObjectValues(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t ownerID, uint32_t objectID, bool excludeDefaultAndArray);

	HRESULT ProcessEquation(uint32_t ownerID, const _variant_t& equation);

	HRESULT ProcessMaskData(uint32_t ownerID, const _variant_t& maskData);

	HRESULT ProcessInputs(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, uint32_t ownerID);
	
	HRESULT ProcessAttributes(uint32_t ownerID, uint32_t objectID, SvXml::SVXMLMaterialsTree::SVBranchHandle hItem);

	bool GetItemValue(const std::string& tag, SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, _variant_t& rValue);
	bool GetValues(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, const std::string& tag, std::vector<_variant_t>& rValueList);
	bool HasTag(SvXml::SVXMLMaterialsTree::SVBranchHandle hItem, const std::string& tag);
};

