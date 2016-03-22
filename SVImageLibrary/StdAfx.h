//******************************************************************************
//* COPYRIGHT (c) 2002 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   StdAfx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:37:08  $
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

#pragma warning( disable: 4786 )	// identifier truncation 255 chars
#pragma warning( 4: 4786 )

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include <afx.h>
#include <afxwin.h>
#include <afxtempl.h>

#include <concrt.h>			//Used by static library
#include <comutil.h>		//Used by static library
#include <ctime>			//Used by static library
#include <deque>			//Used by static library
#include <fstream>
#include <math.h>
#include <map>
#include <set>				//Used by static library
#include <stdint.h>
#include <string>			//Used by static library
#include <tchar.h>			//Used by static library
#include <typeinfo.h>
#include <vector>
#include <utility>			//Used by static library

#include <boost/function.hpp>							//Used by static library
#include <boost/multi_index_container.hpp>				//Used by static library
#include <boost/multi_index/member.hpp>					//Used by static library
#include <boost/multi_index/ordered_index.hpp>			//Used by static library
#pragma endregion Precompiled Headers
