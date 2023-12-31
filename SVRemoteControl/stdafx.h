//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   stdafx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   15 Aug 2014 11:36:28  $
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
#define _WIN32_WINDOWS WINVER // Change this to the appropriate value to target Windows XP or later.
#endif

//#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
//#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
//#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers




#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit
#pragma warning( disable: 4482 )	//! Disables warning with enum name being used

#define WINDOWS_EVENT_LOG_BACKEND
#pragma region Precompiled Headers
#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlcomcli.h>
#include <atlsafe.h>
#include <comutil.h>
#pragma warning (push)
#pragma warning( disable: 4458 )
#include <GdiPlus.h>
#pragma warning (pop)
#include <winbase.h>
#include <boost/config.hpp> // put this first to suppress some VC++ warnings
#define NOMINMAX			// Exclude min/max macros
#include <windows.h>
#include <algorithm>
#include <comdef.h>
#include <cstdio>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <tchar.h>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <list>

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/shared_array.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

#pragma endregion Precompiled Headers

///AVOIDING warnings in Protobuf generated header
#pragma warning( disable:  4800)

class SVControlCommands;
typedef std::shared_ptr<SVControlCommands> CommandDispatcher;
using namespace ATL;


//#ifdef _DEBUG
//#define TRACE_SOCKET TRUE 
//#endif 
