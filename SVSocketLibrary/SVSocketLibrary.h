//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSocketLibrary
//* .File Name       : $Workfile:   SVSocketLibrary.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:17:12  $
//******************************************************************************
#pragma once
#pragma comment (lib, "ws2_32.lib")

#pragma region Includes
#include "SVSocketError.h"
#pragma endregion Includes

namespace Seidenader { namespace SVSocketLibrary
{
	class SVSocketLibrary
	{
	public:
		static SVSocketError::ErrorEnum Init();
		static SVSocketError::ErrorEnum Destroy();

	private:
		SVSocketLibrary();
		~SVSocketLibrary();
	};
} /*namespace SVSocketLibrary*/ } /*namespace Seidenader*/

namespace SvSol = Seidenader::SVSocketLibrary;

