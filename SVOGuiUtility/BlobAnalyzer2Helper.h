//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file BlobAnalyzer2Helper.h
/// This is the file for helper function of the Blob Analyzer 2 GUI
//******************************************************************************
#pragma once

struct LinkedValueData;
namespace SvGcl
{
class GridCtrl;
}

namespace SvOgu
{
void setValueColumn(SvGcl::GridCtrl& rGrid, int pos, int colPos, const LinkedValueData& rData);
void startObjectSelector(SvGcl::GridCtrl& rGrid, const std::string& rName, int rowPos, int colPos, uint32_t ipId, uint32_t objectId, SvPb::LinkedValue& rData);
}