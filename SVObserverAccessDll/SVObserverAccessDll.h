//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVObserverAccessDll.h
/// All Rights Reserved 
//*****************************************************************************
// This class is exported from the SVObserverAccessDll.dll
//******************************************************************************

#include "SVProtoBuf\RunRe.h"
#include "SVProtoBuf\envelope.h"
#include <SVSharedMemoryLibrary\ShareControl.h>
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SVOBSERVERACCESSDLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SVOBSERVERACCESSDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SVOBSERVERACCESSDLL_EXPORTS
#define SVOBSERVERACCESSDLL_API __declspec(dllexport)
#else
#define SVOBSERVERACCESSDLL_API __declspec(dllimport)
#endif
namespace SVObserverAccessDll
{
//! This Function has to be called once before the other functions are used
SVOBSERVERACCESSDLL_API void Init();
//! Release some resources
SVOBSERVERACCESSDLL_API void Finish();
SVOBSERVERACCESSDLL_API bool QueryListName(const RRWS::QueryListNameRequest& req, RRWS::QueryListNameResponse& resp, SVRPC::Error& err);
SVOBSERVERACCESSDLL_API bool  QueryListItem(const RRWS::QueryListItemRequest& req, RRWS::QueryListItemResponse& resp, SVRPC::Error& err);
SVOBSERVERACCESSDLL_API bool GetProductOrReject(const RRWS::GetProductRequest& request, RRWS::GetProductResponse& resp, SVRPC::Error& err);
SVOBSERVERACCESSDLL_API bool GetFailstatus(const RRWS::GetFailStatusRequest& request, RRWS::GetFailStatusResponse& resp, SVRPC::Error& err);
SVOBSERVERACCESSDLL_API bool  GetImageFromCurId(const  RRWS::GetImageFromCurIdRequest& req, RRWS::GetImageFromCurIdResponse& resp, SVRPC::Error& err);
};

