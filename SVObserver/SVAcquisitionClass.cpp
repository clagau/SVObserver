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
#include <fstream>
#include "SVAcquisitionClass.h"

#include "SVDataManagerLibrary/DataManager.h"
#include "SVImageLibrary/SVDigitizerLoadLibraryClass.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVMessage/SVMessage.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVAcquisitionConstructParams.h"
#include "SVDigitizerProcessingClass.h"
#include "SVFileNameManagerClass.h"
#include "SVGlobal.h"
#include "SVImageObjectClass.h"
#include "SVImageProcessingClass.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

SVAcquisitionClass::SVAcquisitionClass( const SVAcquisitionConstructParams& p_rParams )
: SVODataDeviceClass( p_rParams.m_DeviceName.ToString() )
{
	mbIsBufferCreated = false;
	mbTempOnline = false;
	m_ImageAquired = false;
	m_LastImage = NULL;
	mulSize = 0;

	mlStartFrameIndex = -1;

	m_hDigitizer = NULL;

	m_AcquisitionBuffersPtr = new SVImageObjectClass;

	msvImageInfo.SetExtentProperty( SVExtentPropertyPositionPoint, 0 );

	SetDeviceName( p_rParams.m_DeviceName.ToString() );
	SetDigName( p_rParams.m_DigitizerName.ToString() );
	
	SetDigNumber( p_rParams.m_DigNumber );
	SetChannel( p_rParams.m_Channel );
	SetBandSize( p_rParams.m_BandSize );
	SetBandMaxSize( p_rParams.m_BandMaxSize );
	
	if( p_rParams.m_LUTAndLRSet )
	{
		CreateLightReference( p_rParams.m_LRBandSize, p_rParams.m_LRBrightness, p_rParams.m_LRConstrast );
		CreateLut( p_rParams.m_LUTInfo );
	}
}

SVAcquisitionClass::~SVAcquisitionClass()
{
	if( m_LastImage != NULL )
	{
		::SysFreeString( m_LastImage );

		m_LastImage = NULL;
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
	if( m_LastImage != NULL )
	{
		::SysFreeString( m_LastImage );

		m_LastImage = NULL;
	}

	m_pDataManagerHandle.clear();
	m_AcquisitionBuffersPtr.clear();
	m_DeviceParams.Clear();
	m_CameraFileDeviceParams.Clear();

	mLightReference.Reset();

	SVFileNameManagerClass svFileMgr;

	for ( long l = mFiles.GetSize() - 1; -1 < l; l-- )
	{
		svFileMgr.RemoveItem( &(mFiles[l]) );
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

	if( hrOk == S_OK )
	{
		hrOk = l_Status;
	}

	l_Status = DestroyBuffers();

	if( hrOk == S_OK )
	{
		hrOk = l_Status;
	}

	SVFileNameManagerClass svFileMgr;

	for ( long l = mFiles.GetSize() - 1; -1 < l; l-- )
	{
		svFileMgr.RemoveItem( &(mFiles[l]) );
	}

	return hrOk;
}

HRESULT SVAcquisitionClass::Start()
{
	HRESULT hrOk = SVODataDeviceClass::Start();

	m_ImageAquired = false;
	mlStartFrameIndex = -1;
	
	if ( hrOk == S_OK )
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
	
	if ( hrOk == S_OK )
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

		if( hr == S_OK )
		{
			hr = l_Temp;
		}

		l_Temp = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->UnregisterBufferInterface( m_hDigitizer );

		if( hr == S_OK )
		{
			hr = l_Temp;
		}
	}
	else
	{
		if( hr == S_OK )
		{
			hr = E_FAIL;
		}
	}
	
	l_Temp = SVODataDeviceClass::Stop();

	if( hr == S_OK )
	{
		hr = l_Temp;
	}

	return hr;
}

HRESULT SVAcquisitionClass::Reset()
{
	HRESULT l_Status = S_OK;
	HRESULT l_Temp = SVODataDeviceClass::Reset();

	if( l_Status == S_OK )
	{
		l_Status = l_Temp;
	}

	return l_Status;
}

HRESULT SVAcquisitionClass::CreateBuffers( SVImageInfoClass IInfo, unsigned long ulSize )
{
	HRESULT hrOk = FALSE;
	
	hrOk = DestroyBuffers();
	if ( hrOk == S_OK )
	{
		_bstr_t bsName = DeviceName();

		hrOk = TheSVDataManager.CreateManagedIndexArray( m_pDataManagerHandle, bsName, ulSize );

		mulSize = ulSize + 3;
		msvImageInfo = IInfo;
		
		if( hrOk == S_OK )
		{
			if ( IsDigitizerSubsystemValid() )
			{
				hrOk = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->CreateBuffers( m_hDigitizer, ulSize + 3 );
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

			l_Status = l_Status && ( m_AcquisitionBuffersPtr->ResetObject() == S_OK );
			l_Status = l_Status && GetNextIndex( l_Handle ) == S_OK;
			l_Status = l_Status && SetCurrentIndex( l_Handle );

			if( l_Status )
			{
				hrOk = S_OK;
			}
			else
			{
				hrOk = S_FALSE;
			}
		}
		else
		{
			hrOk = S_FALSE;
		}
		
		if (S_OK == hrOk  )
		{
			mbIsBufferCreated = true;
		}
		else
		{
			DestroyBuffers();
		}
	}
	
	return hrOk;
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
		if ( SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->DestroyBuffers( m_hDigitizer ) != S_OK )
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
	HRESULT hrOk = S_OK;

	long l( 0 );

	SVFileNameManagerClass svFileMgr;

	for ( l = mFiles.GetSize() - 1; -1 < l; l-- )
	{
		svFileMgr.RemoveItem( &(mFiles[l]) );
	}

	mFiles = rArray;
	m_CameraFileDeviceParams.Clear();

	for ( l = 0; hrOk == S_OK && l < mFiles.GetSize(); l++ )
	{
		if ( ! svFileMgr.AddItem( &(mFiles[l]) ) )
		{
			hrOk = S_FALSE;
            ASSERT(FALSE);
		}
	}

	rArray = mFiles;
	return hrOk;
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


HRESULT SVAcquisitionClass::CreateLightReference(int iBands, int iBrightness, int iContrast)
{
    ASSERT(iBands > 0);

    HRESULT hrOk = S_FALSE;

	mLightReference.DeleteAll();

	if ( iBands > 0 )
	{
		BOOL bResult = (mLightReference.Create( iBands ) == TRUE);

        if (bResult == TRUE)
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

	if ( pImageInfo != NULL )
	{
		*pImageInfo = msvImageInfo;
	}
	return hrOk;
}

HRESULT SVAcquisitionClass::UnloadFiles()
{
	HRESULT hrOk = S_OK;

	SVFileNameManagerClass svFileMgr;

	for ( long l = mFiles.GetSize() - 1; -1 < l; l-- )
	{
		svFileMgr.RemoveItem( &(mFiles[l]) );
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

CString SVAcquisitionClass::GetRootDeviceName() const
{
	return DeviceName();
}

HRESULT SVAcquisitionClass::GetLut( SVLut& lut )
{
	HRESULT hr = S_FALSE;
	SVLut lutdata;
	hr = GetLutImpl( lutdata );
	if ( hr == S_OK )
	{
		Lut().CopyNoTransform( lutdata );
		if ( Lut().Info().GetTransform() == NULL )
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
	SVLutDeviceParam* pParam = NULL;
	HRESULT hr = m_DeviceParams.GetParameter( DeviceParamLut, pParam );
	if ( hr == S_OK )
	{
		return pParam->lut;
	}
	else
	{
		SVLutDeviceParam lutparam;
		m_DeviceParams.SetParameter( DeviceParamLut, lutparam );
		hr = m_DeviceParams.GetParameter( DeviceParamLut, pParam );
		ASSERT( hr == S_OK && pParam != NULL );
		return pParam->lut;
	}
}

bool SVAcquisitionClass::IsValidBoard() const
{
    return false;
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

	if( hrOk != S_OK )
	{
		SVException l_Log;
		SVString l_Message;

		l_Message.Format( _T( "SVAcquisitionClass::GetNextIndex-Name=%s" ), GetDeviceName() );

		SETEXCEPTION1( l_Log, hrOk, l_Message.ToString() );
		l_Log.LogException( l_Message.ToString() );
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

		if ( ((const SVDeviceParam*) w) != NULL )
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
	ASSERT(FALSE);
	return S_FALSE;	// derived class needs to implement this function
}

SVDeviceParamCollection& SVAcquisitionClass::HardwareCapabilities()
{
	return m_DeviceParams;
}

HRESULT SVAcquisitionClass::ReadCameraFile( const CString& sFile, SVDeviceParamCollection& rParams )
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

HRESULT SVAcquisitionClass::DeallocDigitizer( SVDeviceParamCollection& rDevice )
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

	if( msvImageInfo.GetOutputRectangle( l_Rect ) == S_OK )
	{
		l_Width = l_Rect.right;
	}

	return l_Width;
}

unsigned long SVAcquisitionClass::GetBufferHeight() const
{
	unsigned long l_Height = 0;
	RECT l_Rect;

	if( msvImageInfo.GetOutputRectangle( l_Rect ) == S_OK )
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

	if( l_Status == S_OK )
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

	if( l_Status == S_OK )
	{
		m_ImageAquired = true;

		l_Status = l_Response.SetStartTick( p_rBuffer.GetStartFrameTimeStamp() );

		if( l_Status == S_OK )
		{
			l_Status = l_Response.SetEndTick( p_rBuffer.GetEndFrameTimeStamp() );
		}

		if( l_Status == S_OK )
		{
			l_Status = l_Response.SetIsValid( TRUE );
		}

		if( l_Status == S_OK )
		{
			l_Status = l_Response.SetIsComplete( TRUE );
		}

	}

	if( l_Status == S_OK )
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

	if( ! m_ImageAquired && m_LastImage != NULL && !( m_AcquisitionBuffersPtr.empty() ) )
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

		if( hrOk == S_OK )
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVAcquisitionClass.cpp_v  $
 * 
 *    Rev 1.3   07 Mar 2014 18:02:16   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 * Added DEBUG_NEW.
 * Made  methods const.
 * Moved mbIsBufferCreated here from SVMatroxGigeAcquisitionClass.
 * Added virtual method IsBufferCreated.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Oct 2013 11:54:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:01:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.59   29 Apr 2013 14:04:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:23:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.58   04 Oct 2012 10:36:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.57   16 Sep 2011 15:37:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated acquisition device classes to handle new data management and callback fucntionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.56   05 Jul 2011 14:45:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  724
 * SCR Title:  Add Resource Logging to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to for Last Inspected to defined reference catagories.  Added new logging based on low availiable resources.  Added offline Data Manager dumps of reference counts.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.55   30 Mar 2011 15:52:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.54   22 Mar 2011 07:42:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.53   16 Mar 2011 13:19:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52   25 Feb 2011 12:07:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.51   10 Feb 2011 14:24:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.50   27 Jan 2011 10:47:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.49   04 Nov 2010 13:18:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48   07 Jul 2010 16:10:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to make Single Grab functionality work for IEEE1394 and GigE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   24 Feb 2010 10:30:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to handle storing and restoring the image buffers when the system is going on and off line.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   15 Feb 2010 12:22:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   15 Dec 2009 14:55:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   22 Oct 2009 19:27:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   14 Oct 2009 17:00:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Acquisition class to handle go-online functionality correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   03 Sep 2009 09:59:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   30 Jul 2009 11:12:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   04 Nov 2008 11:26:52   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  639
 * SCR Title:  Update SVObserver to better handle going offline while triggering
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed condition on call to SVOAcquisitionDeviceInterfaceClass::Start()
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   16 Sep 2008 13:32:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetDigitizerSubsystem, GetImageSubsystem, IsValidDigitizerSubsystem, and IsValidImageSubsystem to use device name to support multiple acquisition devices.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   21 Jun 2007 11:51:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37.1.1   25 Aug 2009 17:13:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new threading model.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37.1.0   14 Jul 2009 11:15:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated buffer transfer methodology, data management, and signal processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   29 Nov 2006 15:39:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  583
 * SCR Title:  Add Single Grab capability to the Format7 Dialog in SVObserver
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added CreateSingleGrabImageBuffer method
 * Added DestroySingleGrabImageBuffer method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   29 Nov 2006 14:22:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added DCamAcquistion Proxies.
 * Added ClearDeviceIdentifier
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   25 Aug 2006 07:49:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  567
 * SCR Title:  Update SVObserver to accept Intek driver for D-series SVIMs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed problem with LUTbeing setup when the LUT bands were 0.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   20 Jan 2006 07:52:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  541
 * SCR Title:  Upgrade SVObserver for Pan Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix a debugging issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   08 Nov 2005 15:36:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   15 Sep 2005 07:33:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed potential debug code crash.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   17 Feb 2005 12:49:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to use new ResetObject Methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   02 Nov 2004 15:04:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  459
 * SCR Title:  Fix a problem with inspection lock-up
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added logging
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   18 Feb 2004 08:39:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   moved data manager buffer allocation to derived classes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   05 Dec 2003 14:58:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   un-broke RGB color handling of SetLut
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   05 Dec 2003 14:05:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  400
 * SCR Title:  Fix Bug with Configuration saving (wrong path for the camera file)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   LoadFile - parameter that was passed in was never being updated.  The result caused the file location to revert back to the orginally place that it was picked.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   17 Nov 2003 09:07:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  335
 * SCR Title:  Fix Light Reference and LUT problems with RGB Mono systems
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed problem updating the LUT type
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   31 Oct 2003 08:19:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented Stop virtual member fn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   02 Oct 2003 10:48:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Add new code to handle the defaulting of unset light reference values to the values in the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   22 Sep 2003 14:54:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed Lut handling from member var to DeviceParam
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   17 Sep 2003 09:00:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added AllocDigitizer and DeallocDigitizer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   07 Aug 2003 12:39:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added SingleGrab
 * changed CreateImageBuffer to CreateImageAcquisitionBuffer
 * removed setting mpImaging to NULL in DestroyBuffers
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   31 Jul 2003 10:28:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added IsDigitizerSubsystemValid and IsImageSubsystemValid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   08 Jul 2003 08:36:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changes to use DigitizerSubsystem and to use SVDeviceParamCollection
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   22 May 2003 10:37:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  355
 * SCR Title:  Save tool freeform LUT with the configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed bug with the SetLut function not setting the transform method correctly and that it gets saved correctly
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   22 May 2003 09:28:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  355
 * SCR Title:  Save tool freeform LUT with the configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed bug with the SetLut function not setting the transform method correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   17 Apr 2003 16:14:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   19 Mar 2003 14:24:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated UnlockIndex method to ignore invalid index error code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   13 Mar 2003 08:58:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  335
 * SCR Title:  Fix Light Reference and LUT problems with RGB Mono systems
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetRootDevice
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   10 Mar 2003 15:14:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved lDMIndex functionality to SVCorecoAcquisitionClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   27 Feb 2003 16:23:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  314
 * SCR Title:  SVIM stops inspecting when triggered very fast
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Updated informational trace information to track DataManager and buffer locking problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   24 Feb 2003 10:30:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed merge error
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   20 Feb 2003 11:55:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed problem with loading configurations on different hardware
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   20 Feb 2003 09:58:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed some bugs in SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   12 Feb 2003 10:31:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated EndProcess, GetNextIndex, NotifyEndFrame, and CreateBuffers methods to resolve issues with ViperRGB Board.
 * Added IsBufferEmpty method to allow verification of buffer state.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Feb 2003 10:06:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed mulType and GetType and type enumeration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   06 Feb 2003 10:26:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added Lut functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   28 Jan 2003 16:36:52   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added NotifyStartFrame, NotifyEndFrame, LockBuffer, UnlockBuffer, GetDataManagerHandle, GetNextIndex, LockRequestResource, UnlockRequestResource, EndProcess, LockIndex, UnlockIndex, AttachResponseToRequest and CheckQueues methods to the class.  Modified constructor, Start, GetImageInfo, GetCircleBuffer, DestroyBuffers and CreateBuffers methods to handle new Data Manager and image buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Jan 2003 15:34:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added virtual method IsValidBoard()
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Jan 2003 09:36:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added ResetLut
 * Modified Lut functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jan 2003 15:24:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added:
 * SetImageProcessingClass
 * CreateLut
 * DestroyLut
 * GetLut
 * SetLut
 * member variable mLut
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Dec 2002 13:58:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  311
 * SCR Title:  Fix Light Reference save for RGB Mono
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added GetRootDeviceName method to SVAcquisitionClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 12:43:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
