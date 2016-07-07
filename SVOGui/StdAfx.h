//******************************************************************************
//* COPYRIGHT (c) 2003 by Seidenader Vision Inc., Harrisburg
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
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
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

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include <afx.h>
#include <afxwin.h>
#include <afxtempl.h>
#include <afxcmn.h>
#include <afxctl.h>
#include <afxdlgs.h>
#include <afxdisp.h>        // MFC OLE-Automatisierungsklassen
#include <WinDef.h>

#include <algorithm>
#include <atlsafe.h>
#include <comdef.h>
#include <concrt.h>										//Used by static library
#include <comutil.h>
#include <ctime>			//Used by static library
#include <functional>
#include <guiddef.h>
#include <limits.h>
#include <map>
#include <oleauto.h>
#include <regex>
#include <sequential_tree.h> //Used by static library
#include <set>				//Used by static library
#include <sstream>
#include <string>
#include <tchar.h>			//Used by static library
#include <tuple>			//Used by static library
#include <vector>

#include <boost/any.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/multi_index_container.hpp>				//Used by static library
#include <boost/multi_index/member.hpp>					//Used by static library
#include <boost/multi_index/ordered_index.hpp>			//Used by static library
#include <boost/noncopyable.hpp>

#pragma endregion Precompiled Headers
