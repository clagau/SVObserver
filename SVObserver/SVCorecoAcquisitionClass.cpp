//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision, Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCorecoAcquisitionClass
//* .File Name       : $Workfile:   SVCorecoAcquisitionClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   07 Mar 2014 18:12:00  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVCorecoAcquisitionClass.h"

#include "SVDataManagerLibrary/DataManager.h"
#include "SVImageLibrary/SVDigitizerLoadLibraryClass.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVOINIClass.h"

#include "SVImageObjectClass.h"
#include "SVImageProcessingClass.h"
#include "SVDigitizerProcessingClass.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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
#pragma endregion Declarations

SVCorecoAcquisitionClass::SVCorecoAcquisitionClass( const SVAcquisitionConstructParams& p_rParams )
: SVAcquisitionClass( p_rParams )
{
	mbIsCorFilesLoaded = false;
}

SVCorecoAcquisitionClass::~SVCorecoAcquisitionClass()
{
	DestroyLocal();
}

bool SVCorecoAcquisitionClass::IsValid() const
{
	bool bOk = FALSE;

	bOk = m_hDigitizer != NULL;
  
	bOk = SVAcquisitionClass::IsValid() && bOk;
	
	return bOk;
}

bool SVCorecoAcquisitionClass::IsValidBoard() const
{
	bool bOk = TRUE;
	
	bOk = m_hDigitizer != NULL && bOk;
	
	return bOk;
}

HRESULT SVCorecoAcquisitionClass::Destroy()
{
	HRESULT hrOk = S_FALSE;

	hrOk = DestroyLocal();

	if ( SVAcquisitionClass::Destroy() != S_OK )
	{
		hrOk = S_FALSE;
	}
	
	return hrOk;
}

HRESULT SVCorecoAcquisitionClass::LoadFiles(SVFileNameArrayClass& rFiles)
{
	HRESULT hrOk = S_OK;

	if ( ! mbIsCorFilesLoaded )
	{
		hrOk = UnloadFiles();
		if ( hrOk == S_OK )
		{
			CString csCamFile;
			CString csVicFile;
			

			hrOk = SVAcquisitionClass::LoadFiles( rFiles );

			for ( long l = 0; hrOk == S_OK && l < rFiles.GetSize() &&
					( csCamFile.IsEmpty() || csVicFile.IsEmpty() ); l++)
			{
				CString csExt = rFiles[l].GetExtension();
				
				if ( csExt.CompareNoCase( ".cca" ) == 0 && csCamFile.IsEmpty() )
				{
					csCamFile = rFiles[l].GetFullFileName();
				}
				else if ( csExt.CompareNoCase( ".cvi" ) == 0 && csVicFile.IsEmpty() )
				{
					csVicFile = rFiles[l].GetFullFileName();
				}
			}

			if ( hrOk == S_OK )
			{
				if ( IsDigitizerSubsystemValid() )
				{
					mbIsCorFilesLoaded = true;
					
					if( !csCamFile.IsEmpty() && !csVicFile.IsEmpty())
					{

						SVLutInfo dummy;
						CreateLut( dummy );

						BSTR l_bstrNameCCA = csCamFile.AllocSysString();
						BSTR l_bstrNameCVI = csVicFile.AllocSysString();

						SAFEARRAYBOUND rgsabound[1];
						rgsabound[0].lLbound = 0;
						rgsabound[0].cElements = 2;
						SAFEARRAY* psaNames = SafeArrayCreate(VT_BSTR, 1, rgsabound);
						long lIndex = 0;
						SafeArrayPutElement( psaNames, &lIndex, l_bstrNameCCA );
						lIndex = 1;
						SafeArrayPutElement( psaNames, &lIndex, l_bstrNameCVI );

						hrOk = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->LoadCameraFiles( m_hDigitizer, psaNames );
						if( hrOk != S_OK )
						{
							ASSERT(FALSE);
							AfxMessageBox( "Unable to load cca/cvi files" );
						}

						SafeArrayDestroy(psaNames);
						psaNames = NULL;

						::SysFreeString( l_bstrNameCCA );
						::SysFreeString( l_bstrNameCVI );

					}
				
				}	// if subsystem available
				else
				{
					mbIsCorFilesLoaded = true;

					const SVCameraFormatsDeviceParam* pParam = m_DeviceParams.Parameter( DeviceParamCameraFormats ).DerivedValue( pParam );
					if( pParam == NULL )
					{
						// read cvi file
						// width, height, type
						// store in DeviceParam

						int iWidth = 0;
						int iHeight = 0;
						int iFormat = 0;

						SVOINIClass ini;
						ini.SetFile( csVicFile );
						ini.GetValue(_T("Stream Conditioning"), _T("Crop Width"), 640, &iWidth);
						ini.GetValue(_T("Stream Conditioning"), _T("Crop Height"), 480, &iHeight);
						ini.GetValue(_T("Output"), _T("Output Format"), SVImageFormatUnknown, &iFormat);

						SVImageFormatEnum eFormat = SVImageFormatUnknown;
						switch (iFormat)
						{
							case SV_CORACQ_VAL_OUTPUT_FORMAT_RGB5551: // fall through...
							case SV_CORACQ_VAL_OUTPUT_FORMAT_RGB565: // fall through...
							case SV_CORACQ_VAL_OUTPUT_FORMAT_RGB888: // fall through...
							case SV_CORACQ_VAL_OUTPUT_FORMAT_RGB8888: // fall through...
							case SV_CORACQ_VAL_OUTPUT_FORMAT_RGB101010: // fall through...
							case SV_CORACQ_VAL_OUTPUT_FORMAT_RGB161616: // fall through...
							case SV_CORACQ_VAL_OUTPUT_FORMAT_UYVY: // fall through...
							case SV_CORACQ_VAL_OUTPUT_FORMAT_YUY2: // fall through...
							case SV_CORACQ_VAL_OUTPUT_FORMAT_YVYU: // fall through...
							case SV_CORACQ_VAL_OUTPUT_FORMAT_YUYV: // fall through...
							case SV_CORACQ_VAL_OUTPUT_FORMAT_Y411: // fall through...
							case SV_CORACQ_VAL_OUTPUT_FORMAT_Y211:
								eFormat = SVImageFormatRGB8888;
								break;

							case SV_CORACQ_VAL_OUTPUT_FORMAT_MONO8: // fall through...
							case SV_CORACQ_VAL_OUTPUT_FORMAT_MONO16: // fall through...
							case SV_CORACQ_VAL_OUTPUT_FORMAT_MONO32:
								eFormat = SVImageFormatMono8;
								break;

							default:
								break; // Do nothing.
						}

						SVCameraFormat cf;
						cf.m_strName = _T("default");
						cf.lHeight = cf.lHeightMax = iHeight;
						cf.lWidth = cf.lWidthMax = iWidth;
						cf.eImageType = eFormat;

						SVCameraFormatsDeviceParam param;
						param.options[ cf.m_strName ] = cf;

						m_DeviceParams.SetParameter( DeviceParamCameraFormats, param );
						m_CameraFileDeviceParams.SetParameter( DeviceParamCameraFormats, param );
					}// end if ( pParam ) else
				}// end if subsystem available else
			}
		}
		
		GetCameraImageInfo( &msvImageInfo );
	}// end if ( ! mbIsCorFilesLoaded )

	return hrOk;
}

HRESULT SVCorecoAcquisitionClass::UnloadFiles()
{
	HRESULT hrOk = S_OK;
	
	if ( mbIsCorFilesLoaded )
	{
		hrOk = SVAcquisitionClass::UnloadFiles();
		
		mbIsCorFilesLoaded = false;
		
		if ( IsDigitizerSubsystemValid() )
		{
			if ( SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->UnloadCameraFile( m_hDigitizer ) != S_OK )
			{
				hrOk = S_FALSE;
			}
		}

		SVLutInfo dummy;
		CreateLut( dummy );
	}

	return hrOk;
}

HRESULT SVCorecoAcquisitionClass::LoadLightReference( SVLightReference& rArray )
{
	HRESULT l_hrOk = S_FALSE;

	if ( IsValidBoard() )
	{
		SVDeviceParamWrapper* pw;
		
		l_hrOk = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->GetParameter(m_hDigitizer, DeviceParamLightReference, &pw);

		if ( l_hrOk == S_OK )
		{
			SVLightReferenceDeviceParam* pParam = pw->DerivedValue( pParam );
			rArray = pParam->lr;
		}
		HRESULT hr = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->DestroyParameter(m_hDigitizer, pw);
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVCorecoAcquisitionClass::CreateLightReference(int iBands, int iBrightness, int iContrast)
{
	HRESULT hrOk = S_FALSE;

	hrOk = SVAcquisitionClass::CreateLightReference( iBands, iBrightness, iContrast );

	if ( hrOk == S_OK )
	{
		// RGB  Brightness    -2462 ... 2371
		// RGB  Contrast       8024 ... 15305
		// Quad Brightness    -6700 ... 800
		// Quad Contrast       8400 ... 10900


		long lDefaultBrightness	= DEFAULT_BRIGHTNESS;   // 0%
		long lDefaultContrast;
		long lBrightnessMin, lBrightnessMax;
		long lContrastMin, lContrastMax;
		if ( BandMaxSize() == 3 )	// RGB
		{
			if ( BandSize() == 1 )	// RGB Mono
			{
				lDefaultContrast = DEFAULT_CONTRAST_RGB_MONO;
			}
			else	// RGB Color
			{
				lDefaultContrast = DEFAULT_CONTRAST;
			}

			lBrightnessMin = -2462;
			lBrightnessMax = 2371;
			lContrastMin = 8024;
			lContrastMax = 15305;
		}
		else	// Quad
		{
			lDefaultContrast = DEFAULT_CONTRAST;             // 100%
			lBrightnessMin = -6700;
			lBrightnessMax = 800;
			lContrastMin = 8400;
			lContrastMax = 10900;
		}

		for ( int i = 0; hrOk == S_OK && i < iBands; i++)
		{
			if ( (hrOk = CreateLightReferenceBand( i, 2 )) == S_OK )
			{
				SVLightReferenceBand& rBand = mLightReference.m_aLightReferenceBand[ i ];
				//LR_BRIGHTNESS
				rBand.Attribute( LR_BRIGHTNESS ).strName = _T( "Brightness" );
				rBand.Attribute( LR_BRIGHTNESS ).iIDCaption = iBrightness; // IDS_BRIGHTNESS_REF
				rBand.Attribute( LR_BRIGHTNESS ).lValue = lDefaultBrightness;
				rBand.Attribute( LR_BRIGHTNESS ).lMin = lBrightnessMin;
				rBand.Attribute( LR_BRIGHTNESS ).lMax = lBrightnessMax;
				rBand.Attribute( LR_BRIGHTNESS ).lStep = 1;

				rBand.Attribute( LR_CONTRAST ).strName = _T( "Contrast" );
				rBand.Attribute( LR_CONTRAST ).iIDCaption = iContrast; // IDS_CONTRAST_REF
				rBand.Attribute( LR_CONTRAST ).lValue = lDefaultContrast;
				rBand.Attribute( LR_CONTRAST ).lMin = lContrastMin;
				rBand.Attribute( LR_CONTRAST ).lMax = lContrastMax;
				rBand.Attribute( LR_CONTRAST ).lStep = 1;

				DWORD& rdwTypeBrightness = rBand.Attribute( LR_BRIGHTNESS ).dwType;
				DWORD& rdwTypeContrast = rBand.Attribute( LR_CONTRAST ).dwType;

				if ( iBands == 1 )
				{
					rdwTypeBrightness = ( DWORD ) ( SVLightReferenceTypeBrightness );
					rdwTypeContrast = ( DWORD ) ( SVLightReferenceTypeContrast   );
				}
				else
				{
					switch ( i )
					{
						case 0:
						{
							rdwTypeBrightness = ( DWORD ) ( SVLightReferenceTypeBrightnessRed );
							rdwTypeContrast = ( DWORD ) ( SVLightReferenceTypeContrastRed   );
							break;
						}
						case 1:
						{
							rdwTypeBrightness = ( DWORD ) ( SVLightReferenceTypeBrightnessGreen );
							rdwTypeContrast = ( DWORD ) ( SVLightReferenceTypeContrastGreen   );
							break;
						}
						case 2:
						{
							rdwTypeBrightness = ( DWORD ) ( SVLightReferenceTypeBrightnessBlue );
							rdwTypeContrast = ( DWORD ) ( SVLightReferenceTypeContrastBlue   );
							break;
						}
						default:
						{
							rdwTypeBrightness = ( DWORD ) ( SVLightReferenceTypeBrightness );
							rdwTypeContrast = ( DWORD ) ( SVLightReferenceTypeContrast   );
							break;
						}
					}
				}
			}
		}

		if ( hrOk != S_OK )
		{
			mLightReference.DeleteAll();
		}
	}

	return hrOk;
}

HRESULT SVCorecoAcquisitionClass::GetMaxLightReferenceValue( unsigned long ulType, long &rlValue ) const
{
	HRESULT hrOk = S_FALSE;

	if ( IsDigitizerSubsystemValid() &&
	     m_hDigitizer != NULL )
	{
		SVDeviceParamWrapper* pw;
		hrOk = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->GetParameter( m_hDigitizer, DeviceParamLightReference, &pw );
		SVLightReferenceDeviceParam* pParam = pw->DerivedValue( pParam );

		switch ( ulType )
		{
			case SVLightReferenceTypeBrightness:	  // Quad, fall through...
			case SVLightReferenceTypeBrightnessRed:   // RGB, fall through...
			case SVLightReferenceTypeBrightnessGreen: // RGB, fall through...
			case SVLightReferenceTypeBrightnessBlue:  // RGB
				rlValue = pParam->lr.Band(0).Attribute(LR_BRIGHTNESS).lMax;
				break;

			case SVLightReferenceTypeContrast:        // Quad, fall through...
			case SVLightReferenceTypeContrastRed:     // RGB, fall through...
			case SVLightReferenceTypeContrastGreen:   // RGB, fall through...
			case SVLightReferenceTypeContrastBlue:    // RGB
				rlValue = pParam->lr.Band(0).Attribute(LR_CONTRAST).lMax;
				break;

			default:
				break; // Do nothing.
		}

		hrOk = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->DestroyParameter( m_hDigitizer, pw );
		
	}
	else
	{
		// RGB  Brightness    -2462 ... 2371
		// RGB  Contrast       8024 ... 15305
		// Quad Brightness    -6700 ... 800
		// Quad Contrast       8400 ... 10900
		switch ( ulType )
		{
			case SVLightReferenceTypeBrightness:	  // Quad, fall through...
				rlValue = 800;
				break;

			case SVLightReferenceTypeBrightnessRed:   // RGB, fall through...
			case SVLightReferenceTypeBrightnessGreen: // RGB, fall through...
			case SVLightReferenceTypeBrightnessBlue:  // RGB
				rlValue = 2371;
				break;

			case SVLightReferenceTypeContrast:        // Quad
				rlValue = 10900;
				break;
			case SVLightReferenceTypeContrastRed:     // RGB, fall through...
			case SVLightReferenceTypeContrastGreen:   // RGB, fall through...
			case SVLightReferenceTypeContrastBlue:    // RGB
				rlValue = 15305;
				break;

			default:
				break; // Do nothing.
		}

		hrOk = S_OK;
	}

	return hrOk;
}

HRESULT SVCorecoAcquisitionClass::GetMinLightReferenceValue( unsigned long ulType, long &rlValue ) const
{
	HRESULT hrOk = S_FALSE;

	if ( IsDigitizerSubsystemValid() &&
	     m_hDigitizer != NULL )
	{
		SVDeviceParamWrapper* pw;
		hrOk = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->GetParameter( m_hDigitizer, DeviceParamLightReference, &pw );
		SVLightReferenceDeviceParam* pParam = pw->DerivedValue( pParam );

		switch ( ulType )
		{
			case SVLightReferenceTypeBrightness:	  // Quad, fall through...
			case SVLightReferenceTypeBrightnessRed:   // RGB, fall through...
			case SVLightReferenceTypeBrightnessGreen: // RGB, fall through...
			case SVLightReferenceTypeBrightnessBlue:  // RGB
				rlValue = pParam->lr.Band(0).Attribute(LR_BRIGHTNESS).lMin;
				break;

			case SVLightReferenceTypeContrast:        // Quad, fall through...
			case SVLightReferenceTypeContrastRed:     // RGB, fall through...
			case SVLightReferenceTypeContrastGreen:   // RGB, fall through...
			case SVLightReferenceTypeContrastBlue:    // RGB
				rlValue = pParam->lr.Band(0).Attribute(LR_CONTRAST).lMin;
				break;

			default:
				break; // Do nothing.
		}
		hrOk = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->DestroyParameter( m_hDigitizer, pw );
		
	}
	else
	{
		// RGB  Brightness    -2462 ... 2371
		// RGB  Contrast       8024 ... 15305
		// Quad Brightness    -6700 ... 800
		// Quad Contrast       8400 ... 10900
		switch ( ulType )
		{
			case SVLightReferenceTypeBrightness:	  // Quad
				rlValue = -6700;
				break;
			case SVLightReferenceTypeBrightnessRed:   // RGB, fall through...
			case SVLightReferenceTypeBrightnessGreen: // RGB, fall through...
			case SVLightReferenceTypeBrightnessBlue:  // RGB
				rlValue = -2462;
				break;

			case SVLightReferenceTypeContrast:        // Quad
				rlValue = 8400;
				break;
			case SVLightReferenceTypeContrastRed:     // RGB, fall through...
			case SVLightReferenceTypeContrastGreen:   // RGB, fall through...
			case SVLightReferenceTypeContrastBlue:    // RGB
				rlValue = 8024;
				break;

			default:
				break; // Do nothing.
		}

		hrOk = S_OK;
	}

	return hrOk;
}

HRESULT SVCorecoAcquisitionClass::GetLightReferenceValueStep( unsigned long ulType, unsigned long &rulValue ) const
{
	HRESULT hrOk = S_FALSE;

	if ( IsDigitizerSubsystemValid() &&
	     m_hDigitizer != NULL )
	{
		SVDeviceParamWrapper* pw;
		hrOk = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->GetParameter( m_hDigitizer, DeviceParamLightReference, &pw );
		SVLightReferenceDeviceParam* pParam = pw->DerivedValue( pParam );

		switch ( ulType )
		{
			case SVLightReferenceTypeBrightness:	  // Quad, fall through...
			case SVLightReferenceTypeBrightnessRed:   // RGB, fall through...
			case SVLightReferenceTypeBrightnessGreen: // RGB, fall through...
			case SVLightReferenceTypeBrightnessBlue:  // RGB
				rulValue = pParam->lr.Band(0).Attribute(LR_BRIGHTNESS).lStep;
				break;

			case SVLightReferenceTypeContrast:        // Quad, fall through...
			case SVLightReferenceTypeContrastRed:     // RGB, fall through...
			case SVLightReferenceTypeContrastGreen:   // RGB, fall through...
			case SVLightReferenceTypeContrastBlue:    // RGB
				rulValue = pParam->lr.Band(0).Attribute(LR_CONTRAST).lStep;
				break;

			default:
				break; // Do nothing.
		}
		hrOk = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->DestroyParameter( m_hDigitizer, pw );
		
	}
	else
	{
		rulValue = 1;

		hrOk = S_OK;
	}

	return hrOk;
}

HRESULT SVCorecoAcquisitionClass::SetLightReferenceImpl( SVLightReference& rLR )
{
	HRESULT hrOk = S_OK;

	SVDeviceParamWrapper w;
	SVLightReferenceDeviceParam param;
	param.lr = rLR;
	w = param;

	if ( IsValidBoard() )
	{
		hrOk = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->SetParameter( m_hDigitizer, &w );
	}

	return hrOk;
}

HRESULT SVCorecoAcquisitionClass::GetImageInfo(SVImageInfoClass *pImageInfo) const
{
	HRESULT hrOk = S_FALSE;

	if ( pImageInfo != NULL )
	{
		*pImageInfo = msvImageInfo;
		
		hrOk = S_OK;
	}
	
	return hrOk;
}

HRESULT SVCorecoAcquisitionClass::ResetLightReference()
{
	return CreateLightReference( miBandMaxSize, IDS_BRIGHTNESS_REF, IDS_CONTRAST_REF );
}

HRESULT SVCorecoAcquisitionClass::DestroyLocal()
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

	return hrOk;
}

HRESULT SVCorecoAcquisitionClass::CreateLut( const SVLutInfo& info )
{
	HRESULT hr = GetLutImpl( Lut() );
	if ( hr != S_OK )
	{
		Lut().Create( info );
	}
	Lut().SetTransform( SVDefaultLutTransform() );
	Lut().SetTransformOperation( SVLutTransformOperationNormal() );
	Lut().Transform();
	SetLutImpl( Lut() );

	return S_OK;
}

HRESULT SVCorecoAcquisitionClass::GetLutImpl( SVLut& lut )
{
	HRESULT l_hrOk = S_FALSE;

	if ( IsValidBoard() )
	{

		SVDeviceParamWrapper* pw;
		
		l_hrOk = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->GetParameter(m_hDigitizer, DeviceParamLut, &pw);

		if ( l_hrOk == S_OK )
		{
			SVLutDeviceParam* pParam = pw->DerivedValue( pParam );
			lut = pParam->lut;
		}
		l_hrOk = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->DestroyParameter(m_hDigitizer, pw);
	}

	return l_hrOk;
}

HRESULT SVCorecoAcquisitionClass::SetLutImpl( const SVLut& lut )
{
	HRESULT l_hrOk = S_FALSE;

	if ( IsValidBoard() )
	{
		SVDeviceParamWrapper w;
		SVLutDeviceParam param;
		param.lut = lut;
		w = param;
		
		l_hrOk = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName)->SetParameter(m_hDigitizer, &w);
	}

	return l_hrOk;
}

HRESULT SVCorecoAcquisitionClass::GetCameraImageInfo(SVImageInfoClass *pImageInfo)
{
	HRESULT hrOk = S_FALSE;

	unsigned long bufWidth = 640;
	unsigned long bufHeight = 480;
	unsigned long uiBandNumber = 1;
	int iFormat = SVImageFormatUnknown;

	if ( IsValidBoard() &&
	     IsDigitizerSubsystemValid() )
	{
		SVDigitizerLoadLibraryClass* pDigitizer = SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(mcsDigName);

		pDigitizer->GetBufferHeight( m_hDigitizer, &bufHeight );
		pDigitizer->GetBufferWidth( m_hDigitizer, &bufWidth );
		pDigitizer->GetBufferFormat( m_hDigitizer, &iFormat );
	}// end if subsystem available
	else
	{
		// get camera format from DeviceParams
		const SVCameraFormatsDeviceParam* pParam = m_DeviceParams.Parameter( DeviceParamCameraFormats ).DerivedValue( pParam );
		if ( pParam )
		{
			bufHeight = pParam->options.find( _T("default") )->second.lHeight;
			bufWidth = pParam->options.find( _T("default") )->second.lWidth;
			iFormat = pParam->options.find( _T("default") )->second.eImageType;
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
		default:
		{
			break; // Do nothing.
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
		AfxMessageBox( "Failed to get acquisition source information!", MB_ICONEXCLAMATION );
	}

	return hrOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVCorecoAcquisitionClass.cpp_v  $
 * 
 *    Rev 1.2   07 Mar 2014 18:12:00   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 *   Added DEBUG_NEW.
 *   Made methods const.
 *   Various code changes to better follow coding guidelines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:02:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.49   29 Apr 2013 14:06:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:03:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48   16 Sep 2011 15:50:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated camera management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   22 Mar 2011 07:45:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   16 Mar 2011 13:23:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated extent functionality and renamed extent methods to correct extent issues and add clarity to the logical flow.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   10 Feb 2011 14:33:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   04 Nov 2010 13:30:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   15 Feb 2010 12:25:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   15 Dec 2009 15:13:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   03 Sep 2009 09:59:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   30 Jul 2009 11:18:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   04 Nov 2008 11:47:08   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  639
 * SCR Title:  Update SVObserver to better handle going offline while triggering
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added m_numberOfBuffers
 * Changed the Start function to call UnlockBuffer for each buffer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   16 Sep 2008 13:37:48   sjones
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
 *    Rev 1.37   10 Sep 2008 16:48:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Device Name access
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   24 Jul 2007 11:47:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade to Matrox Imaging Library 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   21 Jun 2007 12:01:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34.1.0   14 Jul 2009 12:44:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Udated code to use the new data manager objects and signal processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   20 Jan 2006 08:13:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  541
 * SCR Title:  Upgrade SVObserver for Pan Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix a pointer issue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   10 Nov 2005 06:44:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to rename member variable for more clarity.  Used new SVObserverApp method to determine type of hardware.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   17 Feb 2005 13:29:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   02 Nov 2004 15:06:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  459
 * SCR Title:  Fix a problem with inspection lock-up
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added logging
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   18 Feb 2004 08:39:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   moved data manager buffer allocation to derived classes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   05 Dec 2003 14:06:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  400
 * SCR Title:  Fix Bug with Configuration saving (wrong path for the camera file)
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with LoadFiles.  Will load the camera files even if the DigitizerSubSystem is invalid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   17 Oct 2003 13:02:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated exposed function prototypes to change parameters from a class to individual items.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   16 Oct 2003 09:12:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed LoadLightReference to work correctly with 1394
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   02 Oct 2003 10:48:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  386
 * SCR Title:  Change Light Reference handling to match earlier versions of SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Add new code to handle the defaulting of unset light reference values to the values in the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   22 Sep 2003 14:54:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed Lut handling from member var to DeviceParam
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   09 Sep 2003 14:01:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  353
 * SCR Title:  New ViperRGB Firmware
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed code dealing with Trash Buffer.  The RGB will be using the cycle mode Next Empty
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   29 Aug 2003 13:17:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fix RGB Color default contrast
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   07 Aug 2003 12:51:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed CreateImageBuffer to CreateImageAcquisitionBuffer
 * changed DestroyImageBuffer to DestroyImageAcquisitionBuffer
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   31 Jul 2003 10:30:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   modifications to allow loading / creation without digitizer present
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   08 Jul 2003 11:55:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved Coreco to DLL
 * changed architecture for IO
 * changed architecture for Digital
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   17 Apr 2003 17:25:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   13 Mar 2003 08:58:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  335
 * SCR Title:  Fix Light Reference and LUT problems with RGB Mono systems
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GetRootDevice
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   10 Mar 2003 15:14:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved lDMIndex functionality to SVCorecoAcquisitionClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   27 Feb 2003 16:27:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  314
 * SCR Title:  SVIM stops inspecting when triggered very fast
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Updated informational trace information to track DataManager and buffer locking problems.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   20 Feb 2003 11:55:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed problem with loading configurations on different hardware
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   12 Feb 2003 16:49:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed problem with GetImageInfo.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   12 Feb 2003 10:53:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added GetNextRequest, NotifyEndFrame, Reset, CreateLightReference, and Create methods to fix issues with the ViperRGB.
 * Updated UnlockBuffer, LockBuffer, NotifyStartFrame, UnregisterAllCallbacks, UnregisterCallback, RegisterCallback, StartProcess, CanProcess, DestroyLocal, UnloadFiles, Stop, Start, DestroyBuffers, CreateBuffers, LoadFiles, IsValidBoard, IsValid and constructor methods to fix issues with ViperRGB.
 * Updated global callback function SVOStartFrameEndFrameCallback to fix issues with ViperRGB.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   07 Feb 2003 10:11:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed mulType reference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   06 Feb 2003 10:39:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added Lut functions (was in ImageProcessing)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   29 Jan 2003 09:42:00   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated all appropriate methods with SVImageBufferHandleStruct or SVImageObjectClass parameter for methods that used SVImageInfoClass as a parameter.  Updated start frame and end frame tracking techniques for handling the new acquisition tranfer mode next empty.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   22 Jan 2003 13:23:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed minor problem.  had a minus instead of an equal sign.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   22 Jan 2003 09:20:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new method IsValidBoard() which will check to see if the mcorAcq and mcorBoard  is !CORHANDLE_IS_NULL.  Also changed the method IsValid to use the macro CORHANDLE_IS_NULL instead of SV_IS_VALID_CORHANDLE.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Jan 2003 10:13:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changes made to IsValid.  Uses CORHANDLE_IS_NULL macro instead of SV_IS_CORHANDLE_VALID
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   10 Jan 2003 09:43:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added ResetLut
 * Modified SetLut to use bands
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   03 Jan 2003 13:23:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the check for finding the board.  
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Jan 2003 15:26:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added:
 * CreateLut
 * DestroyLut
 * GetLut
 * SetLut
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Dec 2002 20:01:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   SVCorecoAcquisitionClass::SVCorecoAcquisitionClass
 * SVCorecoAcquisitionClass::LoadFiles
 * SVCorecoAcquisitionClass::GetImageInfo
 * SVCorecoAcquisitionClass::CreateBuffers
 * - Modified to fix problem when loading a configuration on a machine without a frame grabber card.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Dec 2002 19:06:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  310
 * SCR Title:  Update SVObserver to Version 4.01 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods associated with ModelessDebug dialog.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 Dec 2002 21:50:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  311
 * SCR Title:  Fix Light Reference save for RGB Mono
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added GetRootDeviceName method to SVCorecoAcquisitionClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 12:51:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
