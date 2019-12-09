//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file stdafx.h
/// All Rights Reserved
//******************************************************************************
/// include file for standard system include files,
/// or project specific include files that are used frequently, but
/// are changed infrequently
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

#define NOMINMAX
#define VC_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include <windows.h>

#include <comutil.h>
#include <conio.h>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <stdio.h>
#include <string>
#include <tchar.h>
#include <thread>
#include <vector>
#include <windows.h>

#include <boost/any.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp> //Used by static library
#include <boost/log/trivial.hpp>
#include <boost/thread.hpp>

#define BOOST_DATE_TIME_NO_LIB
#include <boost/scoped_array.hpp>
#include <boost/shared_array.hpp> //Used by static library
#include <boost/utility.hpp>      //Used by static library
#pragma endregion Precompiled Headers
