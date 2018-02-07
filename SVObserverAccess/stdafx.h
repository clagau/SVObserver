// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
//avoid warning: 'identifier': decorated name length exceeded, name was truncated
//@Todo[MEC][8.00] [22.12.2017] remove this warning https://msdn.microsoft.com/de-de/library/074af4b6.aspx
#pragma warning( disable: 4503 ) 

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <thread>
#include <vector>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>			//Used by static library

#define BOOST_DATE_TIME_NO_LIB

#include <boost/multi_index_container.hpp>				//Used by static library
#include <boost/multi_index/member.hpp>					//Used by static library
#include <boost/multi_index/ordered_index.hpp>			//Used by static library
#include <boost/shared_array.hpp>							//Used by static library
#include <boost/scoped_array.hpp>
#include <boost/utility.hpp>								//Used by static library
#include <comutil.h>


