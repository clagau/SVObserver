//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
#include "InitialInformationHandler.h"
#include "RootObject.h"
#include "Definitions/GlobalConst.h"
#include "InspectionEngine/SVDigitizerProcessingClass.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#include "SVLibrary/SVOINILoader.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "Triggering/SVTriggerProcessingClass.h"
#include "Triggering/SVTriggerConstants.h"
#pragma endregion Includes


static const HRESULT ErrorMatroxServiceNotRunning = 0xcf00116f;

extern bool g_bUseCorrectListRecursion;

InitialInformationHandler g_IniInfoHandler;

InitialInformationHandler& getIniInfoHandler()
{
	return g_IniInfoHandler;
}



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

	TCHAR szSystemDir[MAX_PATH + 1];
	CString csSystemDir;

	::GetSystemDirectory(szSystemDir, MAX_PATH + 1);
	csSystemDir.Format("%s\\OEMINFO.INI", szSystemDir);

	// clear these variables
	m_InitialInfo.m_ProductName.clear();

	m_InitialInfo.ResetBoardNames();

	m_InitialInfo.ResetModelNumberInformation();

	// load the SVIM.ini, OEMINFO.ini, and HARDWARE.ini
	IniLoader.LoadIniFiles(SvStl::GlobalPath::Inst().GetSVIMIniPath(), csSystemDir, SvStl::GlobalPath::Inst().GetHardwareIniPath());

	if (true == IniLoader.isModelNumberDecodable())
	{
		m_InitializationStatusFlags = S_OK;
		g_bUseCorrectListRecursion = IniLoader.UseCorrectListRecursion();

		RootObject::setRootChildValue(SvDef::FqnEnvironmentModelNumber, IniLoader.ModelNumberString());
		RootObject::setRootChildValue(SvDef::FqnEnvironmentSerialNumber, IniLoader.SerialNumberString());
		RootObject::setRootChildValue(SvDef::FqnEnvironmentWinKey, IniLoader.WinKeyString());

		for (int i = 0; i < SvLib::MaxTriggers; i++)
		{
			bool Value(false);
			Value = IniLoader.TriggerEdgeEqualsNoCase(i, std::string(_T("R")));
			SVIOConfigurationInterfaceClass::Instance().SetSVIMTriggerValue(i, Value);
			Value = IniLoader.StrobeEdgeEqualsNoCase(i, std::string(_T("R")));
			SVIOConfigurationInterfaceClass::Instance().SetSVIMStrobeValue(i, Value);
			Value = IniLoader.StartFrameTypeEqualsNoCase(i, std::string(_T("Y")));
			SVIOConfigurationInterfaceClass::Instance().SetSVIMStrobeStartFrameActive(i, Value);
		}

		m_InitializationStatusFlags |= LoadTriggerResultDLL();
		m_InitializationStatusFlags |= LoadSoftwareTriggerDLL();
		m_InitializationStatusFlags |= LoadCameraTriggerDLL();
		m_InitializationStatusFlags |= LoadAcquisitionDLL();
		m_InitializationStatusFlags |= LoadFileAcquisitionDLL();

		SvSyl::setReleasedSvoVersion(m_InitialInfo.m_ReleasedSvoVersion);
	}
	else
	{
		Log_Assert(FALSE);

		m_InitializationStatusFlags = S_FALSE;
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_SVObserver_ModelNumberInvalid, SvStl::SourceFileParams(StdMessageParams));
	}
}

HRESULT InitialInformationHandler::LoadTriggerResultDLL()
{
	HRESULT hrOk = S_OK;

	if (nullptr != m_triggerDllHandle)
	{
		// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
		Sleep(0);
		::FreeLibrary(m_triggerDllHandle);
		m_triggerDllHandle = nullptr;

	}

	if (!m_InitialInfo.m_TriggerResultDll.empty())
	{
		m_triggerDllHandle = ::LoadLibrary(m_InitialInfo.m_TriggerResultDll.c_str());
		// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
		Sleep(0);
	}

	if(nullptr != m_triggerDllHandle)
	{
		hrOk = hrOk | LoadDigitalPart();
		hrOk = hrOk | LoadTriggerPart();
	}
	else
	{
		if (m_InitialInfo.m_IOBoard != _T("00"))
		{
			hrOk = hrOk | SV_HARDWARE_FAILURE_IO;
		}

		if (m_InitialInfo.m_Trigger != _T("00"))
		{
			hrOk = hrOk | SV_HARDWARE_FAILURE_TRIGGER;
		}
	}

	return hrOk;
}

HRESULT InitialInformationHandler::CloseTriggerResultDLL()
{
	SvTrig::SVTriggerProcessingClass::Instance().clear();

	m_svDLLTriggers.Close();
	m_svDLLSoftwareTriggers.Close();
	m_svDLLCameraTriggers.Close();
	SVIOConfigurationInterfaceClass::Instance().CloseDigital();

	if (::FreeLibrary(m_triggerDllHandle))
	{
		// This sleep(0) was added after the FreeLibrary to fix a bug where the system ran out of resources.
		Sleep(0);
		m_triggerDllHandle = nullptr;
		return S_OK;
	}
	else
	{
		m_triggerDllHandle = nullptr;
		return E_FAIL;
	}
}

HRESULT InitialInformationHandler::LoadSoftwareTriggerDLL()
{
	HRESULT hrOk = S_OK;

	if (!m_InitialInfo.m_SoftwareTriggerDLL.empty())
	{
		if (S_OK != m_svDLLSoftwareTriggers.Open(m_InitialInfo.m_SoftwareTriggerDLL.c_str()))
		{
			hrOk = hrOk | SV_HARDWARE_FAILURE_SOFTWARETRIGGER;
		}

		if (S_OK != SvTrig::SVTriggerProcessingClass::Instance().UpdateTriggerSubsystem(&m_svDLLSoftwareTriggers))
		{
			hrOk = hrOk | SV_HARDWARE_FAILURE_SOFTWARETRIGGER;
		}
	}
	return hrOk;
}

HRESULT InitialInformationHandler::LoadCameraTriggerDLL()
{
	HRESULT hrOk = S_OK;

	if (!m_InitialInfo.m_CameraTriggerDLL.empty())
	{
		HRESULT ResultLoadDLL(m_svDLLCameraTriggers.Open(m_InitialInfo.m_CameraTriggerDLL.c_str()));
		if (S_OK != ResultLoadDLL)
		{
			hrOk = hrOk | SV_HARDWARE_FAILURE_CAMERATRIGGER;
		}

		if (S_OK == ResultLoadDLL)
		{
			_variant_t value;
			value.SetString(SvTrig::CameraTriggerName);
			m_svDLLCameraTriggers.SetParameterValue(1, SVIOParameterEnum::SVBoardName, value);
			if (S_OK != SvTrig::SVTriggerProcessingClass::Instance().UpdateTriggerSubsystem(&m_svDLLCameraTriggers))
			{
				hrOk = hrOk | SV_HARDWARE_FAILURE_CAMERATRIGGER;
			}
		}
	}
	return hrOk;
}

HRESULT InitialInformationHandler::LoadAcquisitionDLL()
{
	HRESULT hrOk = S_OK;

	if (!m_InitialInfo.m_DigitizerDLL.empty())
	{
		HRESULT ResultLoadDLL(m_dllDigitizers.Open(m_InitialInfo.m_DigitizerDLL.c_str()));
		if (S_OK != ResultLoadDLL)
		{
			//This is the error result which indicates that the Matrox Gige service is not running
			if (ErrorMatroxServiceNotRunning == ResultLoadDLL)
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Exception.setMessage(SVMSG_SVO_90_MATROX_SERVICE_NOT_RUNNING, m_InitialInfo.m_DigitizerDLL.c_str(), SvStl::SourceFileParams(StdMessageParams));
			}
			else
			{
				hrOk = hrOk | SV_HARDWARE_FAILURE_ACQUISITION;
			}
		}
		else
		{
			if (S_OK != SvIe::SVDigitizerProcessingClass::Instance().UpdateDigitizerSubsystem(&m_dllDigitizers))
			{
				hrOk = hrOk | SV_HARDWARE_FAILURE_ACQUISITION;
			}
		}
	}
	else
	{
		if (m_InitialInfo.m_FrameGrabber != _T("00"))
		{
			hrOk = hrOk | SV_HARDWARE_FAILURE_ACQUISITION;
		}
	}

	return hrOk;
}


HRESULT InitialInformationHandler::CloseAcquisitionDLL()
{
	SvIe::SVDigitizerProcessingClass::Instance().clear();

	m_dllDigitizers.Close();

	m_dllFileAcquisition.Close();

	return S_OK;
}

HRESULT InitialInformationHandler::LoadFileAcquisitionDLL()
{
	HRESULT hrOk = S_OK;

	if (!m_InitialInfo.m_FileAcquisitionDLL.empty())
	{
		if (S_OK != m_dllFileAcquisition.Open(m_InitialInfo.m_FileAcquisitionDLL.c_str()))
		{
			hrOk = hrOk | SV_HARDWARE_FAILURE_FILEACQUISITION;
		}

		if (S_OK != SvIe::SVDigitizerProcessingClass::Instance().UpdateDigitizerSubsystem(&m_dllFileAcquisition))
		{
			hrOk = hrOk | SV_HARDWARE_FAILURE_FILEACQUISITION;
		}
	}
	return hrOk;
}

HRESULT InitialInformationHandler::INIReset()
{
	HRESULT hrOk = S_OK;

	if (0 == SvUl::CompareNoCase(m_InitialInfo.m_ReloadAcquisitionDLL, _T("Y")))
	{
		CloseAcquisitionDLL();
	}

	if (0 == SvUl::CompareNoCase(m_InitialInfo.m_ReloadTriggerResultDLL, _T("Y")))
	{
		CloseTriggerResultDLL();

		hrOk = hrOk | LoadTriggerResultDLL();
		hrOk = hrOk | LoadSoftwareTriggerDLL();
		hrOk = hrOk | LoadCameraTriggerDLL();
	}

	if (0 == SvUl::CompareNoCase(m_InitialInfo.m_ReloadAcquisitionDLL, _T("Y")))
	{
		hrOk = hrOk | LoadAcquisitionDLL();
		hrOk = hrOk | LoadFileAcquisitionDLL();
	}

	return hrOk;
}

HRESULT InitialInformationHandler::INIClose()
{
	m_InitializationStatusFlags = SV_HARDWARE_FAILURE_ALL;

	m_InitialInfo.m_ProductName.clear();

	m_InitialInfo.ResetModelNumberInformation();

	m_InitialInfo.ResetBoardNames();

	CloseAcquisitionDLL();

	CloseTriggerResultDLL();

	return S_OK;
}
#pragma endregion Public Methods

HRESULT InitialInformationHandler::LoadDigitalPart()
{
	if (SVIOConfigurationInterfaceClass::Instance().OpenDigital(m_triggerDllHandle) != S_OK)
	{
		return SV_HARDWARE_FAILURE_IO;
	}
	else
	{	// Send the System type to the IO Board.
		VARIANT vt;
		vt.vt = VT_I4;

		// Hardware.ini has the new IOBoardOption.
		if (!m_InitialInfo.m_IOBoardOption.empty())
		{
			vt.lVal = atol(m_InitialInfo.m_IOBoardOption.c_str());
			SVIOConfigurationInterfaceClass::Instance().SetParameterValue(SVBoardType, &vt);
		}
		else
		{	// Legacy behavior.... Hardware.Ini file does not have new entry...
			vt.lVal = atol(m_InitialInfo.m_IOBoard.c_str());
			SVIOConfigurationInterfaceClass::Instance().SetParameterValue(SVBoardType, &vt);
		}
	}
	return S_OK;
}

HRESULT InitialInformationHandler::LoadTriggerPart()
{
	HRESULT hrOk {S_OK};
	if (S_OK != m_svDLLTriggers.Open(m_triggerDllHandle))
	{
		hrOk = hrOk | SV_HARDWARE_FAILURE_TRIGGER;
	}

	if (S_OK != SvTrig::SVTriggerProcessingClass::Instance().UpdateTriggerSubsystem(&m_svDLLTriggers))
	{
		hrOk = hrOk | SV_HARDWARE_FAILURE_TRIGGER;
	}

	for (int i = 0; i < 4; i++)
	{
		unsigned long triggerHandle = m_svDLLTriggers.GetHandle(i);

		if (0 < triggerHandle)
		{
			bool rising;

			SVIOConfigurationInterfaceClass::Instance().GetIOTriggerValue(i, rising);

			_variant_t value = rising ? 1L : -1L;

			// SVSignalEdge enum is used here to make the code more clear.
			// however at some time in the future the Dll parameters may be implemented
			// as an array and therefore this enum may not apply.
			m_svDLLTriggers.SetParameterValue(triggerHandle, SVSignalEdge, value);
		}
	}
	return hrOk;
}

