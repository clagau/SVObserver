#pragma once

#pragma region Includes
#include "SVSharedImage.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSharedMemoryLibrary
{
	typedef boost::interprocess::vector< SVSharedImage, SVSharedImageAllocator > SVSharedImageVector;
	typedef boost::interprocess::allocator< SVSharedImageVector, segment_manager_t > SVSharedImageVectorAllocator;

	typedef SVSharedImageVector SVSharedImageContainer;
} /*namespace SVSharedMemoryLibrary*/ } /*namespace Seidenader*/

namespace SvSml = Seidenader::SVSharedMemoryLibrary;
