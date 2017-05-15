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

#include "SVUtilityLibrary/SVString.h"

namespace SvSyl
{
	class SVVersionInfo
	{
	public:
		static SVString GetVersion();
		static unsigned long GetLongVersion();
		static SVString GetTitleVersion();
		static SVString GetShortTitleVersion();

	private:
		SVVersionInfo();
		~SVVersionInfo();
	};
} //namespace SvSyl
