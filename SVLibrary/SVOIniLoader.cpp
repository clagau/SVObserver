// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOIniLoader.cpp
// * .File Name       : $Workfile:   SVOIniLoader.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   10 Jun 2013 18:08:04  $
// ******************************************************************************
#pragma region Includes
#include"stdafx.h"
#include "InitialInformation.h"
#include "SVOIniLoader.h"
#include "SVOIniClass.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations
constexpr char* EmptyString = _T("");
constexpr char* NTag = _T("N");
constexpr char* YTag = _T("Y");
constexpr char* RTag = _T("R");

constexpr char* OEMSpecificSectionTag = _T("OEMSpecific");
constexpr char* ProductIDTag = _T("ProductID");
constexpr char* ModelNoTag = _T("ModelNo");
constexpr char* EmptyModelNo = _T("00 00 00 00");
constexpr char* SerialNoTag = _T("SerialNo");
constexpr char* SingleCameraTag = _T("SingleCamera");

constexpr char* SVIMInfoSectionTag = _T("SVIM Information");
constexpr char* SettingsTag = _T("Settings");
constexpr char* LegacyEquationParsingTag = _T("LegacyEquationParsing");
constexpr char* TriggerEdgeCameraTag = _T("Trigger Edge Camera_%d");
constexpr char* StrobeEdgeCameraTag = _T("Strobe Edge Camera_%d");
constexpr char* UseStrobeasStartFrameCameraTag = _T("Use Strobe as Start Frame Camera_%d");
constexpr char* ShowUpdateFirmwareTag = _T("ShowUpdateFirmware");
constexpr char* DisplaySectionTag = _T("Display");
constexpr char* ForcedImageUpdateTimeInSecondsTag = _T("ForcedImageUpdateTimeInSeconds");
constexpr char* PreTriggerTimeWindowTag = _T("PreTriggerTimeWindow");

constexpr char* NAKSectionTag = _T("NAK_SETTINGS");
constexpr char*  NAKMode = _T("NAKMode");
constexpr char*  NAKParameter = _T("NAKPar");

constexpr char* ProcessorSectionTag = _T("Processor");
constexpr char* RAIDTag = _T("RAID");
constexpr char* FrameGrabberSectionTag = _T("Frame Grabber");
constexpr char* DigitizerDLLTag = _T("DigitizerDLL");
constexpr char* TriggerDLLTag = _T("TriggerDLL");

constexpr char* SVIM_X2_GD2A = _T("SVIM X2-GD2A");
constexpr char* SVIM_X2_GD8A = _T("SVIM X2-GD8A");
constexpr char* SVIM_X2_GD1A = _T("SVIM X2-GD1A");

constexpr char* PacketSizeTag = _T("PacketSize");

constexpr char* IOBoardSectionTag = _T("IO Board");
constexpr char* DigitalIODLLTag = _T("DigitalIODLL");
constexpr char* ReloadDLLTag = _T("ReloadDLL");

constexpr char* FrameGrabberTag = _T("FrameGrabber");
constexpr char* ProductNameTag = _T("ProductName");
constexpr char* IOBoardModeTag = _T("IOBoardMode");
constexpr char* IOBoardModeDefault = _T("10");
constexpr char* Opto22InputInvertTag = _T("Opto22InputInvert");
constexpr char* Opto22OutputInvertTag = _T("Opto22OutputInvert");
constexpr char* OptionsSectionTag = _T("Options");
constexpr char* FileAcquisitionDeviceFilename = _T("SVFileAcquisitionDevice.DLL");
constexpr char* SoftwareTriggerDeviceFilename = _T("SVSoftwareTriggerDevice.DLL");
constexpr char* CameraTriggerDeviceFilename = _T("SVEmptyIO.DLL");
#pragma endregion Declarations

namespace SvLib
{
void SVOIniLoader::LoadIniFiles(LPCTSTR svimIniFile, LPCTSTR oemIniFile, LPCTSTR hardwareIniFile)
{
	LoadOEMIni(oemIniFile);

	if (isModelNumberDecodable())
	{
		LoadSVIMIni(svimIniFile);
		LoadHardwareIni(hardwareIniFile);
	}
	//@TODO [Arvid][7.50][6.6.2017] hier sollte man besser eine Liste der Dateien zurückgeben, die nicht geladen werden konnten. Das würde die Erzeugung vernünftiger Fehlerbeschreibungen "weiter oben" erleichtern.
}

void SVOIniLoader::LoadOEMIni(LPCTSTR oemIniFile)
{
	SVOINIClass OemIni(oemIniFile);

	std::string  WinKey = OemIni.GetValueString(OEMSpecificSectionTag, ProductIDTag, EmptyString);
	std::string ModelNumber = SvUl::Trim(OemIni.GetValueString(OEMSpecificSectionTag, ModelNoTag, EmptyModelNo).c_str());
	ModelNumber = SvUl::MakeUpper(ModelNumber.c_str());

	std::string serialNumber;
	if (!readSerialNumberFromSystem(serialNumber))
	{
		serialNumber = OemIni.GetValueString(OEMSpecificSectionTag, SerialNoTag, EmptyString);
	}
	else
	{
		OemIni.SetValueString(OEMSpecificSectionTag, SerialNoTag, serialNumber.c_str());
	}

	DecodeModelNumber(ModelNumber.c_str());

	if (m_modelNumberIsDecodable)
	{
		m_WinKey = WinKey;
		m_ModelNumber = ModelNumber;
		m_SerialNumber = serialNumber;

		std::string SingleCamera = SvUl::Trim(OemIni.GetValueString(OEMSpecificSectionTag, SingleCameraTag, NTag).c_str());
		if (SingleCamera == YTag)
		{
			m_bSingleCameraModel = true;
		}
	}
	else
	{
		m_WinKey.clear();
		m_ModelNumber.clear();
		m_SerialNumber.clear();
	}
}

void  SVOIniLoader::LoadSVIMIni(LPCTSTR svimIniFile)
{
	SVOINIClass SvimIni(svimIniFile); //@TODO [Arvid][10.00][20.8.2020] hier sollte geprüft werden, ob diese Datei überhaupt existiert! 
	//aktuell: wenn nicht, erscheint keine Fehlermeldung!

	int LegacyEquation = SvimIni.GetValueInt(SVIMInfoSectionTag, LegacyEquationParsingTag, 0);
	m_bUseCorrectListRecursion = (0 == LegacyEquation);

	for (int i = 0; i < MaxTriggers; i++)
	{
		std::string TagName;

		// Get the Trigger Edge type
		TagName = SvUl::Format(TriggerEdgeCameraTag, i);
		m_TriggerEdge[i] = SvimIni.GetValueString(SVIMInfoSectionTag, TagName.c_str(), RTag);
		SvimIni.SetValueString(SVIMInfoSectionTag, TagName.c_str(), m_TriggerEdge[i].c_str());

		// Get the Strobe Edge type
		TagName = SvUl::Format(StrobeEdgeCameraTag, i);
		m_StrobeEdge[i] = SvimIni.GetValueString(SVIMInfoSectionTag, TagName.c_str(), RTag);
		SvimIni.SetValueString(SVIMInfoSectionTag, TagName.c_str(), m_StrobeEdge[i].c_str());

		// Use Strobe as Start Frame=Y/N
		TagName = SvUl::Format(UseStrobeasStartFrameCameraTag, i);
		m_StartFrameType[i] = SvimIni.GetValueString(SVIMInfoSectionTag, TagName.c_str(), NTag);
		SvimIni.SetValueString(SVIMInfoSectionTag, TagName.c_str(), m_StartFrameType[i].c_str());
	}

	// Force Image Update Time (in seconds)
	int forcedImageUpdateTime = SvimIni.GetValueInt(DisplaySectionTag, ForcedImageUpdateTimeInSecondsTag, 0);
	m_rInitialInfo.m_forcedImageUpdateTimeInSeconds = static_cast<unsigned char> (forcedImageUpdateTime);
	m_rInitialInfo.m_NAKMode = static_cast<SvDef::NakGeneration>(SvimIni.GetValueInt(NAKSectionTag, NAKMode, SvDef::NakGeneration::Bursts));
	m_rInitialInfo.m_NAKParameter = SvimIni.GetValueInt(NAKSectionTag, NAKParameter, SvDef::DefaultNakParameter);
	m_rInitialInfo.m_preTriggerTimeWindow = SvimIni.GetValueDouble(SettingsTag, PreTriggerTimeWindowTag, 0.0);
}

HRESULT SVOIniLoader::LoadHardwareIni(LPCTSTR hardwareIniFile)
{
	HRESULT l_hrOk = S_OK;

	SVOINIClass HardwareINI(hardwareIniFile); //@TODO [Arvid][10.00][20.8.2020] hier sollte geprüft werden, ob diese Datei überhaupt existiert! 
	//aktuell: wenn nicht, erscheint keine Fehlermeldung!

	m_rInitialInfo.m_ProductName.clear();
	m_rInitialInfo.m_SoftwareTriggerDLL.clear();
	m_rInitialInfo.m_CameraTriggerDLL.clear();
	m_rInitialInfo.m_FileAcquisitionDLL.clear();

	m_rInitialInfo.ResetBoardNames();

	m_rInitialInfo.m_IOBoardOption = IOBoardModeDefault;

	m_Opto22InputInvert = NTag;
	m_Opto22OutputInvert = NTag;

	m_rInitialInfo.m_ProcessorBoardName = HardwareINI.GetValueString(ProcessorSectionTag, m_rInitialInfo.m_Processor.c_str(), EmptyString);
	if (!m_rInitialInfo.m_ProcessorBoardName.empty())
	{
		m_rInitialInfo.m_RAIDBoardName = HardwareINI.GetValueString(m_rInitialInfo.m_ProcessorBoardName.c_str(), RAIDTag, EmptyString);

		m_rInitialInfo.m_ProductName = HardwareINI.GetValueString(m_rInitialInfo.m_ProcessorBoardName.c_str(), ProductNameTag, EmptyString);
	}

	m_rInitialInfo.m_AcquisitionBoardName = HardwareINI.GetValueString(FrameGrabberSectionTag, m_rInitialInfo.m_FrameGrabber.c_str(), EmptyString);
	if (!m_rInitialInfo.m_AcquisitionBoardName.empty())
	{
		m_rInitialInfo.m_DigitizerDLL = HardwareINI.GetValueString(m_rInitialInfo.m_AcquisitionBoardName.c_str(), DigitizerDLLTag, EmptyString);

		m_rInitialInfo.m_TriggerDLL = HardwareINI.GetValueString(m_rInitialInfo.m_AcquisitionBoardName.c_str(), TriggerDLLTag, EmptyString);

		if (m_rInitialInfo.m_ProductName.empty())
		{
			m_rInitialInfo.m_ProductName = SvUl::Trim(HardwareINI.GetValueString(m_rInitialInfo.m_AcquisitionBoardName.c_str(), ProductNameTag, EmptyString).c_str());
			if (SVIM_X2_GD2A == m_rInitialInfo.m_ProductName || SVIM_X2_GD8A == m_rInitialInfo.m_ProductName)
			{
				if (m_bSingleCameraModel)
				{
					m_rInitialInfo.m_ProductName = SVIM_X2_GD1A;
				}
			}
		}

		m_rInitialInfo.m_ReloadAcquisitionDLL = HardwareINI.GetValueString(m_rInitialInfo.m_AcquisitionBoardName.c_str(), ReloadDLLTag, YTag);

		if (!m_rInitialInfo.m_TriggerDLL.empty())
		{
			m_rInitialInfo.m_Trigger = m_rInitialInfo.m_FrameGrabber;
			m_rInitialInfo.m_TriggerBoardName = m_rInitialInfo.m_AcquisitionBoardName;
			m_rInitialInfo.m_ReloadTriggerDLL = m_rInitialInfo.m_ReloadAcquisitionDLL;
		}

		// Matrox Gige - Get Packet Size
		int packetSize = 0;
		packetSize = HardwareINI.GetValueInt(m_rInitialInfo.m_AcquisitionBoardName.c_str(), PacketSizeTag, 0);
		m_rInitialInfo.m_gigePacketSize = static_cast<long> (packetSize);
	}

	m_rInitialInfo.m_DigitalBoardName = HardwareINI.GetValueString(IOBoardSectionTag, m_rInitialInfo.m_IOBoard.c_str(), EmptyString);
	if (!m_rInitialInfo.m_DigitalBoardName.empty())
	{
		m_rInitialInfo.m_DigitalIODLL = HardwareINI.GetValueString(m_rInitialInfo.m_DigitalBoardName.c_str(), DigitalIODLLTag, EmptyString);
		m_rInitialInfo.m_ReloadDigitalDLL = HardwareINI.GetValueString(m_rInitialInfo.m_DigitalBoardName.c_str(), ReloadDLLTag, YTag);

		if (m_rInitialInfo.m_TriggerDLL.empty())
		{
			m_rInitialInfo.m_TriggerDLL = HardwareINI.GetValueString(m_rInitialInfo.m_DigitalBoardName.c_str(), TriggerDLLTag, EmptyString);

			if (!m_rInitialInfo.m_TriggerDLL.empty())
			{
				m_rInitialInfo.m_TriggerBoardName = m_rInitialInfo.m_DigitalBoardName;

				m_rInitialInfo.m_ReloadTriggerDLL = m_rInitialInfo.m_ReloadAcquisitionDLL;
			}
		}

		std::string Value;
		Value = HardwareINI.GetValueString(m_rInitialInfo.m_DigitalBoardName.c_str(), IOBoardModeTag, IOBoardModeDefault);
		if (0 < Value.size())
		{
			m_rInitialInfo.m_IOBoardOption = Value;
		}

		Value = HardwareINI.GetValueString(m_rInitialInfo.m_DigitalBoardName.c_str(), Opto22InputInvertTag, NTag);
		if (0 < Value.size())
		{
			m_Opto22InputInvert = Value;
		}

		Value = HardwareINI.GetValueString(m_rInitialInfo.m_DigitalBoardName.c_str(), Opto22OutputInvertTag, NTag);
		if (0 < Value.size())
		{
			m_Opto22OutputInvert = Value;
		}

		// Allow the I/O Section to be shared with multiple frame grabber entries
		// This requires the Frame Grabber Entry to be specified prior to the I/O Entry in the INI file.
		// The Key is the fixed Tag "FrameGrabber" appended with the hexadecimal identifier for the frame grabber.
		// e.g. FrameGrabber2A
		// The value is the product name (could be anything really as only the presence of the entry is required).
		// If an Entry is found then the product name is not overridden.
		std::string frameGrabberEntry(FrameGrabberTag + m_rInitialInfo.m_FrameGrabber);
		Value = HardwareINI.GetValueString(m_rInitialInfo.m_DigitalBoardName.c_str(), frameGrabberEntry.c_str(), EmptyString);
		// if the entry is not found, apply the product name Rule
		if (0 == Value.size())
		{
			// Allow the product name to be overwritten by the IO section product name.
			// This is an exception to the rule "The first one wins".
			// This exception was made for the X Series where the IO board model
			// can dictate the product name.
			Value = HardwareINI.GetValueString(m_rInitialInfo.m_DigitalBoardName.c_str(), ProductNameTag, EmptyString);
			if (0 < Value.size())
			{
				m_rInitialInfo.m_ProductName = Value;
			}
		}
	}

	m_rInitialInfo.m_HardwareOptions = HardwareINI.GetValueString(OptionsSectionTag, m_rInitialInfo.m_Options.c_str(), EmptyString);

	bool l_UseFileAcq = !(m_rInitialInfo.m_DigitizerDLL.empty());
	bool l_UseSoftTrig = !(m_rInitialInfo.m_TriggerDLL.empty());

#ifdef _DEBUG
	l_UseFileAcq = true;
	l_UseSoftTrig = true;
#endif

	if (l_UseFileAcq)
	{
		m_rInitialInfo.m_FileAcquisitionDLL = FileAcquisitionDeviceFilename;
	}

	if (l_UseSoftTrig)
	{
		m_rInitialInfo.m_SoftwareTriggerDLL = SoftwareTriggerDeviceFilename;
	}

	m_rInitialInfo.m_CameraTriggerDLL = CameraTriggerDeviceFilename;

	return l_hrOk;
}

void SVOIniLoader::DecodeModelNumber(LPCTSTR modelNumber)
{
	std::string CheckModelNumber = SvUl::Trim(modelNumber);
	CheckModelNumber = SvUl::MakeUpper(CheckModelNumber.c_str());

	if (11 != CheckModelNumber.size() || EmptyModelNo == CheckModelNumber)
	{
		m_modelNumberIsDecodable = false;
		return;
	}

	m_modelNumberIsDecodable = true;

	m_rInitialInfo.ResetModelNumberInformation();

	m_rInitialInfo.m_Trigger.clear();

	int l_iCount = 0;

	for (int i = 0; i < CheckModelNumber.size(); i++)
	{
		if (::isalnum(CheckModelNumber[i]))
		{
			switch (++l_iCount)
			{
				case 1:
				case 2:
				{
					m_rInitialInfo.m_Processor += CheckModelNumber[i];

					break;
				}
				case 3:
				case 4:
				{
					m_rInitialInfo.m_FrameGrabber += CheckModelNumber[i];

					break;
				}
				case 5:
				case 6:
				{
					m_rInitialInfo.m_IOBoard += CheckModelNumber[i];

					break;
				}
				case 7:
				case 8:
				{
					m_rInitialInfo.m_Options += CheckModelNumber[i];

					break;
				}
				default:
				{
					break;
				}
			}
		}
	}

	m_rInitialInfo.m_Trigger = m_rInitialInfo.m_IOBoard;
}


bool readSerialNumberFromSystem(std::string& serialNumber)
{
	const int cCharSize = 100;
	std::string snFileName = SvStl::GlobalPath::Inst().GetTempPath("sn.txt");
	std::string commandString = "wmic bios get serialnumber > " + snFileName;
	system(commandString.c_str());
	wchar_t sn[cCharSize];
	FILE* fp = fopen(snFileName.c_str(), "r, ccs=UTF-8");
	if (nullptr != fp)
	{
		fgetws(sn, cCharSize, fp); //dummy read of first line
		fgetws(sn, cCharSize, fp); //now sn contains 2nd line
	}

	fclose(fp);          //cleanup temp file
	remove(snFileName.c_str());
	serialNumber = SvUl::Trim(SvUl::Format("%ws", sn).c_str());
	return (8 == serialNumber.size() || 9 == serialNumber.size());
}
} //namespace SvLib
