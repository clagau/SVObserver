#pragma once

#pragma region Includes
#include "SVSharedValue.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	typedef boost::interprocess::vector< SVSharedValue, SVSharedValueAllocator > SVSharedValueVector;
	typedef boost::interprocess::allocator< SVSharedValueVector, segment_manager_t > SVSharedValueVectorAllocator;
	typedef SVSharedValueVector SVSharedValueContainer;
} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;