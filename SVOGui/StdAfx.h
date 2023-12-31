//******************************************************************************
//* COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
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

#define OEMRESOURCE	// OEM resource to use nicer cursors...
#define NOMINMAX			// Exclude min/max macros

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include <algorithm>
namespace Gdiplus
{
using std::min;
using std::max;
}

#include <afx.h>
#include <afxwin.h>
#include <afxbutton.h>
#include <afxtempl.h>
#include <afxcmn.h>
#include <afxctl.h>
#include <afxdlgs.h>
#include <afxdisp.h>        // MFC OLE-Automatisierungsklassen
#include <WinDef.h>


#include <array>
#include <atlsafe.h>
#include <comdef.h>
#include <concrt.h>			//Used by static library
#include <comutil.h>
#include <ctime>			//Used by static library
#include <ddraw.h>
#include <format>
#include <functional>
#include <future>
#include <guiddef.h>
#include <iterator>
#include <limits.h>
#include <map>
#include <memory>
#include <mutex>
#include <oleauto.h>
#include <regex>
#pragma warning(push)
#pragma warning(disable : 4996)
#include <sequential_tree.h>
#pragma warning(pop)
#include <set>				//Used by static library
#include <sstream>
#include <string>
#include <tchar.h>			//Used by static library
#include <tuple>			//Used by static library
#include <variant>
#include <vector>

#define BOOST_BIND_NO_PLACEHOLDERS
#include <boost/lexical_cast.hpp>

#include "SVOResource\resource.h"
#include "SVStatusLibrary/MessageManagerHelper.h"

#pragma endregion Precompiled Headers
