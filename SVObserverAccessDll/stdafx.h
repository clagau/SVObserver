//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file stdafx.h
/// All Rights Reserved 
//*****************************************************************************
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//******************************************************************************

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>


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
#include  <sstream>
#include <stdio.h>
#include <tchar.h>
#include <shlwapi.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <winioctl.h>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#define BOOST_DATE_TIME_NO_LIB
#include <boost/multi_index_container.hpp>				//Used by static library
#include <boost/multi_index/member.hpp>					//Used by static library
#include <boost/multi_index/ordered_index.hpp>			//Used by static library
#include <boost/utility.hpp>
#pragma endregion Precompiled Headers

