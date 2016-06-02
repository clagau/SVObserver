//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVersionInfo
//* .File Name       : $Workfile:   SVVersionInfo.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:04:00  $
//******************************************************************************
#pragma once

//Moved to precompiled header: #include <string>

namespace Seidenader { namespace SVSystemLibrary
{
	class SVVersionInfo
	{
	public:
		static std::string GetVersion();
		static unsigned long GetLongVersion();
		static std::string GetTitleVersion();
		static std::string GetShortTitleVersion();

	private:
		SVVersionInfo();
		~SVVersionInfo();
	};
} /*SVSystemLibrary*/ } /*Seidenader*/

namespace SvSyl = Seidenader::SVSystemLibrary;
