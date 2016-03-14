//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDCamDragonflyDriver
//* .File Name       : $Workfile:   SVDCamDragonflyDriver.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:32:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVDCamDragonflyDriver.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVOMFCLibrary/SVDeviceParamCollection.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"

SVDCamDragonflyDriver::SVDCamDragonflyDriver()
: SVDCamStandardDriver()
{
	m_ulGPIO_XTRA = 0;
}

SVDCamDragonflyDriver::~SVDCamDragonflyDriver()
{
}

SVDCamDriver* SVDCamDragonflyDriver::CreateNew()
{
	return new SVDCamDragonflyDriver;
}

HRESULT SVDCamDragonflyDriver::SetParameter( const SVDeviceParamWrapper& rw )
{
	HRESULT hr = S_OK;

	const SVDeviceParam* pParam = (const SVDeviceParam*) rw;
	if ( pParam != NULL )
	{
		switch ( pParam->Type() )
		{
			case DeviceParamTriggerEdge:
			{
				SVDCamTriggerSourceEnum eTriggerSource = (SVDCamTriggerSourceEnum) LongValue( GetCameraDeviceParam( DeviceParamAcquisitionTriggerType ) );
				switch ( eTriggerSource )
				{
					case HardwareTrigger:
					case SoftwareAsyncTrigger:
					{
						ULONG ulPin = LongValue( GetCameraDeviceParam( DeviceParamCameraTriggerPin ) );
						ULONG ulRegister = Dragonfly_GPIO_CTRL_PIN_0 + ( ulPin * 0x10 );
						ULONG ulValue = 0;
						long l_lEdge = 1;   // 1 = Rising Edge - This must always be rising (rising means moving away from active state).

						SetValueAtBits( &ulValue, DCAM_HIWORD, l_lEdge );
						SetValueAtBits( &ulValue, Dragonfly_GPIO_Pin_Mode_Bits, Dragonfly_GPIO_Pin_Mode_AsyncTrigger );
						HRESULT hrRegister;
					#ifdef _DEBUG
						ULONG ulPreValue = -1;
						ULONG ulGPIO = -1;
						hrRegister = ReadCameraRegister( Dragonfly_GPIO_CONTROL, ulGPIO );
						ULONG ulNumPins = GetValueAtBits( ulGPIO, DCAMBITS_12_15 );
						hrRegister = ReadCameraRegister( ulRegister, ulPreValue );
					#endif

						hrRegister = WriteCameraRegister( ulRegister, ulValue );

					#ifdef _DEBUG
						ULONG ulReadValue=-1;
						hrRegister = ReadCameraRegister( ulRegister, ulReadValue );
						hrRegister+=0;
					#endif

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
				ULONG ulRegister = Dragonfly_SHUTTER_DELAY;
				ULONG ulValue = 0;
				SetValueAtBits( &ulValue, Dragonfly_SHUTTER_DELAY_ShutterDelay_Bits, ulShutterDelay );
				WriteCameraRegister( ulRegister, ulValue );
			}
			break;

			case DeviceParamOutputStrobePulse:
			{
				ULONG ulPin = LongValue( GetCameraDeviceParam( DeviceParamCameraStrobePin ) );
				ULONG ulRegister = Dragonfly_GPIO_CTRL_PIN_0 + ( ulPin * 0x10 );
				ULONG ulValue = 0;
				bool bEnableStrobe = (IsCameraOnline()) && (LongValue( rw ) != 0);
				long l_lEdge = 0;   // 0 = falling edge, 1 = rising edge

				SetValueAtBits( &ulValue, DCAM_HIWORD, l_lEdge );
				SetValueAtBits( &ulValue, Dragonfly_GPIO_Pin_Mode_Bits,
					bEnableStrobe ? Dragonfly_GPIO_Pin_Mode_Strobe : Dragonfly_GPIO_Pin_Mode_Output );	// Mode_Output to work correctly with the Flea
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

HRESULT SVDCamDragonflyDriver::SetParameters( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_OK;

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
					ULONG ulRegister = Dragonfly_GPIO_XTRA_PIN_0 + ( ulPin * 0x10 );
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
						SetValueAtBits(&m_ulGPIO_XTRA, Dragonfly_GPIO_XTRA_StrobeMultiplier_Bits, ulMultiplier );
						WriteCameraRegister( Dragonfly_GPIO_XTRA, m_ulGPIO_XTRA );
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
				SetValueAtBits( &m_ulGPIO_XTRA, Dragonfly_GPIO_XTRA_StrobeStart_Bit, ulStrobeStart );
				SetValueAtBits( &m_ulGPIO_XTRA, Dragonfly_GPIO_XTRA_TriggerQueue_Bit, ulTriggerQueue );
				ULONG ulRegister = Dragonfly_GPIO_XTRA;
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

HRESULT SVDCamDragonflyDriver::GetSoftwareTriggerRegisterAndValue(unsigned long& ulRegister, unsigned long& ulValue)
{
	ulRegister = Dragonfly_SOFT_ASYNC_TRIGGER;
	ulValue = 0;
	return S_OK;
}

