//******************************************************************************
//* COPYRIGHT (c) 2006 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   StdAfx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 11:08:04  $
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

#define _CRT_SECURE_NO_DEPRECATE 
#define _AFX_SECURE_NO_DEPRECATE
#define _ATL_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _SCL_SECURE_NO_WARNINGS

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
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls

#include <comdef.h>
#include <comutil.h>		//Used by static library
#include <ctime>			//Used by static library
#include <deque>
#include <map>
#include <math.h>
#include <set>				//Used by static library
#include <sstream>			//Used by static library
#include <string>
#include <tchar.h>			//Used by static library
#include <vector>			//Used by static library

#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>
#pragma endregion Precompiled Headers

