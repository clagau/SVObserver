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


//Moved to precompiled header: #include <boost/function.hpp>
//Moved to precompiled header: #include <boost/bind.hpp>
#pragma endregion Includes

namespace SvSml
{

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
		enum flag { productItemsDataFlag = 0x1, productItemsImageFlag= 0x2, rejectConditionFlag = 0x4, failStatusFlag = 0x8  };
	}
	

	const static DWORD ListFlags[ListType::Count] =
	{
		ListType::productItemsDataFlag, ListType::productItemsImageFlag, ListType::rejectConditionFlag,ListType::failStatusFlag
	};
	


} //namespace SvSml
