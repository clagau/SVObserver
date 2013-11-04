//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIntekAcquisitionClass
//* .File Name       : $Workfile:   SVIntekAcquisitionClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 16:16:34  $
//******************************************************************************

#include "stdafx.h"
#include "SVIntekAcquisitionClass.h"
#include "SVDataManagerLibrary/DataManager.h"
#include "SVObserver.h"
#include "SVImageProcessingClass.h"
#include "SVDigitizerProcessingClass.h"
#include "SV1394CameraFileLibrary/SV1394CameraFileReader.h"
#include "SV1394CameraFileLibrary/SVDCamDriver.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVi64ValueDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVOMFCLibrary/SVOINIClass.h"
#include "SVMessage/SVMessage.h"
#include "SV1394CameraFileLibrary/SVDCamFactoryRegistrar.h"
#include "SVIntek/SVIntekEnums.h"

enum
{
	SV_CORACQ_VAL_OUTPUT_FORMAT_RGB5551   = 10,
	SV_CORACQ_VAL_OUTPUT_FORMAT_RGB565    = 11,
	SV_CORACQ_VAL_OUTPUT_FORMAT_RGB888    = 12,
	SV_CORACQ_VAL_OUTPUT_FORMAT_RGB8888   = 13,
	SV_CORACQ_VAL_OUTPUT_FORMAT_RGB101010 = 14,
	SV_CORACQ_VAL_OUTPUT_FORMAT_RGB161616 = 15,
	SV_CORACQ_VAL_OUTPUT_FORMAT_UYVY      = 19,
	SV_CORACQ_VAL_OUTPUT_FORMAT_YUY2      = 20,
	SV_CORACQ_VAL_OUTPUT_FORMAT_YVYU      = 21,
	SV_CORACQ_VAL_OUTPUT_FORMAT_YUYV      = 22,
	SV_CORACQ_VAL_OUTPUT_FORMAT_Y411      = 23,
	SV_CORACQ_VAL_OUTPUT_FORMAT_Y211      = 24,

	SV_CORACQ_VAL_OUTPUT_FORMAT_MONO8     = 1,
	SV_CORACQ_VAL_OUTPUT_FORMAT_MONO16    = 3,
	SV_CORACQ_VAL_OUTPUT_FORMAT_MONO32    = 7,
};

SVIntekAcquisitionClass::SVIntekAcquisitionClass( const SVAcquisitionConstructParams& p_rParams )
: SVAcquisitionClass( p_rParams )
{
	mbIsCorFilesLoaded = false;
	mbTempOnline = false;
	
	m_pCameraDriver = NULL;
	m_cameraDriverProxy.SetOwner(this);
}

SVIntekAcquisitionClass::~SVIntekAcquisitionClass()
{
	DestroyLocal();
}

bool SVIntekAcquisitionClass::IsValid()
{
	bool bOk = FALSE;

	bOk = IsValidBoard();

	bOk = SVAcquisitionClass::IsValid() && bOk;
	
	return bOk;
}

bool SVIntekAcquisitionClass::IsValidBoard()
{
	bool bOk = TRUE;
	
	bOk = m_hDigitizer != NULL && bOk;

	if( bOk && m_DeviceParams.ParameterExists( DeviceParamVendorId ) )
	{
		CString l_csVenderId;

		_variant_t l_oValue;

		SVDigitizerLoadLibraryClass* l_psvLibrary = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName);

		if( l_psvLibrary != NULL && l_psvLibrary->ParameterGetValue( m_hDigitizer, 100, 0, &l_oValue ) == S_OK )
		{
			l_csVenderId = l_oValue.bstrVal;

			bOk = l_csVenderId == StringValue( m_DeviceParams.Parameter( DeviceParamVendorId ) );
		}
	}
	
	return bOk;
}

HRESULT SVIntekAcquisitionClass::Destroy()
{
	HRESULT hrOk = S_FALSE;

	hrOk = DestroyLocal();

	if ( SVAcquisitionClass::Destroy() != S_OK )
	{
		hrOk = S_FALSE;
	}
	
	return hrOk;
}

HRESULT SVIntekAcquisitionClass::LoadFiles(SVFileNameArrayClass& rFiles)
{
	m_DeviceParams.Clear();

	HRESULT hr = SVAcquisitionClass::LoadFiles( rFiles );
	if ( hr == S_OK )
	{
		ASSERT( mFiles.GetSize() == 1 );	// only one file
		if ( mFiles.GetSize() == 1 )
		{
			CString sFile = mFiles[0].GetFullFileName();

			SV1394CameraFileInfoStruct info;
			info.sFilename = sFile;
			info.bColorSystem = TheSVObserverApp.IsColorSVIM() ? true : false;

			hr = ReadCameraFileImpl( info, m_CameraFileDeviceParams );

			if ( m_pCameraDriver )
			{
				delete m_pCameraDriver;

				m_pCameraDriver = NULL;
			}

			try
			{
				m_pCameraDriver = TheDCamDriverFactory::Instance().New( info.sCameraType );
			}
			catch ( std::runtime_error& )
			{
			}

			ASSERT( m_pCameraDriver );
			if ( m_pCameraDriver )
			{
				m_DCamAcqProxy.SetOwner( mDeviceName.c_str() );
				m_pCameraDriver->SetOwner( &m_DCamAcqProxy );
				m_cameraDriverProxy.Init(m_pCameraDriver);
			}

			if ( hr == SVMSG_INCORRECT_CHECKSUM )
			{
			}
			SetDeviceParameters( m_CameraFileDeviceParams );
			GetCameraImageInfo( &msvImageInfo );
		}
	}

	return hr;
}

HRESULT SVIntekAcquisitionClass::ReadCameraFile( const CString& sFilename, SVDeviceParamCollection& rParams )
{
	SV1394CameraFileInfoStruct info;
	info.sFilename = sFilename;
	info.bColorSystem = TheSVObserverApp.IsColorSVIM() ? true : false;
	HRESULT hr = ReadCameraFileImpl( info, rParams );

	return hr;
}

HRESULT SVIntekAcquisitionClass::ReadCameraFileImpl( SV1394CameraFileInfoStruct& p_rInfo, SVDeviceParamCollection& p_rParams )
{
	HRESULT hr = S_OK;
	hr = SVDCamDriverProxy::ReadCameraFileImpl(p_rInfo, p_rParams);
	return hr;
}

HRESULT SVIntekAcquisitionClass::SetParameterDefaults( SVDeviceParamCollection& rParams )
{
	return SVDCamDriverProxy::SetParameterDefaults( rParams );
}

HRESULT SVIntekAcquisitionClass::LoadLightReference( SVLightReference& rArray )
{
	HRESULT hr = S_FALSE;

	return hr;
}

HRESULT SVIntekAcquisitionClass::CreateLightReference(int iBands, int iBrightness, int iContrast)
{
	ASSERT(iBands > 0);

	HRESULT hr = S_FALSE;
	hr = SVAcquisitionClass::CreateLightReference( iBands, iBrightness, iContrast );
	
	if ( hr == S_OK )
	{
		DWORD dwDefaultBrightness = 0;	// not used
		for ( int i = 0; hr == S_OK && i < iBands; i++)
		{
			hr = CreateLightReferenceBand( i, 0 );
		}
		if ( hr != S_OK )
		{
			mLightReference.DeleteAll();
		}
	}

	return hr;
}

HRESULT SVIntekAcquisitionClass::GetMaxLightReferenceValue( unsigned long ulType, long &rlValue )
{
	HRESULT hr = S_OK;

	SVDeviceParamEnum eParam = (SVDeviceParamEnum) ulType;
	const SVLongValueDeviceParam* pParam = m_CameraFileDeviceParams.Parameter( eParam ).DerivedValue( pParam );
	if ( pParam )
		rlValue = pParam->info.max;
	else
	{
		rlValue = 0;
		hr = S_FALSE;
	}

	return hr;
}

HRESULT SVIntekAcquisitionClass::GetMinLightReferenceValue( unsigned long ulType, long &rlValue )
{
	HRESULT hr = S_OK;

	const SVDeviceParamEnum eParam = (SVDeviceParamEnum) ulType;
	const SVLongValueDeviceParam* pParam = m_CameraFileDeviceParams.Parameter( eParam ).DerivedValue( pParam );
	if ( pParam )
		rlValue = pParam->info.min;
	else
	{
		rlValue = 0;
		hr = S_FALSE;
	}

	return hr;
}

HRESULT SVIntekAcquisitionClass::GetLightReferenceValueStep( unsigned long ulType, unsigned long &rulValue )
{
	HRESULT hr = S_OK;
	rulValue = 1;
	return hr;
}

bool SVIntekAcquisitionClass::CameraMatchesCameraFile()
{
	bool l_bOk = true;
	if (m_hDigitizer != NULL && SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName) != NULL)
	{
		l_bOk = m_cameraDriverProxy.CameraMatchesCameraFile(m_CameraFileDeviceParams, m_hDigitizer, SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName));
	}
	return l_bOk;
}

HRESULT SVIntekAcquisitionClass::InitializeDevice( const SVDeviceParamWrapper& rwParam )
{
	HRESULT hr = S_OK;

	if( ! CameraMatchesCameraFile() )
	{
		hr = SVMSG_SVO_20_INCORRECT_CAMERA_FILE;
	}

	if ( hr == S_OK )
	{
		UpdateLightReferenceAttributes( rwParam );

		if (m_hDigitizer != NULL && IsDigitizerSubsystemValid())
		{
			hr = m_cameraDriverProxy.InitializeDevice(rwParam, m_DeviceParams, m_hDigitizer, SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName));
		}
	}

	return hr;
}

HRESULT SVIntekAcquisitionClass::InitializeDevice( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_OK;

	if( ! CameraMatchesCameraFile() )
	{
		hr = SVMSG_SVO_20_INCORRECT_CAMERA_FILE;
	}

	UpdateLightReferenceAttributes( rDeviceParams );

	if ( hr == S_OK )
	{
		if (m_hDigitizer != NULL && IsDigitizerSubsystemValid())
		{
			hr = m_cameraDriverProxy.InitializeDevice(rDeviceParams, m_hDigitizer, SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName));
		}
		else
		{
			if (!IsDigitizerSubsystemValid())
			{
				// Update image info with format setting from device parameters
				GetCameraImageInfo( &msvImageInfo );
			}
		}
	}

	return hr;
}

HRESULT SVIntekAcquisitionClass::UpdateLightReferenceAttributes( const SVDeviceParamCollection& rParams )
{
	SVDeviceParamMap::const_iterator iter;
	for ( iter = rParams.mapParameters.begin(); iter != rParams.mapParameters.end(); ++iter)
	{
		const SVDeviceParamWrapper& rw = iter->second;
		UpdateLightReferenceAttributes( rw );
	}
	return S_OK;
}

HRESULT SVIntekAcquisitionClass::UpdateLightReferenceAttributes( const SVDeviceParamWrapper& rwParam )
{
	if ( rwParam.IsValid() )
	{
		switch ( rwParam->Type() )
		{
			case DeviceParamWhiteBalanceUB:
			case DeviceParamWhiteBalanceVR:
			{
				
				const SVLongValueDeviceParam* pParam = rwParam.DerivedValue( pParam );
				if ( mLightReference.NumBands() > 1 )
				{
					int iBand = rwParam->Type() == DeviceParamWhiteBalanceVR ? 0 : 2;
					UpdateLightReferenceAttributes( iBand, pParam );
				}
				
			}
			break;

			case DeviceParamBrightness:
			case DeviceParamGain:
			case DeviceParamExposure:
			case DeviceParamGamma:
			case DeviceParamHue:
			case DeviceParamSaturation:
			{
				// update LR
				for (int iBand = 0; iBand < 1; iBand++)	// we don't need duplicate data for each band; these are not specific to each band
				{
					const SVLongValueDeviceParam* pParam = rwParam.DerivedValue( pParam );
					UpdateLightReferenceAttributes( iBand, pParam );
				}
			}
		}
	}

	return S_OK;
}

HRESULT SVIntekAcquisitionClass::UpdateLightReferenceAttributes( int iBand, const SVLongValueDeviceParam* pParam )
{
	HRESULT hr = S_FALSE;

	SVLightReferenceAttributeInfo* pAttribute = mLightReference.Band( iBand ).AttributeByType( (DWORD) pParam->Type() );
	const SVLongValueDeviceParam* pCFParam = m_CameraFileDeviceParams.Parameter(pParam->Type()).DerivedValue( pCFParam );
	if ( pAttribute )
	{
		pAttribute->lValue = pParam->lValue;
		if ( pCFParam )
		{
			pAttribute->lMax = pCFParam->info.max;
			pAttribute->lMin = pCFParam->info.min;
		}
	}
	else
	{
		SVLightReferenceAttributeInfo attribute;

		switch ( pParam->Type() )
		{
			case DeviceParamGamma:
				attribute.strName = pCFParam->VisualName();
				break;
			case DeviceParamBrightness:
				attribute.strName = pCFParam->VisualName();
				break;
			case DeviceParamGain:
				attribute.strName = _T("Contrast");
				break;
			case DeviceParamExposure:
				attribute.strName = pCFParam->VisualName();
				break;
			case DeviceParamWhiteBalanceUB:
			case DeviceParamWhiteBalanceVR:
				attribute.strName = _T("White Balance");
				break;
		}
		
		attribute.dwType = (DWORD) pParam->Type();
		attribute.lStep = 1;
		attribute.lValue = pParam->lValue;
		if ( pCFParam )
		{
			attribute.lMax = pCFParam->info.max;
			attribute.lMin = pCFParam->info.max;
		}
		mLightReference.Band(iBand).AddAttribute( attribute );
		hr = S_OK;
	}// end if pAttribute else

	return hr;
}

HRESULT SVIntekAcquisitionClass::WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue )
{
	HRESULT l_hrOk = S_FALSE;
	if ( SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName) != NULL )
	{
		l_hrOk = m_cameraDriverProxy.WriteCameraRegister(ulAddress, ulValue, m_hDigitizer, SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName));
	}
	return l_hrOk;
}

HRESULT SVIntekAcquisitionClass::ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue )
{
	HRESULT l_hrOk = S_FALSE;
	if ( SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName) != NULL )
	{
		l_hrOk = m_cameraDriverProxy.ReadCameraRegister( ulAddress, rulValue, m_hDigitizer, SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName));
	}

	return l_hrOk;
}

HRESULT SVIntekAcquisitionClass::WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName) != NULL )
	{
		l_hrOk = m_cameraDriverProxy.WriteCameraRegisterBlock(ulAddress, ulCount, paulValue, m_hDigitizer, SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName));
	}
	return l_hrOk;
}

HRESULT SVIntekAcquisitionClass::ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName) != NULL )
	{
		l_hrOk = m_cameraDriverProxy.ReadCameraRegisterBlock(ulAddress, ulCount, paulValue, m_hDigitizer, SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName));
	}
	return l_hrOk;
}

HRESULT SVIntekAcquisitionClass::SetStandardCameraParameter( const SVDeviceParamWrapper& rw )
{
	HRESULT l_hrOk = S_OK;
			/*
			DeviceParamDeviceDigNum         = 100,
			DeviceParamDeviceName           = 101,
			DeviceParamDeviceChannelName    = 102,

			DeviceParamVendorId             = 103,
			DeviceParamVendorName           = 104,
			DeviceParamModelName            = 105,

			// physical camera params
			DeviceParamSerialNumber         = 1000,
			DeviceParamSerialNumberString   = 1001,
			DeviceParamRegisters            = 1006,
			DeviceParamSizeBands            = 1009,
			DeviceParamFormatStrings        = 1011,
			DeviceParamFormat7Support       = 1012,
			DeviceParamOutputStrobePulse    = 1014,
			DeviceParamStrobePulseDelay     = 1015,
			DeviceParamStrobePulseDuration  = 1016,
			DeviceParamCameraTriggerPin     = 1017,
			DeviceParamCameraStrobePin      = 1018,
			DeviceParamTriggerMode          = 1021,
			DeviceParamNumCameraQueuedTriggers = 1022,
			DeviceParamTestPattern          = 1023,
			DeviceParamShutterDelay			= 1024,
			DeviceParamStrobeStartMode      = 1025,
			DeviceParamTriggerQueueMode     = 1026,
			DeviceParamStrobePulseAdvance   = 1027,
			DeviceParamStrobePulseMultiplier= 1033,
			DeviceParamNumCameraBuffers     = 1034,

			// Imaging parameters
			DeviceParamCameraFormats  = 4000,
			DeviceParamLightReference = 4001,
			DeviceParamLut            = 4002,

			// Acquisition I/O parameters
			DeviceParamAcquisitionTriggerSelection = 5000,	// ?? not used
			DeviceParamAcquisitionTriggerType      = 5001,	// external, software, software grab
			DeviceParamAcquisitionTriggerName      = 5002,	// NOT USED YET; will when connecting triggers with acquisition devices

			DeviceParamAcquisitionTriggerEdge      = 5003,  // Internal Parameters Only - Rising or Falling
			DeviceParamAcquisitionStrobeEdge       = 5013,  // Internal Parameters Only - Rising or Falling

			DeviceParamIOStrobeInvert = 5100,
			DeviceParamIOTriggerInvert = 5101,

			DeviceParamCameraTriggerMode = 5200,

			// Intek parameters

			SVIntekParameterDCamStart = 0,
			SVIntekParameterDCamTriggerMode = 0,        // 00
			SVIntekParameterDCamIsoSpeed,               // 01
			SVIntekParameterDCamFormat,                 // 02
			SVIntekParameterDCamMode,                   // 03
			SVIntekParameterDCamRate,                   // 04
			SVIntekParameterDCamYIsRawMode,             // 05
			SVIntekParameterDCamBayerPattern,           // 06
			SVIntekParameterDCamY16InverseOrder,        // 07
			SVIntekParameterDCamFormat7XOffset,         // 08
			SVIntekParameterDCamFormat7YOffset,         // 09
			SVIntekParameterDCamFormat7XSize,           // 10
			SVIntekParameterDCamFormat7YSize,           // 11
			SVIntekParameterDCamFormat7ColorFormat,     // 12
			SVIntekParameterDCamFormat7IsoPacketSize,   // 13
			SVIntekParameterDCamFeatureBrightness,      // 14
			SVIntekParameterDCamFeatureAutoExposure,    // 15
			SVIntekParameterDCamFeatureSharpness,       // 16
			SVIntekParameterDCamFeatureWhiteBalanceU,   // 17
			SVIntekParameterDCamFeatureWhiteBalanceV,   // 18
			SVIntekParameterDCamFeatureHue,             // 19
			SVIntekParameterDCamFeatureSaturation,      // 20
			SVIntekParameterDCamFeatureGamma,           // 21
			SVIntekParameterDCamFeatureShutter,         // 22
			SVIntekParameterDCamFeatureGain,            // 23
			SVIntekParameterDCamFeatureIris,            // 24
			SVIntekParameterDCamFeatureFocus,           // 25
			SVIntekParameterDCamFeatureTemperature,     // 26
			SVIntekParameterDCamFeatureZoom,            // 27
			SVIntekParameterDCamFeaturePan,             // 28
			SVIntekParameterDCamFeatureTilt,            // 29
			SVIntekParameterDCamFeatureOpticalFilter,   // 30
			SVIntekParameterDCamSize,

			SVIntekParameterDCamVendorId = 100,         // 100 - Read Only parameter
			SVIntekParameterDCamVendorName,             // 101 - Read Only parameter
			SVIntekParameterDCamModelName,              // 102 - Read Only parameter
			SVIntekParameterDCamSerialNumberHigh,       // 103 - Read Only parameter
			SVIntekParameterDCamSerialNumberLow,        // 104 - Read Only parameter
			SVIntekParameterDCamSerialNumberString,     // 105 - Read Only parameter
			SVIntekParameterDCamColorFormat,            // 106 - Read Only parameter
			SVIntekParameterDCamIsoPacketSize,          // 107 - Read Only parameter
			SVIntekParameterDCamIsoPacketCount,         // 108 - Read Only parameter

			SVIntekParameterDCamDCamRegisterAddress = 200, // 200
			SVIntekParameterDCamDCamRegisterDataQuad,      // 201
			SVIntekParameterDCamDCamRegisterDataBlock,     // 202
			
			//*/
	if ( SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName) != NULL )
	{
		if ( m_hDigitizer != NULL )
		{
			l_hrOk = m_cameraDriverProxy.SetStandardCameraParameter(rw, m_DeviceParams, m_hDigitizer, SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName));
		}
	}
	if( l_hrOk == S_OK )
	{
		// refresh image info
		GetCameraImageInfo( &msvImageInfo );
	}
	return l_hrOk;
}

bool SVIntekAcquisitionClass::IsOnline() const
{
	return IsStarted() || mbTempOnline;
}

HRESULT SVIntekAcquisitionClass::SetLightReferenceImpl( SVLightReference& rLR )
{
	HRESULT hr = S_OK;

	for ( int iBand = 0; iBand < rLR.NumBands(); iBand++ )
	{
		for ( int iAttribute=0; iAttribute < rLR.Band(iBand).NumAttributes(); iAttribute++)
		{
			SVLightReferenceAttributeInfo& rAttribute = rLR.Band( iBand ).Attribute( iAttribute );
			SVDeviceParamEnum eParam = (SVDeviceParamEnum) rAttribute.dwType;
			SVLongValueDeviceParam* pParam = m_DeviceParams.GetParameter( eParam ).DerivedValue( pParam );
			if ( pParam )
			{
				pParam->lValue = rAttribute.lValue;
				SVDeviceParamWrapper w = pParam;
				InitializeDevice(w);
			}
		}
	}

	return hr;
}

HRESULT SVIntekAcquisitionClass::GetImageInfo(SVImageInfoClass *pImageInfo)
{
	HRESULT hrOk = S_FALSE;
	
	if ( pImageInfo != NULL )
	{
		*pImageInfo = msvImageInfo;
		
		hrOk = S_OK;
	}
	
	return hrOk;
}

HRESULT SVIntekAcquisitionClass::Start()
{
	HRESULT hrOk = S_OK;
	
	if( ! IsStarted() )
	{
		if ( IsDigitizerSubsystemValid() )
		{
			if( ! CameraMatchesCameraFile() )
			{
				hrOk = SVMSG_SVO_20_INCORRECT_CAMERA_FILE;
			}

			if( hrOk == S_OK )
			{
				hrOk = SVAcquisitionClass::Start();
			}
		}
    else
    {
      hrOk = S_FALSE;
    }
	}
	
	return hrOk;
}

HRESULT SVIntekAcquisitionClass::Stop()
{
	HRESULT hrOk = S_OK;

	if ( IsStarted() )
	{
		hrOk = SVAcquisitionClass::Stop();
		
		if( m_pCameraDriver != NULL )
		{
			m_pCameraDriver->GoOffline();
		}
  }
	
	return hrOk;
}

HRESULT SVIntekAcquisitionClass::DestroyLocal()
{
	HRESULT hrOk = S_OK;

	if ( Stop() != S_OK )
	{
		hrOk = S_FALSE;
	}

	if ( DestroyBuffers() != S_OK )
	{
		hrOk = S_FALSE;
	}

	if ( mbIsCorFilesLoaded )
	{
		if ( UnloadFiles() != S_OK )
		{
			hrOk = S_FALSE;
		}
	}

	if ( ResetLightReference() != S_OK )
	{
		hrOk = S_FALSE;
	}

	if ( m_pCameraDriver )
	{
		delete m_pCameraDriver;
		m_pCameraDriver = NULL;
	}

	return hrOk;
}

HRESULT SVIntekAcquisitionClass::CreateLut( const SVLutInfo& info )
{
	SVLutInfo lutinfo( info );

	lutinfo.SetBands(1);
	lutinfo.SetBandSize(1);

	Lut().Create( lutinfo );

	return S_OK;
}

HRESULT SVIntekAcquisitionClass::DestroyLut( )
{
	HRESULT hr = S_OK;

	Lut().Destroy();
	
	return hr;
}

HRESULT SVIntekAcquisitionClass::GetLutImpl( SVLut& lut )
{
	HRESULT hr = S_OK;

	lut = Lut();

	return hr;
}


HRESULT SVIntekAcquisitionClass::SetLutImpl( const SVLut& lut )
{
	HRESULT hr = S_OK;

	SVLutDeviceParam lutparam;
	lutparam.lut = lut;
	SVDeviceParamWrapper w = lutparam;
	if (  CameraMatchesCameraFile() )
	{
		hr = m_pCameraDriver->SetParameter( w );
	}

	return hr;
}

HRESULT SVIntekAcquisitionClass::GetCameraImageInfo(SVImageInfoClass *pImageInfo)
{
	HRESULT hrOk = S_FALSE;

	unsigned long bufWidth = 640;
	unsigned long bufHeight = 480;
	unsigned long uiBandNumber = 1;
	int iFormat = SVImageFormatUnknown;

	if ( IsValidBoard() &&
	     IsDigitizerSubsystemValid() )
	{

		SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->GetBufferHeight( m_hDigitizer, &bufHeight );
		SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->GetBufferWidth( m_hDigitizer, &bufWidth );
		SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->GetBufferFormat( m_hDigitizer, &iFormat );
	}// end if subsystem available
	else
	{
		SVCameraFormatsDeviceParam* pParam;
		pParam = m_DeviceParams.GetParameter( DeviceParamCameraFormats).DerivedValue( pParam );
		if ( pParam )
		{
			SVCameraFormat& rFormat = pParam->options[ pParam->strValue ];
			bufWidth = rFormat.lWidth;
			bufHeight = rFormat.lHeight;
			iFormat = rFormat.eImageType;
		}
	}

	// Band number depends on video type...
	switch( iFormat )
	{
		case SVImageFormatMono8:  // Mono
		{
			uiBandNumber = 1;
			break;
		}
		case SVImageFormatRGB888:	  // RGB
		case SVImageFormatRGB8888:  // RGB
		{
			uiBandNumber = 3;
			break;
		}
	}

	pImageInfo->SetImageProperty( SVImagePropertyFormat, iFormat );
	pImageInfo->SetImageProperty( SVImagePropertyPixelDepth, SV8BitUnsigned );
	pImageInfo->SetImageProperty( SVImagePropertyBandNumber, uiBandNumber );
	pImageInfo->SetImageProperty( SVImagePropertyBandLink, 0 );

	pImageInfo->SetExtentProperty( SVExtentPropertyOutputPositionPoint, 0 );
	pImageInfo->SetExtentProperty( SVExtentPropertyWidth, bufWidth );
	pImageInfo->SetExtentProperty( SVExtentPropertyHeight, bufHeight );

	hrOk = S_OK;

	if ( hrOk != S_OK )
	{
		AfxMessageBox( "Failed to get acquisition source information!", MB_ICONEXCLAMATION );
	}

	return hrOk;
}

HRESULT SVIntekAcquisitionClass::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_OK;

	// SEJ - send notification to start tracking main camera parameters
	if ( IsDigitizerSubsystemValid() )
	{
		_variant_t dummy;
		hr = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->ParameterSetValue(m_hDigitizer, SVIntekBeginTrackParameters, 0, &dummy);
	}
	
	if ( hr == S_OK )
	{
		hr = SVAcquisitionClass::SetDeviceParameters( rDeviceParams );
	}

	if ( hr == S_OK )
	{
		hr = InitializeDevice( rDeviceParams );
	}
	return hr;
}

HRESULT SVIntekAcquisitionClass::IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams )
{
	HRESULT l_hrOk = S_OK;

	if( m_hDigitizer != NULL && rDeviceParams.ParameterExists( DeviceParamVendorId ) )
	{
		CString l_csVenderId;

		_variant_t l_oValue;

		SVDigitizerLoadLibraryClass* l_psvLibrary = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName);

		if( l_psvLibrary != NULL && l_psvLibrary->ParameterGetValue( m_hDigitizer, 100, 0, &l_oValue ) == S_OK )
		{
			l_csVenderId = l_oValue.bstrVal;

			if( l_csVenderId == StringValue( rDeviceParams.Parameter( DeviceParamVendorId ) ) )
			{
				if( rDeviceParams.ParameterExists( DeviceParamModelName ) &&
					l_psvLibrary->ParameterGetValue( m_hDigitizer, 102, 0, &l_oValue ) == S_OK )
				{
					CString sHardwareModel( l_oValue.bstrVal, ::SysStringLen( l_oValue.bstrVal ) );

					CString sModel = StringValue(rDeviceParams.Parameter( DeviceParamModelName )).ToString();

					if (   sHardwareModel != _T("") && sModel != _T("")
						&& sHardwareModel.CompareNoCase(sModel) != 0 )
					{
						l_hrOk = S_FALSE;
					}
				}
			}
			else
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	return l_hrOk;
}

void SVIntekAcquisitionClass::ClearDeviceIdentifier()
{
	m_DeviceParams.SetParameter( DeviceParamSerialNumber, SVi64ValueDeviceParam( 0 ) );
	m_DeviceParams.SetParameter( DeviceParamSerialNumberString, SVStringValueDeviceParam( "" ) );
}

HRESULT SVIntekAcquisitionClass::SingleGrab( SVSmartHandlePointer p_SingleGrabHandle )
{
	HRESULT hr = SVAcquisitionClass::SingleGrab( p_SingleGrabHandle );

	if( hr == S_OK )
	{
		mbTempOnline = true;

		Start();

		if ( m_pCameraDriver->CanSoftwareTrigger() )
		{
			// turn strobe on if enabled (online)
			SVDeviceParamWrapper w = m_DeviceParams.Parameter(DeviceParamOutputStrobePulse);
			InitializeDevice(w);

			// do the grab in another thread
			DWORD dwThreadId=0;
			HANDLE hThread=0;
			hThread = ::CreateThread(NULL, 0, SingleGrabHelperFn, this, 0, &dwThreadId);

			// grab a single frame
			m_pCameraDriver->SoftwareTrigger();
			::WaitForSingleObject(hThread, 2000);

			// turn strobe off (offline)
			InitializeDevice(w);
		}
		else
		{
			// turn strobe on if enabled (online)
			SVDeviceParamWrapper w = m_DeviceParams.Parameter(DeviceParamOutputStrobePulse);
			InitializeDevice(w);

			// do the grab in another thread
			DWORD dwThreadId=0;
			HANDLE hThread=0;
			hThread = ::CreateThread(NULL, 0, SingleGrabHelperFn, this, 0, &dwThreadId);

			// turn off IsoEnable, Set OneShot
			::WaitForSingleObject(hThread, 2000);

			// turn strobe off (offline)
			InitializeDevice(w);
		}
		Stop();

		mbTempOnline = false;
	}
	else
	{
		hr = S_FALSE;
	}

	return hr;
}

HRESULT SVIntekAcquisitionClass::StartDigitizer()
{
	HRESULT hrOk = S_OK;
	
	// SEJ - send notification to end tracking main camera parameters
	_variant_t dummy;

	hrOk = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->ParameterSetValue(m_hDigitizer, SVIntekEndTrackParameters, 0, &dummy);

	if( m_pCameraDriver != NULL )
	{
		m_pCameraDriver->GoOnline();
	}
	else
	{
		Stop();

		hrOk = E_FAIL;
	}

	if( hrOk == S_OK )
	{
		hrOk = SVAcquisitionClass::StartDigitizer();
	}

	return hrOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVIntekAcquisitionClass.cpp_v  $
 * 
 *    Rev 1.1   08 May 2013 16:16:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   29 Apr 2013 14:16:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:05:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   04 Oct 2012 10:45:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   16 Sep 2011 16:00:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects to use new resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   30 Mar 2011 16:04:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   22 Mar 2011 07:54:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   16 Mar 2011 13:35:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   18 Feb 2011 09:58:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove duplicate string class, and fixed string conversion issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   04 Nov 2010 13:39:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   07 Jul 2010 16:10:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to make Single Grab functionality work for IEEE1394 and GigE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   15 Feb 2010 12:29:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   16 Dec 2009 11:08:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   22 Oct 2009 19:30:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   14 Oct 2009 17:00:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Acquisition class to handle go-online functionality correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   03 Sep 2009 10:08:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   14 Aug 2009 14:26:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to merge branch functionality back into main line code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   30 Jul 2009 11:56:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   03 Jun 2009 09:52:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Include file definitions due to changes in SVDeviceParams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   26 Jan 2009 11:21:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  648
 * SCR Title:  Regression Test Image Resolution
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised InitializeDevice to set the image resolution according to the camera device parameters if the digitizer is not present.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   16 Sep 2008 13:40:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetDigitizerSubsystem and GetImageSubsystem  to use device name to support multiple acquisition devices.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   10 Sep 2008 16:48:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Device Name access
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   24 Jul 2007 13:31:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   21 Jun 2007 13:05:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8.1.0   14 Jul 2009 14:00:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   21 Dec 2006 13:48:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  582
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.15
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised InitializeDevice to check for valid digitizer subsystem to correct an access violation when loading a configuration containing cameras that aren't pyhsically connected.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   29 Nov 2006 15:44:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  583
 * SCR Title:  Add Single Grab capability to the Format7 Dialog in SVObserver
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Single Grab functionality
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Nov 2006 14:26:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added DCamAcquisistion Proxy
 * Added DCamDriver Proxy
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   29 Nov 2006 13:58:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  582
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.15
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed unused variables.
 * Revised to signal Start Tracking camera parameters 
 * Revised to signal End Tracking camera parameters
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Aug 2006 15:10:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  567
 * SCR Title:  Update SVObserver to accept Intek driver for D-series SVIMs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated code to not monitor the isValidBoard functionality and used CameraFileMatchCamera functionality to allow camera parameters to be written to the local camera parameters structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Jul 2006 14:29:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  567
 * SCR Title:  Update SVObserver to accept Intek driver for D-series SVIMs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated the read and write camera register methods to or 0xf0f00000 to the address.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Jun 2006 15:10:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  565
 * SCR Title:  Fix 1394 default greyscale camera mode problem with color camera file
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added check for mono product and adjust 1394 camera file reader for color camera files
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Mar 2006 14:37:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  550
 * SCR Title:  Fix Format7 problem when changing width of image using the Intek driver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class to present a variable for debug purposes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Nov 2005 15:54:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
