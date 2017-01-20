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
#include "CameraLibrary/SVDeviceParams.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVObjectLibrary/GlobalConst.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
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
	ASSERT( nullptr != pCamera && nullptr != pResponse );

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

BOOL SVVirtualCamera::GetImageInfo(SVImageInfoClass *pImageInfo)
{
	BOOL bOk = FALSE;

	if( !( mpsvDevice.empty() ) )
	{
		bOk = S_OK == mpsvDevice->GetImageInfo( pImageInfo );
		if ( nullptr != pImageInfo )
		{
			bOk = S_OK == pImageInfo->SetImageProperty( SVImagePropertyBandLink, mlBandLink );
		}
	}

	return bOk;
}

long SVVirtualCamera::GetImageDepth() const
{
	long l_Depth = 0;

	if( !( mpsvDevice.empty() ) )
	{
		l_Depth = mpsvDevice->GetCircleBufferSize();
	}

	return l_Depth;
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

BOOL SVVirtualCamera::Create( LPCTSTR DeviceName )
{
	BOOL bOk = TRUE;

	if( !( mpsvDevice.empty() ) )
	{
		bOk = Destroy();
	}

	SVDigitizerProcessingClass::Instance().SetDigitizerColor( DeviceName, m_IsColor );
	mpsvDevice = SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( DeviceName );

	bOk = !( mpsvDevice.empty() ) && bOk;

	if ( bOk )
	{
		SetBandLink( mpsvDevice->Channel() );

		m_outObjectInfo.ObjectTypeInfo.ObjectType = SVVirtualCameraType;

		bOk = m_CallbackList.Create() && bOk;

		createCameraParameters();
		updateCameraParameters();
	}

	return bOk;
}

BOOL SVVirtualCamera::Destroy()
{
	BOOL bOk = TRUE;

	bOk = DestroyLocal();

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
			BasicValueObjectPtr pBasicValueObject = m_CameraValues.getValueObject( rNameInfo.GetObjectArrayName( Index + 1 ).c_str() );

			if( !pBasicValueObject.empty() )
			{
				rpObject = dynamic_cast<SVObjectClass*> (pBasicValueObject.get());
			}
		}
	}

	return l_Status;
}

BOOL SVVirtualCamera::CanGoOnline() const
{
	return mpsvDevice && mpsvDevice->IsValid();
}// end CanGoOnline

BOOL SVVirtualCamera::GoOnline()
{
	BOOL bGoOnline = mpsvDevice && ( S_OK == mpsvDevice->Start() );

	if ( bGoOnline )
	{
		DWORD bLog = AfxGetApp()->GetProfileInt(_T("Debug"), _T("LogTSFEF"), FALSE);
		AfxGetApp()->WriteProfileInt(_T("Debug"), _T("LogTSFEF"), bLog );
		if ( bLog )
		{
			SVString Name = SvUl_SF::Format(_T("%s\\TSFEF_%s.log"), SvStl::GlobalPath::Inst().GetTempPath().c_str(), SVString(mpsvDevice->DeviceName() + _T("_vc")).c_str());
			m_LogFile.Open( Name.c_str(), CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite );
		}
	}

	return bGoOnline;
}// end GoOnline

BOOL SVVirtualCamera::GoOffline()
{
	if ( m_LogFile.m_hFile != CFile::hFileNull )
	{
		m_LogFile.Close();
	}

	return mpsvDevice && ( S_OK == mpsvDevice->Stop() );
}// end GoOffline

BOOL SVVirtualCamera::RegisterFinishProcess( void *pvOwner, LPSVFINISHPROC pCallback )
{
	BOOL bOk = FALSE;

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
				if ( !( pData.empty() ) )
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
				SVOCallbackClassPtr pData = new SVOCallbackClass;

				if ( !( pData.empty() ) )
				{
					pData->mpCallback = pCallback;
					pData->mpvOwner = pvOwner;
					pData->mpvCaller = this;

					if ( m_CallbackList.AddTail( pData ) )
					{
						bOk = TRUE;
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

BOOL SVVirtualCamera::UnregisterFinishProcess(void *pvOwner, LPSVFINISHPROC pCallback)
{
	BOOL bOk = FALSE;

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

				if ( !( pData.empty() ) )
				{
					if ( pData->mpCallback == pCallback &&
					     pData->mpvOwner == pvOwner &&
						 pData->mpvCaller == this )
					{
						if ( m_CallbackList.RemoveAt( l ) )
						{
							bOk = TRUE;
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

				if ( !( pData.empty() ) )
				{
					(pData->mpCallback)( pData->mpvOwner, pData->mpvCaller, (void*) pResponse );
				}
			}
		}
	}
}

BOOL SVVirtualCamera::DestroyLocal()
{
	BOOL bOk = GoOffline();

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

				if ( !( pData.empty() ) && nullptr != mpsvDevice )
				{
					bOk &= S_OK == mpsvDevice->UnregisterCallback( SVVirtualCamera::SVImageCallback, 
					                                       pData->mpvOwner, 
					                                       pData->mpvCaller );
				}

				if ( ! m_CallbackList.RemoveAt( l ) )
				{
					bOk = FALSE;
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

HRESULT SVVirtualCamera::GetSourceImageIndex( SVDataManagerHandle* pHandle, const SVGuidSVCameraInfoStructMap& rGuidCameraMap ) const
{
	HRESULT Result( E_FAIL );

	if( nullptr != pHandle )
	{
		SVGuidSVCameraInfoStructMap::const_iterator Iter( rGuidCameraMap.find( GetUniqueObjectID() ) );

		if( rGuidCameraMap.end() != Iter   )
		{
			Result = pHandle->Assign( Iter->second.GetSourceImageDMIndexHandle(), Iter->second.GetSourceImageDMIndexHandle().GetLockType() );
		}
		else
		{
			pHandle->clear();
		}
	}

	return Result;
}

HRESULT SVVirtualCamera::ReserveNextImageHandleIndex( SVDataManagerHandle& p_rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType ) const
{
	HRESULT l_Status = S_OK;

	BOOL bOk = nullptr != mpsvDevice;
	SVDataManagerHandle	l_DMIndexHandle;

	if( !( mpsvDevice.empty() ) )
	{
		l_Status = mpsvDevice->GetNextIndex( l_DMIndexHandle );

		if( S_OK == l_Status )
		{
			if( ReserveImageHandleIndex( l_DMIndexHandle ) )
			{
				l_Status = p_rDMIndexHandle.Assign( l_DMIndexHandle, p_LockType );
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

bool SVVirtualCamera::ReserveImageHandleIndex( const SVDataManagerHandle& p_rDMIndexHandle ) const
{
	bool Result = (nullptr != mpsvDevice);

	if( Result )
	{
		if( -1 < p_rDMIndexHandle.GetIndex() )
		{
			Result = mpsvDevice->GetCircleBuffer()->SetCurrentIndex( p_rDMIndexHandle );
		}
	}

	return Result;
}

bool SVVirtualCamera::CopyValue( const SVDataManagerHandle& p_From, const SVDataManagerHandle& p_To )
{
	bool Result( false );

	SVImageObjectClassPtr imagePtr = mpsvDevice->GetCircleBuffer();
	if (!imagePtr.empty())
	{
		Result = imagePtr->CopyValue( p_From, p_To );
	}

	return Result;
}

void SVVirtualCamera::DumpDMInfo( LPCTSTR p_szName ) const
{
	if( nullptr != mpsvDevice )
	{
		mpsvDevice->DumpDMInfo( p_szName );
	}
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

void SVVirtualCamera::SetImageFilename(const SVString& filename)
{
	m_imageFilename = filename;
}

LPCTSTR SVVirtualCamera::GetImageDirectoryName() const
{
	return m_imageDirectoryName.c_str();
}

void SVVirtualCamera::SetImageDirectoryName(const SVString& directoryName)
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
		if( !( pAcq.empty() ) )
		{
			SvTh::SVAcquisitionInitiator  acqInitiator;

			// need the digitizer name here ...
			SVDigitizerLoadLibraryClass* pAcqDLL = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(pAcq->DigName().c_str());

			if (pAcqDLL)
			{
				acqInitiator.Add(pAcqDLL, pAcq->m_hDigitizer);
			}

			hr = m_triggerRelay.RegisterTriggerRelay(triggerDLL, ulHandle, SvTh::SVFileAcquisitionInitiator(acqInitiator));
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
	BasicValueObjectPtr pValue;
	SVGUID ParameterUID( SV_GUID_NULL );
	//Initialize the parameters and set their unique GUID
	pValue = m_CameraValues.setValueObject( SvOl::FqnCameraSerialNumber, _T(""), this, SVCameraObjectType );
	ParameterUID = CameraBaseSerialNumberUidGuid;
	ParameterUID.ToGUID().Data1 += m_CameraID;
	SVObjectManagerClass::Instance().ChangeUniqueObjectID( pValue.get(), ParameterUID );

	pValue = m_CameraValues.setValueObject( SvOl::FqnCameraGain, 0L, this, SVCameraObjectType );
	ParameterUID = CameraBaseGainUidGuid;
	ParameterUID.ToGUID().Data1 += m_CameraID;
	SVObjectManagerClass::Instance().ChangeUniqueObjectID( pValue.get(), ParameterUID );

	pValue = m_CameraValues.setValueObject( SvOl::FqnCameraShutter, 0L, this, SVCameraObjectType );
	ParameterUID = CameraBaseShutterUidGuid;
	ParameterUID.ToGUID().Data1 += m_CameraID;
	SVObjectManagerClass::Instance().ChangeUniqueObjectID( pValue.get(), ParameterUID );
}

HRESULT SVVirtualCamera::updateCameraParameters()
{
	HRESULT Result = S_OK;

	if( mpsvDevice.empty())
	{
		Result = E_FAIL;
		return Result;
	}

	SVDeviceParamCollection CameraParameters;
	Result = mpsvDevice->GetDeviceParameters( CameraParameters );
	if(S_OK == Result)
	{
		SVDeviceParam* pDeviceParam = nullptr;

		pDeviceParam = CameraParameters.GetParameter( DeviceParamSerialNumberString );

		if( nullptr != pDeviceParam )
		{
			variant_t SerialNumberValue;
			pDeviceParam->GetValue(SerialNumberValue.GetVARIANT());
			pDeviceParam = nullptr;
			m_CameraValues.setValueObject( SvOl::FqnCameraSerialNumber, SerialNumberValue, this, SVCameraObjectType );
		}
		pDeviceParam = CameraParameters.GetParameter( DeviceParamGain );
		Result = updateCameraLongParameter( SvOl::FqnCameraGain, dynamic_cast< SVLongValueDeviceParam* >( pDeviceParam ) );

		pDeviceParam = CameraParameters.GetParameter( DeviceParamShutter );
		Result = updateCameraLongParameter( SvOl::FqnCameraShutter, dynamic_cast< SVLongValueDeviceParam* >( pDeviceParam ) );
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
		BasicValueObjectPtr pValueObject = m_CameraValues.setValueObject( Name, Value, this, SVCameraObjectType );
		if( !pValueObject.empty() )
		{
			pValueObject->ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
			Result = S_OK;
		}
	}

	return Result;
}

HRESULT SVVirtualCamera::updateDeviceParameters(SVDeviceParamCollection& rCameraParameters)
{
	HRESULT Result = E_FAIL;

	if( mpsvDevice.empty())
	{
		return Result;
	}

	if( S_OK == mpsvDevice->GetDeviceParameters( rCameraParameters ) )
	{
		SVDeviceParamCollection	ChangedCameraParameters;
		SVDeviceParam* pDeviceParam = nullptr;
		pDeviceParam = rCameraParameters.GetParameter( DeviceParamGain );
		BasicValueObjectPtr pValueObject;
		pValueObject = m_CameraValues.getValueObject( SvOl::FqnCameraGain );

		if( !pValueObject.empty() )
		{
			Result = pValueObject->updateDeviceParameter( pDeviceParam );
			ChangedCameraParameters.SetParameter(pDeviceParam);
			Result = S_OK;
		}

		if( S_OK == Result )
		{
			pDeviceParam = rCameraParameters.GetParameter( DeviceParamShutter );
			pValueObject = m_CameraValues.getValueObject( SvOl::FqnCameraShutter );
			if( !pValueObject.empty() )
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

