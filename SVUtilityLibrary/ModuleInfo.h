//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/08/03,2020/08/03 by Seidenader Maschinenbau GmbH. All Rights Reserved 
/// \file ModulInfo.h
/// DESCRIPTION
//******************************************************************************

#pragma once

namespace SvUl
{
	class ModuleInfo
	{
	public:
		static bool  listProcessModules(DWORD dwPID, std::vector<std::string>& modulenames);
		static bool  isProcessModuleName(DWORD dwPID, const std::string& modulename);
	};
}



