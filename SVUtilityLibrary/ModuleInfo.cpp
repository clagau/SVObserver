//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/08/03,2020/08/03 by Seidenader Maschinenbau GmbH. All Rights Reserved 
/// \file ModulInfo.cpp
/// DESCRIPTION
//******************************************************************************

#pragma region Includes 
#include "stdafx.h"
#include "ModuleInfo.h"
#include <TlHelp32.h>
#include "SVUtilityLibrary\StringHelper.h"
#pragma endregion Includes



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
namespace  SvUl
{
	
	bool ModuleInfo::listProcessModules(DWORD dwPID, std::vector<std::string>& modulnames)
	{
		HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
		MODULEENTRY32 me32;

		//  Take a snapshot of all modules in the specified process. 
		hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
		if (hModuleSnap == INVALID_HANDLE_VALUE)
		{
		
			return(FALSE);
		}

		//  Set the size of the structure before using it. 
		me32.dwSize = sizeof(MODULEENTRY32);

		//  Retrieve information about the first module, 
		//  and exit if unsuccessful 
		if (!Module32First(hModuleSnap, &me32))
		{
		
			CloseHandle(hModuleSnap);     // Must clean up the snapshot object! 
			return(FALSE);
		}

		//  Now walk the module list of the process, 
		//  and display information about each module 
		do
		{
			//lot more infos available if necessary
			modulnames.push_back(me32.szModule);

		} while (Module32Next(hModuleSnap, &me32));

		//TRACE(TEXT("\n"));

		//  Do not forget to clean up the snapshot object. 
		CloseHandle(hModuleSnap);
		return(TRUE);
	}


	
	
	bool  ModuleInfo::isProcessModuleName(DWORD ProcessId ,const std::string& name)
	{
		std::vector<std::string> Modulnames;
		listProcessModules(ProcessId, Modulnames);
		auto f = std::find_if(Modulnames.begin(), Modulnames.end(), [&name](const std::string& modulname)
		{
			return(0 == SvUl::CompareNoCase( name, modulname));
		});
		if (f != Modulnames.end())
		{
			return true;
		}
		
		return false;
	
	}

}//namespace  SvUl
