//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   stdafx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   03 Oct 2013 14:43:26  $
//******************************************************************************

#pragma once

#define NOMINMAX			// Exclude min/max macros

#define _WINSOCKAPI_    // stops windows.h including winsock.h

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include "targetver.h"
#include <Windows.h>
#include <atlenc.h>

#include <algorithm>
#include <comutil.h>		//Used by static library
#include <conio.h>
#include <fstream>
#include <iomanip>
#include <list>
#include <process.h>
#include <signal.h>
#include <Psapi.h>
#include <stdio.h>
#include <set>				//Used by static library
#include <sstream>			//Used by static library
#include <string>
#include <tchar.h>
#include <tlhelp32.h>
#include <utility>
#include <vector>
#include <winsock2.h>		//Used by static library
#include <ws2tcpip.h>		//Used by static library

#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>			//Used by static library
#define BOOST_INTERPROCESS_SHARED_DIR_PATH "V:/boost_interprocess"
#define BOOST_DATE_TIME_NO_LIB
#include <boost/interprocess/detail/shared_dir_helpers.hpp>	//Used by static library
#include <boost/interprocess/file_mapping.hpp>				//Used by static library
#include <boost/interprocess/mapped_region.hpp>				//Used by static library
#include <boost/interprocess/managed_shared_memory.hpp>		//Used by static library
#include <boost/interprocess/allocators/allocator.hpp>		//Used by static library
#include <boost/interprocess/containers/map.hpp>			//Used by static library
#include <boost/interprocess/containers/flat_map.hpp>		//Used by static library
#include <boost/interprocess/containers/string.hpp>			//Used by static library
#include <boost/interprocess/containers/vector.hpp>			//Used by static library
#include <boost/interprocess/offset_ptr.hpp>				//Used by static library
#include <boost/interprocess/sync/interprocess_mutex.hpp>	//Used by static library
#include <boost/interprocess/sync/scoped_lock.hpp>			//Used by static library
#include <boost/multi_index_container.hpp>				//Used by static library
#include <boost/multi_index/member.hpp>					//Used by static library
#include <boost/multi_index/ordered_index.hpp>			//Used by static library
#include <boost/shared_array.hpp>							//Used by static library
#include <boost/scoped_array.hpp>
#include <boost/utility.hpp>								//Used by static library
#pragma endregion Precompiled Headers
//#define TRACE_FAILURE true