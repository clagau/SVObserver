//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file InitialInformationHandler.cpp
/// All Rights Reserved
//*****************************************************************************
/// Implementation of class InitialInformation which is meant to pass around
/// information collected from INI files 
/// (which are currently read in by SVOIniLoader)
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "InitialInformation.h"
#pragma endregion Includes


#pragma region Declarations
constexpr const char* UnknownboardTag = _T("Unknown board");
#pragma endregion Declarations


namespace SvLib
{
#pragma region Public Methods
	void InitialInformation::ResetBoardNames()
	{
		m_ProcessorBoardName = UnknownboardTag;
		m_TriggerBoardName = UnknownboardTag;
		m_FileAcquisitionBoardName = UnknownboardTag;
		m_AcquisitionBoardName = UnknownboardTag;
		m_DigitalBoardName = UnknownboardTag;
	}


	void InitialInformation::ResetDllInformation()
	{
		m_TriggerDLL.clear();
		m_SoftwareTriggerDLL.clear();
		m_CameraTriggerDLL.clear();
		m_DigitizerDLL.clear();
		m_FileAcquisitionDLL.clear();
		m_DigitalIODLL.clear();

		m_ReloadTriggerDLL = _T("Y");
		m_ReloadAcquisitionDLL = _T("Y");
		m_ReloadDigitalDLL = _T("Y");
	}


	void InitialInformation::ResetModelNumberInformation()
	{
		m_Processor.clear();
		m_FrameGrabber.clear();
		m_IOBoard.clear();
		m_Options.clear();
	}


	SvDef::StringVector InitialInformation::GetModelNumberVector() const
	{
		SvDef::StringVector	msgList;
		msgList.push_back(m_Processor);
		msgList.push_back(m_FrameGrabber);
		msgList.push_back(m_IOBoard);
		msgList.push_back(m_Options);
		return msgList;
	}

	std::string InitialInformation::ModelNumberString() const
	{
		return boost::algorithm::join(GetModelNumberVector(), " ");
	}


	std::string InitialInformation::InitializationFailureDescription(HRESULT InitializationStatusFlags) const
	{
		SvDef::StringVector msgDllLoadFailuresList;

		if ((InitializationStatusFlags & SV_HARDWARE_FAILURE_IO) == SV_HARDWARE_FAILURE_IO)
		{
			msgDllLoadFailuresList.push_back(_T("Discrete IO: ") + m_DigitalBoardName + _T("/") + m_DigitalIODLL);
		}
		if ((InitializationStatusFlags & SV_HARDWARE_FAILURE_TRIGGER) == SV_HARDWARE_FAILURE_TRIGGER)
		{
			msgDllLoadFailuresList.push_back(_T("Triggering: ") + m_TriggerBoardName + _T("/") + m_TriggerDLL);
		}
		if ((InitializationStatusFlags & SV_HARDWARE_FAILURE_SOFTWARETRIGGER) == SV_HARDWARE_FAILURE_SOFTWARETRIGGER)
		{
			msgDllLoadFailuresList.push_back(_T("SW Trigger: ") + m_SoftwareTriggerDLL); // m_SoftwareTriggerDLL does not make much sense here: there is no Hardware associated with software triggering
		}
		if ((InitializationStatusFlags & SV_HARDWARE_FAILURE_ACQUISITION) == SV_HARDWARE_FAILURE_ACQUISITION)
		{
			msgDllLoadFailuresList.push_back(_T("Image Acquisition: ") + m_AcquisitionBoardName + _T("/") + m_DigitizerDLL);
		}
		if ((InitializationStatusFlags & SV_HARDWARE_FAILURE_FILEACQUISITION) == SV_HARDWARE_FAILURE_FILEACQUISITION)
		{
			msgDllLoadFailuresList.push_back(_T("File Acquisition: ") + m_FileAcquisitionDLL);// m_FileAcquisitionBoardName does not make much sense here: there is no Hardware associated with file acquisition
		}
		if ((InitializationStatusFlags & SV_HARDWARE_FAILURE_ALL) != InitializationStatusFlags)
		{
			msgDllLoadFailuresList.push_back(std::string("(unknown Problem) "));
		}

		return boost::algorithm::join(msgDllLoadFailuresList, "\n");
	}
#pragma endregion Public Methods




} //namespace SvLib