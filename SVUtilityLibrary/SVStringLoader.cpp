//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// SVStringLoader is the interface to Load an SVString from a resource 
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVStringLoader.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace Utility
	{
		SVString SVStringLoader(HINSTANCE resHandle, UINT resID)
		{
			char str[1024]; 
			str[0] = '\0';
			int len = ::LoadStringA(resHandle, resID, reinterpret_cast<LPSTR>(&str), sizeof(str) - sizeof(char));
			if (len < 0)
			{
				assert(false);
			}
			return SVString(str);
		}
	}
}
