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
#include "ObjectInterfaces/SVPPQEnums.h"
#include "SVUtilityLibrary/SVString.h"
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

		SVString m_ModelNumber;
		SVString m_WinKey;
		SVString m_SerialNumber;

		SVString m_TriggerEdge[MaxTriggers];
		SVString m_StrobeEdge[MaxTriggers];
		SVString m_StartFrameType[MaxTriggers];

		bool m_bUseCorrectListRecursion;

		HRESULT m_hrDecodeModelNumber;

	private:
		struct InitialInformation &m_rInitialInfo;

		bool m_bSingleCameraModel;

		SVString m_Opto22InputInvert;
		SVString m_Opto22OutputInvert;
	};

} //namespace SvLib
