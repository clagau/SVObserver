//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
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
#include <algorithm>
#include <cmath>
#include <comdef.h>
#include <concrt.h>
#include <ctime>
#include <deque>
#include <fstream>
#include <io.h>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <ppl.h>
#include <sequential_tree.h>
#include <set>
#include <string>
#include <tchar.h>
#include <utility>
#include <vector>

#include <boost/function.hpp>							//Used by static library
#include <boost/circular_buffer.hpp>					//Used by static library
#include <boost/multi_index_container.hpp>				//Used by static library
#include <boost/multi_index/member.hpp>					//Used by static library
#include <boost/multi_index/ordered_index.hpp>			//Used by static library
#pragma endregion Precompiled Headers