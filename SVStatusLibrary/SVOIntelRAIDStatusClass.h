// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOIntelRAIDStatusClass
// * .File Name       : $Workfile:   SVOIntelRAIDStatusClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 13:02:38  $
// ******************************************************************************

#pragma once

namespace SvStl
{
	class SVOIntelRAIDStatusClass
	{
	public:
		SVOIntelRAIDStatusClass();
		virtual ~SVOIntelRAIDStatusClass();

		HRESULT UpdateStatus();
		HRESULT CheckStatus();

		const std::string& GetRaidStatus();
		const std::string& GetErrorStatus();

		const HANDLE GetCheckEvent();

	protected:
		HANDLE m_hCheckEvent;

		std::string m_RaidStatus;
		std::string m_ErrorStatus;
	};
} //namespace SvStl
