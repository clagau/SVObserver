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
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVObjectClass;
class SVObjectScriptParserClass;

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
	SVInspectionTreeParser(SVTreeType& rTreeCtrl, typename SVTreeType::SVBranchHandle hItem, unsigned long parserHandle, const GUID& OwnerGuid, SVObjectClass* pOwnerObject, CWnd* pWnd);
	virtual ~SVInspectionTreeParser();
	virtual HRESULT DoParse();
	virtual size_t GetTotal() const;

	static HRESULT CreateInspectionObject(GUID& inspectionGuid, SVTreeType& p_rTree, typename SVTreeType::SVBranchHandle hItem);

private:
	HRESULT Process(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID);
	
	HRESULT ProcessChildren(typename SVTreeType::SVBranchHandle hParentItem, const GUID& ownerID);

	HRESULT ProcessFriend(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID);
	HRESULT ProcessFriends(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID);
	
	HRESULT ProcessEmbeddeds(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID);
	HRESULT ProcessEmbedded(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID);
	HRESULT ProcessEmbeddedValues(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID, const GUID& objectID, const _variant_t& defaultValue, SVObjectScriptDataObjectTypeEnum dataType);

	HRESULT ProcessBranchObjectValues(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID, const GUID& objectID);
	HRESULT ProcessLeafObjectValues(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID, const GUID& objectID);

	HRESULT ProcessEquation(const GUID& ownerID, const _variant_t& equation);

	HRESULT ProcessMaskData(const GUID& ownerID, const _variant_t& maskData);

	HRESULT ProcessInputs(typename SVTreeType::SVBranchHandle hItem, const GUID& ownerID);
	
	HRESULT ProcessAttributes(const GUID& ownerID, const GUID& objectID, typename SVTreeType::SVBranchHandle hItem);

	bool GetItemValue(const SVString& tag, typename SVTreeType::SVBranchHandle hItem, _variant_t& rValue);
	bool GetValues(typename SVTreeType::SVBranchHandle hItem, const SVString& tag, SVVariantList& rValueList);
	bool HasTag(typename SVTreeType::SVBranchHandle hItem, const SVString& tag);
};

#include "SVInspectionTreeParser.inl"

