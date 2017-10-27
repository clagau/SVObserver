//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStartStopStreamJsonBuilder
//* .File Name       : $Workfile:   SVStartStopStreamJsonBuilder.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   16 Jan 2015 10:18:26  $
//******************************************************************************
#pragma once
#pragma region Includes
#include <string>
#pragma endregion Includes

namespace SeidenaderVision
{
	class SVStartStopStreamJsonBuilder
	{
	public:
		static std::string BuildStartCommand(const std::wstring& p_ListName);
		static std::string BuildStopCommand(const std::wstring& p_ListName);
	};
}
