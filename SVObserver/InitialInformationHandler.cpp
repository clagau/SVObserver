//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file InitialInformationHandler.cpp
/// All Rights Reserved
//*****************************************************************************
/// Implementation  of class InitialInformationHandler
/// which oversees the loading and unloading of various DLLs as well as the 
/// evaluation reading of INI files
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <iostream>
#include "SVObserver.h"

#include "SVUtilityLibrary/StringHelper.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVLibrary\SVOINIClass.h"
#include "SVLibrary\SVOINILoader.h"
#include "InspectionEngine/SVDigitizerProcessingClass.h"
#include "SVIOLibrary\SVIOParameterEnum.h"
#include "TriggerInformation/SVHardwareManifest.h"
#include "TriggerInformation/SVTriggerProcessingClass.h"
#include "InitialInformationHandler.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "Definitions/GlobalConst.h"
#include "RootObject.h"

#pragma endregion Includes


static const HRESULT ErrorMatroxServiceNotRunning = 0xcf00116f;

extern bool g_bUseCorrectListRecursion;


#pragma region Public Methods
void InitialInformationHandler::ResetInformation()
{
	m_InitialInfo.m_ProductName.clear();

	m_InitialInfo.ResetBoardNames();

	m_InitialInfo.ResetDllInformation();

	m_InitialInfo.ResetModelNumberInformation();

	m_InitialInfo.m_Trigger.clear();
}

void InitialInformationHandler::LoadIniFilesAndDlls()
{
	SvLib::SVOIniLoader IniLoader(m_InitialInfo);

	TCHAR l_szSystemDir[MAX_PATH + 1];
	CString l_csSystemDir;

	::GetSystemDirectory(l_szSystemDir, MAX_PATH + 1);
	l_csSystemDir.Format("%s\\OEMINFO.INI", l_szSystemDir);

	// clear these variables
	m_InitialInfo.m_ProductName.clear();

	m_InitialInfo.ResetBoardNames();

	m_InitialInfo.ResetModelNumberInformation();

	// load the SVIM.ini, OEMINFO.ini, and HARDWARE.ini
	HRESULT l_hrOk = IniLoader.LoadIniFiles(SvStl::GlobalPath::Inst().GetSVIMIniPath(), l_csSystemDir, SvStl::GlobalPath::Inst().GetHardwareIniPath());

	if (S_OK == l_hrOk)
	{
		g_bUseCorrectListRecursion = IniLoader.m_bUseCorrectListRecursion;

		RootObject::setRootChildValue(SvDef::FqnEnvironmentModelNumber, IniLoader.m_ModelNumber.c_str());
		RootObject::setRootChildValue(SvDef::FqnEnvironmentSerialNumber, IniLoader.m_SerialNumber.c_str());
		RootObject::setRootChildValue(SvDef::FqnEnvironmentWinKey, IniLoader.m_WinKey.c_str());

		for (int i = 0; i < SvLib::MaxTriggers; i++)
		{
			bool Value(false);
			Value = (0 == SvUl::CompareNoCase(IniLoader.m_TriggerEdge[i], std::string(_T("R"))));
			SVIOConfigurationInterfaceClass::Instance().SetSVIMTriggerValue(i, Value);
			Value = (0 == SvUl::CompareNoCase(IniLoader.m_StrobeEdge[i], std::string(_T("R"))));
			SVIOConfigurationInterfaceClass::Instance().SetSVIMStrobeValue(i, Value);
			Value = (0 == SvUl::CompareNoCase(IniLoader.m_StartFrameType[i], std::string(_T("Y"))));
			SVIOConfigurationInterfaceClass::Instance().SetSVIMStrobeStartFrameActive(i, Value);
		}

		l_hrOk = l_hrOk | LoadDigitalDLL();
		l_hrOk = l_hrOk | LoadTriggerDLL();
		l_hrOk = l_hrOk | LoadSoftwareTriggerDLL();
		l_hrOk = l_hrOk | LoadAcquisitionTriggerDLL();
		l_hrOk = l_hrOk | LoadAcquisitionDLL();
		l_hrOk = l_hrOk | LoadFileAcquisitionDLL();
	}
	else
	{
		if (S_OK != IniLoader.m_hrDecodeModelNumber)
		{
			ASSERT(FALSE);
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
			Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_ModelNumberInvalid, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10236);
		}

	}
	m_InitializationStatusFlags = l_hrOk;
}


HRESULT InitialInformationHandler::LoadTriggerDLL()
{
	HRESULT l_hrOk = S_OK;

	if (!m_InitialInfo.m_TriggerDLL.empty())
	{
		VARIANT l_varValue;

		::VariantInit(&l_varValue);

		l_varValue.vt = VT_I4;

		if (S_OK != m_svDLLTriggers.Open(m_InitialInfo.m_TriggerDLL.c_str()))
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_TRIGGER;
		}

		if (S_OK != SvTi::SVTriggerProcessingClass::Instance().UpdateTriggerSubsystem(&m_svDLLTriggers))
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_TRIGGER;
		}

		for (int i = 0; i < 4; i++)
		{
			unsigned long l_ulHandle;

			if (S_OK == m_svDLLTriggers.GetHandle(&l_ulHandle, i))
			{
				bool l_bRising;

				SVIOConfigurationInterfaceClass::Instance().GetIOTriggerValue(i, l_bRising);

				if (l_bRising)
				{
					l_varValue.lVal = 1;
				}
				else
				{
					l_varValue.lVal = -1;
				}
				// SVSignalEdge enum is used here to make the code more clear.
				// however at some time in the future the Dll parameters may be implemented
				// as an array and therefore this enum may not apply.
				m_svDLLTriggers.SetParameterValue(l_ulHandle, SVSignalEdge, &l_varValue);
			}
		}
	}
	else
	{
		if (m_InitialInfo.m_Trigger != _T("00"))
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_TRIGGER;
		}
	}

	return l_hrOk;
}

HRESULT InitialInformationHandler::CloseTriggerDLL()
{
	SvTi::SVTriggerProcessingClass::Instance().clear();

	m_svDLLTriggers.Close();
	m_svDLLSoftwareTriggers.Close();
	m_svDLLAcquisitionTriggers.Close();

	return S_OK;
}

HRESULT InitialInformationHandler::LoadSoftwareTriggerDLL()
{
	HRESULT l_hrOk = S_OK;

	if (!m_InitialInfo.m_SoftwareTriggerDLL.empty())
	{
		if (S_OK != m_svDLLSoftwareTriggers.Open(m_InitialInfo.m_SoftwareTriggerDLL.c_str()))
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_SOFTWARETRIGGER;
		}

		if (S_OK != SvTi::SVTriggerProcessingClass::Instance().UpdateTriggerSubsystem(&m_svDLLSoftwareTriggers))
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_SOFTWARETRIGGER;
		}
	}
	return l_hrOk;
}

HRESULT InitialInformationHandler::LoadAcquisitionTriggerDLL()
{
	HRESULT l_hrOk = S_OK;

	if (!m_InitialInfo.m_AcquisitionTriggerDLL.empty())
	{
		HRESULT ResultLoadDLL(m_svDLLAcquisitionTriggers.Open(m_InitialInfo.m_AcquisitionTriggerDLL.c_str()));
		//Do not care about the Matrox service here as it will be handled in the LoadAcquisitionDLL
		if (S_OK != ResultLoadDLL && ErrorMatroxServiceNotRunning != ResultLoadDLL)
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_CAMERATRIGGER;
		}

		if (S_OK == ResultLoadDLL)
		{
			if (S_OK != SvTi::SVTriggerProcessingClass::Instance().UpdateTriggerSubsystem(&m_svDLLAcquisitionTriggers))
			{
				l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_CAMERATRIGGER;
			}
		}
	}
	return l_hrOk;
}

HRESULT InitialInformationHandler::LoadAcquisitionDLL()
{
	HRESULT l_hrOk = S_OK;

	if (!m_InitialInfo.m_DigitizerDLL.empty())
	{
		HRESULT ResultLoadDLL(m_svDLLDigitizers.Open(m_InitialInfo.m_DigitizerDLL.c_str()));
		if (S_OK != ResultLoadDLL)
		{
			//This is the error result which indicates that the Matrox Gige service is not running
			if (ErrorMatroxServiceNotRunning == ResultLoadDLL)
			{
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Exception.setMessage(SVMSG_SVO_90_MATROX_SERVICE_NOT_RUNNING, m_InitialInfo.m_DigitizerDLL.c_str(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_25048_NoMatroxService);
			}
			else
			{
				l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_ACQUISITION;
			}
		}
		else
		{
			if (S_OK != SvIe::SVDigitizerProcessingClass::Instance().UpdateDigitizerSubsystem(&m_svDLLDigitizers))
			{
				l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_ACQUISITION;
			}
		}
	}
	else
	{
		if (m_InitialInfo.m_FrameGrabber != _T("00"))
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_ACQUISITION;
		}
	}

	return l_hrOk;
}


HRESULT InitialInformationHandler::CloseAcquisitionDLL()
{
	SvIe::SVDigitizerProcessingClass::Instance().clear();

	m_svDLLDigitizers.Close();

	m_svDLLFileAcquisition.Close();

	return S_OK;
}

HRESULT InitialInformationHandler::LoadFileAcquisitionDLL()
{
	HRESULT l_hrOk = S_OK;

	if (!m_InitialInfo.m_FileAcquisitionDLL.empty())
	{
		if (S_OK != m_svDLLFileAcquisition.Open(m_InitialInfo.m_FileAcquisitionDLL.c_str()))
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_FILEACQUISITION;
		}

		if (S_OK != SvIe::SVDigitizerProcessingClass::Instance().UpdateDigitizerSubsystem(&m_svDLLFileAcquisition))
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_FILEACQUISITION;
		}
	}
	return l_hrOk;
}

HRESULT InitialInformationHandler::LoadDigitalDLL()
{
	HRESULT l_hrOk = S_OK;

	if (!m_InitialInfo.m_DigitalIODLL.empty())
	{
		if (SVIOConfigurationInterfaceClass::Instance().OpenDigital(m_InitialInfo.m_DigitalIODLL.c_str()) != S_OK)
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_IO;
		}
		else
		{	// Send the System type to the IO Board.
			VARIANT l_vt;
			l_vt.vt = VT_I4;

			// Hardware.ini has the new IOBoardOption.
			if (!m_InitialInfo.m_IOBoardOption.empty())
			{
				l_vt.lVal = atol(m_InitialInfo.m_IOBoardOption.c_str());
				SVIOConfigurationInterfaceClass::Instance().SetParameterValue(SVBoardType, &l_vt);
			}
			else
			{	// Legacy behavior.... Hardware.Ini file does not have new entry...
				l_vt.lVal = atol(m_InitialInfo.m_IOBoard.c_str());
				SVIOConfigurationInterfaceClass::Instance().SetParameterValue(SVBoardType, &l_vt);
			}
		}
	}
	else
	{
		if (m_InitialInfo.m_IOBoard != _T("00"))
		{
			l_hrOk = l_hrOk | SV_HARDWARE_FAILURE_IO;
		}
	}

	return l_hrOk;
}

HRESULT InitialInformationHandler::CloseDigitalDLL()
{
	SVIOConfigurationInterfaceClass::Instance().CloseDigital();

	return S_OK;
}

HRESULT InitialInformationHandler::INIReset()
{
	HRESULT l_hrOk = S_OK;

	if (0 == SvUl::CompareNoCase(m_InitialInfo.m_ReloadAcquisitionDLL, _T("Y")))
	{
		CloseAcquisitionDLL();
	}

	if (0 == SvUl::CompareNoCase(m_InitialInfo.m_ReloadTriggerDLL, _T("Y")))
	{
		CloseTriggerDLL();
	}

	if (0 == SvUl::CompareNoCase(m_InitialInfo.m_ReloadDigitalDLL, _T("Y")))
	{
		CloseDigitalDLL();
	}

	if (0 == SvUl::CompareNoCase(m_InitialInfo.m_ReloadDigitalDLL, _T("Y")))
	{
		l_hrOk = l_hrOk | LoadDigitalDLL();
	}

	if (0 == SvUl::CompareNoCase(m_InitialInfo.m_ReloadTriggerDLL, _T("Y")))
	{
		l_hrOk = l_hrOk | LoadTriggerDLL();
		l_hrOk = l_hrOk | LoadSoftwareTriggerDLL();
		l_hrOk = l_hrOk | LoadAcquisitionTriggerDLL();
	}

	if (0 == SvUl::CompareNoCase(m_InitialInfo.m_ReloadAcquisitionDLL, _T("Y")))
	{
		l_hrOk = l_hrOk | LoadAcquisitionDLL();
		l_hrOk = l_hrOk | LoadFileAcquisitionDLL();
	}

	return l_hrOk;
}

HRESULT InitialInformationHandler::INIClose()
{
	m_InitializationStatusFlags = SV_HARDWARE_FAILURE_ALL;

	m_InitialInfo.m_ProductName.clear();

	m_InitialInfo.ResetModelNumberInformation();

	m_InitialInfo.ResetBoardNames();

	CloseAcquisitionDLL();

	CloseTriggerDLL();

	CloseDigitalDLL();

	return S_OK;
}
#pragma endregion Public Methods


