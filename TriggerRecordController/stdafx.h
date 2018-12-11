//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file stdafx.h
/// All Rights Reserved
//*****************************************************************************
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

#ifndef _DEBUG
#define _SECURE_SCL 0
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define NOMINMAX			// Exclude min/max macros

#define _CRT_SECURE_NO_DEPRECATE
//#define _CRT_SECURE_NO_WARNINGS
#define _AFX_SECURE_NO_DEPRECATE
#define _ATL_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _STL_SECURE_NO_WARNINGS

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include <assert.h>
#include <algorithm>
#include <atomic>
#include <comdef.h>
#include <future>
#include <memory>
#include <map>
#include <set>
#include <vector>
#include <ctime>				//Used by static library
#include <tchar.h>
#include <WinBase.h>

#include <boost/function.hpp>
#include <boost/multi_index_container.hpp>				//Used by static library
#include <boost/multi_index/member.hpp>					//Used by static library
#include <boost/multi_index/ordered_index.hpp>			//Used by static library
#pragma endregion Precompiled Headers