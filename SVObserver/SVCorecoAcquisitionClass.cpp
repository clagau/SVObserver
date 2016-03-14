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

