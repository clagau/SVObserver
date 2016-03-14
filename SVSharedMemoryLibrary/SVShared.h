//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVShared
//* .File Name       : $Workfile:   SVShared.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 Aug 2014 17:07:38  $
//******************************************************************************
#pragma once
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <vector>
#include <deque>
#include <string>
#include <iostream>
#include <iomanip>
#include <limits>

//#define BOOST_USE_WINDOWS_H // does not work correctly // for boost 1_56_0
#define BOOST_INTERPROCESS_SHARED_DIR_PATH "V:/boost_interprocess"
#define BOOST_DATE_TIME_NO_LIB

#include <boost/interprocess/detail/shared_dir_helpers.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/flat_map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/offset_ptr.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <boost/function.hpp>
#include <boost/bind.hpp>

//#include <vld.h>

//Typedefs of allocators and containers
typedef boost::interprocess::managed_shared_memory::segment_manager                       segment_manager_t;
typedef boost::interprocess::allocator<void, segment_manager_t>                           void_allocator;
typedef boost::interprocess::allocator<char, segment_manager_t>                           char_allocator;
typedef boost::interprocess::basic_string<char, std::char_traits<char>, char_allocator>   char_string;
typedef boost::interprocess::allocator<char_string, segment_manager_t>                    char_string_allocator;
typedef boost::interprocess::offset_t													  offset_t;

struct statics
{
	static const int K = 1024;
	static const int M = K * K;
	static const size_t page_sz = 4 * K;
};

namespace ds
{
	enum DataState:long { none = 0x00, ready = 0x01000000, writing = 0x02000000, reading = 0x0000ffff };
}

namespace img
{
	enum type { bmp, png, jpg };
}

