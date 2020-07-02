//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SharedMemIniFileTags.h
/// All Rights Reserved 
//*****************************************************************************
///Files hold tags for shared memory entries in SVIM.ini
//******************************************************************************
#pragma once
namespace SvSml
{
	constexpr char* SharedMemorySectionTag = _T("SharedMemory");
	constexpr char* NumProductSlotTag = _T("NumProductSlot");
	constexpr char* CreateTimeoutTag = _T("CreateTimeout");
	constexpr char* CreateWaitTimeTag = _T("CreateWaitTime");
}