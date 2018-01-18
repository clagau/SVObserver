//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file dllmain.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Entry point for dll
//******************************************************************************
#include "stdafx.h"
#include <google/protobuf/stubs/common.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		google::protobuf::ShutdownProtobufLibrary();
		break;
	}
	return TRUE;
}

