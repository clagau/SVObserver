//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   StdAfx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 12:57:50  $
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

#ifndef _DEBUG
#define _SECURE_SCL 0
#endif

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include <afx.h>
#include <afxwin.h>
#include <afxcmn.h>			// MFC-Unterstützung für gängige Windows-Steuerelemente
#include <afxext.h>         // MFC-Erweiterungen
#include <afxdisp.h>        // MFC OLE-Automatisierungsklassen
#include <afxdlgs.h>
#include <afxtempl.h>
#include <afxpriv.h>
#include <afxdlgs.h>

#include <algorithm>
#include <assert.h>
#include <comutil.h>		//Used by static library
#include <concrt.h>			//Used by static library
#include <ctime>			//Used by static library
#include <deque>			//Used by static library
#include <Dlgs.h>
#include <functional>
#include <Guiddef.h>
#include <list>
#include <io.h>
#include <iterator>			//Used by static library
#include <oleauto.h>
#include <map>
#include <math.h>
#include <numeric>
#include <set>				//Used by static library
#include <sstream>
#include <string>
#include <tchar.h>			//Used by static library
#include <vector>			//Used by static library
#include "SVStatusLibrary/MessageManagerHelper.h"
#pragma endregion Precompiled Headers
