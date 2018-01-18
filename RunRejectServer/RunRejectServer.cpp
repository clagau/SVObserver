//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : runrejctsvr
//* .File Name       : $Workfile:   runrejctsvr.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.16  $
//* .Check In Date   : $Date:   28 Oct 2014 10:45:20  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#define M_MIL_USE_SAFE_TYPE 0
#include <mil.h>
#include <iostream>
#include "RunRejectService.h"
#include "SVObserverAccess\SVObserverAccess.h"
#include <SVStatusLibrary\MessageManager.h>
#include <SVMessage\SVMessage.h>
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

typedef void(*SignalHandlerPointer)(int);
SignalHandlerPointer previousHandler(nullptr);

bool CheckCommandLineArgs(int argc, _TCHAR* argv[], LPCTSTR option)
{
	bool bFound = false;
	if (argc > 1)
	{
		for (int i = 1; i < argc && !bFound; i++)
		{
			if (_tcsicmp(argv[i], option) == 0)
			{
				bFound = true;
			}
		}
	}
	return bFound;
}

// Command Line arguments: /nocheck
// /nocheck means to ignore the 2 GiG size requirement
void StartThreads(DWORD argc, LPTSTR  *argv)
{
	/// Allocate MilSystem
	MIL_INT AppId = MappAlloc(M_DEFAULT, M_NULL);
	if (AppId == M_NULL)
	{
		std::cout << "MapAlloc failed";
		return;
	}
	MappControl(M_ERROR, M_PRINT_DISABLE);

	// check command line args - if /nocheck is specified - ignore the < 2 Gig error
	bool bCheckSizeOverride = CheckCommandLineArgs(argc, argv, _T("/nocheck"));
	try
	{
		ObserverAccess::SharedResourcesOk(bCheckSizeOverride);
		ObserverAccess::ExecuteRunReServer(ObserverAccess::DefaultPort);
		signal(SIGSEGV, previousHandler);
	}
	catch (std::exception& rExp)
	{
		SvStl::MessageMgrStd Exception(SvStl::LogOnly);
		SvDef::StringVector msgList;
		msgList.push_back(rExp.what());
		Exception.setMessage(SVMSG_RRS_3_GENERAL_ERROR, SvStl::Tid_FailedtoStart, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	MappFree(AppId);
}

int _tmain(int argc, _TCHAR* argv[])
{
	int rc = 0;

	SvStl::MessageMgrStd Exception(SvStl::LogOnly);
	Exception.setMessage(SVMSG_RRS_0_STARTED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
	//Function pointer for starting the threads
	gp_StartThreads = &StartThreads;

	SERVICE_TABLE_ENTRY ServiceTable[] =
	{
		{ cServiceName, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
		{ nullptr, nullptr}
	};

	if (!StartServiceCtrlDispatcher(ServiceTable))
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)	
		OutputDebugString(_T("StartServiceCtrlDispatcher returned error"));
#endif		
		rc = GetLastError();
		// running as console (for Debug?)
		if (ERROR_FAILED_SERVICE_CONTROLLER_CONNECT == rc)
		{
			StartThreads(argc, argv);
		}
	}

	Exception.setMessage(SVMSG_RRS_1_STOPPED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
	return rc;
}
