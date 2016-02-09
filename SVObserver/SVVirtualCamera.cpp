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
#include "SVOMFCLibrary/SVDeviceParams.h"
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

SVVirtualCamera::SVVirtualCamera( LPCSTR ObjectName )
: SVObjectClass( ObjectName )
	,mlBandLink( 0 )
	,m_bFileAcquisition( false )
	,m_IsColor( false )
	,m_bImageSizeEditModeFileBased( true )
	,m_imageLoadingMode( 0 )
{
	m_imageSize.cx = 0;
	m_imageSize.cy = 0;
}

SVVirtualCamera::SVVirtualCamera( SVObjectClass* POwner, int StringResourceID )
: SVObjectClass( POwner, StringResourceID )
,mlBandLink( 0 )
,m_bFileAcquisition( false )
,m_IsColor( false )
,m_bImageSizeEditModeFileBased( true )
,m_imageLoadingMode( 0 )
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
		Result = UpdateCameraParameters();
	}
	
	return Result;
}

BOOL SVVirtualCamera::Create( LPCTSTR p_szDeviceName )
{
	BOOL bOk = TRUE;

	if( !( mpsvDevice.empty() ) )
	{
		bOk = Destroy();
	}

	SVDigitizerProcessingClass::Instance().SelectDigitizer( p_szDeviceName );
	mpsvDevice = SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( p_szDeviceName );

	bOk = !( mpsvDevice.empty() ) && bOk;

	if ( bOk )
	{
		SetBandLink( mpsvDevice->Channel() );

		outObjectInfo.ObjectTypeInfo.ObjectType = SVVirtualCameraType;

		bOk = m_CallbackList.Create() && bOk;

		UpdateCameraParameters();
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

HRESULT SVVirtualCamera::GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, long Index ) const
{
	rpObject = nullptr;
	HRESULT l_Status = SVObjectClass::GetChildObject( rpObject, rNameInfo, Index );

	if( S_OK != l_Status )
	{
		if( 0 < rNameInfo.m_NameArray.size() && rNameInfo.m_NameArray[ Index ] == GetName() )
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
			CString sName;
			sName.Format(_T("c:\\temp\\TSFEF_%s.log"), mpsvDevice->DeviceName() + _T("_vc"));
			m_LogFile.Open(sName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite );
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

//typedef BOOL (CALLBACK *LPSVFINISHPROC)(void*,void*,SVODataResponseClass*);

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
		UpdateCameraParameters();
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

HRESULT SVVirtualCamera::GetSourceImageIndex( SVDataManagerHandle& p_rHandle, const SVProductInfoStruct& p_rProduct ) const
{
	HRESULT l_Status( S_OK );

	SVDataManagerHandle	l_DMIndex;

	SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::const_iterator l_svIter;

	l_svIter = p_rProduct.m_svCameraInfos.find( const_cast< SVVirtualCamera* >( this ) );

	if( l_svIter != p_rProduct.m_svCameraInfos.end() )
	{
		l_Status = p_rHandle.Assign( l_svIter->second.GetSourceImageDMIndexHandle(), l_svIter->second.GetSourceImageDMIndexHandle().GetLockType() );
	}
	else
	{
		p_rHandle.clear();

		l_Status = E_FAIL;
	}

	return l_Status;
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

BOOL SVVirtualCamera::ReserveImageHandleIndex( const SVDataManagerHandle& p_rDMIndexHandle ) const
{
	BOOL bOk = nullptr != mpsvDevice;

	if( bOk )
	{
		if( -1 < p_rDMIndexHandle.GetIndex() )
		{
			bOk = mpsvDevice->GetCircleBuffer()->SetCurrentIndex( p_rDMIndexHandle );
		}
	}

	return bOk;
}

BOOL SVVirtualCamera::CopyValue( const SVDataManagerHandle& p_From, const SVDataManagerHandle& p_To )
{
	BOOL l_bOk = false;

	SVImageObjectClassPtr imagePtr = mpsvDevice->GetCircleBuffer();
	if (!imagePtr.empty())
	{
		l_bOk = imagePtr->CopyValue( p_From, p_To );
	}

	return l_bOk;
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
	return m_imageFilename.ToString();
}

void SVVirtualCamera::SetImageFilename(const SVString& filename)
{
	m_imageFilename = filename;
}

LPCTSTR SVVirtualCamera::GetImageDirectoryName() const
{
	return m_imageDirectoryName.ToString();
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

	if (triggerDLL && ulHandle > 0)
	{
		SVAcquisitionClassPtr pAcq = GetAcquisitionDevice();
		if( !( pAcq.empty() ) )
		{
			SVAcquisitionInitiator acqInitiator;

			// need the digitizer name here ...
			SVDigitizerLoadLibraryClass* pAcqDLL = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(pAcq->DigName());

			if (pAcqDLL)
			{
				acqInitiator.Add(pAcqDLL, pAcq->m_hDigitizer);
			}

			hr = m_triggerRelay.RegisterTriggerRelay(triggerDLL, ulHandle, SVFileAcquisitionInitiator(acqInitiator));
		}
	}
	return hr;
}

HRESULT SVVirtualCamera::UnregisterTriggerRelay()
{
	return m_triggerRelay.UnregisterTriggerRelay();
}

HRESULT SVVirtualCamera::UpdateCameraParameters()
{
	HRESULT Status = S_OK;

	if( mpsvDevice.empty())
	{
		Status = S_FALSE;
		return Status;
	}

	SVDeviceParamCollection CameraParameters;
	Status = mpsvDevice->GetDeviceParameters( CameraParameters );
	if(S_OK == Status)
	{
		SVDeviceParam* pDeviceParam = nullptr;

		pDeviceParam = CameraParameters.GetParameter( DeviceParamSerialNumberString );

		if( nullptr != pDeviceParam )
		{
			variant_t SerialNumberValue;
			pDeviceParam->GetValue(SerialNumberValue.GetVARIANT());
			pDeviceParam = nullptr;
			m_CameraValues.setValueObject( ::CameraSerialNumber, SerialNumberValue, this, SVCameraObjectType );
		}
		pDeviceParam = CameraParameters.GetParameter( DeviceParamGain );
		Status = UpdateCameraLongParameter( ::CameraGain, dynamic_cast< SVLongValueDeviceParam* >( pDeviceParam ) );

		pDeviceParam = CameraParameters.GetParameter( DeviceParamShutter );
		Status = UpdateCameraLongParameter( ::CameraShutter, dynamic_cast< SVLongValueDeviceParam* >( pDeviceParam ) );
	}
	return Status;
}

HRESULT SVVirtualCamera::UpdateCameraLongParameter( LPCTSTR Name, const SVLongValueDeviceParam* pLongValueDeviceParam )
{
	HRESULT Status = S_FALSE;

	if( nullptr != pLongValueDeviceParam )
	{
		variant_t Value;
		Value = pLongValueDeviceParam->GetScaledValue();
		BasicValueObjectPtr pValueObject = m_CameraValues.setValueObject( Name, Value, this, SVCameraObjectType );
		if( !pValueObject.empty() )
		{
			pValueObject->ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE | SV_SETABLE_ONLINE;
			Status = S_OK;
		}
	}

	return Status;
}

HRESULT SVVirtualCamera::UpdateDeviceParameters(SVDeviceParamCollection& rCameraParameters)
{
	HRESULT Status = S_OK;

	if( mpsvDevice.empty())
	{
		Status = S_FALSE;
		return Status;
	}

	if( S_OK == mpsvDevice->GetDeviceParameters( rCameraParameters ) )
	{
		SVDeviceParamCollection	ChangedCameraParameters;
		SVDeviceParam* pDeviceParam = nullptr;
		pDeviceParam = rCameraParameters.GetParameter( DeviceParamGain );
		BasicValueObjectPtr pValueObject;
		pValueObject = m_CameraValues.getValueObject( ::CameraGain );

		if( !pValueObject.empty() )
		{
			Status = pValueObject->updateDeviceParameter( pDeviceParam );
			ChangedCameraParameters.SetParameter(pDeviceParam);
		}
		else
		{
			Status = S_FALSE;
		}

		if( S_OK == Status )
		{
			pDeviceParam = rCameraParameters.GetParameter( DeviceParamShutter );
			pValueObject = m_CameraValues.getValueObject( ::CameraShutter );
			if( !pValueObject.empty() )
			{
				Status = pValueObject->updateDeviceParameter( pDeviceParam );
				ChangedCameraParameters.SetParameter(pDeviceParam);
			}
			else
			{
				Status = S_FALSE;
			}
		}

		if( S_OK == Status )
		{
			mpsvDevice->SetDeviceParameters( ChangedCameraParameters );
		}
	}

	return Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVVirtualCamera.cpp_v  $
 * 
 *    Rev 1.6   17 Jul 2014 20:51:54   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   02 Jun 2014 10:22:00   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Fixed GetChildObject method.
 * Added UpdateCameraLongParameter method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Mar 2014 23:24:48   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Updated UpdateCameraParameters method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Mar 2014 15:33:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added GetChildObject, UpdateDeviceParameters, UpdateCameraParameters methods to get and set the object values "SerialNumber", "Gain" and "Shutter" ("SerialNumber" is read only) which are inserted into the OM.
 *   Added method RefreshObject.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Mar 2014 18:24:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 *   Added DEBUG_NEW.
 *   Made methods const.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:44:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   29 Apr 2013 14:33:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 16:20:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   08 Aug 2012 15:42:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  761
 * SCR Title:  Fix issue with Clip on Adjust LUT Dialog using GigE cameras
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated code to fix issues with LUT and Light Reference Dialogs when camera has no LUT or Light Reference.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   19 Jun 2012 14:29:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   16 Sep 2011 16:23:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects with new device and resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   30 Mar 2011 16:15:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   22 Mar 2011 08:03:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   16 Mar 2011 14:01:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   04 Nov 2010 14:18:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   30 Jul 2009 13:14:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   08 Oct 2008 09:38:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revsied to add Trigger Relay and File Acquisition Initiator to support the use of hardware based triggers with File Acquisition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   16 Sep 2008 14:38:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods and member variables in support of FileAcquistion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   21 Jun 2007 15:07:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18.1.0   14 Jul 2009 15:37:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code with new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   26 Feb 2007 14:25:52   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  579
 * SCR Title:  Upgrade SVObserver to 4.79 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Modified TSFEF logging code to show Invalid values as none rather than showing the defaulted value.  This makes the log easier to read and understand when there is a problem.  This code is not executed unless the debug - TSFEF registry flag is set.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   23 Aug 2005 10:13:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to handle copy forward correctly.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   22 Aug 2005 10:30:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   10 Aug 2005 12:48:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to improve performance and reduce system delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   17 Feb 2005 14:55:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changes to support new Image Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   18 Feb 2004 09:44:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed logging
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   03 Feb 2004 17:31:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added logging
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   08 Jul 2003 07:58:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed SVLightReferenceArray to SVLightReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   22 Apr 2003 17:30:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   27 Mar 2003 13:09:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed it so that when the object is destroyed it makes sure to not only empty the callback list but to also unregister the callbacks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   24 Mar 2003 17:49:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  319
 * SCR Title:  Update SVObserver to Version 4.20 Release
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated StartProcess, RegisterFinishProcess, and UnregisterFinishProcess methods to handle multiple PPQs attached to a single virtual camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   12 Feb 2003 10:39:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated UnregisterFinishProcess, RegisterFinishProcess, and StartProcess methods to fix the ViperRGB issues.
 * Updated SVOVirtualCameraCallbackPtr callback function to fix the ViperRGB issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   31 Jan 2003 09:39:38   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added GetDataManagerHandle method to use the new image circle buffer methodology.  Updated FinishProcess method to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Jan 2003 17:41:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  309
 * SCR Title:  Implement the ability to lock acquired or processed image from the SIAC.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Get CircleBuffer and Index functions to be used to lock images from the data in the SVProductInfoStruct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Jan 2003 09:54:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Modified SetLut
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Jan 2003 15:32:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added:
 * GetLut
 * SetLut
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Dec 2002 15:23:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   long GetSourceImageIndex( SVProductInfoStruct* pProduct );
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Dec 2002 22:35:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  311
 * SCR Title:  Fix Light Reference save for RGB Mono
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVVirtualCamera class with accessor function for mlBandLink and made the member variable protected.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:41:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226, 272
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/