//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file InitialInformationHandler.h
/// All Rights Reserved
//*****************************************************************************
/// Declaration of class InitialInformation which is meant to pass around
/// information collected from INI files 
/// (which are currently read in by SVOIniLoader)
//******************************************************************************

#pragma once

#include "SVUtilityLibrary/SVString.h"
#include "ObjectInterfaces/SVPPQEnums.h"

enum SVHardwareErrorEnums
{
	SV_HARDWARE_FAILURE_IO = 0xC0000001L,
	SV_HARDWARE_FAILURE_TRIGGER = 0xC0000004L,
	SV_HARDWARE_FAILURE_ACQUISITION = 0xC0000008L,
	SV_HARDWARE_FAILURE_NO_CAMAERAS = 0xC0000010L,
	SV_HARDWARE_FAILURE_SOFTWARETRIGGER = 0xC0000020L,
	SV_HARDWARE_FAILURE_FILEACQUISITION = 0xC0000040L,
	SV_HARDWARE_FAILURE_CAMERATRIGGER = 0xC0000080L,
	SV_HARDWARE_FAILURE_ALL = 0xC00000FFL, // this is all the above combined

	SV_CAN_GO_ONLINE_FAILURE_ACQUISITION = 0xC0000200L,
};


namespace SvLib
{
	struct InitialInformation
	{

	public:
#pragma region Constructor
		InitialInformation();
#pragma endregion Constructor

#pragma region Public Methods
		void ResetBoardNames();
		void ResetDllInformation();
		void ResetModelNumberInformation();

		SVStringVector GetModelNumberVector() const;

		SVString GetModelNumberString() const;

		SVString InitializationFailureDescription(HRESULT InitializationStatusFlags) const;
#pragma endregion Public Methods

#pragma region Member variables
	public:
		SVString m_ProcessorBoardName;
		SVString m_TriggerBoardName;
		SVString m_AcquisitionBoardName;
		SVString m_DigitalBoardName;
		SVString m_RAIDBoardName;
		SVString m_FileAcquisitionBoardName;

		SVString m_DigitizerDLL;
		SVString m_TriggerDLL;
		SVString m_DigitalDLL;
		SVString m_FileAcquisitionDLL;
		SVString m_SoftwareTriggerDLL;
		SVString m_AcquisitionTriggerDLL;

		SVString m_ReloadAcquisitionDLL;
		SVString m_ReloadTriggerDLL;
		SVString m_ReloadDigitalDLL;

		SVString m_HardwareOptions;
		SVString m_IOBoardOption;
		SVString m_Trigger;

		//Elements of SVIM model number:
		SVString m_Processor;
		SVString m_FrameGrabber;
		SVString m_IOBoard;
		SVString m_Options;
		SVString m_ProductName;
		long m_gigePacketSize;

		SvOi::NakGeneration m_NAKMode;
		int m_NAKParameter;

		// The ForcedImageUpdateTimeInSeconds comes from the SVIM.ini file
		// It is to be placed in the [Display] section and has the following syntax:
		// ForcedImageUpdateTimeInSeconds=N
		// Where N is zero to 255, zero means no forced update.
		unsigned char m_forcedImageUpdateTimeInSeconds;
#pragma endregion Member variables
	};
} //namespace SvLib

