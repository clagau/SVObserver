//*****************************************************************************
/// \copyright COPYRIGHT (c) 2020,2020 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file Stdafx.h
/// \brief include file for standard system include files
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

#ifndef _WIN32_WINDOWS			// Allow use of features specific to Windows XP or later.
#define _WIN32_WINDOWS WINVER	// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN
#define VC_LEAN_AND_MEAN

#define _CRT_SECURE_NO_DEPRECATE 
#define _CRT_NON_CONFORMING_SWPRINTFS

#ifndef _DEBUG
#define _SECURE_SCL 0
#endif

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include <windows.h>

#include <atomic>
#include <array>
#include <chrono>
#include <comdef.h>
#include <comutil.h>
#include <conio.h>
#include <format>
#include <fstream>
#include <functional>
#include <iostream> 
#include <map>
#include <memory>
#include <mutex>
#include <iostream>
#include <queue>
#include <stdio.h>
#include <string>
#include <tchar.h>
#include <thread>
#pragma endregion Precompiled Headers
