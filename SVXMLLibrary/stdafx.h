//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   stdafx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:46:46  $
//******************************************************************************

#pragma once

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows 2K or later.
#define WINVER 0x0600		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows 2K or later.                   
#define _WIN32_WINNT WINVER	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 2K or later.
#define _WIN32_WINDOWS WINVER // Change this to the appropriate value to target Windows 2K or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

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

#include <assert.h>
#include <algorithm>
#include <atomic>			//Used by static library
#include <ctime>			//Used by static library
#include <comdef.h>
#include <comutil.h>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <map>
#include <memory> 
#pragma warning(push)
#pragma warning(disable : 4996)
#include <sequential_tree.h>
#pragma warning(pop)
#include <sstream>
#include <string>
#include <tchar.h>			//Used by static library
#include <typeinfo>


#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/scoped_array.hpp>


#pragma endregion Precompiled Headers

