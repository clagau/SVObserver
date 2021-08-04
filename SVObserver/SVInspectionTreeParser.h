//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
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
#pragma endregion Includes

class SVObjectClass;
class SVObjectScriptParser;

template< typename SVTreeType >
class SVInspectionTreeParser : public SVObjectScriptParserBase
{
private:
	SVTreeType& m_rTree;
	typename SVTreeType::SVBranchHandle m_rootItem;
	size_t m_count;
	size_t m_totalSize;
	bool m_ReplaceUniqueID;

public:
	SVInspectionTreeParser(SVTreeType& rTreeCtrl, typename SVTreeType::SVBranchHandle hItem, unsigned long parserHandle, uint32_t OwnerId, SVObjectClass* pOwnerObject, CWnd* pWnd);
	virtual ~SVInspectionTreeParser();
	virtual HRESULT DoParse() override;
	virtual size_t GetTotal() const override;

	static HRESULT CreateInspectionObject(uint32_t& rInspectionId, SVTreeType& p_rTree, typename SVTreeType::SVBranchHandle hItem);

private:
	HRESULT Process(typename SVTreeType::SVBranchHandle hItem, uint32_t ownerID);
	
	HRESULT ProcessChildren(typename SVTreeType::SVBranchHandle hParentItem, uint32_t ownerID);

	HRESULT ProcessFriend(typename SVTreeType::SVBranchHandle hItem, uint32_t ownerID);
	HRESULT ProcessFriends(typename SVTreeType::SVBranchHandle hItem, uint32_t ownerID);
	
	HRESULT ProcessEmbeddeds(typename SVTreeType::SVBranchHandle hItem, uint32_t ownerID);
	HRESULT ProcessEmbedded(typename SVTreeType::SVBranchHandle hItem, uint32_t ownerID);
	HRESULT ProcessEmbeddedChilds(typename SVTreeType::SVBranchHandle hItem, uint32_t ownerID);
	HRESULT ProcessEmbeddedValues(typename SVTreeType::SVBranchHandle hItem, uint32_t ownerID, uint32_t objectID, SVObjectScriptDataObjectTypeEnum dataType);

	HRESULT ProcessBranchObjectValues(typename SVTreeType::SVBranchHandle hItem, uint32_t ownerID, uint32_t objectID, bool excludeDefaultAndArray);
	HRESULT ProcessLeafObjectValues(typename SVTreeType::SVBranchHandle hItem, uint32_t ownerID, uint32_t objectID, bool excludeDefaultAndArray);

	HRESULT ProcessEquation(uint32_t ownerID, const _variant_t& equation);

	HRESULT ProcessMaskData(uint32_t ownerID, const _variant_t& maskData);

	HRESULT ProcessInputs(typename SVTreeType::SVBranchHandle hItem, uint32_t ownerID);
	
	HRESULT ProcessAttributes(uint32_t ownerID, uint32_t objectID, typename SVTreeType::SVBranchHandle hItem);

	bool GetItemValue(const std::string& tag, typename SVTreeType::SVBranchHandle hItem, _variant_t& rValue);
	bool GetValues(typename SVTreeType::SVBranchHandle hItem, const std::string& tag, std::vector<_variant_t>& rValueList);
	bool HasTag(typename SVTreeType::SVBranchHandle hItem, const std::string& tag);
};

#include "SVInspectionTreeParser.inl"

