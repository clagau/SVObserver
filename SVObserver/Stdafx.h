//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : stdafx
//* .File Name       : $Workfile:   Stdafx.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   19 Dec 2014 14:02:00  $
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

#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#define NOMINMAX			// Exclude min/max macros

#define _CRT_SECURE_NO_DEPRECATE 
#define _AFX_SECURE_NO_DEPRECATE
#define _ATL_SECURE_NO_DEPRECATE
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _SCL_SECURE_NO_WARNINGS

#ifndef _DEBUG
#define _CRTDBG_MAP_ALLOC
#define _SECURE_SCL 0
#endif

#define _ATL_APARTMENT_THREADED

#pragma warning( disable: 4482 )
#pragma warning( disable: 4503 )
#pragma warning( disable: 4786 )	// identifier truncation 255 chars
#pragma warning( 4: 4786 )
#pragma warning( disable: 4250 )	// disable warning for dominance of inherits (need e.g. for ISVObjectClass)
#pragma warning( disable: 4172 )	//Disable returning address warning for boost graphs

#define OEMRESOURCE	// OEM resource to use nicer cursors...

//************************************
//! Precompiled headers section
//************************************
#pragma region Precompiled Headers
#include <algorithm>
namespace Gdiplus
{
	using std::min;
	using std::max;
}


#include <afx.h>
#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxctl.h>
#include <afxcmn.h>			// MFC-Unterstützung für gängige Windows-Steuerelemente
#include <afxext.h>         // MFC-Erweiterungen
#include <afxole.h>         // MFC OLE-Klassen
#include <afxodlgs.h>       // MFC OLE-Dialogfeldklassen
#include <afxdisp.h>        // MFC OLE-Automatisierungsklassen
#include <afxcview.h>		// MFC CListView Support
#include <afxtabview.h>
#include <afxtempl.h>		// MFC Templates
#include <afxvisualmanager.h>
#include <afxmt.h>
#include <afxcoll.h>
#include <afxpriv.h>
#include <atlbase.h>
#include <atlcom.h>
#include <atltypes.h>
//#include <afxdialogex.h>

#include <array>
#include <atlsafe.h>
#include <assert.h>
#include <cmath>
#include <colordlg.h>
#include <comdef.h>
#include <cstdio>				//Used by static library
#include <ctime>				//Used by static library
#include <concrt.h>
#include <crtdbg.h>
#include <cstdlib>
#include <ddraw.h>
#include <deque>
#include <direct.h>
#include <filesystem>
#include <float.h>
#include <fstream>
#include <functional>
#include <future>
#include <io.h>
#include <iostream>
#include <initguid.h>
#include <intrin.h>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <math.h>
#include <memory>
#include <mutex>
#include <numeric>
#include <Objidl.h>
#include <regex>
#pragma warning(push)
#pragma warning(disable : 4996)
#include <sequential_tree.h>
#pragma warning(pop)
#include <set>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <tuple>
#include <typeinfo>
#include <utility>
#include <vector>
#include <thread>
#include <winsock2.h>		//Used by static library
#include <ws2tcpip.h>		//Used by static library
#include <xmllite.h>

#define BOOST_BIND_NO_PLACEHOLDERS
#include <boost\algorithm\string.hpp>
#include <boost\algorithm\string\replace.hpp>
#include <boost/any.hpp>
#pragma warning(push)
#pragma warning(disable : 4702)
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#pragma warning(pop)
#include <boost/circular_buffer.hpp>
#include <boost/config.hpp>
#define BOOST_DATE_TIME_NO_LIB
#include <boost/date_time.hpp>
#include <boost/foreach.hpp>
#include <boost/fusion/container.hpp>						//Used by static library
#include <boost/fusion/algorithm.hpp>						//Used by static library
#pragma warning(push)
#pragma warning(disable : 4244 4458 4459)
#include <boost/graph/adjacency_list.hpp>					//Used by static library
#include <boost/graph/depth_first_search.hpp>				//Used by static library
#include <boost/graph/breadth_first_search.hpp>				//Used by static library
#include <boost/graph/reverse_graph.hpp>					//Used by static library
#include <boost/graph/filtered_graph.hpp>					//Used by static library
#include <boost/graph/graphviz.hpp>							//Used by static library
#pragma warning(pop)
#include <boost/lexical_cast.hpp>
#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/scoped_array.hpp>							//Used by static library
#include <boost/shared_array.hpp>							//Used by static library
#include <boost/tokenizer.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility.hpp>
#include <google\protobuf\stubs\common.h>
#pragma endregion Precompiled Headers

// Global includes that everyone needs
//@ Not every module needs these includes - fix this - SEJ
#include "SVOResource\resource.h"



