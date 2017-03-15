//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   stdafx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 Aug 2014 16:02:12  $
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

#ifndef _DEBUG
#define _SECURE_SCL 0
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define NOMINMAX			// Exclude min/max macros

#define _CRT_SECURE_NO_DEPRECATE
//#define _CRT_SECURE_NO_WARNINGS
#define _AFX_SECURE_NO_DEPRECATE
#define _ATL_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _STL_SECURE_NO_WARNINGS

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include <windows.h>

#include <cmath>
#include <comdef.h>
#include <deque>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <intrin.h>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <stdio.h>
#include <tchar.h>
#include <shlwapi.h>
#include <string>
#include <vector>
#include <winioctl.h>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#define BOOST_INTERPROCESS_SHARED_DIR_PATH "V:/boost_interprocess"
#define BOOST_DATE_TIME_NO_LIB
#include <boost/interprocess/detail/shared_dir_helpers.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/flat_map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/utility.hpp>
#pragma endregion Precompiled Headers



