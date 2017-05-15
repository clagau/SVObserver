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

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <iostream>
//Moved to precompiled header: #include <iomanip>
//Moved to precompiled header: #include <limits>

//#define BOOST_USE_WINDOWS_H // does not work correctly // for boost 1_56_0

//Moved to precompiled header: #include <boost/interprocess/detail/shared_dir_helpers.hpp>
//Moved to precompiled header: #include <boost/interprocess/file_mapping.hpp>
//Moved to precompiled header: #include <boost/interprocess/mapped_region.hpp>
//Moved to precompiled header: #include <boost/interprocess/managed_shared_memory.hpp>
//Moved to precompiled header: #include <boost/interprocess/allocators/allocator.hpp>
//Moved to precompiled header: #include <boost/interprocess/containers/map.hpp>
//Moved to precompiled header: #include <boost/interprocess/containers/flat_map.hpp>
//Moved to precompiled header: #include <boost/interprocess/containers/string.hpp>
//Moved to precompiled header: #include <boost/interprocess/containers/vector.hpp>
//Moved to precompiled header: #include <boost/interprocess/offset_ptr.hpp>
//Moved to precompiled header: #include <boost/interprocess/sync/interprocess_mutex.hpp>
//Moved to precompiled header: #include <boost/interprocess/sync/scoped_lock.hpp>

//Moved to precompiled header: #include <boost/function.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>
#pragma endregion Includes

namespace bip = boost::interprocess;

namespace SvSml
{
	//Typedefs of allocators and containers
	typedef bip::managed_shared_memory::segment_manager                       segment_manager_t;
	typedef bip::allocator<void, segment_manager_t>                           void_allocator;
	typedef bip::allocator<char, segment_manager_t>                           char_allocator;
	typedef bip::basic_string<char, std::char_traits<char>, char_allocator>   bip_string;
	typedef bip::allocator<bip_string, segment_manager_t>					  bip_string_allocator;
	typedef bip::offset_t													  offset_t;

	struct statics
	{
		static const int max_result_size = 512;
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
	
	
	namespace ListType
	{
		//! enum for monitorlist sublist type	
		enum  typ { productItemsData =0 ,productItemsImage, rejectCondition, failStatus, Count };
	}
	
} //namespace SvSml
