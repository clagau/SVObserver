//******************************************************************************
/// \copyright (c) 2017,2017 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define NOMINMAX
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#define _CRT_SECURE_NO_WARNINGS

#pragma warning(disable : 4996) ///Required to be able to copy std::vector to protobuf repeated field

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers

#include <atomic>
#include <comutil.h>
#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <stdio.h>
#include <tchar.h>
#include <vector>

#pragma warning(push)
#pragma warning(disable : 4702)
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#pragma warning(pop)
#pragma endregion Precompiled Headers
