//******************************************************************************
//* COPYRIGHT (c) 2011 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .File Name       : $Workfile:   SVVersionInfo.h  $
#pragma once



namespace SvSyl
{
	// Version numbering code as used in getSvoVersionAsUnsignedLong()
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

	std::string getSvoVersionAsString(bool showRcInformation=true);
	unsigned long getSvoVersionAsUnsignedLong();
	void setReleasedSvoVersion(const std::string& rReleasedVersionString);

} //namespace SvSyl
