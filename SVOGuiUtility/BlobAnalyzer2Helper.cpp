//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file BlobAnalyzer2Helper.h
/// This is the file for helper function of the Blob Analyzer 2 GUI
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "LinkedValue.h"
#include "LinkedValueSelectorDialog.h"
#include "GridCtrlLibrary/GridCellBase.h"
#include "GridCtrlLibrary/GridCtrl.h"
#include "Definitions/Color.h"
#pragma endregion Includes

namespace SvOgu
{
void setValueColumn(SvGcl::GridCtrl& rGrid, int pos, int colPos, const LinkedValueData& rData)
{
	CString valueString = static_cast<CString>(rData.m_Value);
	if (valueString.IsEmpty())
	{
		valueString = static_cast<CString>(rData.m_defaultValue);
	}

	rGrid.SetItemText(pos, colPos, valueString);
	if (SvPb::LinkedSelectedOption::DirectValue == rData.m_selectedOption)
	{
		rGrid.SetItemBkColour(pos, colPos, SvDef::White);
		rGrid.SetItemState(pos, colPos, rGrid.GetItemState(pos, colPos) & (~GVIS_READONLY));
	}
	else
	{
		rGrid.SetItemBkColour(pos, colPos, SvDef::WhiteSmoke);
		rGrid.SetItemState(pos, colPos, rGrid.GetItemState(pos, colPos) | GVIS_READONLY);
	}
}

void startObjectSelector(SvGcl::GridCtrl& rGrid, const std::string& rName, int rowPos, int colPos, uint32_t ipId, uint32_t objectId, SvPb::LinkedValue& rData)
{
	auto featureData = convertLinkedValue(rData);
	LinkedValueSelectorDialog dlg(ipId, objectId, rName, featureData, VT_R8);
	if (IDOK == dlg.DoModal())
	{
		setValueColumn(rGrid, rowPos, colPos, dlg.getData());
		rData = convertLinkedValue(dlg.getData());
		rGrid.Refresh();
	}
}
}