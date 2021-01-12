//******************************************************************************
//* COPYRIGHT (c) 2003 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   Stdafx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   19 Dec 2014 14:02:00  $
//******************************************************************************

#pragma once 
// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0600		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT WINVER	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows XP or later.
#define _WIN32_WINDOWS WINVER // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#define NOMINMAX			// Exclude min/max macros

#define _CRT_SECURE_NO_DEPRECATE 
#define _AFX_SECURE_NO_DEPRECATE
#define _ATL_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _SCL_SECURE_NO_WARNINGS

#ifndef _DEBUG
#define _CRTDBG_MAP_ALLOC
#define _SECURE_SCL 0
#endif

#define _ATL_APARTMENT_THREADED

#define OEMRESOURCE	// OEM resource to use nicer cursors...

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include <Windows.h>

#include <comdef.h>
#include <concrt.h>
#include <cstdlib>
#include <list>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>				//Used by static library
#include <set>					//Used by static library
#include <tchar.h>				//Used by static library
#include <unordered_map>

#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/config.hpp>
#define BOOST_DATE_TIME_NO_LIB
#include <boost/date_time.hpp>
#include <boost/function.hpp>	//Used by static library
#include <boost/utility.hpp>
#pragma endregion Precompiled Headers


