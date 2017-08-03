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
	static const TCHAR* const SharedMemorySectionTag = _T("SharedMemory");
	static const TCHAR* const MonitorStoreSizeTag = _T("MonitorStoreSize");
	static const TCHAR* const NumProductSlotTag = _T("NumProductSlot");
	static const TCHAR* const CreateTimeoutTag = _T("CreateTimeout");
	static const TCHAR* const CreateWaitTimeTag = _T("CreateWaitTime");
}