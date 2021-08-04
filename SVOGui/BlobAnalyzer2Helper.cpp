//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobAnalyzer2Helper.h
/// This is the file for helper function of the Blob Analyzer 2 GUI
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "LinkedValue.h"
#include "LinkedValueSelectorDialog.h"
#include "GridCtrlLibrary/GridCellBase.h"
#include "GridCtrlLibrary/GridCtrl.h"
#pragma endregion Includes

namespace SvOg
{
void setValueColumn(SvGcl::GridCtrl& rGrid, int pos, int colPos, const LinkedValueData& rData)
{
	CString valueString = static_cast<CString>(rData.m_Value);
	if (valueString.IsEmpty())
	{
		valueString = static_cast<CString>(rData.m_defaultValue);
	}

	rGrid.SetItemText(pos, colPos, valueString);
	if (SvPb::LinkedSelectedType::DirectValue == rData.m_type)
	{
		rGrid.SetItemState(pos, colPos, rGrid.GetItemState(pos, colPos) & (~GVIS_READONLY));
	}
	else
	{
		rGrid.SetItemState(pos, colPos, rGrid.GetItemState(pos, colPos) | GVIS_READONLY);
	}
}

void startObjectSelector(SvGcl::GridCtrl& rGrid, const std::string& rName, int rowPos, int colPos, uint32_t ipId, uint32_t toolId, uint32_t objectId, SvPb::LinkedValue& rData)
{
	ObjectSelectorData selectorData;
	LinkedValueSelectorTypesEnum possibleType = LinkedValueSelectorTypesEnum::All;
	selectorData.m_type = SvPb::allNumberValueObjects;
	selectorData.m_attribute = SvPb::selectableForEquation;
	selectorData.m_searchArea = {SvPb::SearchArea::globalConstantItems, SvPb::SearchArea::cameraObject, SvPb::SearchArea::ppqItems, SvPb::SearchArea::toolsetItems};
	selectorData.m_excludeSameLineageVector = {toolId};
	selectorData.m_stopAtId = toolId;
	auto featureData = convertLinkedValue(rData);
	LinkedValueSelectorDialog dlg(ipId, objectId, rName, featureData, VT_R8, selectorData, nullptr, possibleType);
	if (IDOK == dlg.DoModal())
	{
		setValueColumn(rGrid, rowPos, colPos, dlg.getData());
		rData = convertLinkedValue(dlg.getData());
		rGrid.Refresh();
	}
}
}