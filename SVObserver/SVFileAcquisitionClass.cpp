//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionClass 
//* .File Name       : $Workfile:   SVFileAcquisitionClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   07 Mar 2014 18:14:42  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVFileAcquisitionClass.h"

#include "SVDataManagerLibrary/DataManager.h"
#include "SVImageLibrary/SVDigitizerLoadLibraryClass.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVOINIClass.h"

#include "SVDigitizerProcessingClass.h"
#include "SVImageObjectClass.h"
#include "SVImageProcessingClass.h"
#include "SVStatusLibrary/MessageManagerResource.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "TextDefinesSvO.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

SVFileAcquisitionClass::SVFileAcquisitionClass( const SVAcquisitionConstructParams& p_rParams )
: SVAcquisitionClass( p_rParams )
{
	mbIsFilesLoaded = false;
}

SVFileAcquisitionClass::~SVFileAcquisitionClass()
{
	DestroyLocal();
}

bool SVFileAcquisitionClass::IsValid() const
{
	bool bOk = true;

	bOk = m_hDigitizer != NULL && bOk;
	bOk = SVAcquisitionClass::IsValid() && bOk;

	return bOk;
}

bool SVFileAcquisitionClass::IsValidBoard() const
{
	bool bOk = true;
	
	bOk = m_hDigitizer != NULL && bOk;

	return bOk;
}

HRESULT SVFileAcquisitionClass::Destroy()
{
	HRESULT hrOk = DestroyLocal();

	if ( SVAcquisitionClass::Destroy() != S_OK )
	{
		hrOk = S_FALSE;
	}
	return hrOk;
}

HRESULT SVFileAcquisitionClass::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_OK;

	// Send these to the FileAcquisition device
	if ( IsDigitizerSubsystemValid() )
	{
		hr = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->SetParameters( m_hDigitizer, &rDeviceParams );
	}
	return hr;
}

HRESULT SVFileAcquisitionClass::LoadFiles(SVFileNameArrayClass& rFiles)
{
	HRESULT hrOk = S_OK;
	GetCameraImageInfo( &msvImageInfo );
	return hrOk;
}

HRESULT SVFileAcquisitionClass::UnloadFiles()
{
	HRESULT hrOk = S_OK;
	return hrOk;
}

HRESULT SVFileAcquisitionClass::GetLightReference( SVLightReference& rArray ) const
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionClass::SetLightReference( SVLightReference& rArray, int iBand /* = -1 */)
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionClass::CreateLightReference(int iBands, int iBrightness, int iContrast)
{
	HRESULT hrOk = S_FALSE;
	return hrOk;
}

HRESULT SVFileAcquisitionClass::CreateLightReferenceBand( int iBand, int iAttributes )
{
	HRESULT hrOk = S_FALSE;
	return hrOk;
}

HRESULT SVFileAcquisitionClass::ResetLightReference()
{
	HRESULT hrOk = S_FALSE;
	return hrOk;
}

HRESULT SVFileAcquisitionClass::GetImageInfo(SVImageInfoClass *pImageInfo) const
{
	HRESULT hrOk = S_FALSE;

	if ( pImageInfo != NULL )
	{
		*pImageInfo = msvImageInfo;
		
		hrOk = S_OK;
	}
	return hrOk;
}

HRESULT SVFileAcquisitionClass::DestroyLocal()
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

	if ( mbIsFilesLoaded )
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

	return hrOk;
}

HRESULT SVFileAcquisitionClass::GetLut( SVLut& lut )
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionClass::SetLut( const SVLutBand& lutBand )
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionClass::SetLut( const SVLut& lut, int iBand )
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionClass::ResetLut()
{
	HRESULT hr = S_FALSE;
	return hr;
}	

HRESULT SVFileAcquisitionClass::GetLutImpl( SVLut& lut )
{
	HRESULT l_hrOk = S_FALSE;
	return l_hrOk;
}

HRESULT SVFileAcquisitionClass::GetCameraImageInfo(SVImageInfoClass *pImageInfo)
{
	HRESULT hrOk = S_FALSE;

	unsigned long bufWidth = 640;
	unsigned long bufHeight = 480;
	unsigned long uiBandNumber = 1;
	int iFormat = SVImageFormatUnknown;

	if ( IsValidBoard() && IsDigitizerSubsystemValid() )
	{
		SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->GetBufferHeight( m_hDigitizer, &bufHeight );
		SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->GetBufferWidth( m_hDigitizer, &bufWidth );
		SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->GetBufferFormat( m_hDigitizer, &iFormat );
	}// end if subsystem available
	else
	{
		// get camera format from DeviceParams
		const SVCameraFormatsDeviceParam* pParam = m_DeviceParams.Parameter( DeviceParamCameraFormats ).DerivedValue( pParam );
		if ( pParam )
		{
			bufHeight = pParam->options.find( _T("default") )->second.m_lHeight;
			bufWidth = pParam->options.find( _T("default") )->second.m_lWidth;
			iFormat = pParam->options.find( _T("default") )->second.m_eImageType;
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
		case SVImageFormatRGB8888:  // RGB
		{
			uiBandNumber = 3;
			break;
		}
	}

	pImageInfo->SetImageProperty( SVImagePropertyFormat, iFormat );
	pImageInfo->SetImageProperty( SVImagePropertyPixelDepth, 8 );
	pImageInfo->SetImageProperty( SVImagePropertyBandNumber, uiBandNumber );
	pImageInfo->SetImageProperty( SVImagePropertyBandLink, 0 );

	pImageInfo->SetExtentProperty( SVExtentPropertyOutputPositionPoint, 0 );
	pImageInfo->SetExtentProperty( SVExtentPropertyWidth, bufWidth );
	pImageInfo->SetExtentProperty( SVExtentPropertyHeight, bufHeight );

	hrOk = S_OK;

	if ( hrOk != S_OK )
	{
		SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_Acquisition_SourceInformationFailed, StdMessageParams, SvOi::Err_10050 ); 
	}

	return hrOk;
}

