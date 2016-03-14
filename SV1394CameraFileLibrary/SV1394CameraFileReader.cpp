//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV1394CameraFileReader
//* .File Name       : $Workfile:   SV1394CameraFileReader.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:32:56  $
//******************************************************************************

#include "stdafx.h"
#include "SV1394CameraFileReader.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVOMFCLibrary/SVStringValueDeviceParam.h"
#include "SVMessage/SVMessage.h"

SV1394CameraFileReader::SV1394CameraFileReader(SV1394CameraFileInfoStruct& rInfo) : m_rInfo( rInfo )
{
}

HRESULT SV1394CameraFileReader::ReadParams( SVDeviceParamCollection& rParams )
{
	HRESULT hr = S_OK;

	// calculate checksum
	CStdioFile file;
	if ( file.Open( m_rInfo.sFilename, CFile::modeRead | CFile::shareDenyWrite ) )
	{
		WORD w = GetChecksum( file );
		file.Close();
		CString sFileChecksum;
		sFileChecksum.Format(_T("%04X"), w);
		CString sChecksum;
		DWORD nChars = GetPrivateProfileString( _T("Checksum"), _T("Checksum"), _T(""), sChecksum.GetBuffer(10), 10, m_rInfo.sFilename);
		sChecksum.ReleaseBuffer(nChars);

		if (sChecksum != sFileChecksum)
		{
			// checksum doesn't match
			//hr = S_FALSE;
			hr = SVMSG_INCORRECT_CHECKSUM;
		}


		{	// begin block

			CString sSection;
			sSection = _T("Info");

			CString sVal;
			DWORD dwLen = GetPrivateProfileString(sSection, _T("Version"), _T(""), sVal.GetBuffer(32), 32, m_rInfo.sFilename );
			sVal.ReleaseBuffer(dwLen);
			m_rInfo.sVersion = sVal;

			sVal = _T("");
			dwLen = GetPrivateProfileString( sSection, _T("CameraType"), _T(""), sVal.GetBuffer(128), 128, m_rInfo.sFilename );
			sVal.ReleaseBuffer();
			m_rInfo.sCameraType = sVal;

			ReadCameraFileStringParam( rParams, DeviceParamVendorId,  sSection );
			ReadCameraFileStringParam( rParams, DeviceParamModelName, sSection );
			ReadCameraFileStringParam( rParams, DeviceParamFirmware,  sSection );


			sSection = _T("Settings");

			ReadCameraFileLongParam( rParams, DeviceParamAcquisitionTriggerType, sSection );
			ReadCameraFileCameraFormatsParam( rParams, DeviceParamCameraFormats, sSection );
			ReadCameraFileLutParam( rParams, DeviceParamLut, sSection );

			ReadCameraFileLongParam( rParams, DeviceParamShutter, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamGamma, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamBrightness, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamGain, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamSharpness, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamHue, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamSaturation, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamExposure, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamWhiteBalanceUB, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamWhiteBalanceVR, sSection );

			ReadCameraFileLongParam( rParams, DeviceParamOutputStrobePulse, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamStrobePulseDelay, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamStrobePulseAdvance, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamStrobePulseDuration, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamStrobePulseMultiplier, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraTriggerPin, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraStrobePin, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamTriggerMode, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamTestPattern, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamShutterDelay, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamStrobeStartMode, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamTriggerQueueMode, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamTriggerEdge, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamNumCameraBuffers, sSection );

			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister1,  sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister2,  sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister3,  sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister4,  sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister5,  sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister6,  sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister7,  sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister8,  sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister9,  sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister10, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister11, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister12, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister13, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister14, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister15, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister16, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister17, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister18, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister19, sSection );
			ReadCameraFileLongParam( rParams, DeviceParamCameraRegister20, sSection );
			
			ReadCameraFileBoolParam( rParams, DeviceParamIOStrobeInvert, sSection );
			ReadCameraFileBoolParam( rParams, DeviceParamIOTriggerInvert, sSection );

			ReadCameraFileBoolParam( rParams, DeviceParamAcquisitionStrobeEdge, sSection );
			ReadCameraFileBoolParam( rParams, DeviceParamAcquisitionTriggerEdge, sSection );

			ReadCameraFileLongParam( rParams, DeviceParamCameraTriggerMode, sSection );
		}// end block

	}
	else
	{
		// couldn't open file
		hr = S_FALSE;
	}


	return hr;
}

WORD SV1394CameraFileReader::GetChecksum(CStdioFile& rFile)
{
	WORD wChecksum=0;
	
	CString sLine;
	while ( rFile.ReadString(sLine) )
	{
		if ( sLine == _T("[Checksum]") )
			break;
		for (int i=0; i < sLine.GetLength(); i++)
		{
			WORD w = sLine[i];
			ROL(wChecksum);
			wChecksum = wChecksum + w;
		}
	}

	return wChecksum;
}

HRESULT SV1394CameraFileReader::ReadCameraFileStringParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const CString& sSection )
{
	const int iKEY_DOES_NOT_EXIST = -987654321;
	const int MAX_STRING_BUFFER = 128;
	const CString sFilename( m_rInfo.sFilename );

	CString sKey = SVDeviceParam::GetParameterName( e );

	CString sVal;
	DWORD dwLen = GetPrivateProfileString(sSection, sKey, _T(""), sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
	sVal.ReleaseBuffer(dwLen);
	if ( sVal != _T("") )
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		//rParams.Parameter( e ) = SVStringValueDeviceParam( sVal );
		SVStringValueDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue(pParam);
		ASSERT( pParam );
		pParam->strValue = sVal;
		pParam->SetName(SVString(sKey));

		sVal = _T("");
		dwLen = GetPrivateProfileString(sSection, sKey+_T("_Description"), _T(""), sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
		sVal.ReleaseBuffer(dwLen);
		pParam->SetDescription(SVString(sVal));

		sVal = _T("");
		dwLen = GetPrivateProfileString(sSection, sKey+_T("_VisualName"), sKey, sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
		sVal.ReleaseBuffer(dwLen);
		pParam->SetVisualName(SVString(sVal));
		
		int iVal = GetPrivateProfileInt(sSection, sKey+_T("_Order"), iKEY_DOES_NOT_EXIST, sFilename);
		if ( iVal != iKEY_DOES_NOT_EXIST )
			pParam->SetOrder( iVal );

		iVal = GetPrivateProfileInt(sSection, sKey+_T("_DetailLevel"), iKEY_DOES_NOT_EXIST, sFilename);
		if ( iVal != iKEY_DOES_NOT_EXIST )
			pParam->SetDetailLevel( iVal );

		// now load options
		const CString sKEY_DOES_NOT_EXIST = _T("");
		int iOption=0;
		CString sOptionKey;
		CString sOptionDescription;
		sOptionKey.Format(_T("%s_Option_%d"), sKey, ++iOption);
		while ( (dwLen = GetPrivateProfileString( sSection, sOptionKey, sKEY_DOES_NOT_EXIST, sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename )) 
		         > 0 )
		{
			sVal.ReleaseBuffer(dwLen);
			sOptionKey.Format(_T("%s_Option_%d_Description"), sKey, iOption);
			dwLen = GetPrivateProfileString(sSection, sOptionKey, _T(""), sOptionDescription.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
			sOptionDescription.ReleaseBuffer(dwLen);
			
			SVStringValueDeviceParam::OptionType option = SVStringValueDeviceParam::OptionType( SVString(sVal), SVString(sOptionDescription) );
			pParam->info.options.push_back(option);

			sOptionKey.Format(_T("%s_Option_%d"), sKey, ++iOption);
		}
	}
	return S_OK;
}

HRESULT SV1394CameraFileReader::ReadCameraFileBoolParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const CString& sSection )
{
	const int iKEY_DOES_NOT_EXIST = -987654321;
	const int MAX_STRING_BUFFER = 128;
	const CString sFilename( m_rInfo.sFilename );

	CString sKey = SVDeviceParam::GetParameterName( e );

	int iVal = GetPrivateProfileInt(sSection, sKey, iKEY_DOES_NOT_EXIST, sFilename);
	if ( iVal != iKEY_DOES_NOT_EXIST )
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		//rParams.Parameter( e ) = SVBoolValueDeviceParam((bool) iVal);
		SVBoolValueDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue(pParam);
		ASSERT( pParam );
		pParam->bValue = iVal != 0;
		pParam->SetName(SVString(sKey));

		CString sVal;
		DWORD dwLen = GetPrivateProfileString(sSection, sKey+_T("_Description"), _T(""), sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
		sVal.ReleaseBuffer(dwLen);
		pParam->SetDescription(SVString(sVal));
		
		sVal = _T("");
		dwLen = GetPrivateProfileString(sSection, sKey+_T("_VisualName"), sKey, sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
		sVal.ReleaseBuffer(dwLen);
		pParam->SetVisualName(SVString(sVal));
		
		iVal = GetPrivateProfileInt(sSection, sKey+_T("_Order"), iKEY_DOES_NOT_EXIST, sFilename);
		if ( iVal != iKEY_DOES_NOT_EXIST )
			pParam->SetOrder( iVal );
		
		iVal = GetPrivateProfileInt(sSection, sKey+_T("_DetailLevel"), iKEY_DOES_NOT_EXIST, sFilename);
		if ( iVal != iKEY_DOES_NOT_EXIST )
			pParam->SetDetailLevel( iVal );

		// now load options
		int iOption=0;
		CString sOptionKey;
		CString sOptionDescription;
		sOptionKey.Format(_T("%s_Option_%d"), sKey, ++iOption);
		while ( (iVal = GetPrivateProfileInt( sSection, sOptionKey, iKEY_DOES_NOT_EXIST, sFilename )) 
		         != iKEY_DOES_NOT_EXIST )
		{
			sOptionKey.Format(_T("%s_Option_%d_Description"), sKey, iOption);
			DWORD dwLen = GetPrivateProfileString(sSection, sOptionKey, _T(""), sOptionDescription.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
			sOptionDescription.ReleaseBuffer(dwLen);
			
			SVBoolValueDeviceParam::OptionType option( iVal != 0, SVString(sOptionDescription) );
			pParam->info.options.push_back(option);

			sOptionKey.Format(_T("%s_Option_%d"), sKey, ++iOption);
		}
	}
	return S_OK;
}

HRESULT SV1394CameraFileReader::ReadCameraFileLongParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const CString& sSection )
{
	const int iKEY_DOES_NOT_EXIST = -987654321;
	const int MAX_STRING_BUFFER = 128;
	const CString sFilename( m_rInfo.sFilename );

	CString sKey = SVDeviceParam::GetParameterName( e );

	int iVal = GetPrivateProfileInt(sSection, sKey, iKEY_DOES_NOT_EXIST, sFilename);
	if (iVal != iKEY_DOES_NOT_EXIST)
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		//rParams.Parameter( e ) = SVLongValueDeviceParam( iVal );
		SVLongValueDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue(pParam);
		ASSERT( pParam );
		pParam->lValue = iVal;
		pParam->SetName(SVString(sKey));
		
		CString sVal;
		DWORD dwLen = GetPrivateProfileString(sSection, sKey+_T("_Description"), _T(""), sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
		sVal.ReleaseBuffer(dwLen);
		pParam->SetDescription(SVString(sVal));

		sVal = _T("");
		dwLen = GetPrivateProfileString(sSection, sKey+_T("_VisualName"), sKey, sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
		sVal.ReleaseBuffer(dwLen);
		pParam->SetVisualName(SVString(sVal));
		
		iVal = GetPrivateProfileInt(sSection, sKey+_T("_Order"), iKEY_DOES_NOT_EXIST, sFilename);
		if ( iVal != iKEY_DOES_NOT_EXIST )
			pParam->SetOrder( iVal );
		
		iVal = GetPrivateProfileInt(sSection, sKey+_T("_DetailLevel"), iKEY_DOES_NOT_EXIST, sFilename);
		if ( iVal != iKEY_DOES_NOT_EXIST )
			pParam->SetDetailLevel( iVal );

		sVal.Empty();
		dwLen = GetPrivateProfileString(sSection, sKey+_T("_Mask"), _T(""), sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
		sVal.ReleaseBuffer(dwLen);
		if ( !sVal.IsEmpty() )
		{
			sVal = _T("0x") + sVal;	// register must be specified as a hex number
			TCHAR* p = NULL;
			unsigned long ulMask = _tcstoul( sVal, &p, 16 );
			pParam->info.mask = ulMask;
		}

		sVal.Empty();
		dwLen = GetPrivateProfileString(sSection, sKey+_T("_Register"), _T(""), sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
		sVal.ReleaseBuffer(dwLen);
		if ( !sVal.IsEmpty() )
		{
			sVal = _T("0x") + sVal;	// register must be specified as a hex number
			TCHAR* p = NULL;
			long lRegister = _tcstoul( sVal, &p, 16 );
			pParam->info.vendorId = lRegister;
		}

		// now load options
		int iOption=0;
		CString sOptionKey;
		CString sOptionDescription;
		sOptionKey.Format(_T("%s_Option_%d"), sKey, ++iOption);
		while ( (iVal = GetPrivateProfileInt( sSection, sOptionKey, iKEY_DOES_NOT_EXIST, sFilename )) 
		         != iKEY_DOES_NOT_EXIST )
		{
			sOptionKey.Format(_T("%s_Option_%d_Description"), sKey, iOption);
			DWORD dwLen = GetPrivateProfileString(sSection, sOptionKey, _T(""), sOptionDescription.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
			sOptionDescription.ReleaseBuffer(dwLen);
			
			SVLongValueDeviceParam::OptionType option( iVal, SVString(sOptionDescription) );
			pParam->info.options.push_back(option);

			sOptionKey.Format(_T("%s_Option_%d"), sKey, ++iOption);
		}

		// now load info (e.g. min/max)
		if ( (iVal = GetPrivateProfileInt(sSection, sKey+_T("_min"), iKEY_DOES_NOT_EXIST, sFilename))
			!= iKEY_DOES_NOT_EXIST )
		{
			pParam->info.min = iVal;
		}

		if ( (iVal = GetPrivateProfileInt(sSection, sKey+_T("_max"), iKEY_DOES_NOT_EXIST, sFilename))
			!= iKEY_DOES_NOT_EXIST )
		{
			pParam->info.max = iVal;
		}

		// load offset, multiplier, and Unit string.
		iVal = GetPrivateProfileInt(sSection, sKey+_T("_offset"), 0, sFilename);
		pParam->info.offset = iVal;

		dwLen = GetPrivateProfileString(sSection, sKey+_T("_multiplier"), "1.0", sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
		sVal.ReleaseBuffer(dwLen);
		pParam->info.multiplier = atof(sVal);
		if(pParam->info.multiplier == 0)
			pParam->info.multiplier = 1.0;

		dwLen = GetPrivateProfileString(sSection, sKey+_T("_unit_divisor"), "1.0", sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
		sVal.ReleaseBuffer(dwLen);
		pParam->info.unit_divisor = atof(sVal);
		if(pParam->info.unit_divisor == 0)
			pParam->info.unit_divisor = 1.0;

		dwLen = GetPrivateProfileString(sSection, sKey+_T("_unit"), "Unit", sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
		sVal.ReleaseBuffer(dwLen);
		pParam->info.sUnits = sVal;

	}
	return S_OK;
}

HRESULT SV1394CameraFileReader::ReadCameraFileCameraFormatsParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const CString& sSection )
{
	const int iKEY_DOES_NOT_EXIST = -987654321;
	const int MAX_STRING_BUFFER = 128;
	const CString sFilename( m_rInfo.sFilename );

	CString sKey = SVDeviceParam::GetParameterName( e );

	CString sVal;
	DWORD dwLen = GetPrivateProfileString(sSection, sKey, _T(""), sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
	sVal.ReleaseBuffer(dwLen);
	if ( sVal != _T("") )
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		SVCameraFormatsDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue( pParam );
		ASSERT( pParam );
		pParam->strValue = sVal;
		pParam->SetName(SVString(sKey));

		sVal = _T("");
		dwLen = GetPrivateProfileString(sSection, sKey+_T("_Description"), _T(""), sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
		sVal.ReleaseBuffer(dwLen);
		pParam->SetDescription(SVString(sVal));
		
		sVal = _T("");
		dwLen = GetPrivateProfileString(sSection, sKey+_T("_VisualName"), sKey, sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
		sVal.ReleaseBuffer(dwLen);
		pParam->SetVisualName(SVString(sVal));
		
		int iVal = GetPrivateProfileInt(sSection, sKey+_T("_Order"), iKEY_DOES_NOT_EXIST, sFilename);
		if ( iVal != iKEY_DOES_NOT_EXIST )
			pParam->SetOrder( iVal );
		
		iVal = GetPrivateProfileInt(sSection, sKey+_T("_DetailLevel"), iKEY_DOES_NOT_EXIST, sFilename);
		if ( iVal != iKEY_DOES_NOT_EXIST )
			pParam->SetDetailLevel( iVal );

		// now load options
		const CString sKEY_DOES_NOT_EXIST = _T("");
		int iOption=0;
		CString sOptionKey;
		CString sOptionDescription;
		CString sOption;
		sOptionKey.Format(_T("%s_Option_%d"), sKey, ++iOption);
		while ( (dwLen = GetPrivateProfileString( sSection, sOptionKey, sKEY_DOES_NOT_EXIST, sOption.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename )) 
		         > 0 )
		{
			SVCameraFormat cf;

			sOption.ReleaseBuffer(dwLen);

			cf.ParseAndAssignCameraFormat( sOption );

			CString sOptionDescriptionKey;
			sOptionDescriptionKey.Format(_T("%s_Option_%d_Description"), sKey, iOption);
			dwLen = GetPrivateProfileString(sSection, sOptionDescriptionKey, _T(""), sOptionDescription.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
			sOptionDescription.ReleaseBuffer(dwLen);
			cf.strDescription = sOptionDescription;

			iVal = GetPrivateProfileInt(sSection, sOptionKey+_T("_HStep"), iKEY_DOES_NOT_EXIST, sFilename);
			if ( iVal != iKEY_DOES_NOT_EXIST )
				cf.lHStep = iVal;
			iVal = GetPrivateProfileInt(sSection, sOptionKey+_T("_VStep"), iKEY_DOES_NOT_EXIST, sFilename);
			if ( iVal != iKEY_DOES_NOT_EXIST )
				cf.lVStep = iVal;
			iVal = GetPrivateProfileInt(sSection, sOptionKey+_T("_HPosStep"), iKEY_DOES_NOT_EXIST, sFilename);
			if ( iVal != iKEY_DOES_NOT_EXIST )
				cf.lHPosStep = iVal;
			else
				cf.lHPosStep = cf.lHStep;
			iVal = GetPrivateProfileInt(sSection, sOptionKey+_T("_VPosStep"), iKEY_DOES_NOT_EXIST, sFilename);
			if ( iVal != iKEY_DOES_NOT_EXIST )
				cf.lVPosStep = iVal;
			else
				cf.lVPosStep = cf.lVStep;

			iVal = GetPrivateProfileInt(sSection, sOptionKey+_T("_MinTransferTime"), iKEY_DOES_NOT_EXIST, sFilename);
			if ( iVal != iKEY_DOES_NOT_EXIST )
				cf.lMinTransferTime = iVal;
			iVal = GetPrivateProfileInt(sSection, sOptionKey+_T("_MaxTransferTime"), iKEY_DOES_NOT_EXIST, sFilename);
			if ( iVal != iKEY_DOES_NOT_EXIST )
				cf.lMaxTransferTime = iVal;
			iVal = GetPrivateProfileInt(sSection, sOptionKey+_T("_OptimizedTransferX"), iKEY_DOES_NOT_EXIST, sFilename);
			if ( iVal != iKEY_DOES_NOT_EXIST )
				cf.bOptimizedTransferRate_X = iVal != 0;
			iVal = GetPrivateProfileInt(sSection, sOptionKey+_T("_OptimizedTransferY"), iKEY_DOES_NOT_EXIST, sFilename);
			if ( iVal != iKEY_DOES_NOT_EXIST )
				cf.bOptimizedTransferRate_Y = iVal != 0;

			cf.lOrder = iOption;
			pParam->options[SVString(sOption)] = cf;

			sOptionKey.Format(_T("%s_Option_%d"), sKey, ++iOption);
		}// end while options

		if ( iOption == 0 )
		{
			// default an RS170 image
			SVCameraFormat cf;
			cf.m_strName = _T("M_RS170");
			cf.strDescription = _T("RS170 - 640x480, Mono 8-bit, 12.5 Hz");
			cf.lWidth = cf.lWidthMax = 640;
			cf.lHeight = cf.lHeightMax = 480;
			cf.lFrameRate = 12;
			cf.bVariableROI = false;

			pParam->options[cf.m_strName] = cf;
			pParam->strValue = cf.m_strName;
		}

		// fix for SCR 565; EB 20060613
		if ( !m_rInfo.bColorSystem && pParam->options[ pParam->strValue ].bColor )
		{
			// find first non-color mode and use it
			SVCameraFormatsDeviceParam::OptionsType::const_iterator iter;
			for ( iter = pParam->options.begin(); iter != pParam->options.end(); ++iter )
			{
				if ( !iter->second.bColor )
				{
					pParam->strValue = iter->second.m_strName;
					break;
				}
			}
		}
	}
	return S_OK;
}

HRESULT SV1394CameraFileReader::ReadCameraFileLutParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const CString& sSection )
{
	const int iKEY_DOES_NOT_EXIST = -987654321;
	const int MAX_STRING_BUFFER = 128;
	const CString sFilename( m_rInfo.sFilename );

	CString sKey = SVDeviceParam::GetParameterName( e );

	CString sVal;
	DWORD dwLen;
	int iTransformOperation = GetPrivateProfileInt(sSection, sKey, iKEY_DOES_NOT_EXIST, sFilename);
	if (iTransformOperation != iKEY_DOES_NOT_EXIST)
	{
		rParams.SetParameter( e, (const SVDeviceParam*) SVDeviceParamTempWrapper(SVDeviceParam::Create( e )) );
		SVLutDeviceParam* pParam = rParams.GetParameter( e ).DerivedValue( pParam );
		ASSERT( pParam );
		pParam->SetName(SVString(sKey));

		sVal = _T("");
		dwLen = GetPrivateProfileString(sSection, sKey+_T("_Description"), _T(""), sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
		sVal.ReleaseBuffer(dwLen);
		pParam->SetDescription(SVString(sVal));
		
		sVal = _T("");
		dwLen = GetPrivateProfileString(sSection, sKey+_T("_VisualName"), sKey, sVal.GetBuffer(MAX_STRING_BUFFER), MAX_STRING_BUFFER, sFilename);
		sVal.ReleaseBuffer(dwLen);
		pParam->SetVisualName(SVString(sVal));
		
		int iVal = GetPrivateProfileInt(sSection, sKey+_T("_Order"), iKEY_DOES_NOT_EXIST, sFilename);
		if ( iVal != iKEY_DOES_NOT_EXIST )
			pParam->SetOrder( iVal );
		
		iVal = GetPrivateProfileInt(sSection, sKey+_T("_DetailLevel"), iKEY_DOES_NOT_EXIST, sFilename);
		if ( iVal != iKEY_DOES_NOT_EXIST )
			pParam->SetDetailLevel( iVal );

		SVLut& rLut = pParam->lut;

		SVLutInfo lutinfo;

		iVal = GetPrivateProfileInt(sSection, sKey+_T("_NumBands"), 1, sFilename);	// default to mono
		lutinfo.SetBands( iVal );	/////!!!   may vary for different image formats!

		iVal = GetPrivateProfileInt(sSection, sKey+_T("_BandSize"), 1, sFilename);
		ASSERT( iVal != 1 );	// file needs to define this!!!
		lutinfo.SetBandSize( iVal );	/////!!!   may vary for different image formats!

		iVal = GetPrivateProfileInt(sSection, sKey+_T("_MaxValue"), 1024, sFilename);
		lutinfo.SetMaxValue( iVal );

		lutinfo.SetTransform(SVDefaultLutTransform());
		rLut.Create(lutinfo);

		SVLutTransformParameters param;
		const SVLutTransformOperation* pOperation = SVLutTransform::GetEquivalentType( (SVLutTransformOperationEnum) iTransformOperation );
		ASSERT(pOperation);
		if (pOperation)
		{
			rLut.SetTransformOperation(*pOperation);
			rLut.Transform(param);
		}
	}
	return S_OK;
}

