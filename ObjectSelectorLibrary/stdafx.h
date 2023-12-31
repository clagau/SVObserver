//******************************************************************************
//* COPYRIGHT (c) 2014 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   stdafx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jul 2014 11:16:22  $
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

#pragma warning( disable: 4503 )
#pragma warning( disable: 4482 )

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcmn.h>         // MFC support for Windows Common Controls

#include <algorithm>
#include <assert.h>
#include <comdef.h>
#include <comutil.h>		//Used by static library
#include <concrt.h>			//Used by static library
#include <ctime>			//Used by static library
#include <list>				//Used by static library
#include <map>				//Used by static library
#pragma warning(push)
#pragma warning(disable : 4996)
#include <sequential_tree.h>//Used by static library
#pragma warning(pop)
#include <set>
#include <string>			//Used by static library
#include <tchar.h>			//Used by static library
#include <vector>			//Used by static library
#pragma endregion Precompiled Headers

