// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOIniLoader.h
// * .File Name       : $Workfile:   SVOIniLoader.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   10 Jun 2013 18:08:06  $
// ******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvLib
{
	#pragma region Declarations
	const int MaxTriggers = 4;
	#pragma endregion Declarations

	class SVOIniLoader
	{
	public:
		explicit SVOIniLoader(InitialInformation& inifileInfo) : m_rInitialInfo(inifileInfo) {}

		const struct InitialInformation &GetInitialInfo() const { return m_rInitialInfo; }

		void LoadIniFiles(LPCTSTR svimIniFile, LPCTSTR oemIniFile, LPCTSTR hardwareIniFile);
		void LoadOEMIni(LPCTSTR oemIniFile);
		void LoadSVIMIni(LPCTSTR svimIniFile);
		HRESULT LoadHardwareIni(LPCTSTR hardwareIniFile);
		void DecodeModelNumber(LPCTSTR modelNumber);

		bool isModelNumberDecodable() {return  m_modelNumberIsDecodable;}

		auto ModelNumberString() const { return m_ModelNumber.c_str(); }
		auto SerialNumberString() const { return m_SerialNumber.c_str(); }
		auto WinKeyString() const {return m_WinKey.c_str(); }
		bool UseCorrectListRecursion() const { return m_bUseCorrectListRecursion; }

		//@TODO [Arvid][10.10][20.08.2020] the remaining public data members in this class should also be private
		std::string m_TriggerEdge[MaxTriggers];
		std::string m_StrobeEdge[MaxTriggers];
		std::string m_StartFrameType[MaxTriggers];

	private:
		struct InitialInformation &m_rInitialInfo;

		std::string m_ModelNumber;
		std::string m_WinKey;
		std::string m_SerialNumber;

		bool m_modelNumberIsDecodable = false;
		bool m_bSingleCameraModel = false;
		bool m_bUseCorrectListRecursion = false;

		std::string m_Opto22InputInvert;
		std::string m_Opto22OutputInvert;

	};

	/// Read SerialNumber from BIOS and if it a valid (8 oder 9 character) the return true
	/// \param serialNumber [in]
	/// \returns bool True if serialNumber could be read and it has (after trim) 8 oder 9 characters.
	bool readSerialNumberFromSystem(std::string& serialNumber);
} //namespace SvLib
