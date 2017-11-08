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
#include "Definitions/SVPPQEnums.h"
#pragma endregion Includes

namespace SvLib
{
	#pragma region Declarations
	const int MaxTriggers = 4;
	#pragma endregion Declarations


	class SVOIniLoader
	{

	public:
		SVOIniLoader(InitialInformation &inifileInfo);
	
		const struct InitialInformation &GetInitialInfo() const { return m_rInitialInfo; }

		HRESULT LoadIniFiles(LPCTSTR svimIniFile, LPCTSTR oemIniFile, LPCTSTR hardwareIniFile);
		HRESULT LoadOEMIni(LPCTSTR oemIniFile);
		HRESULT LoadSVIMIni(LPCTSTR svimIniFile);
		HRESULT LoadHardwareIni(LPCTSTR hardwareIniFile);
		HRESULT DecodeModelNumber(LPCTSTR modelNumber);

		//@TODO [Arvid][7.50][09.06.2017] most of the remaining public data members in this class could be made private with moderate effort

		std::string m_ModelNumber;
		std::string m_WinKey;
		std::string m_SerialNumber;

		std::string m_TriggerEdge[MaxTriggers];
		std::string m_StrobeEdge[MaxTriggers];
		std::string m_StartFrameType[MaxTriggers];

		bool m_bUseCorrectListRecursion;

		HRESULT m_hrDecodeModelNumber;

	private:
		struct InitialInformation &m_rInitialInfo;

		bool m_bSingleCameraModel;

		std::string m_Opto22InputInvert;
		std::string m_Opto22OutputInvert;
	};

} //namespace SvLib
