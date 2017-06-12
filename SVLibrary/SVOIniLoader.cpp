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
#pragma endregion Includes

#pragma region Declarations
static const TCHAR* const EmptyString = _T("");
static const TCHAR* const NTag = _T("N");
static const TCHAR* const YTag = _T("Y");
static const TCHAR* const RTag = _T("R");

static const TCHAR* const OEMSpecificSectionTag = _T("OEMSpecific");
static const TCHAR* const ProductIDTag = _T("ProductID");
static const TCHAR* const ModelNoTag = _T("ModelNo");
static const TCHAR* const EmptyModelNo = _T("00 00 00 00");
static const TCHAR* const SerialNoTag = _T("SerialNo");
static const TCHAR* const SingleCameraTag = _T("SingleCamera");

static const TCHAR* const SVIMInfoSectionTag = _T("SVIM Information");
static const TCHAR* const LegacyEquationParsingTag = _T("LegacyEquationParsing");
static const TCHAR* const TriggerEdgeCameraTag = _T("Trigger Edge Camera_%d");
static const TCHAR* const StrobeEdgeCameraTag = _T("Strobe Edge Camera_%d");
static const TCHAR* const UseStrobeasStartFrameCameraTag = _T("Use Strobe as Start Frame Camera_%d");
static const TCHAR* const ShowUpdateFirmwareTag = _T("ShowUpdateFirmware");
static const TCHAR* const DisplaySectionTag = _T("Display");
static const TCHAR* const ForcedImageUpdateTimeInSecondsTag = _T("ForcedImageUpdateTimeInSeconds");

static const TCHAR* const NAKSectionTag = _T("NAK_SETTINGS");
static const TCHAR* const  NAKMode    = _T("NAKMode");
static const TCHAR* const  NAKParameter    = _T("NAKPar");

static const TCHAR* const ProcessorSectionTag = _T("Processor");
static const TCHAR* const RAIDTag = _T("RAID");
static const TCHAR* const FrameGrabberSectionTag = _T("Frame Grabber");
static const TCHAR* const DigitizerDLLTag = _T("DigitizerDLL");
static const TCHAR* const TriggerDLLTag = _T("TriggerDLL");

static const TCHAR* const SVIM_X2_GD2A = _T("SVIM X2-GD2A");
static const TCHAR* const SVIM_X2_GD8A = _T("SVIM X2-GD8A");
static const TCHAR* const SVIM_X2_GD1A = _T("SVIM X2-GD1A");

static const TCHAR* const PacketSizeTag = _T("PacketSize");

static const TCHAR* const IOBoardSectionTag = _T("IO Board");
static const TCHAR* const DigitalIODLLTag = _T("DigitalIODLL");
static const TCHAR* const ReloadDLLTag = _T("ReloadDLL");

static const TCHAR* const FrameGrabberTag = _T("FrameGrabber");
static const TCHAR* const ProductNameTag = _T("ProductName");
static const TCHAR* const IOBoardModeTag = _T("IOBoardMode");
static const TCHAR* const IOBoardModeDefault = _T("10");
static const TCHAR* const Opto22InputInvertTag = _T("Opto22InputInvert");
static const TCHAR* const Opto22OutputInvertTag = _T("Opto22OutputInvert");
static const TCHAR* const OptionsSectionTag = _T("Options");
static const TCHAR* const FileAcquisitionDeviceFilename = _T("SVFileAcquisitionDevice.DLL");
static const TCHAR* const SoftwareTriggerDeviceFilename = _T("SVSoftwareTriggerDevice.DLL");
#pragma endregion Declarations



namespace SvLib
{
	SVOIniLoader::SVOIniLoader(InitialInformation &inifileInfo)
	: m_rInitialInfo(inifileInfo)
	, m_bUseCorrectListRecursion( false )
	, m_hrDecodeModelNumber( S_OK )
	, m_bSingleCameraModel( false )
	{
	}

	HRESULT SVOIniLoader::LoadIniFiles(LPCTSTR svimIniFile, LPCTSTR oemIniFile, LPCTSTR hardwareIniFile)
	{ 
		HRESULT l_hrOk = LoadOEMIni(oemIniFile);
	
		if (S_OK == l_hrOk)
		{
			l_hrOk = LoadSVIMIni(svimIniFile);
		}
		if (S_OK == l_hrOk)
		{
			l_hrOk = LoadHardwareIni(hardwareIniFile);
		}
		return l_hrOk;  //@TODO [Arvid][7.50][6.6.2017] hier sollte man besser eine Liste der Dateien zurückgeben, die nicht geladen werden konnten. Das würde die Erzeugung vernünftiger Fehlerbeschreibungen "weiter oben" erleichtern.
	}

	HRESULT SVOIniLoader::LoadOEMIni(LPCTSTR oemIniFile)
	{
		SVOINIClass OemIni( oemIniFile );
	
		SVString  WinKey = OemIni.GetValueString( OEMSpecificSectionTag, ProductIDTag, EmptyString );
		SVString ModelNumber = OemIni.GetValueString ( OEMSpecificSectionTag, ModelNoTag, EmptyModelNo );
		SvUl_SF::TrimLeft( ModelNumber );
		SvUl_SF::TrimRight( ModelNumber );
		SvUl_SF::MakeUpper( ModelNumber );
	
		SVString SerialNumber = OemIni.GetValueString( OEMSpecificSectionTag, SerialNoTag, EmptyString );

		m_hrDecodeModelNumber = DecodeModelNumber(ModelNumber.c_str());

		if ( S_OK == m_hrDecodeModelNumber )
		{
			m_WinKey = WinKey;
			m_ModelNumber = ModelNumber;
			m_SerialNumber = SerialNumber;

			SVString SingleCamera = OemIni.GetValueString( OEMSpecificSectionTag, SingleCameraTag, NTag );
			SvUl_SF::Trim( SingleCamera );
			if ( SingleCamera == YTag )
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
		
		return m_hrDecodeModelNumber;
	}

	HRESULT SVOIniLoader::LoadSVIMIni(LPCTSTR svimIniFile)
	{
		HRESULT Result = S_OK;
		SVOINIClass SvimIni( svimIniFile );
		BSTR l_bstrValue = nullptr;
	
		int LegacyEquation = SvimIni.GetValueInt( SVIMInfoSectionTag, LegacyEquationParsingTag, 0 );
		m_bUseCorrectListRecursion = (0 == LegacyEquation);

		for( int i = 0; i < MaxTriggers; i++ )
		{
			SVString TagName;

			// Get the Trigger Edge type
			TagName = SvUl_SF::Format( TriggerEdgeCameraTag, i );
			m_TriggerEdge[i] = SvimIni.GetValueString( SVIMInfoSectionTag, TagName.c_str(), RTag );
			SvimIni.SetValueString( SVIMInfoSectionTag, TagName.c_str(), m_TriggerEdge[i].c_str() );

			// Get the Strobe Edge type
			TagName = SvUl_SF::Format( StrobeEdgeCameraTag, i );
			m_StrobeEdge[i] = SvimIni.GetValueString( SVIMInfoSectionTag, TagName.c_str(), RTag );
			SvimIni.SetValueString( SVIMInfoSectionTag, TagName.c_str(), m_StrobeEdge[i].c_str() );
		
			// Use Strobe as Start Frame=Y/N
			TagName = SvUl_SF::Format( UseStrobeasStartFrameCameraTag, i );
			m_StartFrameType[i] =SvimIni.GetValueString( SVIMInfoSectionTag, TagName.c_str(), NTag );
			SvimIni.SetValueString( SVIMInfoSectionTag, TagName.c_str(), m_StartFrameType[i].c_str() );
		}

		// Force Image Update Time (in seconds)
		int forcedImageUpdateTime = SvimIni.GetValueInt( DisplaySectionTag, ForcedImageUpdateTimeInSecondsTag, 0 );
		m_rInitialInfo.m_forcedImageUpdateTimeInSeconds = static_cast<unsigned char> (forcedImageUpdateTime);
		m_rInitialInfo.m_NAKMode = static_cast<SvOi::NakGeneration>( SvimIni.GetValueInt(NAKSectionTag, NAKMode, SvOi::NakGeneration::Bursts));
		m_rInitialInfo.m_NAKParameter = SvimIni.GetValueInt(NAKSectionTag, NAKParameter, SvOi::DefaultNakParameter);
		return Result;
	}

	HRESULT SVOIniLoader::LoadHardwareIni(LPCTSTR hardwareIniFile)
	{
		HRESULT l_hrOk = S_OK;

		SVOINIClass HardwareINI( hardwareIniFile );

		m_rInitialInfo.m_ProductName.clear();
		m_rInitialInfo.m_SoftwareTriggerDLL.clear();
		m_rInitialInfo.m_AcquisitionTriggerDLL.clear();
		m_rInitialInfo.m_FileAcquisitionDLL.clear();

		m_rInitialInfo.ResetBoardNames();
	
		m_IOBoardOption = IOBoardModeDefault;

		m_Opto22InputInvert = NTag;
		m_Opto22OutputInvert = NTag;

		m_rInitialInfo.m_ProcessorBoardName = HardwareINI.GetValueString( ProcessorSectionTag, m_rInitialInfo.m_Processor.c_str(), EmptyString );
		if( !m_rInitialInfo.m_ProcessorBoardName.empty() )
		{
			m_rInitialInfo.m_RAIDBoardName = HardwareINI.GetValueString(m_rInitialInfo.m_ProcessorBoardName.c_str(), RAIDTag, EmptyString );

			m_rInitialInfo.m_ProductName = HardwareINI.GetValueString(m_rInitialInfo.m_ProcessorBoardName.c_str(), ProductNameTag, EmptyString );
		}

		m_rInitialInfo.m_AcquisitionBoardName = HardwareINI.GetValueString( FrameGrabberSectionTag, m_rInitialInfo.m_FrameGrabber.c_str(), EmptyString );
		if( !m_rInitialInfo.m_AcquisitionBoardName.empty() )
		{
			m_rInitialInfo.m_DigitizerDLL = HardwareINI.GetValueString(m_rInitialInfo.m_AcquisitionBoardName.c_str(), DigitizerDLLTag, EmptyString );

			m_rInitialInfo.m_TriggerDLL = HardwareINI.GetValueString(m_rInitialInfo.m_AcquisitionBoardName.c_str(), TriggerDLLTag, EmptyString );

			if (m_rInitialInfo.m_ProductName.empty() )
			{
				m_rInitialInfo.m_ProductName = HardwareINI.GetValueString(m_rInitialInfo.m_AcquisitionBoardName.c_str(), ProductNameTag, EmptyString );
				SvUl_SF::TrimLeft(m_rInitialInfo.m_ProductName );
				SvUl_SF::TrimRight(m_rInitialInfo.m_ProductName );
				if ( SVIM_X2_GD2A == m_rInitialInfo.m_ProductName || SVIM_X2_GD8A == m_rInitialInfo.m_ProductName  )
				{
					if ( m_bSingleCameraModel )
					{
						m_rInitialInfo.m_ProductName = SVIM_X2_GD1A;
					}
				}
			}

			m_rInitialInfo.m_ReloadAcquisitionDLL = HardwareINI.GetValueString(m_rInitialInfo.m_AcquisitionBoardName.c_str(), ReloadDLLTag, YTag );

			if( !m_rInitialInfo.m_TriggerDLL.empty() )
			{
				m_rInitialInfo.m_Trigger = m_rInitialInfo.m_FrameGrabber;
				m_rInitialInfo.m_TriggerBoardName = m_rInitialInfo.m_AcquisitionBoardName;
				m_rInitialInfo.m_ReloadTriggerDLL = m_rInitialInfo.m_ReloadAcquisitionDLL;
			}
		
			// Matrox Gige - Get Packet Size
			int packetSize = 0;
			packetSize = HardwareINI.GetValueInt(m_rInitialInfo.m_AcquisitionBoardName.c_str(), PacketSizeTag, 0 );
			m_rInitialInfo.m_gigePacketSize = static_cast<long> (packetSize);
		}

		m_rInitialInfo.m_DigitalBoardName = HardwareINI.GetValueString( IOBoardSectionTag, m_rInitialInfo.m_IOBoard.c_str(), EmptyString );
		if( !m_rInitialInfo.m_DigitalBoardName.empty() )
		{
			m_rInitialInfo.m_DigitalDLL = HardwareINI.GetValueString(m_rInitialInfo.m_DigitalBoardName.c_str(), DigitalIODLLTag, EmptyString );
			m_rInitialInfo.m_ReloadDigitalDLL = HardwareINI.GetValueString(m_rInitialInfo.m_DigitalBoardName.c_str(), ReloadDLLTag, YTag );

			if (m_rInitialInfo.m_TriggerDLL.empty() )
			{
				m_rInitialInfo.m_TriggerDLL = HardwareINI.GetValueString(m_rInitialInfo.m_DigitalBoardName.c_str(), TriggerDLLTag, EmptyString );

				if( !m_rInitialInfo.m_TriggerDLL.empty() )
				{
					m_rInitialInfo.m_TriggerBoardName = m_rInitialInfo.m_DigitalBoardName;

					m_rInitialInfo.m_ReloadTriggerDLL = m_rInitialInfo.m_ReloadAcquisitionDLL;
				}
			}

			SVString Value;
			Value = HardwareINI.GetValueString(m_rInitialInfo.m_DigitalBoardName.c_str(), IOBoardModeTag, IOBoardModeDefault );
			if( 0 < Value.size() )
			{
				m_IOBoardOption = Value;
			}

			Value = HardwareINI.GetValueString(m_rInitialInfo.m_DigitalBoardName.c_str(), Opto22InputInvertTag, NTag );
			if( 0 < Value.size() )
			{
				m_Opto22InputInvert = Value;
			}

			Value = HardwareINI.GetValueString(m_rInitialInfo.m_DigitalBoardName.c_str(), Opto22OutputInvertTag, NTag );
			if( 0 < Value.size() )
			{
				m_Opto22OutputInvert = Value;
			}

			// Allow the I/O Section to be shared with multiple frame grabber entries
			// This requires the Frame Grabber Entry to be specified prior to the I/O Entry in the INI file.
			// The Key is the fixed Tag "FrameGrabber" appended with the hexadecimal identifier for the frame grabber.
			// e.g. FrameGrabber2A
			// The value is the product name (could be anything really as only the presence of the entry is required).
			// If an Entry is found then the product name is not overridden.
			SVString frameGrabberEntry (FrameGrabberTag + m_rInitialInfo.m_FrameGrabber);
			Value = HardwareINI.GetValueString(m_rInitialInfo.m_DigitalBoardName.c_str(), frameGrabberEntry.c_str(), EmptyString );
			// if the entry is not found, apply the product name Rule
			if( 0 == Value.size() )
			{
				// Allow the product name to be overwritten by the IO section product name.
				// This is an exception to the rule "The first one wins".
				// This exception was made for the X Series where the IO board model
				// can dictate the product name.
				Value = HardwareINI.GetValueString(m_rInitialInfo.m_DigitalBoardName.c_str(), ProductNameTag, EmptyString );
				if( 0 < Value.size() )
				{
					m_rInitialInfo.m_ProductName = Value;
				}
			}
		}

		m_rInitialInfo.m_HardwareOptions = HardwareINI.GetValueString( OptionsSectionTag, m_rInitialInfo.m_Options.c_str(), EmptyString );

		bool l_UseFileAcq = !(m_rInitialInfo.m_DigitizerDLL.empty() );
		bool l_UseSoftTrig = !(m_rInitialInfo.m_TriggerDLL.empty() );

		#ifdef _DEBUG
			l_UseFileAcq = true;
			l_UseSoftTrig = true;
		#endif

		if( l_UseFileAcq )
		{
			m_rInitialInfo.m_FileAcquisitionDLL = FileAcquisitionDeviceFilename;
		}

		if( l_UseSoftTrig )
		{
			m_rInitialInfo.m_SoftwareTriggerDLL = SoftwareTriggerDeviceFilename;
		}

		m_rInitialInfo.m_AcquisitionTriggerDLL = m_rInitialInfo.m_DigitizerDLL; // Digitizer must be set before triggers..

		return l_hrOk;
	}

	HRESULT SVOIniLoader::DecodeModelNumber(LPCTSTR modelNumber)
	{
		SVString CheckModelNumber( modelNumber );
		SvUl_SF::TrimLeft( CheckModelNumber );
		SvUl_SF::TrimRight( CheckModelNumber );
		SvUl_SF::MakeUpper( CheckModelNumber );
	
		if (11 != CheckModelNumber.size() || EmptyModelNo == CheckModelNumber)
		{
			return S_FALSE;
		}

		m_rInitialInfo.ResetModelNumberInformation();

		m_rInitialInfo.m_Trigger.clear();

		int l_iCount = 0;

		for ( int i = 0; i < CheckModelNumber.size(); i++ )
		{
			if ( ::isalnum( CheckModelNumber[i] ) )
			{
				switch( ++l_iCount )
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
		
		return S_OK;
	}

} //namespace SvLib