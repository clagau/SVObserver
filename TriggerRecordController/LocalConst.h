//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file LocalConst.h
/// All Rights Reserved
//*****************************************************************************
/// File for the constants for the triggerRecordController library.
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvTrc
{
constexpr int cTriggerRecordAddOn = 2; //number of additional slots for internal use
constexpr int cMaxTriggerRecords = 305;
const LPCTSTR GNameResetEvent = _T("Global\\SvTrc_ResetEvent");
const LPCTSTR GNameTridEvent = _T("Global\\SvTrc_TridEvent");
}