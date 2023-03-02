//******************************************************************************
//* COPYRIGHT (c) 2007 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   StdAfx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 11:31:06  $
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
#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxext.h>         // MFC extensions
#include <afxole.h>

#include <algorithm>
#include <assert.h>
#include <atomic>			//Used by static library
#include <comdef.h>
#include <crtdbg.h>
#include <comutil.h>		//Used by static library
#include <ctime>			//Used by static library
#include <deque>			//Used by static library
#include <format>
#include <map>
#include <memory>			//Used by static library
#pragma warning(push)
#pragma warning(disable : 4996)
#include <sequential_tree.h>
#pragma warning(pop)
#include <set>
#include <sstream>			//Used by static library
#include <stdio.h>
#include <string>			//Used by static library
#include <tchar.h>			//Used by static library
#include <vector>
#pragma endregion Precompiled Headers
