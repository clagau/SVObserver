//******************************************************************************
//* COPYRIGHT (c) 2005 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   StdAfx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:34:56  $
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

#pragma warning( disable: 4786 )	// identifier truncation 255 chars
#pragma warning( disable: 4482 )

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#define _CRT_SECURE_NO_DEPRECATE 
#define _AFX_SECURE_NO_DEPRECATE
#define _ATL_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS

#ifndef _DEBUG
#define _SECURE_SCL 0
#endif

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include <windows.h>

#include <concrt.h>			//Used by static library
#include <comutil.h>		//Used by static library
#include <ctime>			//Used by static library
#include <deque>
#include <guiddef.h>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>			//Used by static library
#include <tchar.h>			//Used by static library
#include <typeinfo>
#include <utility>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/config.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/graph/adjacency_list.hpp>				//Used by static library
#include <boost/graph/depth_first_search.hpp>			//Used by static library
#include <boost/graph/breadth_first_search.hpp>			//Used by static library
#include <boost/graph/reverse_graph.hpp>				//Used by static library
#include <boost/graph/filtered_graph.hpp>				//Used by static library
#include <boost/graph/graphviz.hpp>						//Used by static library
#include <boost/scoped_array.hpp>						//Used by static library
#include <boost/tokenizer.hpp>
#include <boost/utility.hpp>
#pragma endregion Precompiled Headers
