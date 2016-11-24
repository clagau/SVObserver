//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision, Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAcquisitionClass
//* .File Name       : $Workfile:   SVAcquisitionClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   07 Mar 2014 18:02:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <fstream>

#include "SVAcquisitionClass.h"
#include "SVDataManagerLibrary/DataManager.h"
#include "SVImageLibrary/SVDigitizerLoadLibraryClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVMessage/SVMessage.h"
#include "SVTimerLibrary/SVClock.h"
#include "TriggerInformation/SVAcquisitionConstructParams.h"
#include "SVDigitizerProcessingClass.h"
#include "SVSystemLibrary/SVFileNameManagerClass.h"
#include "SVGlobal.h"
#include "SVOCore/SVImageObjectClass.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVSVIMStateClass.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

SVAcquisitionClass::SVAcquisitionClass( const SvTi::SVAcquisitionConstructParams& p_rParams )
: SVODataDeviceClass( p_rParams.m_DigitizerName.c_str() )
,m_LUTAndLRSet( p_rParams.m_LUTAndLRSet )
{
	mbIsBufferCreated = false;
	mbTempOnline = false;
	m_ImageAquired = false;
	m_LastImage = nullptr;
	mulSize = 0;

	mlStartFrameIndex = -1;

	m_hDigitizer = 0;

	m_AcquisitionBuffersPtr = new SVImageObjectClass;

	msvImageInfo.SetExtentProperty( SVExtentPropertyPositionPoint, 0 );

	SetDigName( p_rParams.m_DigitizerName.c_str() );
	
	SetDigNumber( p_rParams.m_DigNumber );
	SetChannel( p_rParams.m_Channel );
	SetBandSize( p_rParams.m_BandSize );
	SetBandMaxSize( p_rParams.m_BandMaxSize );
	
	if( m_LUTAndLRSet )
	{
		CreateLightReference( p_rParams.m_LRBandSize );
		CreateLut( p_rParams.m_LUTInfo );
	}
}

SVAcquisitionClass::~SVAcquisitionClass()
{
	if( nullptr != m_LastImage )
	{
		::SysFreeString( m_LastImage );

		m_LastImage = nullptr;
	}
}

bool SVAcquisitionClass::IsValid() const
{
	return ( SVODataDeviceClass::IsValid() );
}

bool SVAcquisitionClass::IsStarted() const
{
	return ( SVODataDeviceClass::IsStarted() );
}

void SVAcquisitionClass::ClearDevice()
{
	if( nullptr != m_LastImage )
	{
		::SysFreeString( m_LastImage );

		m_LastImage = nullptr;
	}

	m_pDataManagerHandle.clear();
	m_AcquisitionBuffersPtr.clear();
	m_DeviceParams.Clear();
	m_CameraFileDeviceParams.Clear();

	mLightReference.Reset();

	for ( long l = mFiles.GetSize() - 1; -1 < l; l-- )
	{
		SVFileNameManagerClass::RemoveItem( &(mFiles[l]) );
	}

	m_SingleGrabHandle.clear();

	SVODataDeviceClass::ClearDevice();
}

HRESULT SVAcquisitionClass::Create( unsigned long ulSize )
{
	HRESULT l_Status = SVODataDeviceClass::Create( ulSize );

	return l_Status;
}

HRESULT SVAcquisitionClass::Destroy()
{
	HRESULT hrOk = S_OK;
	HRESULT l_Status = S_OK;

	DestroyLut();


	l_Status = SVODataDeviceClass::Destroy();

	if( S_OK == hrOk  )
	{
		hrOk = l_Status;
	}

	l_Status = DestroyBuffers();

	if( S_OK == hrOk  )
	{
		hrOk = l_Status;
	}

	for ( long l = mFiles.GetSize() - 1; -1 < l; l-- )
	{
		SVFileNameManagerClass::RemoveItem( &(mFiles[l]) );
	}

	return hrOk;
}

HRESULT SVAcquisitionClass::Start()
{
	HRESULT hrOk = SVODataDeviceClass::Start();

	m_ImageAquired = false;
	mlStartFrameIndex = -1;
	
	if ( S_OK == hrOk  )
	{
		if( !( m_AcquisitionBuffersPtr.empty() ) )
		{
			hrOk = m_AcquisitionBuffersPtr->ResetObject();
		}
		else
		{
			hrOk = S_FALSE;
		}
	}
	
	if ( S_OK == hrOk )
	{
		hrOk = StartDigitizer();
	}

	return hrOk;
}

HRESULT SVAcquisitionClass::Stop()
{
	HRESULT hr = S_OK;
	HRESULT l_Temp = S_OK;

	if( IsDigitizerSubsystemValid() )
	{
		l_Temp = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->Stop( m_hDigitizer );

		if( S_OK == hr )
		{
			hr = l_Temp;
		}

		l_Temp = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->UnregisterBufferInterface( m_hDigitizer );

		if( S_OK == hr )
		{
			hr = l_Temp;
		}
	}
	else
	{
		if( S_OK == hr  )
		{
			hr = E_FAIL;
		}
	}
	
	l_Temp = SVODataDeviceClass::Stop();

	if( S_OK == hr )
	{
		hr = l_Temp;
	}

	return hr;
}

HRESULT SVAcquisitionClass::Reset()
{
	HRESULT l_Status = S_OK;
	HRESULT l_Temp = SVODataDeviceClass::Reset();

	if( S_OK == l_Status )
	{
		l_Status = l_Temp;
	}

	return l_Status;
}

HRESULT SVAcquisitionClass::CreateBuffers( SVImageInfoClass IInfo, unsigned long ulSize )
{
	HRESULT Result = S_FALSE;
	
	Result = DestroyBuffers();
	if ( S_OK == Result )
	{
		_bstr_t bsName = DeviceName();

		Result = TheSVDataManager.CreateManagedIndexArray( m_pDataManagerHandle, bsName, ulSize );

		mulSize = ulSize + 3;
		msvImageInfo = IInfo;
		
		if( S_OK == Result )
		{
			if ( IsDigitizerSubsystemValid() )
			{
				Result = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->CreateBuffers( m_hDigitizer, ulSize + 3 );
			}

			bool l_Status = true;

			SVDataManagerHandle l_Handle;

			if( m_AcquisitionBuffersPtr.empty() )
			{
				m_AcquisitionBuffersPtr = new SVImageObjectClass;
			}

			l_Status = l_Status && !( m_AcquisitionBuffersPtr.empty() );

			if( l_Status )
			{
				m_AcquisitionBuffersPtr->resize( mulSize );
				m_AcquisitionBuffersPtr->SetParentImageObject( SVImageObjectClassPtr() );
				m_AcquisitionBuffersPtr->SetImageInfo( msvImageInfo );
			}

			l_Status = l_Status && ( S_OK == m_AcquisitionBuffersPtr->ResetObject()  );
			l_Status = l_Status && S_OK == GetNextIndex( l_Handle );
			l_Status = l_Status && SetCurrentIndex( l_Handle );

			if( l_Status && S_OK == Result )
			{
				Result = S_OK;
			}
			else if ( !l_Status )
			{
				Result = S_FALSE;
			}
		}
		else
		{
			Result = S_FALSE;
		}
		
		if (S_OK == Result  )
		{
			mbIsBufferCreated = true;
		}
		else
		{
			DestroyBuffers();
		}
	}
	
	return Result;
}

HRESULT SVAcquisitionClass::DestroyBuffers()
{
	HRESULT hrOk = S_OK;
	
	mulSize = 0;
		
	mlStartFrameIndex = -1;
	
	if( !( m_AcquisitionBuffersPtr.empty() ) )
	{
		m_AcquisitionBuffersPtr->clear();
	}
	
	if ( IsDigitizerSubsystemValid() )
	{
		if ( S_OK != SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->DestroyBuffers( m_hDigitizer ) )
		{
			hrOk = S_FALSE;
		}
	}

	m_pDataManagerHandle.clear();

	mbIsBufferCreated = false;

	return hrOk;
}

HRESULT SVAcquisitionClass::LoadFiles(SVFileNameArrayClass &rArray)
{
	HRESULT Result = S_OK;
	bool LogOnly( false );

	long l( 0 );

	for ( l = mFiles.GetSize() - 1; -1 < l; l-- )
	{
		SVFileNameManagerClass::RemoveItem( &(mFiles[l]) );
	}

	mFiles = rArray;
	m_CameraFileDeviceParams.Clear();

	for ( l = 0; S_OK == Result  && l < mFiles.GetSize(); l++ )
	{
		if ( ! SVFileNameManagerClass::AddItem( &(mFiles[l]) ) )
		{
			if( SVSVIMStateClass::CheckState( SV_STATE_REMOTE_CMD ) )
			{
				LogOnly = true;
			}
			if( LogOnly )
			{
				SvStl::MessageMgrStd Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_74_LOAD_FILE, mFiles[l].GetFullFileName(), SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25047_LoadFileFailed );
			}
			else
			{
				SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
				if( IDYES == Exception.setMessage( SVMSG_SVO_74_LOAD_FILE, mFiles[l].GetFullFileName(), SvStl::SourceFileParams(StdMessageParams), SvOi::Err_25047_LoadFileFailed, SV_GUID_NULL, MB_YESNO ) )
				{
					//All other missing files will only be logged
					LogOnly = true;
				}
				else
				{
					//This stops loading without any further messages
					return SVMSG_SVO_IGNORE_EXCEPTION;
				}
			}
		}
	}

	rArray = mFiles;
	return Result;
}

SVImageObjectClassPtr SVAcquisitionClass::GetCircleBuffer()
{
	return m_AcquisitionBuffersPtr;
}

long SVAcquisitionClass::GetCircleBufferSize() const
{
	long l_Size = 0;

	if( !( m_AcquisitionBuffersPtr.empty() ) )
	{
		l_Size = m_AcquisitionBuffersPtr->size();
	}

	return l_Size;
}


HRESULT SVAcquisitionClass::CreateLightReference( int iBands )
{
    ASSERT(iBands > 0);

    HRESULT hrOk = S_FALSE;

	mLightReference.DeleteAll();

	if ( iBands > 0 )
	{
		BOOL bResult = (TRUE == mLightReference.Create( iBands ));

        if (TRUE == bResult )
		{
            hrOk = S_OK;
		}
        else
		{
            hrOk = S_FALSE;
		}
	}

	return hrOk;
}

HRESULT SVAcquisitionClass::LoadLightReference(SVLightReference& rLR)
{
	HRESULT hrOk = S_FALSE;

	return hrOk;
}

HRESULT SVAcquisitionClass::GetLightReference( SVLightReference& rLR ) const
{
	HRESULT hrOk = S_OK;

	rLR = mLightReference;

	return hrOk;
}

HRESULT SVAcquisitionClass::SetLightReference(SVLightReference& rLR, int iWhichBand /* = -1 */)
{
	HRESULT hrOk = S_OK;

    int iBeginBand=0, iEndBand = rLR.NumBands()-1;
    if (iWhichBand >= 0 && mLightReference.IsSameDimensions( rLR ))
    {
        iBeginBand = iEndBand = iWhichBand;
        mLightReference.SetBandData(rLR, iWhichBand);
    }
    else
    {
    	mLightReference = rLR;
    }

	SetLightReferenceImpl( mLightReference );

	return hrOk;
}

HRESULT SVAcquisitionClass::CreateLightReferenceBand( int iBand, int iAttributes )
{
	HRESULT hrOk = S_FALSE;

	if ( iBand < mLightReference.NumBands() && mLightReference.m_aLightReferenceBand.size() > 0 )
	{
		if ( mLightReference.m_aLightReferenceBand[iBand].Create( iAttributes ) )
		{
			hrOk = S_OK;
		}
	}

	return hrOk;
}

HRESULT SVAcquisitionClass::GetMaxLightReferenceValue( unsigned long ulType, long &rlValue ) const
{
	HRESULT hrOk = S_FALSE;

	return hrOk;
}

HRESULT SVAcquisitionClass::GetMinLightReferenceValue( unsigned long ulType, long &rlValue ) const
{
	HRESULT hrOk = S_FALSE;

	return hrOk;
}

HRESULT SVAcquisitionClass::GetLightReferenceValueStep( unsigned long ulType, unsigned long &rulValue ) const
{
	HRESULT hrOk = S_FALSE;

	return hrOk;
}

HRESULT SVAcquisitionClass::WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue )
{
	return E_NOTIMPL;
}

HRESULT SVAcquisitionClass::ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue )
{
	return E_NOTIMPL;
}

HRESULT SVAcquisitionClass::WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return E_NOTIMPL;
}

HRESULT SVAcquisitionClass::ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return E_NOTIMPL;
}

HRESULT SVAcquisitionClass::SetStandardCameraParameter( const SVDeviceParamWrapper& rwParam )
{
	return E_NOTIMPL;
}

bool SVAcquisitionClass::IsOnline() const
{
	return IsStarted();
}

HRESULT SVAcquisitionClass::SetLightReferenceImpl( SVLightReference& rLR )
{
	HRESULT hrOk = S_FALSE;

	return hrOk;
}

HRESULT SVAcquisitionClass::GetImageInfo( SVImageInfoClass *pImageInfo ) const
{
	HRESULT hrOk = S_FALSE;

	if ( nullptr != pImageInfo )
	{
		*pImageInfo = msvImageInfo;
	}
	return hrOk;
}

HRESULT SVAcquisitionClass::UnloadFiles()
{
	HRESULT hrOk = S_OK;

	for ( long l = mFiles.GetSize() - 1; -1 < l; l-- )
	{
		SVFileNameManagerClass::RemoveItem( &(mFiles[l]) );
	}

	mFiles.RemoveAll();

	return hrOk;
}

HRESULT SVAcquisitionClass::ResetLightReference()
{
	HRESULT hrOk = S_OK;

	return hrOk;
}

HRESULT SVAcquisitionClass::GetFileNameArraySize( long &rlSize ) const
{
	HRESULT hrOk = S_OK;

	rlSize = mFiles.GetSize();

	return hrOk;
}

HRESULT SVAcquisitionClass::GetFileName( long lIndex, SVFileNameClass &rFileName ) const
{
	HRESULT hrOk = S_FALSE;

	if ( 0 <= lIndex && lIndex < mFiles.GetSize() )
	{
		hrOk = S_OK;

		rFileName = mFiles[lIndex];
	}

	return hrOk;
}

HRESULT SVAcquisitionClass::GetLut( SVLut& lut )
{
	HRESULT hr = S_FALSE;
	SVLut lutdata;
	hr = GetLutImpl( lutdata );
	if ( S_OK == hr )
	{
		Lut().CopyNoTransform( lutdata );
		if ( nullptr == Lut().Info().GetTransform() )
		{
			Lut().SetTransform( SVDefaultLutTransform() );
			Lut().SetTransformOperation( SVLutTransformOperationNormal() );
			Lut().Transform();
		}
	}
	lut = Lut();
	hr = S_OK;
	return hr;
}

HRESULT SVAcquisitionClass::GetLutImpl( SVLut& lut )
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVAcquisitionClass::SetLut( const SVLutBand& lutBand )
{
	SVLut lut;
	GetLut( lut );
	lut.CopyBandData(lutBand);
	return SetLut( lut );
}

HRESULT SVAcquisitionClass::SetLut( const SVLut& lut, int iBand )
{
	HRESULT hr = S_FALSE;

	bool bSuccess = true;

	long l;
	long lSize;
	const SVLutTransformOperation* pTransform = lut.GetTransformOperation();
	if( bSuccess && pTransform )
	{
		bSuccess &= Lut().SetTransformOperation( *pTransform );
	}// end if

	lSize = lut.NumBands();
	bool bBandsEqual=true;
	if ( lSize > 0 )
	{
		for (l=1; l < lSize && bBandsEqual; l++)
		{
			bBandsEqual = bBandsEqual && lut(l) == lut(0);
		}
		if ( bBandsEqual )
		{
			pTransform = lut(0).GetTransformOperation();
			Lut().SetTransformOperation( *pTransform );
		}

		bSuccess = Lut().CopyBandData(lut, iBand);
	}

	if ( bSuccess && IsValidBoard() )
	{
		if (iBand == -1)
		{
			hr = SetLutImpl( Lut() );
		}
		else
		{
			hr = SetLutImpl( Lut() );	// Lut() already has modified band data
		}
	}
	else
	{
		hr = bSuccess ? S_OK : S_FALSE;
	}

	return hr;
}

HRESULT SVAcquisitionClass::SetLutImpl( const SVLut& lut )
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVAcquisitionClass::CreateLut( const SVLutInfo& info )
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVAcquisitionClass::DestroyLut()
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVAcquisitionClass::ResetLut()
{
	Lut().SetTransformOperation(SVLutTransformOperationNormal());
	Lut().Transform();
	return S_OK;
}

SVLut& SVAcquisitionClass::Lut()
{
	SVLutDeviceParam* pParam = nullptr;
	HRESULT hr = m_DeviceParams.GetParameter( DeviceParamLut, pParam );
	if ( S_OK == hr )
	{
		return pParam->lut;
	}
	
	SVLutDeviceParam lutparam;
	m_DeviceParams.SetParameter( DeviceParamLut, lutparam );
	hr = m_DeviceParams.GetParameter( DeviceParamLut, pParam );
	ASSERT( S_OK == hr && nullptr != pParam );
	return pParam->lut;
}

bool SVAcquisitionClass::IsValidBoard() const
{
    return false;
}

HRESULT SVAcquisitionClass::SetNumberOfBands( int NumberOfBands )
{
	HRESULT Result( S_OK );

	SetBandMaxSize( NumberOfBands );
	SetBandSize( NumberOfBands );
	
	if( m_LUTAndLRSet )
	{
		CreateLightReference( NumberOfBands );
	}

	return Result;
}

HRESULT SVAcquisitionClass::GetNextIndex( SVDataManagerHandle &rDMHandle ) const
{
	return GetNextIndex( rDMHandle, SV_ACQUISITION );
}

HRESULT SVAcquisitionClass::GetNextIndex( SVDataManagerHandle &rDMHandle, SVDataManagerLockTypeEnum p_LockType ) const
{
	HRESULT hrOk = S_FALSE;
		
	//	This will lock the index!
	hrOk = TheSVDataManager.GetNextAvailableBufferIndexNoWait( m_pDataManagerHandle, p_LockType, rDMHandle );

	if( S_OK != hrOk )
	{
		SVStringArray msgList;
		msgList.push_back(GetDeviceName());
		SvStl::MessageMgrStd Exception( SvStl::LogOnly );
		Exception.setMessage( static_cast<DWORD> (hrOk), SvOi::Tid_SVAcquisitionClass_GetNextIndex, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}
	
	return hrOk;
}

HRESULT SVAcquisitionClass::GetDeviceParameters( SVDeviceParamCollection& rDeviceParams )
{
	rDeviceParams = m_DeviceParams;
	return S_OK;
}

HRESULT SVAcquisitionClass::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_OK;

	// don't do a straight assignment ( m_DeviceParams = rDeviceParams )
	// only copy those parameters that are specified.
	SVDeviceParamMap::const_iterator iter;
	for (iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); ++iter)
	{
		const SVDeviceParamWrapper& w = iter->second;

		if ( nullptr != ((const SVDeviceParam*) w) )
		{
			m_DeviceParams.GetParameter( iter->first ) = w;
		}
	}

	return hr;
}

HRESULT SVAcquisitionClass::IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams )
{
	return E_NOTIMPL;	// derived class needs to override this
}


HRESULT SVAcquisitionClass::GetCameraFileParameters( SVDeviceParamCollection& rDeviceParams )
{
	rDeviceParams = m_CameraFileDeviceParams;
	return S_OK;
}

HRESULT SVAcquisitionClass::GetHardwareDeviceCapabilities( SVDeviceParamCollection& rDeviceParams )
{
	rDeviceParams = HardwareCapabilities();
	return S_OK;
}

HRESULT SVAcquisitionClass::SetHardwareDeviceCapabilities( const SVDeviceParamCollection& rDeviceParams )
{
	ASSERT(false);
	return S_FALSE;	// derived class needs to implement this function
}

SVDeviceParamCollection& SVAcquisitionClass::HardwareCapabilities()
{
	return m_DeviceParams;
}

HRESULT SVAcquisitionClass::ReadCameraFile( const SVString& , SVDeviceParamCollection& rParams )
{
	return E_NOTIMPL;
}

bool SVAcquisitionClass::IsDigitizerSubsystemValid() const
{
	return SVDigitizerProcessingClass::Instance().IsValidDigitizerSubsystem(mcsDigName);
}

DWORD WINAPI SVAcquisitionClass::SingleGrabHelperFn(LPVOID lpParameter)
{
	SVAcquisitionClass* pAcq = reinterpret_cast< SVAcquisitionClass* >( lpParameter );
	while( pAcq->mbTempOnline )
	{
		Sleep(0);
	}
	return 0;
}

HRESULT SVAcquisitionClass::SingleGrab( SVSmartHandlePointer p_SingleGrabHandle )
{
	HRESULT l_Status = S_OK;

	if( !IsOnline() )
	{
		m_SingleGrabHandle = p_SingleGrabHandle;
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVAcquisitionClass::AllocDigitizer( SVDeviceParamCollection& rDevice )
{
	return E_NOTIMPL;
}

// do nothing, just a stub
void SVAcquisitionClass::ClearDeviceIdentifier()
{
}

BOOL SVAcquisitionClass::GetCurrentIndex( SVDataManagerHandle& rDMIndexHandle ) const
{
	BOOL l_Status = !( m_AcquisitionBuffersPtr.empty() );

	if( l_Status )
	{
		l_Status = m_AcquisitionBuffersPtr->GetCurrentIndex( rDMIndexHandle );
	}

	return l_Status;
}

BOOL SVAcquisitionClass::GetCurrentIndex( SVDataManagerHandle& rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType ) const
{
	BOOL l_Status = !( m_AcquisitionBuffersPtr.empty() );

	if( l_Status )
	{
		l_Status = m_AcquisitionBuffersPtr->GetCurrentIndex( rDMIndexHandle, p_LockType );
	}

	return l_Status;
}

BOOL SVAcquisitionClass::SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle )
{
	BOOL l_Status = !( m_AcquisitionBuffersPtr.empty() );

	if( l_Status )
	{
		l_Status = m_AcquisitionBuffersPtr->SetCurrentIndex( rDMIndexHandle );
	}

	return l_Status;
}

BOOL SVAcquisitionClass::SetCurrentIndex( const SVDataManagerHandle& rDMIndexHandle, SVDataManagerLockTypeEnum p_LockType )
{
	BOOL l_Status = !( m_AcquisitionBuffersPtr.empty() );

	if( l_Status )
	{
		l_Status = m_AcquisitionBuffersPtr->SetCurrentIndex( rDMIndexHandle, p_LockType );
	}

	return l_Status;
}
	
void SVAcquisitionClass::DumpDMInfo( LPCTSTR p_szName ) const
{
	if( ! m_pDataManagerHandle.empty() )
	{
		m_pDataManagerHandle->Dump( p_szName );
	}
}

SVClock::SVTimeStamp SVAcquisitionClass::GetTimeStamp() const
{
	SVClock::SVTimeStamp l_TimeStamp = SVClock::GetTimeStamp();
	return l_TimeStamp;
}

unsigned long SVAcquisitionClass::GetBufferWidth() const
{
	unsigned long l_Width = 0;
	RECT l_Rect;

	if( S_OK == msvImageInfo.GetOutputRectangle( l_Rect ) )
	{
		l_Width = l_Rect.right;
	}

	return l_Width;
}

unsigned long SVAcquisitionClass::GetBufferHeight() const
{
	unsigned long l_Height = 0;
	RECT l_Rect;

	if( S_OK == msvImageInfo.GetOutputRectangle( l_Rect ) )
	{
		l_Height = l_Rect.bottom;
	}

	return l_Height;
}

int SVAcquisitionClass::GetBufferFormat() const
{
	int l_Format = SVImageFormatUnknown;

	msvImageInfo.GetImageProperty( SVImagePropertyFormat, l_Format );

	return l_Format;
}

HRESULT SVAcquisitionClass::GetNextBuffer( SVImageBufferInterface& p_rBuffer )
{
	SVDataManagerHandle l_Handle;

	HRESULT l_Status = GetNextIndex( l_Handle );

	if( S_OK == l_Status )
	{
		if( !( m_AcquisitionBuffersPtr.empty() ) && SetCurrentIndex( l_Handle ) )
		{
			SVSmartHandlePointer l_ImageHandle;

			if( m_AcquisitionBuffersPtr->GetImageHandle( l_Handle.GetIndex(), l_ImageHandle ) && ! l_ImageHandle.empty() )
			{
				l_Status = p_rBuffer.Assign( l_ImageHandle, l_Handle );
			}
			else
			{
				l_Status = E_FAIL;

				p_rBuffer.clear();
			}
		}
		else
		{
			l_Status = E_FAIL;

			p_rBuffer.clear();
		}
	}
	else
	{
		p_rBuffer.clear();
	}

	return l_Status;
}

HRESULT SVAcquisitionClass::UpdateWithCompletedBuffer( const SVImageBufferInterface& p_rBuffer )
{
	HRESULT l_Status = S_OK;

	if( ! m_SingleGrabHandle.empty() )
	{
		SVImageBufferHandleImage l_From;
		SVImageBufferHandleImage l_To;

		if( !( p_rBuffer.m_ImageHandle.empty() ) )
		{
			p_rBuffer.m_ImageHandle->GetData( l_From );
			m_SingleGrabHandle->GetData( l_To );
		}

		if( !( l_From.empty() ) && !( l_To.empty() ) )
		{
			SVMatroxBufferInterface::CopyBuffer( l_To.GetBuffer(), l_From.GetBuffer() );
		}

		m_SingleGrabHandle.clear();

		this->mbTempOnline = false;
	}

	SVODataResponseClass l_Response;

	l_Status = l_Response.mDMHandle.Assign( p_rBuffer.m_IndexHandle, p_rBuffer.m_IndexHandle.GetLockType() );

	if( S_OK == l_Status )
	{
		m_ImageAquired = true;

		l_Status = l_Response.SetStartTick( p_rBuffer.GetStartFrameTimeStamp() );

		if( S_OK == l_Status )
		{
			l_Status = l_Response.SetEndTick( p_rBuffer.GetEndFrameTimeStamp() );
		}

		if( S_OK == l_Status )
		{
			l_Status = l_Response.SetIsValid( true );
		}

		if( S_OK == l_Status )
		{
			l_Status = l_Response.SetIsComplete( true );
		}

	}

	if( S_OK == l_Status )
	{
		l_Status = Notify( l_Response );
	}

	return l_Status;
}

HRESULT SVAcquisitionClass::StoreLastImage()
{
	HRESULT l_Status = S_OK;
		
	if( m_ImageAquired && !( m_AcquisitionBuffersPtr.empty() ) )
	{
		l_Status = m_AcquisitionBuffersPtr->CopyToBSTR( m_LastImage );
	}

	return l_Status;
}

HRESULT SVAcquisitionClass::RestoreLastImage()
{
	HRESULT l_Status = S_OK;

	if( ! m_ImageAquired && nullptr != m_LastImage && !( m_AcquisitionBuffersPtr.empty() ) )
	{
		l_Status = m_AcquisitionBuffersPtr->CopyFromBSTR( m_LastImage );
	}

	return l_Status;
}

HRESULT SVAcquisitionClass::StartDigitizer()
{
	HRESULT hrOk = S_OK;
	
	if ( IsDigitizerSubsystemValid() )
	{
		hrOk = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->RegisterBufferInterface( m_hDigitizer, this );

		if( S_OK == hrOk )
		{
			hrOk = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->Start( m_hDigitizer );
		}
	}
	else
	{
		hrOk = E_FAIL;
	}

	return hrOk;
}

