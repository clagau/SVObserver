//******************************************************************************
//* COPYRIGHT (c) 2014 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   stdafx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Oct 2014 10:32:34  $
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

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define NOMINMAX			// Exclude min/max macros

#ifndef _DEBUG
#define _SECURE_SCL 0
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxadv.h>         // For CSharedFile
#include <afxconv.h>        // For LPTSTR -> LPSTR macros
#include "afxdtctl.h"		// for CDateTimeCtrl
#include <afxtempl.h>
#include <afxole.h>

#include "TCHAR.h"

#include <algorithm>
#include <assert.h>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <ctime>
#pragma endregion Precompiled Headers
