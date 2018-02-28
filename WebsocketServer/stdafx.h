//******************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file stdafx.h
/// All Rights Reserved
//******************************************************************************
/// include file for standard system include files,
/// or project specific include files that are used frequently, but
/// are changed infrequently
//******************************************************************************

#pragma once

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

#include <boost/any.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp> //Used by static library
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <stdio.h>
#include <string>
#include <tchar.h>
#include <thread>
#include <vector>
//#define BOOST_INTERPROCESS_SHARED_DIR_PATH "V:/boost_interprocess"
#define BOOST_DATE_TIME_NO_LIB

#include <boost/multi_index/member.hpp>                     //Used by static library
#include <boost/multi_index/ordered_index.hpp>              //Used by static library
#include <boost/multi_index_container.hpp>                  //Used by static library
#include <boost/scoped_array.hpp>
#include <boost/shared_array.hpp> //Used by static library
#include <boost/utility.hpp>      //Used by static library

#include <comutil.h>
#include <windows.h>

