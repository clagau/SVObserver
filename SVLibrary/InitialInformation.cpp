//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
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
//Moved to precompiled header: #include <boost\algorithm\string.hpp>
#pragma endregion Includes


#pragma region Declarations
static const TCHAR* const UnknownboardTag = _T("Unknown board");
#pragma endregion Declarations


namespace SvLib
{
#pragma region Constructor

	InitialInformation::InitialInformation() : m_gigePacketSize(0)
		, m_NAKMode(SvDef::NakGeneration::Bursts)
		, m_NAKParameter(SvDef::DefaultNakParameter)
	, m_forcedImageUpdateTimeInSeconds(0) // zero means no forced image update
	{}
#pragma endregion Constructor

#pragma region Public Methods
	void InitialInformation::ResetBoardNames()
	{
		m_ProcessorBoardName = UnknownboardTag;
		m_TriggerBoardName = UnknownboardTag;
		m_FileAcquisitionBoardName = UnknownboardTag;
		m_AcquisitionBoardName = UnknownboardTag;
		m_DigitalBoardName = UnknownboardTag;
		m_RAIDBoardName = UnknownboardTag;
	}


	void InitialInformation::ResetDllInformation()
	{
		m_TriggerDLL.clear();
		m_SoftwareTriggerDLL.clear();
		m_AcquisitionTriggerDLL.clear();
		m_DigitizerDLL.clear();
		m_FileAcquisitionDLL.clear();
		m_DigitalDLL.clear();

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


	SVStringVector InitialInformation::GetModelNumberVector() const
	{
		SVStringVector	msgList;
		msgList.push_back(m_Processor);
		msgList.push_back(m_FrameGrabber);
		msgList.push_back(m_IOBoard);
		msgList.push_back(m_Options);
		return msgList;
	}

	SVString InitialInformation::GetModelNumberString() const
	{
		return boost::algorithm::join(GetModelNumberVector(), " ");
	}


	SVString InitialInformation::InitializationFailureDescription(HRESULT InitializationStatusFlags) const
	{
		SVStringVector msgDllLoadFailuresList;

		if ((InitializationStatusFlags & SV_HARDWARE_FAILURE_IO) == SV_HARDWARE_FAILURE_IO)
		{
			msgDllLoadFailuresList.push_back(_T("Discrete IO: ") + m_DigitalBoardName + _T("/") + m_DigitalDLL);
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
			msgDllLoadFailuresList.push_back(SVString("(unknown Problem) "));
		}

		return boost::algorithm::join(msgDllLoadFailuresList, "\n");
	}
#pragma endregion Public Methods




} //namespace SvLib