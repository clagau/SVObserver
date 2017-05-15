//*****************************************************************************
// \copyright COPYRIGHT (c) 2016 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the typedef for the SVInterfaceList
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectTypeInfoStruct.h"
#include "SVContainerLibrary/SVVector.h"
#pragma endregion Includes

namespace SvOi
{
	typedef SVVector<SVObjectTypeInfoStruct> SVInterfaceVector;
} //namespace SvOi
