//*****************************************************************************
/// \copyright (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SharedMemIniFileTags.h
/// All Rights Reserved 
//*****************************************************************************
///Files hold tags for shared memory entries in SVIM.ini
//******************************************************************************
#pragma once
namespace SvSml
{
	constexpr const char* SharedMemorySectionTag = _T("SharedMemory");
	constexpr const char* NumProductSlotTag = _T("NumProductSlot");
	constexpr const char* CreateTimeoutTag = _T("CreateTimeout");
	constexpr const char* CreateWaitTimeTag = _T("CreateWaitTime");
}