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
	// Version numbering code:
	//
	// DwCurrentVersion, Type DWORD
	// Meaning:		0x00mmssbb => Version m.s  - Beta b 
	// EXCEPTION:	if b = 255 is the release indicator 
	//					No Beta number in released Versions!!!
	//	e.g.
	//				0x000100FF => Version 1.0
	//		        0x00011219 => Version 1.18 Beta 25
	//				0x00020001 => Version 2.0  Beta 1
	union SVVersionUnion
	{
		struct SVVersionParts
		{
			uint8_t  m_Beta;
			uint8_t  m_Minor;
			uint8_t  m_Major;
			uint8_t  m_Unused;
		} m_VersionParts;

		unsigned long m_Version = 0;

	};

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
