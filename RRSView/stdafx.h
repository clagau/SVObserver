
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

#include <afxwin.h>         // MFC core and standard components
#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#include <string>
#include <vector>
#include <map>
#include <set>

#pragma warning (push)
#pragma warning (disable : 4005)

#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>			//Used by static library
#pragma warning (push)
#pragma warning (default : 4005)


#define BOOST_INTERPROCESS_SHARED_DIR_PATH "V:/boost_interprocess"
#define BOOST_DATE_TIME_NO_LIB


//#include <boost/interprocess/detail/shared_dir_helpers.hpp>	//Used by static library
#include <boost/interprocess/file_mapping.hpp>				//Used by static library
#include <boost/interprocess/managed_shared_memory.hpp>		//Used by static library
#include <boost/interprocess/containers/map.hpp>			//Used by static library
#include <boost/interprocess/containers/string.hpp>			//Used by static library
#include <boost/interprocess/containers/vector.hpp>			//Used by static library





#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


