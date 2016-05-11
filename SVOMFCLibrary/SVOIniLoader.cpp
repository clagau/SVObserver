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
#include "SVOIniLoader.h"
#include "SVOIniClass.h"
#pragma endregion Includes

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

static const TCHAR* const UnknownboardTag = _T("Unknown board");
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

SVOIniLoader::SVOIniLoader() 
: m_bUseCorrectListRecursion( false )
, m_hrOEMFailure( S_OK )
, m_gigePacketSize( 0 )
, m_bSingleCameraModel( false )
, m_forcedImageUpdateTimeInSeconds(0) // zero means no forced image update
{
}

HRESULT SVOIniLoader::Load(LPCTSTR svimIniFile, LPCTSTR oemIniFile, LPCTSTR hardwareIniFile)
{
	HRESULT l_hrOk = LoadOEMIni(oemIniFile);
	
	if (S_OK == l_hrOk)
	{
		l_hrOk = LoadSVIMIni(svimIniFile);
	}
	if (S_OK == l_hrOk)
	{
		l_hrOk = LoadHardwareIni(hardwareIniFile, m_csOptions);
	}
	return l_hrOk;
}

HRESULT SVOIniLoader::LoadOEMIni(LPCTSTR oemIniFile)
{
	HRESULT l_hrOk = S_OK;
	SVOINIClass l_svINI;
	
	BSTR l_bstrValue = nullptr;
	CString l_csWinKey;
	
	l_svINI.GetValue( OEMSpecificSectionTag, ProductIDTag, EmptyString, &l_bstrValue, oemIniFile );
	l_csWinKey = l_bstrValue;
	::SysFreeString( l_bstrValue );
	l_bstrValue = nullptr;

	CString l_csModelNumber;
	
	l_svINI.GetValue( OEMSpecificSectionTag, ModelNoTag, EmptyModelNo, &l_bstrValue, oemIniFile );
	l_csModelNumber = l_bstrValue;
	::SysFreeString( l_bstrValue );
	l_bstrValue = nullptr;
	
	l_csModelNumber.TrimLeft();
	l_csModelNumber.TrimRight();
	l_csModelNumber.MakeUpper();
	
	CString l_csSerialNumber;
	
	l_svINI.GetValue( OEMSpecificSectionTag, SerialNoTag, EmptyString, &l_bstrValue, oemIniFile );
	l_csSerialNumber = l_bstrValue;
	::SysFreeString( l_bstrValue );
	l_bstrValue = nullptr;

	l_hrOk = DecodeModelNumber(l_csModelNumber);
	if ( S_OK == l_hrOk )
	{
		m_csWinKey = l_csWinKey;
		m_csModelNumber = l_csModelNumber;
		m_csSerialNumber = l_csSerialNumber;
		l_svINI.GetValue( OEMSpecificSectionTag, SingleCameraTag, NTag, &l_bstrValue, oemIniFile );
		CString l_csSingleCamera = l_bstrValue;
		if ( l_csSingleCamera.Left( 1 ) == YTag )
		{
			m_bSingleCameraModel = true;
		}
	}
	else
	{
		m_csWinKey.Empty();
		m_csModelNumber.Empty();
		m_csSerialNumber.Empty();
	}
		
	m_hrOEMFailure = l_hrOk;
	return l_hrOk;
}

HRESULT SVOIniLoader::LoadSVIMIni(LPCTSTR svimIniFile)
{
	HRESULT l_hrOk = S_OK;
	SVOINIClass l_svINI;
	BSTR l_bstrValue = nullptr;
	
	int l_iLegacyEquation = 0;
	l_svINI.GetValue(SVIMInfoSectionTag, LegacyEquationParsingTag, 0, &l_iLegacyEquation, svimIniFile);
	m_bUseCorrectListRecursion = !l_iLegacyEquation;

	for ( int i = 0; i < 4; i++ )
	{
		CString l_csCameraName;

		// Get the Trigger Edge type
		l_csCameraName.Format( TriggerEdgeCameraTag, i );
		l_svINI.GetValue( SVIMInfoSectionTag, l_csCameraName, RTag, &l_bstrValue, svimIniFile );
		m_csTriggerEdge[i] = l_bstrValue;
		l_svINI.SetValue( SVIMInfoSectionTag, l_csCameraName, m_csTriggerEdge[i], svimIniFile );

		// Get the Strobe Edge type
		l_csCameraName.Format( StrobeEdgeCameraTag, i );
		l_svINI.GetValue( SVIMInfoSectionTag, l_csCameraName, RTag, &l_bstrValue, svimIniFile );
		m_csStrobeEdge[i] = l_bstrValue;
		l_svINI.SetValue( SVIMInfoSectionTag, l_csCameraName, m_csStrobeEdge[i], svimIniFile );
		
		// Use Strobe as Start Frame=Y/N
		l_csCameraName.Format( UseStrobeasStartFrameCameraTag, i );
		l_svINI.GetValue( SVIMInfoSectionTag, l_csCameraName, NTag, &l_bstrValue, svimIniFile );
		m_csStartFrameType[i] = l_bstrValue;
		l_svINI.SetValue( SVIMInfoSectionTag, l_csCameraName, m_csStartFrameType[i], svimIniFile );

	}
	SysFreeString( l_bstrValue );
	l_bstrValue = nullptr;

	// Force Image Update Time (in seconds)
	int forcedImageUpdateTime = 0;
	l_svINI.GetValue(DisplaySectionTag, ForcedImageUpdateTimeInSecondsTag, 0, &forcedImageUpdateTime, svimIniFile);
	m_forcedImageUpdateTimeInSeconds = static_cast<unsigned char>(forcedImageUpdateTime);
	return l_hrOk;
}

HRESULT SVOIniLoader::LoadHardwareIni(LPCTSTR hardwareIniFile, LPCTSTR modelOptions)
{
	HRESULT l_hrOk = S_OK;

	SVOINIClass l_svINI;

	BSTR l_bstrValue = nullptr;

	m_csProductName.Empty();
	m_csSoftwareTriggerDLL.Empty();
	m_csAcquisitionTriggerDLL.Empty();
	m_csFileAcquisitionDLL.Empty();

	m_csProcessorBoardName = UnknownboardTag;
	m_csTriggerBoardName = UnknownboardTag;
	m_csAcquisitionBoardName = UnknownboardTag;
	m_csDigitalBoardName = UnknownboardTag;
	m_csRAIDBoardName = UnknownboardTag;
	
	m_csIOBoardOption = IOBoardModeDefault;

	m_Opto22InputInvert = NTag;
	m_Opto22OutputInvert = NTag;

	l_svINI.GetValue( ProcessorSectionTag, m_csProcessor, EmptyString, &l_bstrValue, hardwareIniFile );

	m_csProcessorBoardName = l_bstrValue;

	if( ! m_csProcessorBoardName.IsEmpty() )
	{
		l_svINI.GetValue( m_csProcessorBoardName, RAIDTag, EmptyString, &l_bstrValue, hardwareIniFile );

		m_csRAIDBoardName = l_bstrValue;

		l_svINI.GetValue( m_csProcessorBoardName, ProductNameTag, EmptyString, &l_bstrValue, hardwareIniFile );

		m_csProductName = l_bstrValue;
	}

	l_svINI.GetValue( FrameGrabberSectionTag, m_csFrameGrabber, EmptyString, &l_bstrValue, hardwareIniFile );

	m_csAcquisitionBoardName = l_bstrValue;

	if( ! m_csAcquisitionBoardName.IsEmpty() )
	{
		l_svINI.GetValue( m_csAcquisitionBoardName, DigitizerDLLTag, EmptyString, &l_bstrValue, hardwareIniFile );

		m_csDigitizerDLL = l_bstrValue;

		l_svINI.GetValue( m_csAcquisitionBoardName, TriggerDLLTag, EmptyString, &l_bstrValue, hardwareIniFile );

		m_csTriggerDLL = l_bstrValue;

		if ( m_csProductName.IsEmpty() )
		{
			l_svINI.GetValue( m_csAcquisitionBoardName, ProductNameTag, EmptyString, &l_bstrValue, hardwareIniFile );

			m_csProductName = l_bstrValue;

			if ( m_csProductName.Left( 12 ) == SVIM_X2_GD2A ||
				m_csProductName.Left( 12 ) == SVIM_X2_GD8A )
			{
				if ( m_bSingleCameraModel )
				{
					m_csProductName = SVIM_X2_GD1A;
				}
			}
		}

		l_svINI.GetValue( m_csAcquisitionBoardName, ReloadDLLTag, YTag, &l_bstrValue, hardwareIniFile );

		m_csReloadAcquisitionDLL = l_bstrValue;

		if( ! m_csTriggerDLL.IsEmpty() )
		{
			m_csTrigger = m_csFrameGrabber;

			m_csTriggerBoardName = m_csAcquisitionBoardName;

			m_csReloadTriggerDLL = m_csReloadAcquisitionDLL;
		}
		
		// Matrox Gige - Get Packet Size
		int packetSize = 0;
		l_svINI.GetValue( m_csAcquisitionBoardName, PacketSizeTag, 0, &packetSize, hardwareIniFile);
		 m_gigePacketSize = static_cast<long>(packetSize);
	}

	l_svINI.GetValue( IOBoardSectionTag, m_csIOBoard, EmptyString, &l_bstrValue, hardwareIniFile );

	m_csDigitalBoardName = l_bstrValue;

	if( ! m_csDigitalBoardName.IsEmpty() )
	{
		l_svINI.GetValue( m_csDigitalBoardName, DigitalIODLLTag, EmptyString, &l_bstrValue, hardwareIniFile );
		
		m_csDigitalDLL = l_bstrValue;

		l_svINI.GetValue( m_csDigitalBoardName, ReloadDLLTag, YTag, &l_bstrValue, hardwareIniFile );

		m_csReloadDigitalDLL = l_bstrValue;

		if ( m_csTriggerDLL.IsEmpty() )
		{
			l_svINI.GetValue( m_csDigitalBoardName, TriggerDLLTag, EmptyString, &l_bstrValue, hardwareIniFile );

			m_csTriggerDLL = l_bstrValue;

			if( ! m_csTriggerDLL.IsEmpty() )
			{
				m_csTriggerBoardName = m_csDigitalBoardName;

				m_csReloadTriggerDLL = m_csReloadAcquisitionDLL;
			}
		}

		l_svINI.GetValue( m_csDigitalBoardName, IOBoardModeTag, IOBoardModeDefault, &l_bstrValue, hardwareIniFile );
		if( ::SysStringLen( l_bstrValue ) > 0 )
		{
			m_csIOBoardOption = l_bstrValue;
		}

		l_svINI.GetValue( m_csDigitalBoardName, Opto22InputInvertTag, NTag, &l_bstrValue, hardwareIniFile );
		if( ::SysStringLen( l_bstrValue ) > 0 )
		{
			m_Opto22InputInvert = l_bstrValue;
		}

		l_svINI.GetValue( m_csDigitalBoardName, Opto22OutputInvertTag, NTag, &l_bstrValue, hardwareIniFile );
		if( ::SysStringLen( l_bstrValue ) > 0 )
		{
			m_Opto22OutputInvert = l_bstrValue;
		}

		// Allow the I/O Section to be shared with multiple frame grabber entries
		// This requires the Frame Grabber Entry to be specified prior to the I/O Entry in the INI file.
		// The Key is the fixed Tag "FrameGrabber" appended with the hexadecimal identifier for the frame grabber.
		// e.g. FrameGrabber2A
		// The value is the product name (could be anything really as only the presence of the entry is required).
		// If an Entry is found then the product name is not overidden.
		CString frameGrabberEntry = FrameGrabberTag;
		frameGrabberEntry += m_csFrameGrabber;
		l_svINI.GetValue( m_csDigitalBoardName, frameGrabberEntry, EmptyString, &l_bstrValue, hardwareIniFile );

		// if the entry is not found, apply the product name Rule
		if( 0 == ::SysStringLen( l_bstrValue )  )
		{
			// Allow the product name to be overwritten by the IO section product name.
			// This is an exception to the rule "The first one wins".
			// This exception was made for the X Series where the IO board model
			// can dictate the product name.
			l_svINI.GetValue( m_csDigitalBoardName, ProductNameTag, EmptyString, &l_bstrValue, hardwareIniFile );

			if( ::SysStringLen( l_bstrValue ) > 0 )
			{
				m_csProductName = l_bstrValue;
			}
		}
	}

	l_svINI.GetValue( OptionsSectionTag, modelOptions, EmptyString, &l_bstrValue, hardwareIniFile );

	m_csHardwareOptions = l_bstrValue;
	::SysFreeString( l_bstrValue );
	l_bstrValue = nullptr;

	bool l_UseFileAcq = !( m_csDigitizerDLL.IsEmpty() );
	bool l_UseSoftTrig = !( m_csTriggerDLL.IsEmpty() );

	#ifdef _DEBUG
		l_UseFileAcq = true;
		l_UseSoftTrig = true;
	#endif

	if( l_UseFileAcq )
	{
		m_csFileAcquisitionDLL = FileAcquisitionDeviceFilename;
	}

	if( l_UseSoftTrig )
	{
		m_csSoftwareTriggerDLL = SoftwareTriggerDeviceFilename;
	}

	m_csAcquisitionTriggerDLL = m_csDigitizerDLL; // Digitizer must be set before triggers..

	return l_hrOk;
}

HRESULT SVOIniLoader::DecodeModelNumber(LPCTSTR modelNumber)
{
	HRESULT l_hrOk = S_OK;
	
	CString l_csModelNumber = modelNumber;
	l_csModelNumber.TrimLeft();
	l_csModelNumber.TrimRight();
	l_csModelNumber.MakeUpper();
	
	if ( 11 != l_csModelNumber.GetLength() || l_csModelNumber == EmptyModelNo )
	{
		l_hrOk = S_FALSE;
	}
	else
	{
		m_csProcessor.Empty();
		m_csFrameGrabber.Empty();
		m_csIOBoard.Empty();
		m_csOptions.Empty();
		m_csTrigger.Empty();

		int l_iCount = 0;

		for ( int i = 0; i < l_csModelNumber.GetLength(); i++ )
		{
			if ( ::isalnum( l_csModelNumber[i] ) )
			{
				switch( ++l_iCount )
				{
					case 1:
					case 2:
					{
						m_csProcessor += l_csModelNumber[i];

						break;
					}
					case 3:
					case 4:
					{
						m_csFrameGrabber += l_csModelNumber[i];

						break;
					}
					case 5:
					case 6:
					{
						m_csIOBoard += l_csModelNumber[i];

						break;
					}
					case 7:
					case 8:
					{
						m_csOptions += l_csModelNumber[i];

						break;
					}
					default:
					{
						break;
					}
				}
			}
		}

		m_csTrigger = m_csIOBoard;
	}
	return l_hrOk;
}

// The ForcedImageUpdateTimeInSeconds comes from the SVIM.ini file
// It is to be placed in the [Display] section and has the following syntax:
// ForcedImageUpdateTimeInSeconds=N
// Where N is zero to 255, zero means no forced update.
unsigned char SVOIniLoader::GetForcedImageUpdateTime() const
{
	return m_forcedImageUpdateTimeInSeconds;
}

