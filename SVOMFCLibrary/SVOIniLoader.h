// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVOIniLoader.h
// * .File Name       : $Workfile:   SVOIniLoader.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   10 Jun 2013 18:08:06  $
// ******************************************************************************

#ifndef INCL_SVOINILOADER_H
#define INCL_SVOINILOADER_H

class SVOIniLoader
{
public:
	CString m_csWinKey;
	CString m_csModelNumber;
	CString m_csSerialNumber;
	CString m_csProcessor;
	CString m_csFrameGrabber;
	CString m_csIOBoard;
	CString m_csOptions;

	CString m_csTrigger;

	CString m_Opto22InputInvert;
	CString m_Opto22OutputInvert;
	CString m_csTriggerEdge[4];
	CString m_csStrobeEdge[4];
	CString m_csStartFrameType[4];

	CString m_csProductName;
	CString m_csProcessorBoardName;
	CString m_csTriggerBoardName;
	CString m_csAcquisitionBoardName;
	CString m_csDigitalBoardName;
	CString m_csRAIDBoardName;
	
	CString m_csDigitizerDLL;
	CString m_csTriggerDLL;
	CString m_csReloadAcquisitionDLL;
	CString m_csReloadTriggerDLL;
	CString m_csDigitalDLL;
	CString m_csReloadDigitalDLL;
	CString m_csFileAcquisitionDLL;
	CString m_csSoftwareTriggerDLL;
	CString m_csAcquisitionTriggerDLL;
	CString m_csHardwareOptions;

	CString m_csFirmwareCommand;
	CString m_csFirmwareWorkingDir;
	CString m_csFirmwareArguments;

	CString m_csShowUpdateFirmware;
	CString m_csPLCDLL;
	CString m_csIOBoardOption;
	
	long m_gigePacketSize;
	bool m_bUseCorrectListRecursion;

	HRESULT m_hrOEMFailure;
		
	SVOIniLoader();
	
	HRESULT Load(LPCTSTR svimIniFile, LPCTSTR oemIniFile, LPCTSTR hardwareIniFile);
	HRESULT LoadOEMIni(LPCTSTR oemIniFile);
	HRESULT LoadSVIMIni(LPCTSTR svimIniFile);
	HRESULT LoadHardwareIni(LPCTSTR hardwareIniFile, LPCTSTR modelOptions);
	HRESULT DecodeModelNumber(LPCTSTR modelNumber);

protected:
	bool m_bSingleCameraModel;
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVOIniLoader.h_v  $
 * 
 *    Rev 1.2   10 Jun 2013 18:08:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   21 May 2013 12:54:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated INI Loader to include the two new elements: Windows Key and Serial Number.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 19:31:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   16 Apr 2013 14:10:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 13:09:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   18 Jan 2013 16:04:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added AcquisitionTriggerDll member variable
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   11 Dec 2012 16:34:46   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (SVOIniLoader class) Added m_bSingleCameraModel.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   15 Aug 2012 17:00:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code associated with gige packet size parameter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   13 Dec 2010 08:38:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update source code to use new Xcede-AN board and Rabbit IO.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   19 Feb 2010 15:52:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated loader to handle a trigger number separate from the I/O board and Acquisition Device.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   16 Jun 2009 17:33:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to support Gige Packet Size entry
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   21 Apr 2009 08:54:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  659
 * SCR Title:  Modify SVObservers IO system behavior to be based on a hardware.ini setting
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added parameter IOBoardMode to hardware.ini under the IO section.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   14 Nov 2008 13:34:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add PLC Dll name in hardware.ini
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Sep 2008 13:13:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added member variable for the name of the FileAcqusitionDevice Dll
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Sep 2008 12:44:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added string for SoftwareTriggerDevice DLL
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Jun 2007 14:48:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
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
