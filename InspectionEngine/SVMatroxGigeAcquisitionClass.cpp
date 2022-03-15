//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeAcquisitionClass
//* .File Name       : $Workfile:   SVMatroxGigeAcquisitionClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   19 Nov 2014 03:35:08  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "SVMatroxGigeAcquisitionClass.h"
#include "SVMessage/SVMessage.h"
#include "SVGigeCameraFileLibrary/SVGigeCameraFileReader.h"
#include "CameraLibrary/SVBoolValueDeviceParam.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "Definitions/SVGigeEnums.h"
#include "ObjectInterfaces/SVImageBufferHandleInterface.h"
#include "SVDigitizerProcessingClass.h"
#include "SVGigeCameraFileLibrary/SVGigeCameraStruct.h"
#include "SVGigeCameraManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "Triggering/SVDigitizerLoadLibraryClass.h"
#pragma endregion Includes

namespace SvIe
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

#pragma region Constructor
SVMatroxGigeAcquisitionClass::SVMatroxGigeAcquisitionClass( const SVAcquisitionConstructParams& p_rParams )
: SVAcquisitionClass( p_rParams )
{
	m_cameraProxy.SetOwner(this);
}

SVMatroxGigeAcquisitionClass::~SVMatroxGigeAcquisitionClass()
{
	DestroyLocal();
}
#pragma endregion Constructor

bool SVMatroxGigeAcquisitionClass::IsValid() const
{
	bool bOk = SVAcquisitionClass::IsBufferCreated();
	bOk = IsValidBoard() && bOk;
	bOk = SVAcquisitionClass::IsValid() && bOk;
	return bOk;
}

bool SVMatroxGigeAcquisitionClass::IsValidBoard() const
{
	bool bOk = true;
	
	bOk = 0 != m_hDigitizer && bOk;

	if( bOk && m_DeviceParams.ParameterExists( DeviceParamVendorId ) )
	{
		SvTrig::SVDigitizerLoadLibraryClass* pLibrary = m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str());
		if( nullptr != pLibrary)
		{
			_variant_t value = pLibrary->ParameterGetValue(m_hDigitizer, SvDef::SVGigeParameterEnum::SVGigeParameterVendorName);
			std::string vendorId = SvUl::createStdString(value);

			bOk = vendorId == StringValue( m_DeviceParams.Parameter(DeviceParamVendorId));
		}
	}
	
	return bOk;
}

bool SVMatroxGigeAcquisitionClass::IsStarted() const
{
	bool bOk = SVAcquisitionClass::IsStarted();
	return bOk;
}

HRESULT SVMatroxGigeAcquisitionClass::Create()
{
	HRESULT hr = S_OK;

	if (false == IsCreated())
	{
		hr = SVAcquisitionClass::Create();
	}

	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::Destroy()
{
	HRESULT hr = DestroyLocal();

	if ( S_OK != SVAcquisitionClass::Destroy()  )
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
// Called from SVOCameraDlg::OnBtnPropVc
// Called from SVOConfigAssistantDlg::SendAcquisitionDataToConfiguration
// Called from SVOPropertyPageDlg::AdjustCameraImageFormat
HRESULT SVMatroxGigeAcquisitionClass::LoadFiles(const SVFileNameArrayClass& rFiles)
{
	m_DeviceParams.Clear();
	m_sequenceCameraFileIndex = -1;

	HRESULT hr = SVAcquisitionClass::LoadFiles( rFiles );
	if ( S_OK == hr )
	{
		for (int i=0; i < static_cast<int> (mFiles.size()); ++i)
		{
			std::string sFile (mFiles[i].GetFullFileName());
			if (mFiles[i].GetExtension() == ".ogc")
			{
				SVGigeCameraFileReader reader(sFile, IsColor());

				hr = reader.ReadCameraFileImpl(m_CameraFileDeviceParams);

				// Set Gige Feature Overrides
				if (S_OK == hr)
				{
					hr = SetGigeFeatureOverrides(reader.GetFeatureOverrides());
				}

				SVDeviceParamCollection DeviceParams(m_CameraFileDeviceParams);
				const SVGigeCameraStructVector& rCameras = SVGigeCameraManager::Instance().GetCameraOrder();
				for (SVGigeCameraStructVector::const_iterator Iter(rCameras.begin()); rCameras.end() != Iter; ++Iter)
				{
					if (Iter->m_DigitizerID == DigNumber())
					{
						DeviceParams.SetParameter(DeviceParamSerialNumberString, SVStringValueDeviceParam(Iter->m_SerialNum.c_str()));
						SVStringValueDeviceParam IP_Address(DeviceParamIPAddress);
						IP_Address = Iter->m_IPAddress.c_str();
						DeviceParams.SetParameter(DeviceParamIPAddress, IP_Address);
						break;
					}
				}
				SetDeviceParameters(DeviceParams);

				GetCameraImageInfo(msvImageInfo);
			}
			else if (mFiles[i].GetExtension() == ".seq")
			{
				m_sequenceCameraFileIndex = i;
			}
		}
	}

	return hr;
}

// Called from SVCameraPage::LoadSVCameraFiles (SVImageTest)
// Called from SVOConfigAssistantDlg::ItemChanged (SVObserver)
// Called from SVOConfigAssistantDlg::CheckCamera (SVObserver)
HRESULT SVMatroxGigeAcquisitionClass::ReadCameraFile( const std::string& rFilename, SVDeviceParamCollection &rParams )
{
	SVGigeCameraFileReader reader(std::string(rFilename), IsColor());

	HRESULT hr = reader.ReadCameraFileImpl( rParams );
	
	if (S_OK == hr)
	{
		// Set Gige Feature Overrides
		hr = SetGigeFeatureOverrides(reader.GetFeatureOverrides());
	}
	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::CreateLightReference( int iBands )
{
	assert(iBands > 0);

	HRESULT hr = SVAcquisitionClass::CreateLightReference( iBands );
	
	if ( S_OK == hr )
	{
		for ( int i = 0; S_OK == hr && i < iBands; i++)
		{
			if ( S_OK == (hr = CreateLightReferenceBand( i, 0 )) )
			{
			}
		}
		if ( S_OK != hr )
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

HRESULT SVMatroxGigeAcquisitionClass::GetLightReferenceValueStep( unsigned long, unsigned long &rulValue ) const
{
	HRESULT hr = S_OK;
	rulValue = 1;
	return hr;
}

bool SVMatroxGigeAcquisitionClass::CameraMatchesCameraFile()
{
	bool l_bOk = true;
	if (0 != m_hDigitizer && IsDigitizerSubsystemValid())
	{
		l_bOk = m_cameraProxy.CameraMatchesCameraFile(m_CameraFileDeviceParams, m_hDigitizer, m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str()));
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

	if ( S_OK == hr )
	{
		UpdateLightReferenceAttributes( rwParam );

		if (0 != m_hDigitizer && IsDigitizerSubsystemValid())
		{
			hr = m_cameraProxy.InitializeDevice(rwParam, m_DeviceParams, m_hDigitizer, m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str()));
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

	if ( S_OK == hr )
	{
		if (0 != m_hDigitizer && IsDigitizerSubsystemValid())
		{
			hr = m_cameraProxy.InitializeDevice(rDeviceParams, m_hDigitizer, m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str()));
		}

		// Update image info with format setting from device parameters
		GetCameraImageInfo( msvImageInfo );
	}

	if ( m_DeviceParams.ParameterExists( DeviceParamAcquisitionTriggerEdge ) )
	{
		bool l_bRising = true;

		if ( m_DeviceParams.ParameterExists( DeviceParamGigeTriggerEdge ) )
		{
			const SVStringValueDeviceParam* pParam = m_DeviceParams.Parameter( DeviceParamGigeTriggerEdge ).DerivedValue( pParam );
			l_bRising = SvUl::CompareNoCase( pParam->strValue, std::string(_T("rising")) ) == 0;
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
			l_bRising = SvUl::CompareNoCase( pParam->strValue, std::string(_T("rising")) ) == 0;
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
	if( mLightReference.m_aLightReferenceBand.size() == 0 || nullptr == pParam )
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
		if( nullptr !=  pCFParam )
		{
			switch ( pParam->Type() )
			{
			case DeviceParamGamma:
				attribute.strName = pCFParam->VisualName();
				break;

			case DeviceParamBrightness:
				attribute.strName = pCFParam->VisualName();
				break;

			case DeviceParamGain:
				attribute.strName = pCFParam->VisualName();
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
	}

	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::SetStandardCameraParameter( const SVDeviceParamWrapper& rw )
{
	HRESULT hr = S_OK;

	if ( IsDigitizerSubsystemValid() && CameraMatchesCameraFile())
	{
		if ( 0 != m_hDigitizer )
		{
			SvTrig::SVDigitizerLoadLibraryClass* pDigitizer = m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str());

			hr = m_cameraProxy.SetStandardCameraParameter(rw, m_DeviceParams, m_hDigitizer, pDigitizer);

			if ( S_OK == hr )
			{
				hr = InitializeDevice( m_DeviceParams );
			}
		}
	}

	if( S_OK == hr )
	{
		// refresh image info
		GetCameraImageInfo( msvImageInfo );
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
	
	if ( nullptr != pImageInfo )
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
		if ( S_OK != Stop() )
		{
			hr = S_FALSE;
		}
	}

	if ( mbIsBufferCreated )
	{
		if ( S_OK != DestroyBuffers() )
		{
			hr = S_FALSE;
		}
	}

	if ( mbIsCamFilesLoaded )
	{
		if ( S_OK != UnloadFiles() )
		{
			hr = S_FALSE;
		}
	}

	if ( S_OK != ResetLightReference() )
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
		hr = m_cameraProxy.SetDigitizerParameter( w, m_hDigitizer, m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str()) );
	}

	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::GetCameraImageInfo( SVImageInfoClass &pImageInfo )
{
	unsigned long bufWidth = 640;
	unsigned long bufHeight = 480;
	int iFormat = SvDef::SVImageFormatUnknown;

	SVCameraFormatsDeviceParam* pParam;
	pParam = m_DeviceParams.GetParameter( DeviceParamCameraFormats).DerivedValue( pParam );
	if ( pParam )
	{
		SVCameraFormat& rFormat = pParam->options[ pParam->strValue ];
		bufWidth = rFormat.m_lWidth;
		bufHeight = rFormat.m_lHeight;
		iFormat = rFormat.m_eImageType;
	}

	// Band number depends on video type...
	unsigned long uiBandNumber = 1;
	if ( iFormat == SvDef::SVImageFormatBGR888 || iFormat == SvDef::SVImageFormatBGR888X )
	{
		uiBandNumber = 3; // Only RGB uses more than 1 band.
	}

	pImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyFormat, iFormat );
	pImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, SV8BitUnsigned );
	pImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, uiBandNumber );
	pImageInfo.SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, 0 );

	pImageInfo.SetExtentProperty( SvPb::SVExtentPropertyOutputPositionPoint, SVPoint<double>(0.0, 0.0));
	pImageInfo.SetExtentProperty( SvPb::SVExtentPropertyWidth, bufWidth );
	pImageInfo.SetExtentProperty( SvPb::SVExtentPropertyHeight, bufHeight );

	return S_OK;
}

HRESULT SVMatroxGigeAcquisitionClass::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_OK;

	// Send notification to start tracking main camera parameters
	if ( IsDigitizerSubsystemValid() && CameraMatchesCameraFile())
	{
		_variant_t dummy;
		hr = m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str())->ParameterSetValue(m_hDigitizer, SvDef::SVGigeBeginTrackParameters, dummy);
	}
	if ( S_OK == hr )
	{
		hr = SVAcquisitionClass::SetDeviceParameters( rDeviceParams );
	}
	if (S_OK == hr)
	{
		if (-1 != m_sequenceCameraFileIndex && m_sequenceCameraFileIndex < static_cast<int> (mFiles.size()))
		{
			_variant_t fileName;
			fileName.SetString(mFiles[m_sequenceCameraFileIndex].GetFullFileName().c_str());
			hr = m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str())->ParameterSetValue(m_hDigitizer, SvDef::SVCameraSequenceFile, fileName);
		}
	}
	if( S_OK == hr )
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

HRESULT SVMatroxGigeAcquisitionClass::SingleGrab(SvOi::SVImageBufferHandlePtr p_SingleGrabHandle)
{
	HRESULT hr = SVAcquisitionClass::SingleGrab( p_SingleGrabHandle );

	if( S_OK == hr )
	{
		SvTrig::SVDigitizerLoadLibraryClass* pDigitizer = m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str());
		if (nullptr != pDigitizer)
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
				hThread = ::CreateThread(nullptr, 0, SingleGrabHelperFn, this, 0, &dwThreadId);

				// grab a single frame
				m_cameraProxy.SoftwareTrigger(m_hDigitizer, pDigitizer);
				::WaitForSingleObject(hThread, 2000);

				::CloseHandle(hThread);
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
				hThread = ::CreateThread(nullptr, 0, SingleGrabHelperFn, this, 0, &dwThreadId);

				// 
				::WaitForSingleObject(hThread, 2000);

				::CloseHandle(hThread);
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
	return m_cameraProxy.IsValidCameraFileParameters( rDeviceParams, m_hDigitizer, m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str()));
}

HRESULT SVMatroxGigeAcquisitionClass::SetGigeFeatureOverrides(const std::string& featureOverrides)
{
	HRESULT hr = S_OK;
	
	if (IsDigitizerSubsystemValid() && CameraMatchesCameraFile())
	{
		if (0 != m_hDigitizer)
		{
			SvTrig::SVDigitizerLoadLibraryClass* pDigitizer = m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str());

			hr = m_cameraProxy.SetGigeFeatureOverrides(featureOverrides, m_hDigitizer, pDigitizer);
		}
	}
	return hr;
}

HRESULT SVMatroxGigeAcquisitionClass::StartDigitizer()
{
	HRESULT hr = S_OK;

	SvTrig::SVDigitizerLoadLibraryClass* pDigitizer = m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str());
	if (pDigitizer)
	{
		// Send notification to end tracking main camera parameters
		_variant_t dummy;
		hr = m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str())->ParameterSetValue(m_hDigitizer, SvDef::SVGigeEndTrackParameters, dummy);

		if( S_OK == hr )
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
} //namespace SvIe
