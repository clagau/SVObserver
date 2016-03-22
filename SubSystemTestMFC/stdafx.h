//******************************************************************************
//* COPYRIGHT (c) 1997 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   stdafx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:16:02  $
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

#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#define _CRT_SECURE_NO_DEPRECATE 
#define _AFX_SECURE_NO_DEPRECATE
#define _ATL_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

#ifndef _DEBUG
#define _SECURE_SCL 0
#endif

#define _ATL_APARTMENT_THREADED

#pragma warning( disable: 4503 )
#pragma warning( disable: 4786 )	// identifier truncation 255 chars
#pragma warning( 4: 4786 )

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include <afx.h>
#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen
#include <afxpriv.h>
#include <afxcmn.h>			// MFC-Unterstützung für gängige Windows-Steuerelemente
#include <atlbase.h>
#include <atlcom.h>
#include <afxole.h>         // MFC OLE-Klassen
#include <afxodlgs.h>       // MFC OLE-Dialogfeldklassen
#include <afxdisp.h>        // MFC OLE-Automatisierungsklassen
#include <afxcview.h>		// MFC CListView Support
#include <afxtempl.h>		// MFC Templates
#include <AFXMT.H>
#include <afxcoll.h>

#include <algorithm>
#include <comutil.h>		//Used by static library
#include <ctime>			//Used by static library
#include <map>
#include <math.h>
#include <set>
#include <string>			//Used by static library
#include <tchar.h>			//Used by static library
#include <vector>

#include <boost/multi_index_container.hpp>					//Used by static library
#include <boost/multi_index/member.hpp>						//Used by static library
#include <boost/multi_index/ordered_index.hpp>				//Used by static library
#pragma endregion Precompiled Headers


