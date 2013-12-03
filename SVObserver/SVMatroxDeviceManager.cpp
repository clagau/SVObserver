// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxDeviceManager
// * .File Name       : $Workfile:   SVMatroxDeviceManager.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 12:28:56  $
// ******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SVMatroxDeviceManager.h"
#include "SV1394CameraManager.h"
#include "SVDcamDriver.h"
#include <vector>
#include <algorithm>

#include <SVDeviceParams.h>
#include <SVImagingDeviceParams.h>
#include <SVMessage.h>
#include <Winsock2.h>	// for ntohl and htonl
#include "SVDCamStandardDriver.h"	// for DCAM enums

SVMatroxDeviceManager::DigIdToDigNumMapType SVMatroxDeviceManager::m_mapDigitizerIds;

namespace
{
	const int MAX_NUM_1394_DIGITIZERS_NO_HUB = 3;
	const int MAX_NUM_1394_DIGITIZERS_PER_BOARD_HUB = 4;
	const int SV_M_ERR_1394_FORMAT_NOT_SUPPORTED = 2333;
	const int SV_M_ERR_1394_NOT_ENOUGH_BANDWIDTH = 2303;
};

SVMatroxDeviceManager::SVMatroxDeviceManager()
{
	m_MilSystem = M_NULL;
	m_lNumDigitizers = 0;
	m_lMaxNumDigitizers = 12;
}

SVMatroxDeviceManager::~SVMatroxDeviceManager()
{
	Destroy();
}


HRESULT SVMatroxDeviceManager::Create()
{
	HRESULT hr = S_OK;

#if defined (_DEBUG) || defined(_MINDEBUG)
	{// scope
	SVMatroxDeviceParamCollection devicestruct;
	SVDeviceParamStructTestCases test( devicestruct );
	}// end scope
#endif

	if ( m_MilSystem == M_NULL )
	{
		// temporarily disable error reporting...
		SVMatroxAppManager::SetErrorDisplay( false );
		MappHookFunction( M_ERROR_CURRENT, SVMatroxDeviceManager::HandleError, NULL );

		// Allocate a system... we may need to allocate one system per digitizer
		// Max 4 digitizers per system?
#if SV_CURRENT_MIL_VERSION >= 0x0710	// MIL 7.1
		// unfortunately, if there are no cameras connected on a 1394 system, this will return M_NULL!!!
 		m_MilSystem = MsysAlloc( M_SYSTEM_METEOR_II_1394, M_DEF_SYSTEM_NUM, M_COMPLETE, M_NULL );
#endif //#if SV_CURRENT_MIL_VERSION >= 0x0710	// MIL 7.1

		long lSystemError = SVMatroxAppManager::GetError();
		CString sSystemError = SVMatroxAppManager::GetErrorStringFull();

		UINT nBitRate = AfxGetApp()->GetProfileInt(_T("Debug"), _T("1394bitrate"), M_DEFAULT);
		if ( m_MilSystem != M_NULL )
			MsysControl( m_MilSystem, M_SPEED, nBitRate );

		// restore error reporting
		MappHookFunction( M_UNHOOK + M_ERROR_CURRENT, SVMatroxDeviceManager::HandleError, NULL );
		SVMatroxAppManager::SetErrorDisplay( true );

		//if ( m_MilSystem != M_NULL )	// if we don't have a board, don't go any farther
		// create dummy devices anyway
		{

			if ( m_MilSystem != M_NULL )
			{
				if ( MsysInquire( m_MilSystem, M_USE_MMX, M_NULL ) == M_ENABLE )
				{
					// MMX CPU detected. Do we care?
				}
				else   
				{
					// MMX CPU NOT detected
				}

				// Will this setting be necessary?
				// MsysControl( m_MilSystem, M_DISPLAY_DOUBLE_BUFFERING, M_ENABLE );
				// MsysControl( m_MilSystem, M_DISPLAY_DOUBLE_BUFFERING, M_DISABLE );

				// Inquire the number of digitizers available on the system
				MsysInquire(m_MilSystem, M_DIGITIZER_NUM, &m_lNumDigitizers);
			}
			else
			{
				m_lNumDigitizers = 0;
			}

			//if ( m_lNumDigitizers > 0 )
			if ( true )
			{
				int iDig( 0 );

				std::vector<SVMatroxDeviceParamCollection> aDigs;
				for (iDig = 0; iDig < m_lMaxNumDigitizers; iDig++)
				{
					SVMatroxDeviceParamCollection device;
					CString l_DataFormat(_T("M_RS170"));

					// temp
					//if ( m_lMaxNumDigitizers > DEFAULT_MAX_NUM_1394_DIGITIZERS )
					if ( m_bHubSystem )
					{
						// what to do about the hi-res flea and hi-res dragonfly which don't support 640x480???
						// Ignore that for now (2004 08 06)
						CString sFormat("640X480_Y@15FPS");
						sFormat = TheSVObserverApp.INI().GetValueString(_T("1394"), _T("DefaultAlloc"), sFormat ).c_str();
						sFormat = _T("M_") + sFormat;
						l_DataFormat = sFormat;
						//l_DataFormat = "M_640X480_Y@3.75FPS";
						//l_DataFormat = "M_640X480_Y@7.5FPS";
						//l_DataFormat = "M_640X480_Y@15FPS";
						//l_DataFormat = "M_640X480_Y@30FPS";
					}

					//MIL_TEXT_PTR l_DataFormat = M_XXY_DATA[@Z[FPS]];  // string format; use M_FORMAT_SUPPORTED Inquire to get list
					// M_SHARED_BANDWIDTH ?

					device.milSystem = m_MilSystem;
					device.iMilDigNum = iDig;

					SVCameraFormatsDeviceParam cf;
					cf.strValue = CString( l_DataFormat );
					device.SetParameter( DeviceParamCameraFormats, cf );
					//device.SetParameter(DeviceParamFormatString, DeviceParam(CString(l_DataFormat)));

					HRESULT hrAlloc = AllocDigitizer( device );

					aDigs.push_back( device );

					switch ( hrAlloc )
					{
						case SV_M_ERR_1394_FORMAT_NOT_SUPPORTED:
						{
							int a=0;
							a++;
							break;
						}
						case SV_M_ERR_1394_NOT_ENOUGH_BANDWIDTH:
						{
							int a=0;
							a++;
							break;
						}
					}

					//AddDigitizer( device );	// see below

				}

				// order cameras by their serial number, so that the lowest serial number is dig_0.
				// That way there will be predictable results since the real order of digs is random.

				std::sort(aDigs.begin(), aDigs.end());


				SV1394CameraStructSet set;
				for ( iDig = 0; iDig < static_cast<int>(aDigs.size()); iDig++ )
				{
					SV1394CameraStruct camera;
						
					camera.strSerialNum = StringValue( aDigs[iDig].Parameter(DeviceParamSerialNumberString) );
					camera.strModelName = StringValue( aDigs[iDig].Parameter(DeviceParamModelName) );
					camera.strVendorName = StringValue( aDigs[iDig].Parameter(DeviceParamVendorName) );
					camera.iPosition = iDig;
					set.Add( camera );
				}


#if SV_CURRENT_MIL_VERSION >= 0x0710	// MIL 7.1
				TheSV1394CameraManager.SetConnectedCameras( set );

				TheSV1394CameraManager.GetCameraList( set );
#endif	//#if SV_CURRENT_MIL_VERSION >= 0x0710	// MIL 7.1
				ASSERT( set.GetSize() == aDigs.size() );

				int iAllocatedDigCount=0;
				for ( int iIndex = 0; iIndex < set.GetSize(); iIndex++ )
				{
					SV1394CameraStruct l_CameraStruct;
					set.FindByPosition(iIndex, &l_CameraStruct);
					int iDigPos = -1;
					for ( iDig = 0; iDig < static_cast<int>(aDigs.size()); iDig++ )
					{
						if ( l_CameraStruct.strSerialNum == StringValue( aDigs[iDig].Parameter(DeviceParamSerialNumberString) ) )
						{
							iDigPos = iDig;
							break;
						}
					}
					iDig = iIndex;
					if ( iDigPos == -1 )
						iDigPos = iIndex;

					SVMatroxDeviceParamCollection& rDevice = aDigs[iDigPos];
					rDevice.SetParameter(DeviceParamDeviceDigNum, DeviceParam(iDig) );
					CString sName;
					sName.Format(_T("Matrox_1394.Dig_%d"), iDig);
					rDevice.SetParameter(DeviceParamDeviceName, SVStringValueDeviceParam(sName) );
					//sName.Format(_T("Matrox_1394.Dig_%d.Ch_0"), iDig);
					//rDevice.SetParameter(DeviceParamDeviceChannelName, SVStringValueDeviceParam(sName) );
					AddDigitizer( rDevice );
					CString sSerialNumber;
					
					if ( rDevice.ParameterExists(DeviceParamSerialNumberString) )
					{
						sSerialNumber = StringValue(rDevice.Parameter(DeviceParamSerialNumberString));
						iAllocatedDigCount++;
					}
					else
					{
						sSerialNumber = _T("");
					}

					// write serial number
					AfxGetApp()->WriteProfileString(_T("Settings"),
					                                CString(StringValue(rDevice.Parameter(DeviceParamDeviceName))),
					                                sSerialNumber);
				}

				AfxGetApp()->WriteProfileInt(_T("Settings"), _T("Matrox_1394.Count"), iAllocatedDigCount );

			}// end if ( m_lNumDigitizers > 0 )
			else
			{
				hr = SV_FALSE;
			}
		}// end if ( m_MilSystem != M_NULL )
		/*
		else
		{
			SV1394CameraStructSet set;
			for ( int iDig = 0; iDig < NUM_DIGITIZERS; iDig++ )
			{
				SV1394CameraStruct camera;
					
				camera.strSerialNum = _T("");
				camera.iPosition = iDig;
				set.Add( camera );
			}

			TheSV1394CameraManager.SetConnectedCameras( set );
		}
		*/
	}// end if ( m_MilSystem != M_NULL )
	else	// already created
	{
	}




	return hr;
}

bool SVMatroxDeviceManager::IsValidDigitizerSubsystem()
{
	return m_MilSystem != M_NULL;
}

HRESULT SVMatroxDeviceManager::Destroy()
{
	// free digitizers
	/*
	SVMatroxDeviceParamCollection device;
	CString sKey;
	POSITION pos = m_mapDigitizerParams.GetStartPosition();
	while ( pos != NULL )
	{
		m_mapDigitizerParams.GetNextAssoc( pos, sKey, device );
		RemoveDigitizer( sKey );
		FreeDigitizer( device );
	}
	*/
	DigitizerParamMapType::iterator iter;
	//for ( iter = m_mapDigitizerParams.begin(); iter != m_mapDigitizerParams.end(); iter++ )
	while ( !m_mapDigitizerParams.empty() )
	{
		iter = m_mapDigitizerParams.begin();
		HRESULT hrFree = FreeDigitizer( iter->second );
		ASSERT( hrFree == S_OK );
		HRESULT hrRemove = RemoveDigitizer( iter->first );
		ASSERT( hrRemove == S_OK );
	}

	if (m_MilSystem)
	{
		MsysFree (m_MilSystem);     // Free the system
		m_MilSystem = M_NULL;
	}

	::Sleep( 1000 );	// prevent freeze-up in 1394 system

	return S_OK;
}

HRESULT SVMatroxDeviceManager::Reset()
{
	HRESULT hrDestroy = Destroy();
	HRESULT hrCreate = Create();
	if (    FAILED( hrDestroy )
	     || FAILED( hrCreate )  )
	{
		return FAILED(hrCreate) ? hrCreate : hrDestroy;	// favor returning the Create error
	}
	else
		return S_OK;
}

HRESULT SVMatroxDeviceManager::GetAllDevices( SVMatroxDeviceParamCollectionArray& aDevices )
{
	HRESULT hr = S_OK;

	aDevices.RemoveAll();
	/*
	SVMatroxDeviceParamCollection device;
	CString sKey;
	POSITION pos = m_mapDigitizerParams.GetStartPosition();
	while ( pos != NULL )
	{
		m_mapDigitizerParams.GetNextAssoc( pos, sKey, device );
		aDevices.Add( device );
	}
	*/
	DigitizerParamMapType::iterator iter;
	for ( iter = m_mapDigitizerParams.begin(); iter != m_mapDigitizerParams.end(); ++iter )
	{
		aDevices.Add( iter->second );
	}

	return hr;
}

HRESULT SVMatroxDeviceManager::GetDevice( const CString& sDeviceName, SVMatroxDeviceParamCollection& rDevice )
{
	HRESULT hr = SV_FALSE;

	DigitizerParamMapType::iterator iter = m_mapDigitizerParams.find( sDeviceName );
	if ( iter != m_mapDigitizerParams.end() )
	{
		rDevice = iter->second;
		hr = S_OK;
	}
	/*
	SVMatroxDeviceParamCollection l_Device;
	if ( m_mapDigitizerParams.Lookup( sDeviceName, l_Device ) )
	{
		rDevice = l_Device;
		hr = S_OK;
	}
	*/

	return hr;
}

HRESULT SVMatroxDeviceManager::AllocDigitizer( SVMatroxDeviceParamCollection& rDevice )
{
	HRESULT hr = SV_FALSE;
	ASSERT( rDevice.milDig == M_NULL );	// must be freed before calling Alloc
	if ( rDevice.milDig != M_NULL )
	{
		ASSERT( FALSE );
		return hr;
	}

	MIL_ID milDig = M_NULL;
	// may need to correct the conversion from CString to MIL_TEXT_PTR
	//CString sFormat = StringValue(rDevice.Parameter(DeviceParamFormatString));
	SVCameraFormatsDeviceParam* pParam = rDevice.GetParameter( DeviceParamCameraFormats ).DerivedValue( pParam );
	CString sFormat = pParam->strValue;
	
	// do basic normalization
	sFormat.MakeUpper();
	if ( sFormat.Left(2) != _T("M_") )
	{
		sFormat = _T("M_") + sFormat;
	}

	if ( rDevice.milSystem != M_NULL )
	{
		if ( rDevice.iMilDigNum < m_lNumDigitizers )	// to allow for dummy digitizers
		{
			rDevice.bValidDigitizer = true;
			try
			{
				milDig = MdigAlloc( rDevice.milSystem, rDevice.iMilDigNum, (char*) (LPCTSTR) sFormat, M_DEFAULT, M_NULL );
			}
			catch (...)
			{
				// not enough bandwidth exception?
				ASSERT(FALSE);
			}

			long lError = SVMatroxAppManager::GetError();
			if ( lError == SV_M_ERR_1394_FORMAT_NOT_SUPPORTED )	// does not support this format
			{
				int a=0;
				a++;
			}
			else if ( lError == SV_M_ERR_1394_NOT_ENOUGH_BANDWIDTH )	// not enough bandwidth
			{
				int a=0;
				a++;
			}

			if ( lError != M_NULL )
			{
				hr = lError;
				//hr = SVMSG_SVO_19_PROBLEM_ALLOCATING_DIGITIZER;
			}
			else
			{
				hr = S_OK;
			}
		}
	}
	rDevice.SetParameter( DeviceParamDeviceDigNum, DeviceParam(rDevice.iMilDigNum) );
	
	//ASSERT( milDig != M_NULL );
	
	rDevice.milDig = milDig;

	if ( hr == S_OK )
		hr = milDig != M_NULL ? S_OK : SV_FALSE;

	if ( hr == S_OK && milDig != M_NULL )
	{
		try
		{		
		//test stuff
		long lDelay;
		//lDelay=150;
		lDelay=450;

		#ifdef M_INITIALIZATION_DELAY
			MdigControl( milDig, M_INITIALIZATION_DELAY, lDelay);
		#endif // M_INITIALIZATION_DELAY

		long lTriggerSupported;
		MdigInquire( milDig, M_TRIGGER_SUPPORTED, &lTriggerSupported);

		#ifdef M_INITIALIZATION_DELAY
			MdigInquire( milDig, M_INITIALIZATION_DELAY, &lDelay);
		#endif // M_INITIALIZATION_DELAY


		/*
		char* szControlLog = new char [10000];
		memset( szControlLog, 0, 10000 );
		MdigInquire( milDig, M_DIGITIZER_CONTROL_LOG, szControlLog );

		char* szInfoFormat = new char [10000];
		memset( szInfoFormat, 0, 10000 );
		MdigInquire( milDig, M_INFO_FORMAT, szInfoFormat );
		*/







		MdigControl( milDig, M_GRAB_TRIGGER_MODE, M_DEFAULT ); // this is the only setting allowed for 1394

		SVLongValueDeviceParam* pParam = rDevice.GetParameter( DeviceParamAcquisitionTriggerType ).DerivedValue(pParam);
		if ( pParam )
		{
			switch ( pParam->lValue )
			{
			  // hardware async or software async
				case HardwareTrigger:
				case SoftwareAsyncTrigger:
				{
					MdigControl( milDig, M_GRAB_TRIGGER_SOURCE, M_HARDWARE_PORT0 ); // use for hardware triggerable cameras
					MdigControl( milDig, M_GRAB_TIMEOUT, M_INFINITE );	// for triggered cameras

					break;
				}
			}
		}
		else
		{
			//MdigControl( milDig, M_GRAB_TRIGGER_SOURCE, M_SOFTWARE );	// not allowed on the 1394; just use MdigGrab... ??? 7.1 BoardSpecific, p128 says yes, p86 says no; Matrox answer=NOT ALLOWED
		}
		
		//MdigControl( milDig, M_GRAB_MODE, M_ASYNCHRONOUS );
		MdigControl( milDig, M_GRAB_MODE, M_ASYNCHRONOUS_QUEUED );	// this is the one we want
		
#if SV_CURRENT_MIL_VERSION >= 0x0710	// MIL 7.1
		// retrieve serial number of camera
		unsigned long lSerialNumberLow = 0;  // unit (chip) number
		unsigned long lSerialNumberHigh = 0; // vendor number
		MdigInquire( milDig, M_SERIAL_NUMBER_0, &lSerialNumberLow );
		MdigInquire( milDig, M_SERIAL_NUMBER_1, &lSerialNumberHigh );
		unsigned __int64 iUnitID, iVendorID;
		iUnitID = (unsigned __int64) lSerialNumberLow | ((((unsigned __int64) lSerialNumberHigh) & 0xFF) << 32);
		iVendorID = lSerialNumberHigh >> 8;
		CString sTemp;
		CString sSerialNumber;
		//sTemp.Format("%08u-%08u", lSerialNumberHigh, lSerialNumberLow);
		//sTemp.Format("%08u-%08u", lSerialNumberLow, lSerialNumberHigh);	// put unit number before vendor number
		sTemp.Format(_T("%06I64X"), iVendorID);
		rDevice.SetParameter( DeviceParamVendorId, SVStringValueDeviceParam(sTemp) );
		sTemp.Format(_T("%010I64X-%06I64X"), iUnitID, iVendorID);
		sSerialNumber = sTemp;
		rDevice.SetParameter( DeviceParamSerialNumberString, SVStringValueDeviceParam(sTemp) );
		unsigned __int64 iSerialNumber=0;
		iSerialNumber = (iUnitID << 24) | iVendorID;
		//LARGE_INTEGER iSerialNumber;
////		iSerialNumber.LowPart = lSerialNumberLow;
////		iSerialNumber.HighPart = lSerialNumberHigh;
//		// put unit number before vendor number
//		iSerialNumber.LowPart = lSerialNumberLow;
//		iSerialNumber.HighPart = lSerialNumberHigh;
		rDevice.SetParameter( DeviceParamSerialNumber, SVi64ValueDeviceParam(iSerialNumber) );
#endif	//#if SV_CURRENT_MIL_VERSION >= 0x0710	// MIL 7.1

#ifdef M_VENDOR	// if M_VENDOR is defined, M_MODEL should be defined as well. If not, you need Patch 14 mil.h
		// vendor and model strings
		CString strVendor;
		CString strModel;
		{// begin block scope
		long lMaxLength = MdigInquire( rDevice.milDig, M_VENDOR_LENGTH, M_NULL ) + 1;
		char* szVendor = new char [lMaxLength];
		memset( szVendor, 0, lMaxLength * sizeof(char) );
		long lRet = MdigInquire( rDevice.milDig, M_VENDOR, szVendor );

		lMaxLength = MdigInquire( rDevice.milDig, M_MODEL_LENGTH, M_NULL ) + 1;
		char* szModel = new char [lMaxLength];
		memset( szModel, 0, lMaxLength * sizeof(char) );
		lRet = MdigInquire( rDevice.milDig, M_MODEL, szModel );

		CString l_strVendor(szVendor);
		CString l_strModel(szModel);
		strVendor = l_strVendor;
		strModel = l_strModel;
		
		rDevice.SetParameter( DeviceParamVendorName, SVStringValueDeviceParam(strVendor) );
		rDevice.SetParameter( DeviceParamModelName, SVStringValueDeviceParam(strModel) );
		delete szModel;
		delete szVendor;
		}// end block scope
#endif
		// camera settings

		SVMatroxAppManager::SetErrorDisplay( false );

		typedef std::pair<SVDeviceParamEnum, long> LocalMatroxDeviceParamPair;

		std::vector<LocalMatroxDeviceParamPair> aSettings;
		aSettings.push_back( LocalMatroxDeviceParamPair( DeviceParamShutter, DCAM_SHUTTER ) );
		aSettings.push_back( LocalMatroxDeviceParamPair( DeviceParamGamma, DCAM_GAMMA ) );
		aSettings.push_back( LocalMatroxDeviceParamPair( DeviceParamBrightness, DCAM_BRIGHTNESS ) );
		aSettings.push_back( LocalMatroxDeviceParamPair( DeviceParamGain, DCAM_GAIN ) );
		aSettings.push_back( LocalMatroxDeviceParamPair( DeviceParamSharpness, DCAM_SHARPNESS ) );
		aSettings.push_back( LocalMatroxDeviceParamPair( DeviceParamHue, DCAM_HUE ) );
		aSettings.push_back( LocalMatroxDeviceParamPair( DeviceParamSaturation, DCAM_SATURATION ) );
		aSettings.push_back( LocalMatroxDeviceParamPair( DeviceParamExposure, DCAM_AUTO_EXPOSURE ) );

		// THESE STILL USE MIL access
		aSettings.push_back( LocalMatroxDeviceParamPair( DeviceParamWhiteBalanceUB, M_WHITE_BALANCE_U ) );
		aSettings.push_back( LocalMatroxDeviceParamPair( DeviceParamWhiteBalanceVR, M_WHITE_BALANCE_V ) );
		/*
		// do we care about these?
		aSettings.push_back( M_SHARPNESS );
		aSettings.push_back( M_IRIS );
		aSettings.push_back( M_TARGET_TEMPERATURE );
		aSettings.push_back( M_ZOOM );
		aSettings.push_back( M_PAN );
		aSettings.push_back( M_TILT );
		aSettings.push_back( M_OPTICAL_FILTER );
		aSettings.push_back( M_CAPTURE_QUALITY );
		aSettings.push_back( M_CAPTURE_SIZE );
		*/

		SVLongValueDeviceParam param;
		long lVal;
		std::vector<LocalMatroxDeviceParamPair>::iterator iter;
		for ( iter = aSettings.begin(); iter != aSettings.end(); ++iter )
		{
			long lMILError( M_NULL_ERROR );

			param.info.vendorId = iter->second;
			lVal = MdigInquire( milDig, param.info.vendorId, M_NULL );
			lMILError = SVMatroxAppManager::GetStatusCode( _T(__FILE__), __LINE__ );
			if( lMILError == M_NULL_ERROR )
			{
				param.lValue = lVal;
				param.info.min = MdigInquire( milDig, param.info.vendorId + M_MIN_VALUE, M_NULL );
				param.info.max = MdigInquire( milDig, param.info.vendorId + M_MAX_VALUE, M_NULL );
				//MdigControl( milDig, param.info.vendorId, lVal );	// If we set this, it forces manual mode; use M_DEFAULT to revert to automatic mode
			}
			else
			{
				param.Supports(false);
			}
			rDevice.SetParameter( iter->first, param );
		}


		SVMatroxAppManager::SetErrorDisplay( true );

		// Inquire camera information;
		// this really only inquires of the current mode (RS170) so it is meaningless
		long lValue;
		MdigInquire( milDig, M_SIZE_X, &lValue );
		//rDevice.SetParameter( DeviceParamWidth, DeviceParam(lValue) );
		MdigInquire( milDig, M_SIZE_Y, &lValue );
		//rDevice.SetParameter( DeviceParamHeight, DeviceParam(lValue) );
		MdigInquire( milDig, M_SIZE_BAND, &lValue );
		rDevice.SetParameter( DeviceParamSizeBands, DeviceParam(lValue) );

		// find out what formats are supported by the camera
		// from BoardSpecific.pdf page 101
		long lNumFormats = MdigInquire( rDevice.milDig, M_FORMAT_SUPPORTED_NUM, M_NULL );
		long lMaxLength = MdigInquire( rDevice.milDig, M_FORMAT_SUPPORTED_LENGTH, M_NULL );
		char* szFormatStrings = new char [lNumFormats * lMaxLength];
		memset( szFormatStrings, 0, lNumFormats * lMaxLength * sizeof(char) );
		MdigInquire( rDevice.milDig, M_FORMAT_SUPPORTED, szFormatStrings );
		long lOffset=0;
		CString sFormats;

		SVCameraFormatsDeviceParam* pFormatParam = rDevice.GetParameter( DeviceParamCameraFormats).DerivedValue( pFormatParam );
		pFormatParam->options.clear();
		for (long l = 0; l < lNumFormats; l++)
		{
			CString s( &szFormatStrings[lOffset] );
			lOffset += lMaxLength;
			sFormats = sFormats + s + _T("\n");

			SVCameraFormat& rFormat = pFormatParam->options[s];

			rFormat.ParseAndAssignCameraFormat( s );

			// obtain block sizes (subregion)
			// see UNIT_SIZE_INQ and UNIT_POSITION_INQ in the DCAM spec.


		}

		CString strTrace;
		strTrace.Format(_T("\n1394 camera detected: %s %s %s\n"), strVendor, strModel, sSerialNumber);
		::OutputDebugString( strTrace );
		strTrace.Format(_T("Formats supported:\n%s"), sFormats.Left(490));
		::OutputDebugString( strTrace );
//#ifdef _DEBUG
		int iPos = 490;
		while ( iPos + 490 < sFormats.GetLength() )
		{
			TRACE( _T("%s"), sFormats.Mid(490, 490) );
			iPos += 490;
		}
		if ( iPos < sFormats.GetLength() )
		{
			strTrace.Format(_T("%s"), sFormats.Mid( iPos ) );
			::OutputDebugString( strTrace );
		}
//#endif
		::OutputDebugString(_T("\n"));

		/*
		rDevice.SetParameter( DeviceParamFormatStrings, DeviceParam(CString("")) );
		SVStringValueDeviceParam* pFormatParam = rDevice.GetParameter( DeviceParamFormatStrings ).DerivedValue(pFormatParam);
		for (long l = 0; l < lNumFormats; l++)
		{
			CString s( &szFormatStrings[lOffset] );
			sFormats = sFormats + s + _T("\n");
			pFormatParam->options.push_back( (tstring) s );
			lOffset += lMaxLength;
		}
		pFormatParam->strValue = sFormats;
		*/
		delete [] szFormatStrings;

		// the format 7 support will show up as e.g. 640X480_Y_FORMAT_7_0@15FPS
		if ( sFormats.Find("FORMAT_7") != -1)
		{
			rDevice.SetParameter( DeviceParamFormat7Support, DeviceParam(true) );
			// now need to find out subregion limitations
		}
		else
		{
			rDevice.SetParameter( DeviceParamFormat7Support, DeviceParam(false) );
		}

		if ( BoolValue(rDevice.Parameter(DeviceParamFormat7Support)) == true )	// need to figure out when we want this, if at all 
		{
//			MdigControl( milDig, M_SOURCE_COMPENSATION, M_DISABLE );	// hardware cropping only
			MdigControl( milDig, M_SOURCE_COMPENSATION, M_ENABLE );	// software cropping if no hardware cropping (for now)
		}
		else
		{
			MdigControl( milDig, M_SOURCE_COMPENSATION, M_ENABLE );	// software cropping if no hardware cropping (for now)
//			MdigControl( milDig, M_SOURCE_COMPENSATION, M_DISABLE );	// hardware cropping only
		}

		if ( BoolValue(rDevice.Parameter(DeviceParamFormat7Support)) == true )
		{
			// according to documentation, should set size before offset
			//MdigControl( milDig, M_SOURCE_SIZE_X, 0 );	// use with Format 7 to specify ROI. Must be aligned to boundaries of supported subregions
			//MdigControl( milDig, M_SOURCE_SIZE_Y, 0 );	// use with Format 7 to specify ROI. Must be aligned to boundaries of supported subregions
			//MdigControl( milDig, M_SOURCE_OFFSET_X, 0 );	// use with Format 7 to specify ROI. Must be aligned to boundaries of supported subregions
			//MdigControl( milDig, M_SOURCE_OFFSET_Y, 0 );	// use with Format 7 to specify ROI. Must be aligned to boundaries of supported subregions
		}

		CString sDigName;
		sDigName.Format(_T("Matrox_1394.Dig_%d"), LongValue(rDevice.Parameter(DeviceParamDeviceDigNum)));
		rDevice.SetParameter( DeviceParamDeviceName, DeviceParam(sDigName) );
	
		}// end try
		catch (...)
		{
			hr = SV_FALSE;
			ASSERT( FALSE );
		}
	}
	else
	{
		// workaround for problem sorting by serial number when it is nonexistent
		rDevice.SetParameter( DeviceParamSerialNumber, SVi64ValueDeviceParam(0) );
		rDevice.SetParameter( DeviceParamSerialNumberString, SVStringValueDeviceParam(_T("")) );
		rDevice.SetParameter( DeviceParamVendorName, SVStringValueDeviceParam(_T("")) );
		rDevice.SetParameter( DeviceParamModelName, SVStringValueDeviceParam(_T("")) );
	}

	return hr;
}

HRESULT SVMatroxDeviceManager::FreeDigitizer( SVMatroxDeviceParamCollection& rDevice )
{
	HRESULT hr = SV_FALSE;
	
	/*
	SVMatroxDeviceParamCollection l_Device;
	hr = GetDevice( rDevice.sName, l_Device );
	//*/
	hr = S_OK;

	if ( hr == S_OK )
	{
		if ( rDevice.milDig != M_NULL )
		{
			MdigFree( rDevice.milDig );
		}
		rDevice.ClearDig();
		hr = S_OK;
	}

	return hr;
}

HRESULT SVMatroxDeviceManager::ReallocDigitizer( SVMatroxDeviceParamCollection& rDevice )
{
	HRESULT hr = SV_FALSE;
	SVMatroxDeviceParamCollection dummy;
	hr = GetDevice( StringValue(rDevice.Parameter(DeviceParamDeviceName)), dummy );
	if ( hr == S_OK )
	{
		CString sDeviceName = StringValue( rDevice.Parameter(DeviceParamDeviceName) );
		int iDigNum = LongValue( rDevice.Parameter( DeviceParamDeviceDigNum ) );
		hr = RemoveDigitizer( sDeviceName );
		ASSERT( hr == S_OK );
		hr = FreeDigitizer( rDevice );
		ASSERT( hr == S_OK );
		hr = AllocDigitizer( rDevice );
		if ( hr != S_OK )	// translate error for external use
		{
			hr = SVMSG_SVO_19_PROBLEM_ALLOCATING_DIGITIZER;
		}
		rDevice.SetParameter(DeviceParamDeviceDigNum, DeviceParam(iDigNum) );
		rDevice.SetParameter(DeviceParamDeviceName, DeviceParam(sDeviceName) );
		AddDigitizer( rDevice );
	}
	return hr;
}


HRESULT SVMatroxDeviceManager::AddDigitizer(SVMatroxDeviceParamCollection& rDevice )
{
	//m_mapDigitizerParams.SetAt( StringValue(rDevice.Parameter(DeviceParamDeviceName)), rDevice );
	m_mapDigitizerParams[ StringValue(rDevice.Parameter(DeviceParamDeviceName)) ] = rDevice;
	m_mapDigitizerIds[ rDevice.milDig ] = rDevice.iMilDigNum;
	return S_OK;
}

HRESULT SVMatroxDeviceManager::RemoveDigitizer( const CString& sDigName )
{
	/*
	SVMatroxDeviceParamCollection device;
	if ( m_mapDigitizerParams.Lookup( sDigName, device ) )
	{
		m_mapDigitizerParams.RemoveKey( sDigName );
		return S_OK;
	}
	*/
	DigitizerParamMapType::iterator iter = m_mapDigitizerParams.find( sDigName );
	if ( iter != m_mapDigitizerParams.end() )
	{
		DigIdToDigNumMapType::iterator iterId = m_mapDigitizerIds.find( iter->second.milDig );
		if ( iterId != m_mapDigitizerIds.end() )
		{
			m_mapDigitizerIds.erase( iterId );
		}

		m_mapDigitizerParams.erase( iter );
		return S_OK;
	}
	return S_OK;
}

/*static*/ long MFTYPE SVMatroxDeviceManager::HandleError(long lHookType, MIL_ID EventId, void MPTYPE* pUserData)
{
	return M_NULL;
}

HRESULT SVMatroxDeviceManager::WriteCameraRegister( MIL_ID milSystem, MIL_ID milDig, ULONG ulAddress, unsigned long ulValue )
{
	HRESULT hr = S_OK;
	
	const ULONG ulBaseAddress = 0xF0F00000;
	ulAddress |= ulBaseAddress;
	
	try
	{
		if ( M_MIL_CURRENT_VERSION > 7.4	// 7.5
#ifdef M_START_REGISTER_ADDRESS
			&& (ulAddress >= M_START_REGISTER_ADDRESS
			    && ulAddress <= M_END_REGISTER_ADDRESS)
#endif
				)
		{
			MdigControl( milDig, ulAddress, ulValue );
		}
		else
		{
			DigIdToDigNumMapType::iterator iter = m_mapDigitizerIds.find( milDig );
			if ( iter != m_mapDigitizerIds.end() )
			{
				long lDevNum = iter->second;
				// write to a camera register; provided by Shawn Black, Matrox tech support, March 6, 2003
				MsysConfigAccess( milSystem,                  //1394 System ID
								  M_NULL, 
								  M_NULL, 
								  lDevNum,                    //Device of the camera
								  M_1394_CONFIGURATION_SPACE, 
								  M_WRITE + M_BIG_ENDIAN,     //or M_READ + M_BIG_ENDIAN
								  ulAddress,                  //32 least significant bits of the address of the register
								  sizeof(long),               //can only access 4-byte values
								  &ulValue);                  //Address of a long containing the value to write or address of a long where to put the value read
			}
		}
	}
	catch (...)
	{
		hr = SV_FALSE;
	}

	return hr;
}

HRESULT SVMatroxDeviceManager::ReadCameraRegister( MIL_ID milSystem, MIL_ID milDig, ULONG ulAddress, unsigned long& rulValue )
{
	HRESULT hr = S_OK;
	
	const ULONG ulBaseAddress = 0xF0F00000;
	ulAddress |= ulBaseAddress;
	
	try
	{
		if ( M_MIL_CURRENT_VERSION > 7.4	// 7.5
#ifdef M_START_REGISTER_ADDRESS
			&& (ulAddress >= M_START_REGISTER_ADDRESS
			    && ulAddress <= M_END_REGISTER_ADDRESS)
#endif
				)
		{
			if( milDig != M_NULL )
			{
				MdigInquire( milDig, ulAddress, &rulValue );
			}
			else
			{
				hr = S_FALSE;
			}
		}
		else
		{
			// read from a camera register; provided by Shawn Black, Matrox tech support, March 6, 2003
			DigIdToDigNumMapType::iterator iter = m_mapDigitizerIds.find( milDig );
			if ( iter != m_mapDigitizerIds.end() )
			{
				long lDevNum = iter->second;
				MsysConfigAccess( milSystem,                  //1394 System ID
								  M_NULL, 
								  M_NULL, 
								  lDevNum,                    //Device of the camera
								  M_1394_CONFIGURATION_SPACE, 
								  M_READ + M_BIG_ENDIAN,      //or M_WRITE + M_BIG_ENDIAN
								  ulAddress,                  //32 least significant bits of the address of the register
								  sizeof(long),               //can only access 4-byte values
								  &rulValue);                 //Address of a long containing the value to write or address of a long where to put the value read
			}
		}
	}
	catch (...)
	{
		hr = SV_FALSE;
	}

	return hr;
}

HRESULT SVMatroxDeviceManager::WriteCameraRegisterBlock( MIL_ID milSystem, MIL_ID milDig, ULONG ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	HRESULT hr = S_OK;
	
	const ULONG ulBaseAddress = 0xF0F00000;
	ulAddress |= ulBaseAddress;

	try
	{
		ulCount *= sizeof(ULONG);	// count should be number of ULONGs. multiply to get bytes

		// write to a camera register; provided by Shawn Black, Matrox tech support, March 6, 2003
		DigIdToDigNumMapType::iterator iter = m_mapDigitizerIds.find( milDig );
		if ( iter != m_mapDigitizerIds.end() )
		{
			long lDevNum = iter->second;
			MsysConfigAccess( milSystem,                  //1394 System ID
							  M_NULL, 
							  M_NULL, 
							  lDevNum,                    //Device of the camera
							  M_1394_CONFIGURATION_SPACE, 
							  M_WRITE + M_BIG_ENDIAN,     //or M_WRITE + M_BIG_ENDIAN
							  ulAddress,                  //32 least significant bits of the address of the register
							  ulCount,                    //can only access 4-byte values
							  paulValue );                //Address of a long containing the value to write or address of a long where to put the value read
		}
	}
	catch (...)
	{
		hr = SV_FALSE;
	}
	
	return hr;
}

HRESULT SVMatroxDeviceManager::ReadCameraRegisterBlock( MIL_ID milSystem, MIL_ID milDig, ULONG ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	HRESULT hr = S_OK;
	
	const ULONG ulBaseAddress = 0xF0F00000;
	ulAddress |= ulBaseAddress;

	try
	{
		ulCount *= sizeof(ULONG);	// count should be number of ULONGs. multiply to get bytes

		// read from a camera register; provided by Shawn Black, Matrox tech support, March 6, 2003
		DigIdToDigNumMapType::iterator iter = m_mapDigitizerIds.find( milDig );
		if ( iter != m_mapDigitizerIds.end() )
		{
			long lDevNum = iter->second;
			MsysConfigAccess( milSystem,                  //1394 System ID
							  M_NULL, 
							  M_NULL, 
							  lDevNum,                    //Device of the camera
							  M_1394_CONFIGURATION_SPACE, 
							  M_READ + M_BIG_ENDIAN,      //or M_WRITE + M_BIG_ENDIAN
							  ulAddress,                  //32 least significant bits of the address of the register
							  ulCount,                    //can only access 4-byte values
							  paulValue );                //Address of a long containing the value to write or address of a long where to put the value read
		}
	}
	catch (...)
	{
		hr = SV_FALSE;
	}
	
	return hr;
}



long SVMatroxDeviceManager::SwitchByteOrder( long& l )
{
	unsigned long lTemp = ((l & 0xFF) << 24) | ((l & 0xFF00) << 8) | ((l & 0xFF0000) >> 8) | (((unsigned long) l & 0xFF000000) >> 24);
	l = (long) lTemp;
	return l;
}

unsigned long SVMatroxDeviceManager::SwitchByteOrder( unsigned long& l )
{
	unsigned long lTemp = ((l & 0xFF) << 24) | ((l & 0xFF00) << 8) | ((l & 0xFF0000) >> 8) | ((l & 0xFF000000) >> 24);
	l = lTemp;
	return l;
}

int SVMatroxDeviceManager::GetMaxNumDigitizers()
{
	return m_lMaxNumDigitizers;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMatroxDeviceManager.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:28:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   04 Oct 2012 10:45:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   18 Jul 2012 14:10:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed obsolete methods assiciated with overlay drawling and fixed problem with copying data to byte buffer have wrong format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   29 Jun 2012 16:52:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add the needed functionality to track parameter information and reset the camera to the desired parameters on reconnect.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   21 Jun 2007 14:16:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   23 Mar 2007 10:50:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  589
 * SCR Title:  Fix loading D3 and D2 configurations on an X1 platform
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Force maxDigitizers to be 12. This allows any configurations to be read in so they can be converted.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   19 Feb 2007 16:38:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  594
 * SCR Title:  Remove issue with interpreting status response incorrectly
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated MIL error code methodology to use the new SVMatroxAppManager GetStatusCode operator.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   16 Aug 2006 09:52:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  567
 * SCR Title:  Update SVObserver to accept Intek driver for D-series SVIMs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated code to include a check for a NULL device before an attempt was made to read a camera register.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   21 Mar 2006 09:25:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  539
 * SCR Title:  Upgrade SVObserver to the Mimas release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Output camera trace information in Release build as well as Debug build.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   08 Nov 2005 15:59:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   11 Aug 2005 11:54:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   use direct register writes for light reference parameters to avoid lockup issues
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   19 Jul 2005 14:33:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented > 4 camera hub capabilities
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   14 Jul 2005 12:15:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  475
 * SCR Title:  Increase Hub capabilities
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added TRACE for camera detection
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   26 Oct 2004 09:13:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  461
 * SCR Title:  Add 1394 Hub capability to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added hub support (more than 3 digitizers, and change default allocation for hub systems)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   23 Jul 2004 08:32:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  435
 * SCR Title:  Add Bulb Mode Operation to the Digital SVIM for Point Gray Dragonfly Cameras
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated AllocDigitizer method to improve clairity and consistancy in the use of a particular camera parameter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   03 Feb 2004 17:28:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed field name
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   23 Jan 2004 09:57:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code to make the model and vendor name available in the camera manager dialog
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   18 Dec 2003 14:51:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed Connect / Disconnect problem that reset the DeviceChannelName to .Ch_0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   17 Dec 2003 13:06:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   17 Dec 2003 12:51:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed compatibility issue with MIL 6.1
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   24 Nov 2003 14:33:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed to be play nicely with MIL 6.1
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   19 Nov 2003 11:54:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed color problem
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   12 Nov 2003 09:51:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed problem of creating dummy devices when no cams attached
 * connect to SV1394CameraManager to determine order of digitizers
 * fixed block transfer code
 * store VendorId param for each dig
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   03 Nov 2003 16:24:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changes to allow unplugging and plugging cameras
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   31 Oct 2003 08:27:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   support for bit rate setting (M_SPEED)
 * save serial numbers to registry
 * added block transfer code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   22 Oct 2003 09:37:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Sharpness, Hue, Saturation, WhiteBalance, and color image format support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   14 Oct 2003 09:22:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed serial number calculation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   24 Sep 2003 10:51:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   wrapped MIL 7.x in preprocessor #define
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   22 Sep 2003 14:59:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   cleanup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   17 Sep 2003 09:11:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed SerialNumber handling
 * changed Read/Write register handling
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Aug 2003 10:24:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   unit id is now Most Significant part of serial number
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   01 Aug 2003 12:45:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   general cleanup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Jul 2003 10:04:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added #include <SVImagingDeviceParams.h>
 * changed lImageType to eImageType as part of SVCameraFormat change
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Jul 2003 10:52:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Format 7 support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Jul 2003 08:58:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for SVCameraFormatsDeviceParam
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   07 Jul 2003 11:21:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added files to support Matrox acquisition devices
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
