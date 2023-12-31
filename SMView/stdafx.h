//*****************************************************************************
/// \copyright (c) 2017,2017 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file stdafx.h
/// All Rights Reserved 
//*****************************************************************************
/// stdafx.h : include file for standard system include files,
/// or project specific include files that are used frequently,
/// but are changed infrequently
//******************************************************************************

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <afxwin.h>         // MFC core and standard components
#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#include <atomic>
#include <format>
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <set>

#pragma warning (push)
#pragma warning (disable : 4005)

#define BOOST_BIND_NO_PLACEHOLDERS
#define BOOST_DATE_TIME_NO_LIB

#pragma warning (push)
#pragma warning (default : 4005)


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


