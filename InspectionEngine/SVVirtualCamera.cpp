//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVirtualCamera
//* .File Name       : $Workfile:   SVVirtualCamera.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   17 Jul 2014 20:51:54  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "SVVirtualCamera.h"
#include "SVDigitizerProcessingClass.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "Definitions/GlobalConst.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVImageLibrary/SVLut.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

namespace SvIe
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

HRESULT CALLBACK SVVirtualCamera::SVImageCallback( void *pvOwner, void *pvCaller, void *pvResponse )
{
	HRESULT hrOk = S_OK;

	SVVirtualCamera* pCamera = reinterpret_cast< SVVirtualCamera* >( pvCaller );
	SVODataResponseClass* pResponse = reinterpret_cast< SVODataResponseClass* >( pvResponse );

	//Only do an assert check so that in release mode no check is made
	assert( nullptr != pCamera && nullptr != pResponse );

	pCamera->FinishProcess( pResponse );

	return hrOk;
}

SVVirtualCamera::SVVirtualCamera( LPCSTR ObjectName ) : SVObjectClass( ObjectName )
 ,mlBandLink( 0 )
 ,m_bFileAcquisition( false )
 ,m_IsColor( false )
 ,m_bImageSizeEditModeFileBased( true )
 ,m_imageLoadingMode( 0 )
 ,m_CameraID( 0 )
{
	m_imageSize.cx = 0;
	m_imageSize.cy = 0;
}

SVVirtualCamera::SVVirtualCamera( SVObjectClass* POwner, int StringResourceID ) : SVObjectClass( POwner, StringResourceID )
 ,mlBandLink( 0 )
 ,m_bFileAcquisition( false )
 ,m_IsColor( false )
 ,m_bImageSizeEditModeFileBased( true )
 ,m_imageLoadingMode( 0 )
 ,m_CameraID( 0 )
{
	m_imageSize.cx = 0;
	m_imageSize.cy = 0;
}

SVVirtualCamera::~SVVirtualCamera()
{
	DestroyLocal();
}

bool SVVirtualCamera::GetImageInfo(SVImageInfoClass *pImageInfo)
{
	bool bOk = false;

	if(nullptr != mpsvDevice)
	{
		bOk = S_OK == mpsvDevice->GetImageInfo( pImageInfo );
		if ( nullptr != pImageInfo )
		{
			bOk = S_OK == pImageInfo->SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, mlBandLink );
		}
	}

	return bOk;
}

HRESULT SVVirtualCamera::RefreshObject( const SVObjectClass* const pSender, RefreshObjectType Type )
{
	HRESULT Result = S_OK;
	
	//The camera parameters only need to be refreshed when type is PreRefresh
	if( PreRefresh == Type )
	{
		Result = updateCameraParameters();
	}
	
	return Result;
}

bool SVVirtualCamera::Create( LPCTSTR DeviceName )
{
	bool bOk = true;

	if(nullptr != mpsvDevice )
	{
		bOk = Destroy();
	}

	SVDigitizerProcessingClass::Instance().SetDigitizerColor( DeviceName, m_IsColor );
	mpsvDevice = SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( DeviceName );

	bOk = (nullptr != mpsvDevice) && bOk;

	if ( bOk )
	{
		SetBandLink( mpsvDevice->Channel() );

		m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVVirtualCameraType;

		bOk = m_CallbackList.Create() && bOk;

		createCameraParameters();
		updateCameraParameters();
	}

	return bOk;
}

bool SVVirtualCamera::Destroy()
{
	bool bOk = DestroyLocal();

	UnregisterTriggerRelay();

	return bOk;
}// end Destroy	

HRESULT SVVirtualCamera::GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index ) const
{
	rpObject = nullptr;
	HRESULT l_Status = SVObjectClass::GetChildObject( rpObject, rNameInfo, Index );

	if( S_OK != l_Status )
	{
		if( static_cast<const size_t> (Index) < rNameInfo.m_NameArray.size() && rNameInfo.m_NameArray[Index] == GetName() )
		{
			SvVol::BasicValueObjectPtr pBasicValueObject = m_CameraValues.getValueObject( rNameInfo.GetObjectArrayName( Index + 1 ).c_str() );

			if(nullptr != pBasicValueObject)
			{
				rpObject = dynamic_cast<SVObjectClass*> (pBasicValueObject.get());
			}
		}
	}

	return l_Status;
}

bool SVVirtualCamera::CanGoOnline() const
{
	return mpsvDevice && mpsvDevice->IsValid();
}// end CanGoOnline

bool SVVirtualCamera::GoOnline()
{
	return (nullptr != mpsvDevice) && ( S_OK == mpsvDevice->Start() );
}

bool SVVirtualCamera::GoOffline()
{
	return (nullptr != mpsvDevice) && ( S_OK == mpsvDevice->Stop() );
}

bool SVVirtualCamera::RegisterFinishProcess( void *pvOwner, LPSVFINISHPROC pCallback )
{
	bool bOk = false;

	if ( m_CallbackList.IsCreated() )
	{
		if ( m_CallbackList.Lock() )
		{
			long lSize = 0;

			m_CallbackList.GetSize( lSize );
			long l = 0;
			for ( ; l < lSize; l++ )
			{
				SVOCallbackClassPtr pData;

				m_CallbackList.GetAt( l, &pData );
				if (nullptr != pData )
				{
					if ( pData->mpCallback == pCallback &&
					     pData->mpvOwner == pvOwner &&
					     pData->mpvCaller == this )
					{
						break;
					}
				}
				else
				{
					break;
				}
			}

			if ( lSize <= l )
			{
				SVOCallbackClassPtr pData{ new SVOCallbackClass };

				if (nullptr != pData )
				{
					pData->mpCallback = pCallback;
					pData->mpvOwner = pvOwner;
					pData->mpvCaller = this;

					if ( m_CallbackList.AddTail( pData ) )
					{
						bOk = true;
					}

					if( bOk )
					{
						bOk = S_OK == mpsvDevice->RegisterCallback( SVVirtualCamera::SVImageCallback, pData->mpvOwner, pData->mpvCaller );
					}// end if

				}
			}

			m_CallbackList.Unlock();
		}
	}

	return bOk;
}

bool SVVirtualCamera::UnregisterFinishProcess(void *pvOwner, LPSVFINISHPROC pCallback)
{
	bool bOk = false;

	if ( m_CallbackList.IsCreated() )
	{
		if ( m_CallbackList.Lock() )
		{
			long lSize = 0;

			m_CallbackList.GetSize( lSize );

			for ( long l = lSize - 1; -1 < l; l-- )
			{
				SVOCallbackClassPtr pData;
				
				m_CallbackList.GetAt( l, &pData );

				if (nullptr != pData)
				{
					if ( pData->mpCallback == pCallback &&
					     pData->mpvOwner == pvOwner &&
						 pData->mpvCaller == this )
					{
						if ( m_CallbackList.RemoveAt( l ) )
						{
							bOk = true;
						}

						if ( bOk && nullptr != mpsvDevice )
						{
							bOk = S_OK == mpsvDevice->UnregisterCallback( SVVirtualCamera::SVImageCallback, pData->mpvOwner, pData->mpvCaller );
						}	

						break;
					}
				}
				else
				{
					m_CallbackList.RemoveAt( l );
				}
			}

			m_CallbackList.Unlock();
		}
	}

	return bOk;
}

void SVVirtualCamera::FinishProcess( SVODataResponseClass *pResponse )
{
	if ( m_CallbackList.IsCreated() )
	{
		if ( pResponse )
		{		
			long lSize = 0;

			m_CallbackList.GetSize( lSize );

			for ( long l = 0; l < lSize; l++ )
			{
				SVOCallbackClassPtr pData;

				m_CallbackList.GetAt( l, &pData );

				if (nullptr != pData)
				{
					(pData->mpCallback)( pData->mpvOwner, pData->mpvCaller, (void*) pResponse );
				}
			}
		}
	}
}

bool SVVirtualCamera::DestroyLocal()
{
	bool bOk = GoOffline();

	if ( m_CallbackList.IsCreated() )
	{
		if ( m_CallbackList.Lock() )
		{
			long lSize = 0;

			m_CallbackList.GetSize( lSize );

			for ( long l = lSize - 1; -1 < l; l-- )
			{
				SVOCallbackClassPtr pData;
				
				m_CallbackList.GetAt( l, &pData );

				if (nullptr != pData && nullptr != mpsvDevice )
				{
					bOk &= S_OK == mpsvDevice->UnregisterCallback( SVVirtualCamera::SVImageCallback, 
					                                       pData->mpvOwner, 
					                                       pData->mpvCaller );
				}

				if ( ! m_CallbackList.RemoveAt( l ) )
				{
					bOk = false;
				}
			}

			m_CallbackList.Unlock();
		}

		m_CallbackList.Destroy();
	}

	mpsvDevice = nullptr;
	mlBandLink = 0;

	return bOk;
}

HRESULT SVVirtualCamera::GetBand(int& riBand) const
{
	riBand = mlBandLink;
	return S_OK;
}

HRESULT SVVirtualCamera::GetBandSize(int& riBandSize) const
{
	HRESULT hr = S_OK;
	
	if (nullptr != mpsvDevice )
	{
		riBandSize = mpsvDevice->BandSize();
	}
	else
	{
		hr = S_FALSE;
	}
	
	return hr;
}

HRESULT SVVirtualCamera::SetLightReference( SVLightReference& rArray )
{
	HRESULT hr=S_FALSE;
	
	if (GetAcquisitionDevice())
	{
		int iBandSize=0;
		GetBandSize(iBandSize);
		if (iBandSize == 1)
		{
			int iBand=-1;
			GetBand(iBand);
			hr = GetAcquisitionDevice()->SetLightReference(rArray, iBand);
		}
		else
		{
			hr = GetAcquisitionDevice()->SetLightReference(rArray);
		}
		updateCameraParameters();
	}
	
	return hr;
}

HRESULT SVVirtualCamera::GetLightReference( SVLightReference& rArray ) const
{
	HRESULT hr = S_FALSE;
	
	if (GetAcquisitionDevice())
	{
		hr = GetAcquisitionDevice()->GetLightReference(rArray);
	}
	return hr;
}

HRESULT SVVirtualCamera::SetLut( const SVLut& lut )
{
	HRESULT hr=S_FALSE;
	
	if (GetAcquisitionDevice())
	{
		int iBandSize=0;
		GetBandSize(iBandSize);
		if (iBandSize == 1)
		{
			int iBand=-1;
			GetBand(iBand);
			hr = GetAcquisitionDevice()->SetLut(lut, iBand);
		}
		else
		{
			hr = GetAcquisitionDevice()->SetLut( lut );
		}
	}
	
	return hr;
}

HRESULT SVVirtualCamera::GetLut( SVLut& lut ) const
{
	HRESULT hr=S_FALSE;
	
	if (GetAcquisitionDevice())
	{
		int iBandSize=0;
		GetBandSize(iBandSize);
		if (iBandSize == 1)
		{
			int iBand=-1;
			GetBand(iBand);
			hr = GetAcquisitionDevice()->GetLut( lut );
		}
		else
		{
			hr = GetAcquisitionDevice()->GetLut( lut );
		}
	}
	
	return hr;
}

SvTrc::IImagePtr SVVirtualCamera::ReserveNextImageHandle(  ) const
{
	if(nullptr != mpsvDevice)
	{
		return mpsvDevice->GetNextBuffer();
	}

	return nullptr;
}

bool SVVirtualCamera::IsFileAcquisition() const
{
	return m_bFileAcquisition;
}

void SVVirtualCamera::SetFileAcquisitionMode(bool bFileAcquisition)
{
	m_bFileAcquisition = bFileAcquisition;
}

LPCTSTR SVVirtualCamera::GetImageFilename() const
{
	return m_imageFilename.c_str();
}

void SVVirtualCamera::SetImageFilename(const std::string& filename)
{
	m_imageFilename = filename;
}

LPCTSTR SVVirtualCamera::GetImageDirectoryName() const
{
	return m_imageDirectoryName.c_str();
}

void SVVirtualCamera::SetImageDirectoryName(const std::string& directoryName)
{
	m_imageDirectoryName = directoryName;
}

long SVVirtualCamera::GetFileLoadingMode() const
{
	return m_imageLoadingMode;
}

void SVVirtualCamera::SetFileLoadingMode(long mode)
{
	m_imageLoadingMode = mode;
}

bool SVVirtualCamera::IsFileImageSizeEditModeFileBased() const
{
	return m_bImageSizeEditModeFileBased;
}

void SVVirtualCamera::SetFileImageSizeEditModeFileBased(bool m_bFileBased)
{
	m_bImageSizeEditModeFileBased = m_bFileBased;
}

const SIZE& SVVirtualCamera::GetFileImageSize() const
{
	return m_imageSize;
}

void SVVirtualCamera::SetFileImageSize(const SIZE& size)
{
	m_imageSize = size;
}

long SVVirtualCamera::GetFileImageWidth() const
{
	return m_imageSize.cx;
}

void SVVirtualCamera::SetFileImageWidth(long width)
{
	m_imageSize.cx = width;
}

long SVVirtualCamera::GetFileImageHeight() const
{
	return m_imageSize.cy;
}

void SVVirtualCamera::SetFileImageHeight(long height)
{
	m_imageSize.cy = height;
}

HRESULT SVVirtualCamera::RegisterTriggerRelay(SVIOTriggerLoadLibraryClass* triggerDLL, unsigned long ulHandle)
{
	HRESULT hr = S_FALSE;

	if (triggerDLL && 0 != ulHandle)
	{
		SVAcquisitionClassPtr pAcq = GetAcquisitionDevice();
		if(nullptr != pAcq)
		{
			SvTh::SVAcquisitionInitiator  acqInitiator;

			// need the digitizer name here ...
			SvTh::SVDigitizerLoadLibraryClass* pAcqDLL = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(pAcq->DigName().c_str());

			if (pAcqDLL)
			{
				acqInitiator.Add(pAcqDLL, pAcq->m_hDigitizer);
			}

			hr = m_triggerRelay.RegisterTriggerRelay(triggerDLL, ulHandle, SvTi::SVFileAcquisitionInitiator(acqInitiator));
		}
	}
	return hr;
}

HRESULT SVVirtualCamera::UnregisterTriggerRelay()
{
	return m_triggerRelay.UnregisterTriggerRelay();
}

void SVVirtualCamera::createCameraParameters()
{
	SvVol::BasicValueObjectPtr pValue;
	SVGUID ParameterUID( GUID_NULL );
	//Initialize the parameters and set their unique GUID
	pValue = m_CameraValues.setValueObject( SvDef::FqnCameraSerialNumber, _T(""), this, SvPb::SVCameraObjectType );
	ParameterUID = CameraBaseSerialNumberUidGuid;
	ParameterUID.ToGUID().Data1 += m_CameraID;
	SVObjectManagerClass::Instance().ChangeUniqueObjectID( pValue.get(), ParameterUID );

	pValue = m_CameraValues.setValueObject( SvDef::FqnCameraGain, 0L, this, SvPb::SVCameraObjectType );
	ParameterUID = CameraBaseGainUidGuid;
	ParameterUID.ToGUID().Data1 += m_CameraID;
	SVObjectManagerClass::Instance().ChangeUniqueObjectID( pValue.get(), ParameterUID );

	pValue = m_CameraValues.setValueObject( SvDef::FqnCameraShutter, 0L, this, SvPb::SVCameraObjectType );
	ParameterUID = CameraBaseShutterUidGuid;
	ParameterUID.ToGUID().Data1 += m_CameraID;
	SVObjectManagerClass::Instance().ChangeUniqueObjectID( pValue.get(), ParameterUID );
}

HRESULT SVVirtualCamera::updateCameraParameters()
{
	HRESULT Result = S_OK;

	if(nullptr == mpsvDevice)
	{
		Result = E_FAIL;
		return Result;
	}

	SVDeviceParamCollection CameraParameters;
	Result = mpsvDevice->GetDeviceParameters( CameraParameters );
	if(S_OK == Result)
	{
		SVDeviceParam* pDeviceParam = CameraParameters.GetParameter( DeviceParamSerialNumberString );
		if( nullptr != pDeviceParam )
		{
			variant_t SerialNumberValue;
			pDeviceParam->GetValue(SerialNumberValue.GetVARIANT());
			pDeviceParam = nullptr;
			m_CameraValues.setValueObject( SvDef::FqnCameraSerialNumber, SerialNumberValue, this, SvPb::SVCameraObjectType );
		}
		pDeviceParam = CameraParameters.GetParameter( DeviceParamGain );
		/*Result = */updateCameraLongParameter( SvDef::FqnCameraGain, dynamic_cast< SVLongValueDeviceParam* >( pDeviceParam ) );

		pDeviceParam = CameraParameters.GetParameter( DeviceParamShutter );
		Result = updateCameraLongParameter( SvDef::FqnCameraShutter, dynamic_cast< SVLongValueDeviceParam* >( pDeviceParam ) );
	}
	return Result;
}

HRESULT SVVirtualCamera::updateCameraLongParameter( LPCTSTR Name, const SVLongValueDeviceParam* pLongValueDeviceParam )
{
	HRESULT Result = E_FAIL;

	if( nullptr != pLongValueDeviceParam )
	{
		variant_t Value;
		Value = pLongValueDeviceParam->GetScaledValue();
		SvVol::BasicValueObjectPtr pValueObject = m_CameraValues.setValueObject( Name, Value, this, SvPb::SVCameraObjectType );
		if(nullptr != pValueObject)
		{
			pValueObject->SetObjectAttributesAllowed( SvPb::remotelySetable | SvPb::setableOnline, SvOi::SetAttributeType::AddAttribute );
			Result = S_OK;
		}
	}

	return Result;
}

HRESULT SVVirtualCamera::updateDeviceParameters(SVDeviceParamCollection& rCameraParameters)
{
	HRESULT Result = E_FAIL;

	if(nullptr ==  mpsvDevice)
	{
		return Result;
	}

	if( S_OK == mpsvDevice->GetDeviceParameters( rCameraParameters ) )
	{
		SVDeviceParamCollection	ChangedCameraParameters;
		SVDeviceParam* pDeviceParam = rCameraParameters.GetParameter( DeviceParamGain );
		SvVol::BasicValueObjectPtr pValueObject;
		pValueObject = m_CameraValues.getValueObject( SvDef::FqnCameraGain );

		if(nullptr != pValueObject)
		{
			/*Result = */pValueObject->updateDeviceParameter( pDeviceParam );
			ChangedCameraParameters.SetParameter(pDeviceParam);
			Result = S_OK;
		}

		if( S_OK == Result )
		{
			pDeviceParam = rCameraParameters.GetParameter( DeviceParamShutter );
			pValueObject = m_CameraValues.getValueObject( SvDef::FqnCameraShutter );
			if(nullptr != pValueObject)
			{
				Result = pValueObject->updateDeviceParameter( pDeviceParam );
				ChangedCameraParameters.SetParameter(pDeviceParam);
			}
			else
			{
				Result = E_FAIL;
			}
		}

		if( S_OK == Result )
		{
			mpsvDevice->SetDeviceParameters( ChangedCameraParameters );
		}
	}

	return Result;
}

void SVVirtualCamera::setTempImage(const SVMatroxBuffer pImage)
{
	std::lock_guard<std::mutex> guard(m_tmpImage_mutex);
	m_tmpImage.clear();
	if (S_OK == SVMatroxBufferInterface::Create(m_tmpImage, pImage))
	{
		SVMatroxBufferInterface::CopyBuffer(m_tmpImage, pImage);
	}
}

SVMatroxBuffer SVVirtualCamera::getTempImage()
{ 
	std::lock_guard<std::mutex> guard(m_tmpImage_mutex);
	return m_tmpImage; 
}

} //namespace SvIe
