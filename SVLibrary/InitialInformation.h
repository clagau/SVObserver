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

#pragma region Includes
#include "Definitions/SVPPQEnums.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

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
	constexpr  int DefaultMaxPreloadFileNumber = 50;
	constexpr  int DefaultPreloadTimeDelay = -1;
	struct InitialInformation
	{

	public:
#pragma region Constructor
		InitialInformation() = default;
#pragma endregion Constructor

#pragma region Public Methods
		void ResetBoardNames();
		void ResetDllInformation();
		void ResetModelNumberInformation();

		SvDef::StringVector GetModelNumberVector() const;

		std::string ModelNumberString() const;

		std::string InitializationFailureDescription(HRESULT InitializationStatusFlags) const;
#pragma endregion Public Methods

#pragma region Member variables
	public:
		std::string m_ProcessorBoardName;
		std::string m_TriggerBoardName;
		std::string m_AcquisitionBoardName;
		std::string m_DigitalBoardName;
		std::string m_RAIDBoardName;
		std::string m_FileAcquisitionBoardName;

		std::string m_DigitizerDLL;
		std::string m_TriggerDLL;
		std::string m_DigitalIODLL;
		std::string m_FileAcquisitionDLL;
		std::string m_SoftwareTriggerDLL;
		std::string m_CameraTriggerDLL;

		std::string m_ReloadAcquisitionDLL;
		std::string m_ReloadTriggerDLL;
		std::string m_ReloadDigitalDLL;

		std::string m_HardwareOptions;
		std::string m_IOBoardOption;
		std::string m_Trigger;

		//Elements of SVIM model number:
		std::string m_Processor;
		std::string m_FrameGrabber;
		std::string m_IOBoard;
		std::string m_Options;
		std::string m_ProductName;
		long m_gigePacketSize{0L};

		SvDef::NakGeneration m_NAKMode{SvDef::NakGeneration::Bursts};
		int m_NAKParameter{ SvDef::DefaultNakParameter };

		///This value is the time window in ms for cases when the camera start frame arrives before the trigger in discrete IO mode
		double m_preTriggerTimeWindow{0.0};

		// The ForcedImageUpdateTimeInSeconds comes from the SVIM.ini file
		// It is to be placed in the [Display] section and has the following syntax:
		// ForcedImageUpdateTimeInSeconds=N
		// Where N is 0 to 255, 0 means no forced update.
		unsigned char m_forcedImageUpdateTimeInSeconds{0};

		//Parameter for file acquisition 
		long m_MaxPreloadFileNumber{ DefaultMaxPreloadFileNumber };
		long  m_PreloadTimeDelay{ DefaultPreloadTimeDelay };

#pragma endregion Member variables
	};
} //namespace SvLib

