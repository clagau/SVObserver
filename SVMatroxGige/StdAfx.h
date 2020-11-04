//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : StdAfx
//* .File Name       : $Workfile:   StdAfx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:36:26  $
//******************************************************************************

#pragma once

#pragma warning( disable: 4786 )	// identifier truncation 255 chars
#pragma warning( disable: 4995 )	// deprecated warnings

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

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#define NOMINMAX			// Exclude min/max macros

#define _CRT_SECURE_NO_DEPRECATE 
#define _AFX_SECURE_NO_DEPRECATE
#define _ATL_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

#ifndef _DEBUG
#define _SECURE_SCL 0
#endif

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include <windows.h>

#include <atomic>			//Used by static library
#include <bitset>			//Used by static library
#include <ctime>			//Used by static library
#include <comdef.h>
#include <deque>
#include <iterator>			//Used by static library
#include <list>				//Used by static library
#include <map>
#include <memory>
#include <numeric>
#include <set>				//Used by static library
#include <stack>
#include <string>			//Used by static library
#include <tchar.h>			//Used by static library
#include <utility>
#include <unordered_map>
#include <vector>

#include <boost/config.hpp>
#include <boost/function.hpp>							//Used by static library
#include <boost/utility.hpp>

#pragma endregion Precompiled Headers
