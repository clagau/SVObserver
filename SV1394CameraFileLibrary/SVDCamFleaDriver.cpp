//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDCamDragonflyDriver
//* .File Name       : $Workfile:   SVDCamFleaDriver.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:36:40  $
//******************************************************************************

#include "stdafx.h"
#include "SVDCamFleaDriver.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVOMFCLibrary/Regexp.h"
#include "SVOMFCLibrary/SVDeviceParamCollection.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"

SVDCamFleaDriver::SVDCamFleaDriver()
: SVDCamStandardDriver()
{
	m_ulGPIO_XTRA = 0xBAADF00D;
	m_ulABS_CSR_SHUTTER_REGISTER_BASE = 0xDEADBEEF;
}

SVDCamFleaDriver::~SVDCamFleaDriver()
{
}

SVDCamDriver* SVDCamFleaDriver::CreateNew()
{
	return new SVDCamFleaDriver;
}

HRESULT SVDCamFleaDriver::GetSoftwareTriggerRegisterAndValue(unsigned long& ulRegister, unsigned long& ulValue)
{
	ulRegister = DCAM_SOFTWARE_TRIGGER;
	ulValue = 0x80000000;
	return S_OK;
}

HRESULT SVDCamFleaDriver::SetParameter( const SVDeviceParamWrapper& rw )
{
	HRESULT hr = S_OK;

	if ( m_ulGPIO_XTRA == 0xBAADF00D )
	{
		ReadCameraRegister( Flea_GPIO_XTRA, m_ulGPIO_XTRA );
	}
	if ( m_ulABS_CSR_SHUTTER_REGISTER_BASE == 0xDEADBEEF )
	{
		ReadCameraRegister( Flea_ABS_CSR_HI_INQ_7, m_ulABS_CSR_SHUTTER_REGISTER_BASE );
		m_ulABS_CSR_SHUTTER_REGISTER_BASE *= 4;
		m_ulABS_CSR_SHUTTER_REGISTER_BASE -= 0xF00000;
	}

	const SVDeviceParam* pParam = (const SVDeviceParam*) rw;
	if ( pParam != NULL )
	{
		switch ( pParam->Type() )
		{
			case DeviceParamShutter:
			{
				// see http://ptgrey.com/support/kb/index.asp?a=4&q=16

				if ( Firmware( m_strCameraFileFirmware ) >= Firmware("1.1.0.13") )
				{
					// if new firmware, use absolute value shutter CSR

					// set to use Abs Value CSR
					ULONG ulValue=0;
					ReadCameraRegister( DCAM_SHUTTER, ulValue );
					SetBits( ulValue, DCAMBIT1, true ); // turn on ABS CSR
					SetBits( ulValue, DCAMBIT7, false );// set to manual mode
					WriteCameraRegister( DCAM_SHUTTER, ulValue );

					ReadCameraRegister( DCAM_SHUTTER, ulValue );

					const SVLongValueDeviceParam* pShutterParam = rw.DerivedValue( pShutterParam );
					ASSERT( pShutterParam );

					FloatLongConvert temp;
					ReadCameraRegister( m_ulABS_CSR_SHUTTER_REGISTER_BASE + CSR_VALUE_OFFSET, temp.ulValue );	// TEMP DEBUGGING

					float fValue = static_cast <float> (pShutterParam->lValue);
					fValue /= static_cast <float> (pShutterParam->info.unit_divisor);
					FloatLongConvert value;
					value.fValue = fValue;
					WriteCameraRegister( m_ulABS_CSR_SHUTTER_REGISTER_BASE + CSR_VALUE_OFFSET, value.ulValue );

					ReadCameraRegister( m_ulABS_CSR_SHUTTER_REGISTER_BASE + CSR_VALUE_OFFSET, temp.ulValue );	// TEMP DEBUGGING
				}
				else	// otherwise call base class (DCAM shutter)
				{
					hr = SVDCamStandardDriver::SetParameter( rw );
				}
			}
			break;

			case DeviceParamTriggerEdge:
			{
				SVDCamTriggerSourceEnum eTriggerSource = (SVDCamTriggerSourceEnum) LongValue( GetCameraDeviceParam( DeviceParamAcquisitionTriggerType ) );
				switch ( eTriggerSource )
				{
					case HardwareTrigger:
					case SoftwareAsyncTrigger:
					{
						ULONG ulPin = LongValue( GetCameraDeviceParam( DeviceParamCameraTriggerPin ) );
						ULONG ulRegister = Flea_GPIO_CTRL_PIN_0 + ( ulPin * 0x10 );
						ULONG ulValue = 0;
						long l_lEdge = ( GetTriggerEdge() == 1 ) ? 0 : 1;   // Use Edge on Pin to correct edge selection.

						long l_lCameraTriggerMode = LongValue( GetCameraDeviceParam( DeviceParamCameraTriggerMode ) );

						if ( Firmware( m_strCameraFileFirmware ) < Firmware("1.1.0.13") && l_lCameraTriggerMode != 1 )
						{
							l_lEdge = l_lEdge == 1 ? 0 : 1;   //Invert for older versions.
						}

						SetValueAtBits( &ulValue, DCAM_HIWORD, l_lEdge );
						SetValueAtBits( &ulValue, Flea_GPIO_Pin_Mode_Bits, Flea_GPIO_Pin_Mode_AsyncTrigger );
						HRESULT hrRegister;

						hrRegister = WriteCameraRegister( ulRegister, ulValue );

						break;
					}// end case HardwareTrigger: case SoftwareAsyncTrigger:
				}// end switch ( eTriggerSource )
			}// end  DeviceParamTriggerEdge:
			break;

			case DeviceParamShutterDelay:
			{
				const SVLongValueDeviceParam* pLongParam = rw.DerivedValue( pLongParam );
				ASSERT( pLongParam );
				ULONG ulShutterDelay = pLongParam->lValue;
				ULONG ulRegister = DCAM_TRIGGER_DELAY;
				ULONG ulValue = 0;
				SetValueAtBits( &ulValue, Flea_TRIGGER_DELAY_ShutterDelay_Bits, ulShutterDelay );
				WriteCameraRegister( ulRegister, ulValue );
			}
			break;

			case DeviceParamLut:
			{
				ULONG ulInfo=0;
				ULONG ulOffset=0;
				ReadCameraRegister( Flea_LUT_INFO, ulInfo );
				ReadCameraRegister( Flea_LUT_OFFSET, ulOffset );
				int nBitDepth = (ulInfo >> 16) & 0xFF;
				int nEntries = ulInfo & 0xFFFF;
				ulOffset *= 4;
				ulOffset -= 0x00F00000;

				const SVLutDeviceParam* pLutParam = rw.DerivedValue( pLutParam );
				ASSERT( pLutParam );

				ASSERT( (1 << nBitDepth) == (pLutParam->lut.Info().MaxValue() + 1) );	// check correct output
				ASSERT( nEntries == pLutParam->lut.Info().BandSize() );	// check table size

				if (   ( 1 << nBitDepth) == (pLutParam->lut.Info().MaxValue() + 1)
					&& nEntries == pLutParam->lut.Info().BandSize() )
				{
					std::vector<ULONG> aData;
					NeutralLutToRawLut( pLutParam->lut, aData );
					
					int iBlockSize = AfxGetApp()->GetProfileInt(_T("Debug"), _T("1394_Blocksize"), 64) / 4;
					AfxGetApp()->WriteProfileInt(_T("Debug"), _T("1394_Blocksize"), iBlockSize * 4);
					if ( aData.size() > 0 )
					{
						for (int iBlock=0; iBlock < static_cast<int>(aData.size()); iBlock += iBlockSize)
						{
							int iTransferSize = min(iBlockSize, static_cast<int>(aData.size()) - iBlock);
							WriteCameraRegisterBlock( ulOffset + iBlock * 4, iTransferSize, &aData[iBlock] );
						}
					}
				}
				else	// incorrect LUT specification
				{
					hr = S_FALSE;	// for now
				}
			}// end case DeviceParamLut:
			break;

			case DeviceParamOutputStrobePulse:
			{
				ULONG ulPin = LongValue( GetCameraDeviceParam( DeviceParamCameraStrobePin ) );
				ULONG ulRegister = Flea_GPIO_CTRL_PIN_0 + ( ulPin * 0x10 );
				ULONG ulValue = 0;
				bool bEnableStrobe = (IsCameraOnline()) && (LongValue( rw ) != 0);
				long l_lEdge = 0;   // 0 = falling edge, 1 = rising edge

				SetValueAtBits( &ulValue, DCAM_HIWORD, l_lEdge );
				SetValueAtBits( &ulValue, Flea_GPIO_Pin_Mode_Bits,
					bEnableStrobe ? Flea_GPIO_Pin_Mode_Strobe : Flea_GPIO_Pin_Mode_Output );	// Mode_Output to work correctly with the Flea
				HRESULT hrRegister;
			#ifdef _DEBUG
				ULONG ulPreValue = 0;
				ULONG ulReadValue = 0;
				hrRegister = ReadCameraRegister( ulRegister, ulPreValue );
			#endif
			
				hrRegister = WriteCameraRegister( ulRegister, ulValue );

			#ifdef _DEBUG
				hrRegister = ReadCameraRegister( ulRegister, ulReadValue );
				hrRegister += 0;
			#endif
			}
			break;

			default:
				hr = SVDCamStandardDriver::SetParameter( rw );
				break;
		}// end switch ( pParam->Type() )
	}// end if ( pParam != NULL )

	return hr;
}

HRESULT SVDCamFleaDriver::SetParameters( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_OK;

	if ( m_ulGPIO_XTRA == 0xBAADF00D )
	{
		ReadCameraRegister( Flea_GPIO_XTRA, m_ulGPIO_XTRA );
	}

	// store camera file firmware
	if ( rDeviceParams.ParameterExists( DeviceParamFirmware ) )
	{
		const SVDeviceParamWrapper& w = rDeviceParams.Parameter( DeviceParamFirmware );
		const SVStringValueDeviceParam* psv = w.DerivedValue(psv);
		ASSERT( psv );
		m_strCameraFileFirmware = *psv;
	}

	bool bSetStrobeInfo = false;
	bool bSetTriggerInfo = false;

	SVDeviceParamMap::const_iterator iter;
	for ( iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); ++iter)
	{
		const SVDeviceParamWrapper& rw = iter->second;
		if ( !rw.IsValid() )
			continue;

		switch ( rw->Type() )
		{
			case DeviceParamStrobePulseDuration:
			case DeviceParamStrobePulseDelay:
			{
				if ( !bSetStrobeInfo )
				{
					ULONG ulValueDelay = LongValue( GetCameraDeviceParam( DeviceParamStrobePulseDelay ) );
					ULONG ulValueDuration = LongValue( GetCameraDeviceParam( DeviceParamStrobePulseDuration ) );

					ULONG ulPin = LongValue( GetCameraDeviceParam( DeviceParamCameraStrobePin ) );
					ULONG ulRegister = Flea_GPIO_XTRA_PIN_0 + ( ulPin * 0x10 );
					ULONG ulValue = 0;
					SetValueAtBits( &ulValue, DCAM_HIWORD, ulValueDuration );
					SetValueAtBits( &ulValue, DCAM_LOWORD, ulValueDelay );

					HRESULT hrRegister;

					/*
					// the GPIO_CTRL_PIN_x must be set for strobe before this register can be get/set.
					ULONG ulCtrlRegister = Dragonfly_GPIO_CTRL_PIN_0 + ( ulPin * 0x10 );
					ULONG ulCtrlValue=0;
					ULONG ulCtrlStrobe = 0;
					SetValueAtBits( &ulCtrlStrobe, DCAMBITS_12_15, Dragonfly_GPIO_Pin_Mode_Strobe );
					hrRegister = ReadCameraRegister( ulCtrlRegister, ulCtrlValue );
					hrRegister = WriteCameraRegister( ulCtrlRegister, ulCtrlStrobe );
					*/
				#ifdef _DEBUG
					ULONG ulPreValue = 0;
					ULONG ulReadValue = 0;
					hrRegister = ReadCameraRegister( ulRegister, ulPreValue );
				#endif

					hrRegister = WriteCameraRegister( ulRegister, ulValue );
					
				#ifdef _DEBUG
					hrRegister = ReadCameraRegister( ulRegister, ulReadValue );
					hrRegister += 0;
				#endif

					/*
					// reset the GPIO_CTRL_PIN_x to its original value
					hrRegister = WriteCameraRegister( ulCtrlRegister, ulCtrlValue );
					*/

					SVDeviceParamWrapper w = GetCameraDeviceParam( DeviceParamStrobePulseMultiplier );
					if ( w.IsValid() )
					{
						ULONG ulMultiplier = LongValue(w)-1;
						SetValueAtBits(&m_ulGPIO_XTRA, Flea_GPIO_XTRA_StrobeMultiplier_Bits, ulMultiplier );
						WriteCameraRegister( Flea_GPIO_XTRA, m_ulGPIO_XTRA );
					}

					bSetStrobeInfo = true;
				}// end if ( !bSetStrobeInfo )
			}// end  DeviceParamStrobePulseDelay:
			break;

			case DeviceParamStrobeStartMode:
			case DeviceParamTriggerQueueMode:
			{
				ULONG ulStrobeStart = LongValue( GetCameraDeviceParam( DeviceParamStrobeStartMode ) );
				ULONG ulTriggerQueue = LongValue( GetCameraDeviceParam( DeviceParamTriggerQueueMode ) );
				//ULONG ulValue = 0;
				SetValueAtBits( &m_ulGPIO_XTRA, Flea_GPIO_XTRA_StrobeStart_Bit, ulStrobeStart );
				SetValueAtBits( &m_ulGPIO_XTRA, Flea_GPIO_XTRA_TriggerQueue_Bit, ulTriggerQueue );
				ULONG ulRegister = Flea_GPIO_XTRA;
				WriteCameraRegister( ulRegister, m_ulGPIO_XTRA );
			}
			break;

			case DeviceParamCameraTriggerPin:
			case DeviceParamCameraStrobePin:
				// do nothing; used by other parameters
				break;

			default:
				SetParameter( rw );
				break;
		}// end switch ( w->Type() )
	}// end for ( iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); iter++)

	return hr;
}

HRESULT SVDCamFleaDriver::NeutralLutToRawLut( const SVLut& rLut, std::vector<ULONG>& raData )
{
	HRESULT hr = S_OK;

	UINT uiSize = rLut.Info().BandSize();
	ASSERT( uiSize % 4 == 0 );	// must be a multiple of 4
	uiSize /= 4;
	raData.resize(uiSize);
	for (UINT i = 0; i < uiSize; i++)
	{
		BYTE* pby = reinterpret_cast<BYTE*> (&raData[i]);
		*(pby+0) = rLut(0, i*4+0) & 0xFF;
		*(pby+1) = rLut(0, i*4+1) & 0xFF;
		*(pby+2) = rLut(0, i*4+2) & 0xFF;
		*(pby+3) = rLut(0, i*4+3) & 0xFF;
	}

	return hr;
}

SVDCamFleaDriver::Firmware::Firmware( const CString& strFirmwareString )
{
	Regexp re(_T("([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)$"));	// grab last one in the list (match end of line $)
	re.Match( strFirmwareString );
	int iSub = re.SubStrings();
	if ( iSub == 4 )
	{
		int major = atoi( re[1] );
		int minor = atoi( re[2] );
		int build = atoi( re[4] );
		FirmwareStruct::TypeOfReleaseEnum type = static_cast <FirmwareStruct::TypeOfReleaseEnum> ( atoi( re[3] ) );
		m_Firmware = FirmwareStruct(major, minor, type, build);
	}
}

bool SVDCamFleaDriver::Firmware::operator < ( const Firmware& rhs ) const
{
	return m_Firmware.MajorRevision != rhs.m_Firmware.MajorRevision ? m_Firmware.MajorRevision < rhs.m_Firmware.MajorRevision : 
	       m_Firmware.MinorRevision != rhs.m_Firmware.MinorRevision ? m_Firmware.MinorRevision < rhs.m_Firmware.MinorRevision :
	       m_Firmware.BuildNumber   != rhs.m_Firmware.BuildNumber   ? m_Firmware.BuildNumber   < rhs.m_Firmware.BuildNumber   :
	       m_Firmware.TypeOfRelease != rhs.m_Firmware.TypeOfRelease ? m_Firmware.TypeOfRelease < rhs.m_Firmware.TypeOfRelease :
	       false;
}

bool SVDCamFleaDriver::Firmware::operator == ( const Firmware& rhs ) const
{
	return   m_Firmware.MajorRevision == rhs.m_Firmware.MajorRevision
	      && m_Firmware.MinorRevision == rhs.m_Firmware.MinorRevision
	      && m_Firmware.BuildNumber   == rhs.m_Firmware.BuildNumber
	      && m_Firmware.TypeOfRelease == rhs.m_Firmware.TypeOfRelease;
}

unsigned long SVDCamFleaDriver::GetDCAMTriggerPolarity() const
{
	unsigned long l_Result = 0;   // 0=Active Low(Rising Edge); 1=Active High(Falling Edge)

	return l_Result;
}

