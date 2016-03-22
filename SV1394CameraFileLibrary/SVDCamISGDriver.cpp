//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDCamISGDriver
//* .File Name       : $Workfile:   SVDCamISGDriver.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:36:42  $
//******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <vector>
#include "SVDCamISGDriver.h"
#include "SVOMFCLibrary/SVDeviceParamCollection.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"

SVDCamDriver* SVDCamISGMHPASDriver::CreateNew()
{
	return new SVDCamISGMHPASDriver;
}

SVDCamDriver* SVDCamISGLightWiseDriver::CreateNew()
{
	return new SVDCamISGLightWiseDriver;
}

SVDCamISGMHPASDriver::SVDCamISGMHPASDriver()
{
	// important note:
	// These registers are two bytes wide. MIL only allows
	// setting 32-bit values. This must be taken into consideration
	// when setting registers. We need to read or cache register pairs.

	ISG_CNTL1         = 0x7200;  // Trigger and strobe control
	ISG_STAT         = 0x7202;  // trigger status

	ISG_TRGDLY_LO    = 0x7204;  // integration delay
	ISG_TRGDLY_HI    = 0x7206;  // integration delay
	
	ISG_STBADV       = 0x7208;  // strobe advance
	ISG_RTGDLY_LO    = 0x720A;  // retrigger delay

	ISG_RTGDLY_HI    = 0x720C;  // retrigger delay
	ISG_PWM          = 0x720E;  // Pulse Width Mode duty cycle FF=100% 80=50% 00=0.4%
	
	ISG_OUTMODE      = 0x7214;  // Outmode
	ISG_CLKDIV       = 0x7216;	// clock div

	init();
}

SVDCamISGLightWiseDriver::SVDCamISGLightWiseDriver()
{
	// These registers are four bytes wide.
	ISG_CNTL1        = 0x7200;  // Trigger and strobe control
	ISG_STAT         = 0x7204;  // trigger status
	ISG_TRGDLY       = 0x7208;  // integration delay
	ISG_STBADV       = 0x720C;  // strobe advance
	ISG_RTGDLY       = 0x7210;  // retrigger delay
	ISG_PWM          = 0x7214;  // Pulse Width Mode duty cycle FF=100% 80=50% 00=0.4%
	ISG_VERREG       = 0x722C;	// FPGA version
	ISG_CLKDIV       = 0x7224;	// clock div
	ISG_INTCNT       = 0x722C;	// integration count ( shutter raw ); Lightwise only
	ISG_STBDLY       = 0x723C;	// strobe delay; Lightwise only
	ISG_STBDUR       = 0x7240;  // strobe duration; LightWise only
	ISG_LUTAD        = 0x7400;	// Lut address offset
	ISG_LUTDATA      = 0x7404;	// lut data register
	ISG_IBIS_DACFINE = 0x7028;	// register for column luminosity
	ISG_NUM_BUFFERS  = 0x7334;	// num buffers to hold

	init();
}

SVDCamISGDriver::SVDCamISGDriver()
: SVDCamStandardDriver()
{
	ISG_CNTL1 = 0;
	ISG_STAT = 0;
	ISG_TRGDLY_LO = 0;
	ISG_TRGDLY_HI = 0;
	ISG_TRGDLY = 0;
	ISG_STBADV = 0;
	ISG_RTGDLY_LO = 0;
	ISG_RTGDLY_HI = 0;
	ISG_RTGDLY = 0;
	ISG_PWM = 0;
	ISG_OUTMODE = 0;
	ISG_VERREG = 0;
	ISG_CLKDIV = 0;
	ISG_INTCNT = 0;
	ISG_STBDLY = 0;
	ISG_VVDLY = 0;
	ISG_LUTAD = 0;
	ISG_LUTDATA = 0;
	ISG_NUM_BUFFERS = 0;
	ISG_STBDUR = 0;
	ISG_IBIS_DACFINE = 0;
}

void SVDCamISGDriver::init()
{
	m_mapRegisterCache[ISG_CNTL1] = 0;
	// set up default
	SetValueAtBits(&m_mapRegisterCache[ISG_CNTL1], ISG_CNTL1_TriggerModeBits, ISG_CNTL1_TriggerModeHostTriggerOneShot);
	SetValueAtBits(&m_mapRegisterCache[ISG_CNTL1], ISG_CNTL1_StrobeModeBits, ISG_CNTL1_StrobeModeAlwaysLow);
	SetValueAtBits(&m_mapRegisterCache[ISG_CNTL1], ISG_CNTL1_VideoEnable_Bit, 1);

}

void SVDCamISGLightWiseDriver::init()
{
	SVDCamISGDriver::init();
	SetValueAtBits(&m_mapRegisterCache[ISG_CNTL1], ISG_CNTL1_PixelCorrectionEnable_Bit, 1);
	SetValueAtBits(&m_mapRegisterCache[ISG_CNTL1], ISG_CNTL1_StrobeDurationEnable_Bit, 1);
	bool bOptoTrigger = false;
	SetValueAtBits(&m_mapRegisterCache[ISG_CNTL1], ISG_CNTL1_InputTriggerSelect_Bit, bOptoTrigger ? 1 : 0);
}

SVDCamISGDriver::~SVDCamISGDriver()
{
}

HRESULT SVDCamISGDriver::SetParameter( const SVDeviceParamWrapper& rw )
{
	HRESULT hr = S_OK;

	const SVDeviceParam* pParam = (const SVDeviceParam*) rw;
	if ( pParam != NULL )
	{
		switch ( pParam->Type() )
		{
			case DeviceParamOutputStrobePulse:
			{
				ULONG ulRegister = ISG_CNTL1;
				ULONG ulValue = m_mapRegisterCache[ISG_CNTL1];
				long lStrobeType = IsCameraOnline() ? LongValue( rw ) : ISG_CNTL1_StrobeModeAlwaysLow;
				SetValueAtBits( &ulValue, ISG_CNTL1_StrobeModeBits, lStrobeType );
				m_mapRegisterCache[ISG_CNTL1] = ulValue;
				HRESULT hrRegister;
			
				hrRegister = WriteCameraRegister( ulRegister, ulValue );

				if ( CameraDeviceParamExists( DeviceParamAcquisitionStrobeEdge ) )
				{
					SVBoolValueDeviceParam* pParam = GetCameraDeviceParamNonConst( DeviceParamAcquisitionStrobeEdge ).DerivedValue( pParam );

					// Rising Edge = 1,3 -- Falling Edge = 0,2
					pParam->bValue = LongValue( rw ) == 1 || LongValue( rw ) == 3;

					/* // Rising Edge = 0,2 -- Falling Edge = 1,3
					pParam->bValue = LongValue( rw ) == 0 || LongValue( rw ) == 2;
					//*/
				}
				//*
				else
				{
					SVBoolValueDeviceParam l_Edge;
					SVBoolValueDeviceParam::OptionType l_Option;

					l_Option.strDescription = _T("Rising Edge");
					l_Option.value = true;
					l_Edge.info.options.push_back( l_Option );

					l_Option.strDescription = _T("Falling Edge");
					l_Option.value = false;
					l_Edge.info.options.push_back( l_Option );

					l_Edge.SetType( DeviceParamAcquisitionStrobeEdge );
					l_Edge.SetDetailLevel( 99 );

					// Rising Edge = 1,3 -- Falling Edge = 0,2
					l_Edge.bValue = LongValue( rw ) == 1 || LongValue( rw ) == 3;

					/* // Rising Edge = 0,2 -- Falling Edge = 1,3
					l_Edge.bValue = LongValue( rw ) == 0 || LongValue( rw ) == 2;
					//*/

					SVDeviceParamWrapper w = l_Edge;

					SetCameraDeviceParam( w ); // must always have this parameter.
				}
				//*/
			}
			break;

			case DeviceParamTriggerEdge:
			{
				SetValueAtBits( &m_mapRegisterCache[ISG_CNTL1], ISG_CNTL1_TriggerSense_Bit, LongValue( rw ) ? 0 : 1 );
				HRESULT hrRegister;
				hrRegister = WriteCameraRegister( ISG_CNTL1, m_mapRegisterCache[ISG_CNTL1] );
			}
			break;

			case DeviceParamTriggerMode:
			{
				ULONG ulRegister = ISG_CNTL1;
				ULONG ulValue = m_mapRegisterCache[ISG_CNTL1];
				long lTriggerMode = IsCameraOnline() ? LongValue( rw ) : LongValue( rw );
				//long lTriggerMode = IsCameraOnline() ? LongValue( rw ) : ISG_CNTL1_TriggerModeHostTriggerOneShot;
				SetValueAtBits( &ulValue, ISG_CNTL1_TriggerModeBits, lTriggerMode );
				m_mapRegisterCache[ISG_CNTL1] = ulValue;
				HRESULT hrRegister;
				hrRegister = WriteCameraRegister( ulRegister, ulValue );
			}
			break;

			case DeviceParamStrobePulseAdvance:
			{
				ULONG ulValue = LongValue(rw);
				WriteCameraRegister( ISG_STBADV, ulValue );
			}
			break;

			default:
				hr = SVDCamStandardDriver::SetParameter( rw );
				break;
		}// end switch ( pParam->Type() )
	}// end if ( pParam != NULL )

	return hr;
}

HRESULT SVDCamISGMHPASDriver::SetParameter( const SVDeviceParamWrapper& rw )
{
	HRESULT hr = S_OK;

	const SVDeviceParam* pParam = (const SVDeviceParam*) rw;
	if ( pParam != NULL )
	{
		switch ( pParam->Type() )
		{
			case DeviceParamTestPattern:
			{
				HRESULT hrRegister=S_OK;

				ULONG ulRegister = ISG_OUTMODE;
				// this write (32 bits) overwrites the ISG_CLKDIV register, which must be 0.
				ULONG ulValue = 0;
				ULONG ulTestPattern = LongValue( rw ) != 0 ? ISG_OUTMODE_TestModeOn : ISG_OUTMODE_TestModeOff;
				SetValueAtBits( &ulValue, ISG_OUTMODE_TestModeBit, ulTestPattern );
			
				hrRegister = WriteCameraRegister( ulRegister, ulValue );

				break;
			}// end case DeviceParamTestPattern:

			case DeviceParamShutter:	// this param seems flaky for MHPAS; handle it here
			{
				WriteCameraRegister( DCAM_SHUTTER, LongValue(rw) );
			}
			break;

			default:
				hr = SVDCamISGDriver::SetParameter( rw );
				break;
		}// end switch ( pParam->Type() )
	}

	return hr;
}

HRESULT SVDCamISGLightWiseDriver::SetParameter( const SVDeviceParamWrapper& rw )
{
	HRESULT hr = S_OK;

	const SVDeviceParam* pParam = (const SVDeviceParam*) rw;
	if ( pParam != NULL )
	{
		switch ( pParam->Type() )
		{

			case DeviceParamTestPattern:
			{
				ULONG ulTestPattern = LongValue( rw ) != 0 ? ISG_CNTL1_OutputFormat_TestPattern : ISG_CNTL1_OutputFormat_8Bit;
				SetValueAtBits( &m_mapRegisterCache[ISG_CNTL1], ISG_CNTL1_OutputFormat_Bits, ulTestPattern );
			
				HRESULT hrRegister = WriteCameraRegister( ISG_CNTL1, m_mapRegisterCache[ISG_CNTL1] );
				break;
			}

			case DeviceParamLut:
			{
				HRESULT hrRegister = S_OK;
				// disable LUT
				SetValueAtBits( &m_mapRegisterCache[ISG_CNTL1], ISG_CNTL1_LUT_Enable_Bit, 0 );
				hrRegister = WriteCameraRegister( ISG_CNTL1, m_mapRegisterCache[ISG_CNTL1] );

				// Write LUT
				hrRegister = WriteCameraRegister( ISG_LUTAD, 0 );	// initialize autoincrement counter

				const SVLutDeviceParam* pLutParam = rw.DerivedValue( pLutParam );
				ASSERT( pLutParam );
				std::vector<ULONG> aData;
				NeutralLutToRawLut( pLutParam->lut, aData );
				
				int iBlockSize = AfxGetApp()->GetProfileInt(_T("Debug"), _T("1394_Blocksize"), 64) / 4;
				AfxGetApp()->WriteProfileInt(_T("Debug"), _T("1394_Blocksize"), iBlockSize * 4);
				if ( aData.size() > 0 )
				{
					for (int iBlock=0; iBlock < static_cast<int>(aData.size()); iBlock += iBlockSize)
					{
						int iTransferSize = min(iBlockSize, static_cast<int>(aData.size()) - iBlock);
						WriteCameraRegisterBlock( ISG_LUTDATA, iTransferSize, &aData[iBlock] );
					}
				}

				// enable LUT
				SetValueAtBits( &m_mapRegisterCache[ISG_CNTL1], ISG_CNTL1_LUT_Enable_Bit, 1 );
				hrRegister = WriteCameraRegister( ISG_CNTL1, m_mapRegisterCache[ISG_CNTL1] );
			}
			break;

			case DeviceParamShutter:	// don't use standard DCAM register; use custom hi-resolution register
			{
				// the actual value is in units of 25 nanoseconds; multiply by 40 to get units of 1 microsecond
				ULONG ulValue = LongValue(rw);
				WriteCameraRegister( ISG_INTCNT, 40 * ulValue );
			}
			break;

			case DeviceParamShutterDelay:
			{
				ULONG ulValue = LongValue(rw);
				WriteCameraRegister( ISG_TRGDLY, ulValue );
			}
			break;

			case DeviceParamGamma:
			{
				// EB 2004 06 16 - the gamma is redundant because of LUT. Ignore this param
			}
			break;

			case DeviceParamStrobePulseDelay:
			{
				ULONG ulValue = LongValue(rw);
				WriteCameraRegister( ISG_STBDLY, ulValue );
			}
			break;

			case DeviceParamStrobePulseDuration:
			{
				ULONG ulValue = LongValue(rw);
				WriteCameraRegister( ISG_STBDUR, ulValue );
			}
			break;

			case DeviceParamCameraFormats:
			{
				hr = SVDCamISGDriver::SetParameter( rw );
//				ULONG ulValue=0x50;
//				ulValue = AfxGetApp()->GetProfileInt(_T("Debug"), _T("ISG_DACFINE"), ulValue);
//				AfxGetApp()->WriteProfileInt(_T("Debug"), _T("ISG_DACFINE"), ulValue);
//				WriteCameraRegister( ISG_IBIS_DACFINE, ulValue );	//!!! TEMP until they auto-adjust
			}
			break;

			case DeviceParamTriggerMode:
			{
				// piggyback on trigger mode if DeviceParamNumCameraBuffers is not specified
				if ( ! CameraDeviceParamExists( DeviceParamNumCameraBuffers ) )
				{
					ULONG ulRegister = ISG_NUM_BUFFERS;
					ULONG ulValue = 1;	// value of 1 or 3 or 7 = 1 or 2 or 3 buffers respectively;
					HRESULT hrRegister = WriteCameraRegister( ulRegister, ulValue );
				}

				hr = SVDCamISGDriver::SetParameter( rw );	// call base for impl. of TriggerMode
			}
			break;

			case DeviceParamNumCameraBuffers:
			{
				ULONG ulRegister = ISG_NUM_BUFFERS;
				ULONG ulValue = LongValue(rw);	// value of 1 or 3 or 7 = 1 or 2 or 3 buffers respectively;
				HRESULT hrRegister = WriteCameraRegister( ulRegister, ulValue );
			}
			break;

			default:
				hr = SVDCamISGDriver::SetParameter( rw );
				break;
		}// end switch ( pParam->Type() )
	}// end if ( pParam != NULL )

	return hr;
}

HRESULT SVDCamISGDriver::SetParameters( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_OK;

	bool bSetStrobeInfo = false;
	bool bSetTriggerInfo = false;

	SVDeviceParamMap::const_iterator iter;
	for ( iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); ++iter)
	{
		const SVDeviceParamWrapper& rw = iter->second;
		if ( rw.IsValid() )
		{
			switch ( rw->Type() )
			{
				default:
					SetParameter( rw );
					break;
			}// end switch ( w->Type() )
		}
	}// end for ( iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); iter++)

	return hr;
}

HRESULT SVDCamISGDriver::SoftwareTrigger()
{
	SetValueAtBits( &m_mapRegisterCache[ISG_CNTL1], ISG_CNTL1_HostTrigger_Bit, ISG_CNTL1_HostTrigger );
	WriteCameraRegister( ISG_CNTL1, m_mapRegisterCache[ISG_CNTL1] );
	SetValueAtBits( &m_mapRegisterCache[ISG_CNTL1], ISG_CNTL1_HostTrigger_Bit, 0 );

	return S_OK;
}

HRESULT SVDCamISGDriver::GoOnline()
{
	/*
	ULONG ulValue=0x50;
	ulValue = AfxGetApp()->GetProfileInt(_T("Debug"), _T("ISG_DACFINE"), ulValue);
	//AfxGetApp()->WriteProfileInt(_T("Debug"), _T("ISG_DACFINE"), ulValue);
	WriteCameraRegister( ISG_IBIS_DACFINE, ulValue );	//!!! TEMP until they auto-adjust

	ulValue = AfxGetApp()->GetProfileInt(_T("Debug"), _T("ISG_7000"), 0xC0 );
	WriteCameraRegister( 0x7000, ulValue );

	ulValue = AfxGetApp()->GetProfileInt(_T("Debug"), _T("ISG_7228"), 0x60 );
	WriteCameraRegister( 0x7228, ulValue );
	*/

	// trigger first, then strobe
//	SetParameter( GetCameraDeviceParam( DeviceParamTriggerMode ) );
	SetParameter( GetCameraDeviceParam( DeviceParamAcquisitionTriggerType ) );
	SetParameter( GetCameraDeviceParam( DeviceParamOutputStrobePulse ) );

	return S_OK;
}

HRESULT SVDCamISGDriver::GoOffline()
{
	// strobe first, then trigger
	SetParameter( GetCameraDeviceParam( DeviceParamOutputStrobePulse ) );
	SetParameter( GetCameraDeviceParam( DeviceParamAcquisitionTriggerType ) );
//	SetParameter( GetCameraDeviceParam( DeviceParamTriggerMode ) );
	return S_OK;
}

HRESULT SVDCamISGDriver::NeutralLutToRawLut( const SVLut& rLut, std::vector<ULONG>& raData )
{
	HRESULT hr = S_OK;

	UINT uiSize = rLut.Info().BandSize();
	raData.resize(uiSize);
	for (UINT i = 0; i < uiSize; i++)
	{
		raData[i] = rLut(0, i);
	}

	return hr;
}

