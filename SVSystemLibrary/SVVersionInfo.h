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



namespace SvSyl
{
	class SVVersionInfo
	{
	public:
		static std::string GetVersion();
		static unsigned long GetLongVersion();
		static std::string GetShortTitleVersion();

	private:
		SVVersionInfo() = delete;
		~SVVersionInfo() = delete;
	};
} //namespace SvSyl
