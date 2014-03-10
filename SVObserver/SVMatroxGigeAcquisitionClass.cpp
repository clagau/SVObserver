//******************************************************************************
//* COPYRIGHT (c) 2009 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeAcquisitionClass
//* .File Name       : $Workfile:   SVMatroxGigeAcquisitionClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   07 Mar 2014 18:19:00  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVMatroxGigeAcquisitionClass.h"
#include "SVMessage/SVMessage.h"
#include "SVGigeCameraFileLibrary/SVGigeCameraFileReader.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVDataManagerLibrary/DataManager.h"
#include "SVGigeCameraParametersLibrary/SVGigeEnums.h"
#include "SVMatroxGigeCameraProxy.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageObjectClass.h"
#include "SVObserver.h"
#include "SVDigitizerProcessingClass.h"
#include "SVImageProcessingClass.h"
#include "SVOMFCLibrary/SVOINIClass.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

#pragma region Constructor
SVMatroxGigeAcquisitionClass::SVMatroxGigeAcquisitionClass( const SVAcquisitionConstructParams& p_rParams )
: SVAcquisitionClass( p_rParams )
{
	mbIsCamFilesLoaded = false;
	mbIsStarted = false;
	mbTempOnline = false;
	mlLastIndex = -1;

	mlHandleIndexCount = -1;

	m_cameraProxy.SetOwner(this);
}

SVMatroxGigeAcquisitionClass::~SVMatroxGigeAcquisitionClass()
{
	DestroyLocal();
}
#pragma endregion Constructor

bool SVMatroxGigeAcquisitionClass::IsValid() const
{
	bool bOk = FALSE;
	bOk = SVAcquisitionClass::IsBufferCreated();

	bOk = IsValidBoard() && bOk;

	bOk = SVAcquisitionClass::IsValid() && bOk;
	
	return bOk;
}

bool SVMatroxGigeAcquisitionClass::IsValidBoard() const
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

bool SVMatroxGigeAcquisitionClass::IsStarted() const
{
	bool bOk = SVAcquisitionClass::IsStarted();
	return bOk;
}

HRESULT SVMatroxGigeAcquisitionClass::Create( unsigned long ulSize )
{
	HRESULT hr = S_OK;

	if ( ! mbIsCreated )
	{
		hr = SVAcquisitionClass::Create( ulSize );
	}

	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::Destroy()
{
	HRESULT hr = S_FALSE;

	hr = DestroyLocal();

	if ( SVAcquisitionClass::Destroy() != S_OK )
	{
		hr = S_FALSE;
	}
	
	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::GetFileNameArraySize( long &rlSize ) const
{
	HRESULT hr = SVAcquisitionClass::GetFileNameArraySize(rlSize);
	return hr;
}

// Called from SVConfigurationObject::LoadConfiguration
// Called from CSVOCameraDlg::OnBtnPropVc
// Called from CSVOConfigAssistantDlg::SendAcquisitionDataToConfiguration
// Called from CSVOPropertyPageDlg::Format7Dlg
HRESULT SVMatroxGigeAcquisitionClass::LoadFiles(SVFileNameArrayClass& rFiles)
{
	m_DeviceParams.Clear();

	HRESULT hr = SVAcquisitionClass::LoadFiles( rFiles );
	if ( hr == S_OK )
	{
		ASSERT( mFiles.GetSize() == 1 );	// only one file
		if ( mFiles.GetSize() == 1 )
		{
			CString sFile = mFiles[0].GetFullFileName();

			SVGigeCameraFileInfoStruct info;
			info.sFilename = sFile;
			info.bColorSystem = TheSVObserverApp.IsColorSVIM() ? true : false;

			hr = SVGigeCameraProxy::ReadCameraFileImpl( info, m_CameraFileDeviceParams );

			if ( hr == SVMSG_INCORRECT_CHECKSUM )
			{
			}
			// Set Gige Feature Overrides
			hr = SetGigeFeatureOverrides(info.sFeatureOverrides);
	
			SetDeviceParameters( m_CameraFileDeviceParams );
			GetCameraImageInfo( &msvImageInfo );
		}
	}

	return hr;
}

// Called from SVCameraPage::LoadSVCameraFiles (SVImageTest)
// Called from CSVOConfigAssistantDlg::ItemChanged (SVObserver)
// Called from CSVOConfigAssistantDlg::CheckCamera (SVObserver)
HRESULT SVMatroxGigeAcquisitionClass::ReadCameraFile( const CString& sFilename, SVDeviceParamCollection& rParams )
{
	SVGigeCameraFileInfoStruct info;
	info.sFilename = sFilename;
	info.bColorSystem = TheSVObserverApp.IsColorSVIM() ? true : false;
	HRESULT hr = SVGigeCameraProxy::ReadCameraFileImpl( info, rParams );
	
	if (hr == S_OK)
	{
		// Set Gige Feature Overrides
		hr = SetGigeFeatureOverrides(info.sFeatureOverrides);
	}
	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::CreateLightReference(int iBands, int iBrightness, int iContrast)
{
	ASSERT(iBands > 0);

	HRESULT hr = SVAcquisitionClass::CreateLightReference( iBands, iBrightness, iContrast );
	
	if ( hr == S_OK )
	{
		DWORD dwDefaultBrightness = 0;	// not used
		for ( int i = 0; hr == S_OK && i < iBands; i++)
		{
			if ( (hr = CreateLightReferenceBand( i, 0 )) == S_OK )
			{
			}
		}
		if ( hr != S_OK )
		{
			mLightReference.DeleteAll();
		}
	}
	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::GetMaxLightReferenceValue( unsigned long ulType, long &rlValue ) const
{
	HRESULT hr = S_OK;

	SVDeviceParamEnum eParam = static_cast< SVDeviceParamEnum >( ulType );
	const SVLongValueDeviceParam* pParam = m_CameraFileDeviceParams.Parameter( eParam ).DerivedValue( pParam );
	if ( pParam )
	{
		rlValue = pParam->info.max;
	}
	else
	{
		rlValue = 0;
		hr = S_FALSE;
	}

	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::GetMinLightReferenceValue( unsigned long ulType, long &rlValue ) const
{
	HRESULT hr = S_OK;

	const SVDeviceParamEnum eParam = static_cast< SVDeviceParamEnum >( ulType );
	const SVLongValueDeviceParam* pParam = m_CameraFileDeviceParams.Parameter( eParam ).DerivedValue( pParam );
	if ( pParam )
	{
		rlValue = pParam->info.min;
	}
	else
	{
		rlValue = 0;
		hr = S_FALSE;
	}

	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::GetLightReferenceValueStep( unsigned long ulType, unsigned long &rulValue ) const
{
	HRESULT hr = S_OK;
	rulValue = 1;
	return hr;
}

bool SVMatroxGigeAcquisitionClass::CameraMatchesCameraFile()
{
	bool l_bOk = true;
	if (m_hDigitizer != NULL && IsDigitizerSubsystemValid())
	{
		l_bOk = m_cameraProxy.CameraMatchesCameraFile(m_CameraFileDeviceParams, m_hDigitizer, SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName));
	}
	return l_bOk;
}

HRESULT SVMatroxGigeAcquisitionClass::InitializeDevice( const SVDeviceParamWrapper& rwParam )
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
			hr = m_cameraProxy.InitializeDevice(rwParam, m_DeviceParams, m_hDigitizer, SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName));
		}
	}

	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::InitializeDevice( const SVDeviceParamCollection& rDeviceParams )
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
			hr = m_cameraProxy.InitializeDevice(rDeviceParams, m_hDigitizer, SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName));
		}

		// Update image info with format setting from device parameters
		GetCameraImageInfo( &msvImageInfo );
	}

	if ( m_DeviceParams.ParameterExists( DeviceParamAcquisitionTriggerEdge ) )
	{
		bool l_bRising = true;

		if ( m_DeviceParams.ParameterExists( DeviceParamGigeTriggerEdge ) )
		{
			const SVStringValueDeviceParam* pParam = m_DeviceParams.Parameter( DeviceParamGigeTriggerEdge ).DerivedValue( pParam );
			CString l_Temp( _T( "rising" ) );
			l_bRising = l_Temp.CompareNoCase( pParam->strValue.c_str() ) == 0;
		}

		if ( m_DeviceParams.ParameterExists( DeviceParamIOTriggerInvert ) )
		{
			const SVBoolValueDeviceParam* pParam = m_DeviceParams.Parameter( DeviceParamIOTriggerInvert ).DerivedValue( pParam );
			l_bRising = ( pParam->bValue ) ? ! l_bRising : l_bRising;
		}

		SVBoolValueDeviceParam* pEdge = m_DeviceParams.GetParameter( DeviceParamAcquisitionTriggerEdge ).DerivedValue(pEdge);

		pEdge->bValue = l_bRising;
	}

	if ( m_DeviceParams.ParameterExists( DeviceParamAcquisitionStrobeEdge ) )
	{
		bool l_bRising = true;

		if ( m_DeviceParams.ParameterExists( DeviceParamGigeStrobeEdge ) )
		{
			const SVStringValueDeviceParam* pParam = m_DeviceParams.Parameter( DeviceParamGigeStrobeEdge ).DerivedValue( pParam );
			CString l_Temp( _T( "rising" ) );
			l_bRising = l_Temp.CompareNoCase( pParam->strValue.c_str() ) == 0;
		}

		if ( m_DeviceParams.ParameterExists( DeviceParamIOStrobeInvert ) )
		{
			const SVBoolValueDeviceParam* pParam = m_DeviceParams.Parameter( DeviceParamIOStrobeInvert ).DerivedValue( pParam );
			l_bRising = ( pParam->bValue ) ? ! l_bRising : l_bRising;
		}

		SVBoolValueDeviceParam* pEdge = m_DeviceParams.GetParameter( DeviceParamAcquisitionStrobeEdge ).DerivedValue(pEdge);

		pEdge->bValue = l_bRising;
	}

	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::UpdateLightReferenceAttributes( const SVDeviceParamCollection& rParams )
{
	SVDeviceParamMap::const_iterator iter;
	for ( iter = rParams.mapParameters.begin(); iter != rParams.mapParameters.end(); ++iter )
	{
		const SVDeviceParamWrapper& rw = iter->second;
		UpdateLightReferenceAttributes( rw );
	}
	return S_OK;
}

HRESULT SVMatroxGigeAcquisitionClass::UpdateLightReferenceAttributes( const SVDeviceParamWrapper& rwParam )
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

HRESULT SVMatroxGigeAcquisitionClass::UpdateLightReferenceAttributes( int iBand, const SVLongValueDeviceParam* pParam )
{
	HRESULT hr = S_FALSE;
	if( mLightReference.m_aLightReferenceBand.size() == 0 )
	{
		return hr;
	}
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
	}

	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::SetStandardCameraParameter( const SVDeviceParamWrapper& rw )
{
	HRESULT hr = S_OK;
	
	if ( IsDigitizerSubsystemValid() )
	{
		if ( m_hDigitizer != NULL )
		{
			SVDigitizerLoadLibraryClass* pDigitizer = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName);

			hr = m_cameraProxy.SetStandardCameraParameter(rw, m_DeviceParams, m_hDigitizer, pDigitizer);
			
			if ( hr == S_OK )
			{
				hr = InitializeDevice( m_DeviceParams );
			}
		}
	}
	if( hr == S_OK )
	{
		// refresh image info
		GetCameraImageInfo( &msvImageInfo );
	}
	return hr;
}

bool SVMatroxGigeAcquisitionClass::IsOnline() const
{
	return IsStarted() && mbIsStarted || mbTempOnline;
}

HRESULT SVMatroxGigeAcquisitionClass::SetLightReferenceImpl( SVLightReference& rLR )
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

HRESULT SVMatroxGigeAcquisitionClass::GetImageInfo(SVImageInfoClass *pImageInfo) const
{
	HRESULT hr = S_FALSE;
	
	if ( pImageInfo != NULL )
	{
		*pImageInfo = msvImageInfo;
		
		hr = S_OK;
	}
	
	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::DestroyLocal()
{
	HRESULT hr = S_OK;

	if ( mbIsStarted )
	{
		if ( Stop() != S_OK )
		{
			hr = S_FALSE;
		}
	}

	if ( mbIsBufferCreated )
	{
		if ( DestroyBuffers() != S_OK )
		{
			hr = S_FALSE;
		}
	}

	if ( mbIsCamFilesLoaded )
	{
		if ( UnloadFiles() != S_OK )
		{
			hr = S_FALSE;
		}
	}

	if ( ResetLightReference() != S_OK )
	{
		hr = S_FALSE;
	}

	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::CreateLut( const SVLutInfo& info )
{
	SVLutInfo lutinfo( info );

	// temp
	lutinfo.SetBands(1);
	lutinfo.SetBandSize(1);

	Lut().Create( lutinfo );

	return S_OK;
}

HRESULT SVMatroxGigeAcquisitionClass::DestroyLut( )
{
	HRESULT hr = S_OK;

	Lut().Destroy();
		
	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::GetLutImpl( SVLut& lut )
{
	HRESULT hr = S_OK;

	lut = Lut();

	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::SetLutImpl( const SVLut& lut )
{
	HRESULT hr = S_OK;

	SVLutDeviceParam lutparam;
	lutparam.lut = lut;
	SVDeviceParamWrapper w = lutparam;
	if (  CameraMatchesCameraFile() )
	{
		hr = m_cameraProxy.SetDigitizerParameter( w, m_hDigitizer, SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName) );
	}

	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::GetCameraImageInfo( SVImageInfoClass *pImageInfo )
{
	HRESULT hr = S_FALSE;

	unsigned long bufWidth = 640;
	unsigned long bufHeight = 480;
	unsigned long uiBandNumber = 1;
	int iFormat = SVImageFormatUnknown;

	if ( IsValidBoard() && IsDigitizerSubsystemValid() )
	{
		SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->GetBufferHeight( m_hDigitizer, &bufHeight );
		SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->GetBufferWidth( m_hDigitizer, &bufWidth );
		SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->GetBufferFormat( m_hDigitizer, &iFormat );
	}
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
		case SVImageFormatRGB888:	// RGB
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

	hr = S_OK;

	if ( hr != S_OK )
	{
		AfxMessageBox( "Failed to get acquisition source information!", MB_ICONEXCLAMATION );
	}

	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_OK;

	// SEJ - send notification to start tracking main camera parameters
	if ( IsDigitizerSubsystemValid() )
	{
		_variant_t dummy;
		hr = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->ParameterSetValue(m_hDigitizer, SVGigeBeginTrackParameters, 0, &dummy);
	}
	if ( hr == S_OK )
	{
		hr = SVAcquisitionClass::SetDeviceParameters( rDeviceParams );
	}
	if( hr == S_OK )
	{
		hr = InitializeDevice( rDeviceParams );
	}

	return hr;
}

void SVMatroxGigeAcquisitionClass::ClearDeviceIdentifier()
{
	m_DeviceParams.SetParameter( DeviceParamSerialNumber, SVStringValueDeviceParam( "" ) );
	m_DeviceParams.SetParameter( DeviceParamSerialNumberString, SVStringValueDeviceParam( "" ) );
}

HRESULT SVMatroxGigeAcquisitionClass::SingleGrab( SVSmartHandlePointer p_SingleGrabHandle )
{
	HRESULT hr = SVAcquisitionClass::SingleGrab( p_SingleGrabHandle );

	if( hr == S_OK )
	{
		SVDigitizerLoadLibraryClass* pDigitizer = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName);
		if (pDigitizer != NULL)
		{
			mbTempOnline = true;

			// Set the Trigger Source (Get the current setting so we can restore it)?
			m_cameraProxy.EnableSoftwareTrigger(m_hDigitizer, pDigitizer);

			Start();

			if ( m_cameraProxy.CanSoftwareTrigger() )
			{
				// turn strobe on if enabled (online)
				SVDeviceParamWrapper w = m_DeviceParams.Parameter(DeviceParamOutputStrobePulse);
				InitializeDevice(w);

				// do the grab in another thread
				DWORD dwThreadId = 0;
				HANDLE hThread = 0;
				hThread = ::CreateThread(NULL, 0, SingleGrabHelperFn, this, 0, &dwThreadId);

				// grab a single frame
				m_cameraProxy.SoftwareTrigger(m_hDigitizer, pDigitizer);
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
				DWORD dwThreadId = 0;
				HANDLE hThread = 0;
				hThread = ::CreateThread(NULL, 0, SingleGrabHelperFn, this, 0, &dwThreadId);

				// 
				::WaitForSingleObject(hThread, 2000);

				// turn strobe off (offline)
				InitializeDevice(w);
			}

			Stop();

			mbTempOnline = false;
		}
		else
		{
			hr = E_FAIL;
		}
	}
	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams )
{
	return m_cameraProxy.IsValidCameraFileParameters( rDeviceParams, m_hDigitizer, SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName));
}

HRESULT SVMatroxGigeAcquisitionClass::SetGigeFeatureOverrides(const SVString& featureOverrides)
{
	HRESULT hr = S_FALSE;
	
	if (IsDigitizerSubsystemValid())
	{
		if (m_hDigitizer != NULL)
		{
			SVDigitizerLoadLibraryClass* pDigitizer = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName);

			hr = m_cameraProxy.SetGigeFeatureOverrides(featureOverrides, m_hDigitizer, pDigitizer);
		}
	}
	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::StartDigitizer()
{
	HRESULT hr = S_OK;
	
	// SEJ - send notification to end tracking main camera parameters
	_variant_t dummy;

	SVDigitizerLoadLibraryClass* pDigitizer = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName);
	if (pDigitizer)
	{
		hr = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->ParameterSetValue(m_hDigitizer, SVGigeEndTrackParameters, 0, &dummy);

		if( hr == S_OK )
		{
			hr = SVAcquisitionClass::StartDigitizer();
		}
	}
	else
	{
		hr = S_FALSE;
	}
	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVMatroxGigeAcquisitionClass.cpp_v  $
 * 
 *    Rev 1.4   07 Mar 2014 18:19:00   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 *   Added DEBUG_NEW.
 *   Made methods const.
 *   Removed methods that did not change base class functionality.
 *   Moved mbIsBufferCreated to SVAcquisitionClass.
 *   Changed IsValid to call SVAcquisitionClass::IsBufferCreated.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   28 Feb 2014 09:20:12   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  890
 * SCR Title:  Fix SVObserver Memory Leaks
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Prevent crash in updateLightReferenceAttributes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   05 Jul 2013 09:11:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 020.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   03 Jun 2013 09:40:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  840
 * SCR Title:  Fix the GigE camera to camera file validation
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added override Is ValidCameraFileParameters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:16:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   29 Apr 2013 14:21:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to removed modeless dialog include.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   29 Apr 2013 14:19:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to remove include to modeless debug dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 12:33:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   04 Oct 2012 10:45:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   29 Jun 2012 16:52:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add the needed functionality to track parameter information and reset the camera to the desired parameters on reconnect.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   16 Sep 2011 16:06:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated object to use the new resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   22 Mar 2011 07:57:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   16 Mar 2011 13:43:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   10 Feb 2011 15:00:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   04 Nov 2010 13:43:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   30 Jul 2010 13:51:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to foce the trigger edge parameter and strobe edge parameter to be updated correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Jul 2010 16:10:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to make Single Grab functionality work for IEEE1394 and GigE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   02 Jun 2010 08:27:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to allow for GigE Light Reference and LUT to work.  Added functionality to read GigE camera database from camera file to allow for changes to camera functionality to be externalized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Feb 2010 12:37:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Dec 2009 12:00:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Oct 2009 19:32:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Sep 2009 10:29:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 12:04:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:50:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
