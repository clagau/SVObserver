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
// from a DLL simpler. All files within this DLL are compiled with the SVOBSERVERACCESS_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SVOBSERVERACCESSDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SVOBSERVERACCESS_EXPORTS
#define SVOBSERVERACCESS_API __declspec(dllexport)
#else
#define SVOBSERVERACCESS_API __declspec(dllimport)
#endif
namespace SvOa
{
//! This Function has to be called once before the other functions are used
SVOBSERVERACCESS_API void Init();
//! Release some resources
SVOBSERVERACCESS_API void Finish();
SVOBSERVERACCESS_API bool QueryListName(const SvPb::QueryListNameRequest& req, SvPb::QueryListNameResponse& resp, SvPenv::Error& err);
SVOBSERVERACCESS_API bool  QueryListItem(const SvPb::QueryListItemRequest& req, SvPb::QueryListItemResponse& resp, SvPenv::Error& err);
SVOBSERVERACCESS_API bool GetProductOrReject(const SvPb::GetProductRequest& request, SvPb::GetProductResponse& resp, SvPenv::Error& err);
SVOBSERVERACCESS_API bool GetFailstatus(const SvPb::GetFailStatusRequest& request, SvPb::GetFailStatusResponse& resp, SvPenv::Error& err);
SVOBSERVERACCESS_API bool  GetImageFromCurId(const  SvPb::GetImageFromCurIdRequest& req, SvPb::GetImageFromCurIdResponse& resp, SvPenv::Error& err);
};

