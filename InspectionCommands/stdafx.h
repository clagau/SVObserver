//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by K�rber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
//*****************************************************************************
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
#define _CRT_SECURE_NO_WARNINGS

#pragma warning( disable: 4482 )	//! Disables warning with enum name being used

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
//#include <algorithm>
#include <concrt.h>
#include <ctime>
#include <deque>
#include <functional>
#include <future>
#include <iostream>
//#include <iomanip>
#include <iterator>
//#include <limits>
#include <list>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <tchar.h>
#include <utility>
#include <vector>
#include <comdef.h>

#pragma warning(push)
#pragma warning(disable : 4244 4458 4459)
#include <boost/graph/adjacency_list.hpp>				//Used by static library
#include <boost/graph/depth_first_search.hpp>			//Used by static library
#include <boost/graph/breadth_first_search.hpp>			//Used by static library
#include <boost/graph/reverse_graph.hpp>				//Used by static library
#include <boost/graph/filtered_graph.hpp>				//Used by static library
#include <boost/graph/graphviz.hpp>						//Used by static library
#include <boost/scoped_array.hpp>						//Used by static library
#pragma warning(pop)
#pragma endregion Precompiled Headers
