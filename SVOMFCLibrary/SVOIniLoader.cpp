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

#include"stdafx.h"
#include "SVOIniLoader.h"
#include "SVOIniClass.h"

SVOIniLoader::SVOIniLoader() 
: m_bUseCorrectListRecursion( false )
, m_hrOEMFailure( S_OK )
, m_gigePacketSize( 0 )
, m_bSingleCameraModel( false )
{
}

HRESULT SVOIniLoader::Load(LPCTSTR svimIniFile, LPCTSTR oemIniFile, LPCTSTR hardwareIniFile)
{
	HRESULT l_hrOk = LoadOEMIni(oemIniFile);
	
	if (l_hrOk == S_OK)
		l_hrOk = LoadSVIMIni(svimIniFile);
	
	if (l_hrOk == S_OK)
		l_hrOk = LoadHardwareIni(hardwareIniFile, m_csOptions);

	return l_hrOk;
}

HRESULT SVOIniLoader::LoadOEMIni(LPCTSTR oemIniFile)
{
	HRESULT l_hrOk = S_OK;
	SVOINIClass l_svINI;
	
	BSTR l_bstrValue = NULL;
	CString l_csWinKey;
	
	l_svINI.GetValue( "OEMSpecific", "ProductID", "", &l_bstrValue, oemIniFile );
	l_csWinKey = l_bstrValue;
	::SysFreeString( l_bstrValue );
	l_bstrValue = NULL;

	CString l_csModelNumber;
	
	l_svINI.GetValue( "OEMSpecific", "ModelNo", "00 00 00 00", &l_bstrValue, oemIniFile );
	l_csModelNumber = l_bstrValue;
	::SysFreeString( l_bstrValue );
	l_bstrValue = NULL;
	
	l_csModelNumber.TrimLeft();
	l_csModelNumber.TrimRight();
	l_csModelNumber.MakeUpper();
	
	CString l_csSerialNumber;
	
	l_svINI.GetValue( "OEMSpecific", "SerialNo", "", &l_bstrValue, oemIniFile );
	l_csSerialNumber = l_bstrValue;
	::SysFreeString( l_bstrValue );
	l_bstrValue = NULL;

	l_hrOk = DecodeModelNumber(l_csModelNumber);
	if ( l_hrOk == S_OK )
	{
		m_csWinKey = l_csWinKey;
		m_csModelNumber = l_csModelNumber;
		m_csSerialNumber = l_csSerialNumber;
		l_svINI.GetValue( "OEMSpecific", "SingleCamera", "N", &l_bstrValue, oemIniFile );
		CString l_csSingleCamera = l_bstrValue;
		if ( l_csSingleCamera.Left( 1 ) == "Y" )
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
	BSTR l_bstrValue = NULL;
	
	int l_iLegacyEquation = 0;
	l_svINI.GetValue("SVIM Information", "LegacyEquationParsing", 0, &l_iLegacyEquation, svimIniFile);
	m_bUseCorrectListRecursion = !l_iLegacyEquation;

	for ( int i = 0; i < 4; i++ )
	{
		CString l_csCameraName;

		// Get the Trigger Edge type
		l_csCameraName.Format( "Trigger Edge Camera_%d", i );
		l_svINI.GetValue( "SVIM Information", l_csCameraName, "R", &l_bstrValue, svimIniFile );
		m_csTriggerEdge[i] = l_bstrValue;
		l_svINI.SetValue( "SVIM Information", l_csCameraName, m_csTriggerEdge[i], svimIniFile );

		// Get the Strobe Edge type
		l_csCameraName.Format( "Strobe Edge Camera_%d", i );
		l_svINI.GetValue( "SVIM Information", l_csCameraName, "R", &l_bstrValue, svimIniFile );
		m_csStrobeEdge[i] = l_bstrValue;
		l_svINI.SetValue( "SVIM Information", l_csCameraName, m_csStrobeEdge[i], svimIniFile );
		
		// Use Strobe as Start Frame=Y/N
		l_csCameraName.Format( "Use Strobe as Start Frame Camera_%d", i );
		l_svINI.GetValue( "SVIM Information", l_csCameraName, "N", &l_bstrValue, svimIniFile );
		m_csStartFrameType[i] = l_bstrValue;
		l_svINI.SetValue( "SVIM Information", l_csCameraName, m_csStartFrameType[i], svimIniFile );

	}
	m_csShowUpdateFirmware.Empty();
	l_svINI.GetValue("SVIM Information", "ShowUpdateFirmware","N",&l_bstrValue, svimIniFile );
	m_csShowUpdateFirmware = l_bstrValue;
	SysFreeString( l_bstrValue );
	l_bstrValue = NULL;

	return l_hrOk;
}

HRESULT SVOIniLoader::LoadHardwareIni(LPCTSTR hardwareIniFile, LPCTSTR modelOptions)
{
	HRESULT l_hrOk = S_OK;

	SVOINIClass l_svINI;

	BSTR l_bstrValue = NULL;

	m_csProductName.Empty();
	m_csSoftwareTriggerDLL.Empty();
	m_csAcquisitionTriggerDLL.Empty();
	m_csFileAcquisitionDLL.Empty();

	m_csProcessorBoardName = _T( "Unknown board" );
	m_csTriggerBoardName = _T( "Unknown board" );
	m_csAcquisitionBoardName = _T( "Unknown board" );
	m_csDigitalBoardName = _T( "Unknown board" );
	m_csRAIDBoardName = _T( "Unknown board" );
	
	m_csIOBoardOption = _T("10");

	m_Opto22InputInvert = _T( "N" );
	m_Opto22OutputInvert = _T( "N" );

	l_svINI.GetValue( "Processor", m_csProcessor, "", &l_bstrValue, hardwareIniFile );

	m_csProcessorBoardName = l_bstrValue;

	if( ! m_csProcessorBoardName.IsEmpty() )
	{
		l_svINI.GetValue( m_csProcessorBoardName, "RAID", "", &l_bstrValue, hardwareIniFile );

		m_csRAIDBoardName = l_bstrValue;

		l_svINI.GetValue( m_csProcessorBoardName, "ProductName", "", &l_bstrValue, hardwareIniFile );

		m_csProductName = l_bstrValue;

	}

	l_svINI.GetValue( "Frame Grabber", m_csFrameGrabber, "", &l_bstrValue, hardwareIniFile );

	m_csAcquisitionBoardName = l_bstrValue;

	if( ! m_csAcquisitionBoardName.IsEmpty() )
	{
		l_svINI.GetValue( m_csAcquisitionBoardName, "DigitizerDLL", "", &l_bstrValue, hardwareIniFile );

		m_csDigitizerDLL = l_bstrValue;

		l_svINI.GetValue( m_csAcquisitionBoardName, "TriggerDLL", "", &l_bstrValue, hardwareIniFile );

		m_csTriggerDLL = l_bstrValue;

		if ( m_csProductName.IsEmpty() )
		{
			l_svINI.GetValue( m_csAcquisitionBoardName, "ProductName", "", &l_bstrValue, hardwareIniFile );

			m_csProductName = l_bstrValue;

			if ( m_csProductName.Left( 12 ) == "SVIM X2-GD2A" ||
				m_csProductName.Left( 12 ) == "SVIM X2-GD8A" )
			{
				if ( m_bSingleCameraModel )
				{
					m_csProductName = "SVIM X2-GD1A";
				}
			}
		}

		l_svINI.GetValue( m_csAcquisitionBoardName, "ReloadDLL", "Y", &l_bstrValue, hardwareIniFile );

		m_csReloadAcquisitionDLL = l_bstrValue;

		if( ! m_csTriggerDLL.IsEmpty() )
		{
			m_csTrigger = m_csFrameGrabber;

			m_csTriggerBoardName = m_csAcquisitionBoardName;

			m_csReloadTriggerDLL = m_csReloadAcquisitionDLL;
		}
		
		m_csFirmwareCommand.Empty();
		m_csFirmwareWorkingDir.Empty();
		m_csFirmwareArguments.Empty();

		l_svINI.GetValue( m_csAcquisitionBoardName, "UpdateFirmwareCommand", "", &l_bstrValue, hardwareIniFile);
		m_csFirmwareCommand = l_bstrValue;
		if (! m_csFirmwareCommand.IsEmpty() )
		{
			//get the rest of the firmware commands
			l_svINI.GetValue( m_csAcquisitionBoardName, "UpdateFirmwareWorkingDirectory", "", &l_bstrValue, hardwareIniFile);
			m_csFirmwareWorkingDir = l_bstrValue;

			l_svINI.GetValue( m_csAcquisitionBoardName, "UpdateFirmwareArguments", "", &l_bstrValue, hardwareIniFile);
			m_csFirmwareArguments = l_bstrValue;
		}

		// Matrox Gige - Get Packet Size
		int packetSize = 0;
		l_svINI.GetValue( m_csAcquisitionBoardName, "PacketSize", 0, &packetSize, hardwareIniFile);
		 m_gigePacketSize = static_cast<long>(packetSize);
	}

	l_svINI.GetValue( "IO Board", m_csIOBoard, "", &l_bstrValue, hardwareIniFile );

	m_csDigitalBoardName = l_bstrValue;

	if( ! m_csDigitalBoardName.IsEmpty() )
	{
		l_svINI.GetValue( m_csDigitalBoardName, "DigitalIODLL", "", &l_bstrValue, hardwareIniFile );
		
		m_csDigitalDLL = l_bstrValue;

		l_svINI.GetValue( m_csDigitalBoardName, "ReloadDLL", "Y", &l_bstrValue, hardwareIniFile );

		m_csReloadDigitalDLL = l_bstrValue;

		if ( m_csTriggerDLL.IsEmpty() )
		{
			l_svINI.GetValue( m_csDigitalBoardName, "TriggerDLL", "", &l_bstrValue, hardwareIniFile );

			m_csTriggerDLL = l_bstrValue;

			if( ! m_csTriggerDLL.IsEmpty() )
			{
				m_csTriggerBoardName = m_csDigitalBoardName;

				m_csReloadTriggerDLL = m_csReloadAcquisitionDLL;
			}
		}

		l_svINI.GetValue( m_csDigitalBoardName, "IOBoardMode", "10", &l_bstrValue, hardwareIniFile );
		if( ::SysStringLen( l_bstrValue ) > 0 )
		{
			m_csIOBoardOption = l_bstrValue;
		}

		l_svINI.GetValue( m_csDigitalBoardName, "Opto22InputInvert", "N", &l_bstrValue, hardwareIniFile );
		if( ::SysStringLen( l_bstrValue ) > 0 )
		{
			m_Opto22InputInvert = l_bstrValue;
		}

		l_svINI.GetValue( m_csDigitalBoardName, "Opto22OutputInvert", "N", &l_bstrValue, hardwareIniFile );
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
		CString frameGrabberEntry = "FrameGrabber";
		frameGrabberEntry += m_csFrameGrabber;
		l_svINI.GetValue( m_csDigitalBoardName, frameGrabberEntry , "", &l_bstrValue, hardwareIniFile );

		// if the entry is not found, apply the product name Rule
		if( ::SysStringLen( l_bstrValue ) == 0 )
		{
			// Allow the product name to be overwritten by the IO section product name.
			// This is an exception to the rule "The first one wins".
			// This exception was made for the X Series where the IO board model
			// can dictate the product name.
			l_svINI.GetValue( m_csDigitalBoardName, "ProductName", "", &l_bstrValue, hardwareIniFile );

			if( ::SysStringLen( l_bstrValue ) > 0 )
			{
				m_csProductName = l_bstrValue;
			}
		}

	}

	l_svINI.GetValue( "Options", modelOptions, "", &l_bstrValue, hardwareIniFile );

	m_csHardwareOptions = l_bstrValue;
	::SysFreeString( l_bstrValue );
	l_bstrValue = NULL;

	bool l_UseFileAcq = !( m_csDigitizerDLL.IsEmpty() );
	bool l_UseSoftTrig = !( m_csTriggerDLL.IsEmpty() );

	#ifdef _DEBUG
		l_UseFileAcq = true;
		l_UseSoftTrig = true;
	#endif

	if( l_UseFileAcq )
	{
		m_csFileAcquisitionDLL = _T("SVFileAcquisitionDevice.DLL");
	}

	if( l_UseSoftTrig )
	{
		m_csSoftwareTriggerDLL = _T("SVSoftwareTriggerDevice.DLL");
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
	
	if ( l_csModelNumber.GetLength() != 11 || l_csModelNumber == _T("00 00 00 00") )
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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVOIniLoader.cpp_v  $
 * 
 *    Rev 1.2   10 Jun 2013 18:08:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   21 May 2013 12:54:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated INI Loader to include the two new elements: Windows Key and Serial Number.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 19:31:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   16 Apr 2013 14:10:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 13:08:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   18 Jan 2013 16:04:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added AcquisitionTriggerDll member variable
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   11 Dec 2012 16:33:48   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (constructor) Added m_bSingleCameraModel.
 *   (LoadOEMIni) Added set of m_bSingleCameraModel.
 *   (LoadHardwareIni) Set m_csProductName based on m_bSingleCameraModel for GD2A and GD8A.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   15 Aug 2012 17:00:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code associated with gige packet size parameter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   14 Jul 2011 08:04:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  729
 * SCR Title:  Adjust SVObserver to fix perfomance problems due to logging
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated loading to allow the use of file acquisition and timer trigger in debug mode only reguardless of model type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   13 Dec 2010 08:38:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update source code to use new Xcede-AN board and Rabbit IO.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   19 Feb 2010 15:52:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated loader to handle a trigger number separate from the I/O board and Acquisition Device.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   15 Feb 2010 11:59:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Model info loader to handle the new laptop product requirements.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   16 Jun 2009 17:33:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to support Gige Packet Size entry
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   28 Apr 2009 12:56:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  659
 * SCR Title:  Modify SVObservers IO system behavior to be based on a hardware.ini setting
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added default value for m_csIOBoardOption
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   21 Apr 2009 08:54:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  659
 * SCR Title:  Modify SVObservers IO system behavior to be based on a hardware.ini setting
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added parameter IOBoardMode to hardware.ini under the IO section.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   14 Nov 2008 13:34:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add PLC Dll name in hardware.ini
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Sep 2008 13:13:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added member variable for the name of the FileAcqusitionDevice Dll
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   10 Sep 2008 12:44:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added string for SoftwareTriggerDevice DLL
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Oct 2007 14:54:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SysFreeString in LoadSvimIni and LoadHardwareIni
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Jun 2007 10:19:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include removal of all VC6 constraints, project files, and workspace files.  The new VC8 project and solution files will replace the VC6 files with the new tighter compilation rules.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   20 Jun 2007 14:48:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Mar 2007 14:46:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  592
 * SCR Title:  Add  X3 product type to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified LoadHardwareIni to allow the product name to be overwritten by the IO section.  This allows an X Series product type to have its product name influenced by the IO board type.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Mar 2007 06:22:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  595
 * SCR Title:  Fix issues with Utility Menu when switching products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added the loading of firmware settings from the ini file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 12:54:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
