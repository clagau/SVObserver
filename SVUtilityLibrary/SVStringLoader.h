//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// SVStringLoader is the interface to Load an SVString from a resource 
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVString.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace Utility
	{
		SVString SVStringLoader(HINSTANCE resHandle, UINT resID);
	}
}
namespace SvU = Seidenader::Utility;
