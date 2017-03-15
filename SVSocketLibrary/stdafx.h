//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   stdafx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:13:56  $
//******************************************************************************

#pragma once

#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0600		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT WINVER // Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows XP or later.
#define _WIN32_WINDOWS WINVER  // Change this to the appropriate value to target other versions of Windows.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define NOMINMAX			// Exclude min/max macros

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include <windows.h>

#include <algorithm>
#include <comdef.h>
#include <cstdio>
#include <sstream>
#include <string>
#include <tchar.h>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_array.hpp>
#pragma endregion Precompiled Headers
