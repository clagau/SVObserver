//******************************************************************************
//* COPYRIGHT (c) 1997 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   StdAfx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   18 Sep 2014 13:21:38  $
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

#pragma warning ( disable : 4786 )
#pragma warning ( disable : 4482 )

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
#include <afxwin.h>
#include <afxcmn.h>
#include <afxpriv.h>
#include <afxmt.h>
#include <afxtempl.h>
#include <afxdlgs.h>
#include <afxcoll.h>
#include <afxext.h>         // MFC extensions

#include <algorithm>

#include <comutil.h>		//Used by static library
#include <concrt.h>
#include <ctime>			//Used by static library
#include <deque>
#include <direct.h>
#include <format>
#include <io.h>

#include <iostream>
#include <fstream>

#include <iterator>
#include <map>				//Used by static library
#include <set>				//Used by static library
#include <shlwapi.h>
#include <string>			//Used by static library
#include <stdexcept>
#include <tchar.h>			//Used by static library
#include <vector>

#include <boost\algorithm\string.hpp>
#pragma endregion Precompiled Headers
