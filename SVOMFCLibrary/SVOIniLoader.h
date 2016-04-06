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

#pragma once

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
	unsigned char GetForcedImageUpdateTime() const;

protected:
	bool m_bSingleCameraModel;
	unsigned char m_forcedImageUpdateTimeInSeconds;
};

