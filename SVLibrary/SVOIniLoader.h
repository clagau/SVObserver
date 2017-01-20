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

namespace Seidenader { namespace SVLibrary
{
	#pragma region Declarations
	const int MaxTriggers = 4;
	#pragma endregion Declarations

	class SVOIniLoader
	{
	public:
		SVString m_WinKey;
		SVString m_ModelNumber;
		SVString m_SerialNumber;
		SVString m_Processor;
		SVString m_FrameGrabber;
		SVString m_IOBoard;
		SVString m_Options;

		SVString m_Trigger;

		SVString m_Opto22InputInvert;
		SVString m_Opto22OutputInvert;
		SVString m_TriggerEdge[MaxTriggers];
		SVString m_StrobeEdge[MaxTriggers];
		SVString m_StartFrameType[MaxTriggers];

		SVString m_ProductName;
		SVString m_ProcessorBoardName;
		SVString m_TriggerBoardName;
		SVString m_AcquisitionBoardName;
		SVString m_DigitalBoardName;
		SVString m_RAIDBoardName;
	
		SVString m_DigitizerDLL;
		SVString m_TriggerDLL;
		SVString m_ReloadAcquisitionDLL;
		SVString m_ReloadTriggerDLL;
		SVString m_DigitalDLL;
		SVString m_ReloadDigitalDLL;
		SVString m_FileAcquisitionDLL;
		SVString m_SoftwareTriggerDLL;
		SVString m_AcquisitionTriggerDLL;
		SVString m_HardwareOptions;

		SVString m_IOBoardOption;
	
		long m_gigePacketSize;
		bool m_bUseCorrectListRecursion;
	
		HRESULT m_hrOEMFailure;
		
		SVOIniLoader();
	
		HRESULT Load(LPCTSTR svimIniFile, LPCTSTR oemIniFile, LPCTSTR hardwareIniFile);
		HRESULT LoadOEMIni(LPCTSTR oemIniFile);
		HRESULT LoadSVIMIni(LPCTSTR svimIniFile);
		HRESULT LoadHardwareIni(LPCTSTR hardwareIniFile);
		HRESULT DecodeModelNumber(LPCTSTR modelNumber);
		unsigned char GetForcedImageUpdateTime() const;
		NakGeneration GetNAKMode() const;
		int GetNAKPar() const;

	protected:
		bool m_bSingleCameraModel;
		unsigned char m_forcedImageUpdateTimeInSeconds;
		NakGeneration m_NAKMode;
		int m_NAKParameter; 
	};
} /* namespace SVLibrary */ } /* namespace Seidenader */

namespace SvLib = Seidenader::SVLibrary;

