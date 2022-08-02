//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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

#include "Triggering/SVDigitizerLoadLibraryClass.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVDigitizerProcessingClass.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

namespace SvIe
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
	bool result = (0 != m_hDigitizer);
	result = SVAcquisitionClass::IsValid() && result;

	return result;
}

bool SVFileAcquisitionClass::IsValidBoard() const
{
	bool bOk = true;
	
	bOk = 0 != m_hDigitizer && bOk;

	return bOk;
}

HRESULT SVFileAcquisitionClass::Destroy()
{
	HRESULT hrOk = DestroyLocal();

	if ( S_OK != SVAcquisitionClass::Destroy() )
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
		hr = m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str())->SetParameters( m_hDigitizer, rDeviceParams );
	}
	return hr;
}


HRESULT SVFileAcquisitionClass::SetDeviceParameter(int parameter, const _variant_t& rValue)
{
	HRESULT hr = S_OK;

	// Send these to the FileAcquisition device
	if (IsDigitizerSubsystemValid())
	{
		hr = m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str())->ParameterSetValue(m_hDigitizer, parameter, rValue);
	}
	return hr;
}



HRESULT SVFileAcquisitionClass::LoadFiles(const SVFileNameArrayClass& )
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

HRESULT SVFileAcquisitionClass::GetLightReference( SVLightReference&  ) const
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionClass::SetLightReference( SVLightReference& , int  /* = -1 */)
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionClass::CreateLightReference( int  )
{
	HRESULT hrOk = S_FALSE;
	return hrOk;
}

HRESULT SVFileAcquisitionClass::CreateLightReferenceBand( int , int  )
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

	if ( nullptr != pImageInfo )
	{
		*pImageInfo = msvImageInfo;
		
		hrOk = S_OK;
	}
	return hrOk;
}

HRESULT SVFileAcquisitionClass::DestroyLocal()
{
	HRESULT hrOk = S_OK;

	if ( S_OK != Stop() )
	{
		hrOk = S_FALSE;
	}

	if ( S_OK != DestroyBuffers() )
	{
		hrOk = S_FALSE;
	}

	if ( mbIsFilesLoaded )
	{
		if ( S_OK != UnloadFiles() )
		{
			hrOk = S_FALSE;
		}
	}

	if ( S_OK != ResetLightReference() )
	{
		hrOk = S_FALSE;
	}

	return hrOk;
}

HRESULT SVFileAcquisitionClass::GetLut( SVLut&  )
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionClass::SetLut( const SVLutBand&  )
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionClass::SetLut(const SVLut& , int)
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionClass::ResetLut()
{
	HRESULT hr = S_FALSE;
	return hr;
}	

HRESULT SVFileAcquisitionClass::GetLutImpl(SVLut&)
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
	int iFormat = SvDef::SVImageFormatUnknown;

	if ( IsValidBoard() && IsDigitizerSubsystemValid() )
	{
		SvTrig::SVDigitizerLoadLibraryClass* pLibrary = m_rDigitizerProc.GetDigitizerSubsystem(m_DigName.c_str());
		if (nullptr != pLibrary)
		{
			bufHeight = pLibrary->GetBufferHeight(m_hDigitizer);
			bufWidth = pLibrary->GetBufferWidth(m_hDigitizer);
			iFormat = pLibrary->GetBufferFormat(m_hDigitizer);
		}
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
		case SvDef::SVImageFormatMono8:  // Mono
		{
			uiBandNumber = 1;
			break;
		}
		case SvDef::SVImageFormatBGR888X:  // RGB
		{
			uiBandNumber = 3;
			break;
		}
	}

	pImageInfo->SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyFormat, iFormat );
	pImageInfo->SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyPixelDepth, 8 );
	pImageInfo->SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandNumber, uiBandNumber );
	pImageInfo->SetImageProperty( SvDef::SVImagePropertyEnum::SVImagePropertyBandLink, 0 );

	pImageInfo->SetExtentProperty( SvPb::SVExtentPropertyOutputPositionPoint, SVPoint<double>(0.0, 0.0));
	pImageInfo->SetExtentProperty( SvPb::SVExtentPropertyWidth, bufWidth );
	pImageInfo->SetExtentProperty( SvPb::SVExtentPropertyHeight, bufHeight );

	hrOk = S_OK;

	if ( S_OK != hrOk )
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display );
		Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Acquisition_SourceInformationFailed, SvStl::SourceFileParams(StdMessageParams)); 
	}

	return hrOk;
}

} //namespace SvIe
