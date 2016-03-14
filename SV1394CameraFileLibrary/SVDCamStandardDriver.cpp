//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDCamStandardDriver
//* .File Name       : $Workfile:   SVDCamStandardDriver.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:36:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVDCamStandardDriver.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"

SVDCamStandardDriver::SVDCamStandardDriver()
: SVDCamDriver()
{
}

SVDCamStandardDriver::~SVDCamStandardDriver()
{
}

SVDCamDriver* SVDCamStandardDriver::CreateNew()
{
	return new SVDCamStandardDriver;
}

HRESULT SVDCamStandardDriver::SetParameters( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_OK;

	// store camera file firmware
	if ( rDeviceParams.ParameterExists( DeviceParamFirmware ) )
	{
		const SVDeviceParamWrapper& w = rDeviceParams.Parameter( DeviceParamFirmware );
		const SVStringValueDeviceParam* psv = w.DerivedValue(psv);
		ASSERT( psv );
		m_strCameraFileFirmware = *psv;
	}

	SVDeviceParamMap::const_iterator iter;
	for ( iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); ++iter)
	{
		const SVDeviceParamWrapper& rw = iter->second;
		if ( rw.IsValid() )
		{
			hr = SetParameter( rw );
		}
	}

	return hr;
}

HRESULT SVDCamStandardDriver::SetParameter( const SVDeviceParamWrapper& rw )
{
	HRESULT hr = S_OK;

	const SVDeviceParam* pParam = (const SVDeviceParam*) rw;
	if ( pParam != NULL )
	{
		switch ( pParam->Type() )
		{
			case DeviceParamCameraRegister1:
			case DeviceParamCameraRegister2:
			case DeviceParamCameraRegister3:
			case DeviceParamCameraRegister4:
			case DeviceParamCameraRegister5:
			case DeviceParamCameraRegister6:
			case DeviceParamCameraRegister7:
			case DeviceParamCameraRegister8:
			case DeviceParamCameraRegister9:
			case DeviceParamCameraRegister10:
			case DeviceParamCameraRegister11:
			case DeviceParamCameraRegister12:
			case DeviceParamCameraRegister13:
			case DeviceParamCameraRegister14:
			case DeviceParamCameraRegister15:
			case DeviceParamCameraRegister16:
			case DeviceParamCameraRegister17:
			case DeviceParamCameraRegister18:
			case DeviceParamCameraRegister19:
			case DeviceParamCameraRegister20:
			{
				const SVLongValueDeviceParam* pLongParam = rw.DerivedValue( pLongParam );
				ASSERT( pLongParam );
				unsigned long ulRegister = pLongParam->info.vendorId;
				unsigned long ulValue    = pLongParam->lValue;
				unsigned long ulMask     = pLongParam->info.mask;
				if ( ulMask != 0xFFFFFFFF )
				{
					unsigned long ulCurrentValue = 0;
					ReadCameraRegister( ulRegister, ulCurrentValue );
					// this will shift the value into the proper bit positions;
					// the camera file should NOT have pre shifted values
					SetValueAtBits( &ulCurrentValue, ulMask, ulValue );
					ulValue = ulCurrentValue;
				}
				WriteCameraRegister( ulRegister, ulValue );
			}
			break;

			case DeviceParamShutter:
			case DeviceParamGamma:
			case DeviceParamBrightness:
			case DeviceParamGain:
			case DeviceParamExposure:
			{
			#ifdef _DEBUG
				const SVLongValueDeviceParam* pParam = rw.DerivedValue( pParam );
				if ( pParam )
				{
					// need to get the hardware param to get the vendor id (MIL_ID)
					const SVLongValueDeviceParam* pHardwareParam = GetCameraDeviceParam( rw->Type() ).DerivedValue( pHardwareParam );
					if ( pHardwareParam )
					{
						ULONG ulPreValue;
						ULONG ulValue;
						ULONG ulRegister;
						HRESULT hrRegister;
						switch ( rw->Type() )
						{
							case DeviceParamShutter: ulRegister = DCAM_SHUTTER; break;
							case DeviceParamBrightness: ulRegister = DCAM_BRIGHTNESS; break;
							case DeviceParamGain: ulRegister = DCAM_GAIN; break;
							case DeviceParamExposure: ulRegister = DCAM_AUTO_EXPOSURE; break;
							case DeviceParamGamma: ulRegister = DCAM_GAMMA; break;
						}
						hrRegister = ReadCameraRegister( ulRegister, ulPreValue );

						SetDCAMParameter( rw );

						hrRegister = ReadCameraRegister( ulRegister, ulValue );
						hrRegister += 0;
					}
				}
			#else	// #ifdef _DEBUG 
				SetDCAMParameter( rw );
			#endif	//#ifdef _DEBUG #else
				break;
			}

			case DeviceParamAcquisitionTriggerType:
			{
				SVDCamTriggerSourceEnum eTriggerSource = (SVDCamTriggerSourceEnum) LongValue( rw );
				switch ( eTriggerSource )
				{
					case HardwareTrigger:
					case SoftwareAsyncTrigger:
					{
						// tell the camera manually to go into external trigger mode:
						hr = SetDCAMParameter( rw );

						unsigned long l_TriggerMode = 0;

						unsigned long l_DCAMTriggerPolarity = GetDCAMTriggerPolarity(); // 0=Active Low(Rising Edge); 1=Active High(Falling Edge)
						unsigned long l_DCAMOnOff = GetDCAMTriggerOnOff();   // 0=Off; 1=On
						unsigned long l_DCAMTriggerMode = GetDCAMTriggerMode();  // See DCAM Specification.

						SetValueAtBits( &l_TriggerMode, DCAMBIT6, l_DCAMOnOff );  // turn on/off hardware triggering
						SetValueAtBits( &l_TriggerMode, DCAMBITS_12_15, l_DCAMTriggerMode ); // DCAM tirgger mode
						SetValueAtBits( &l_TriggerMode, DCAMBIT7, l_DCAMTriggerPolarity );

						#ifdef _DEBUG
							ULONG ulPreValue = -1;

							ReadCameraRegister( DCAM_TRIGGER_MODE, ulPreValue );
						#endif

						WriteCameraRegister( DCAM_TRIGGER_MODE, l_TriggerMode );

						#ifdef _DEBUG
							ULONG ulValue = -1;

							ReadCameraRegister( DCAM_TRIGGER_MODE, ulValue );
						#endif

						SetDeviceParamAcquisitionTriggerEdge();
					}// end case SoftwareAsyncTrigger:
					break;

					case SoftwareGrabTrigger:
					default:
						hr = SetDCAMParameter( rw );
						break;
				}// end switch ( eTriggerSource )
			}// end case DeviceParamAcquisitionTriggerType:
			break;

			default:
			{
				hr = SetDCAMParameter( rw );
				break;
			}
		}// end switch ( rw->Type() )
	}// end if ( pParam != NULL )
	return hr;
}

HRESULT SVDCamStandardDriver::SoftwareTrigger()
{
	HRESULT l_Status = S_OK;

	if( CanSoftwareTrigger() )
	{
		unsigned long l_ulRegister = 0;
		unsigned long l_ulValue = 0;

		l_Status = GetSoftwareTriggerRegisterAndValue( l_ulRegister, l_ulValue );

		if( l_Status == S_OK )
		{
			l_Status = WriteCameraRegister( l_ulRegister, l_ulValue );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return S_FALSE;
}

bool SVDCamStandardDriver::CanSoftwareTrigger()
{
	return true;
}

HRESULT SVDCamStandardDriver::GetSoftwareTriggerRegisterAndValue(unsigned long& ulRegister, unsigned long& ulValue)
{
	ulRegister = 0;
	ulValue = 0;
	return S_FALSE;
}

HRESULT SVDCamStandardDriver::GoOnline()
{
	// trigger first, then strobe
	SetParameter( GetCameraDeviceParam( DeviceParamAcquisitionTriggerType ) );
	SetParameter( GetCameraDeviceParam( DeviceParamTriggerEdge ) );
	SetParameter( GetCameraDeviceParam( DeviceParamOutputStrobePulse ) );
	return S_OK;
}

HRESULT SVDCamStandardDriver::GoOffline()
{
	// strobe first, then trigger
	SetParameter( GetCameraDeviceParam( DeviceParamOutputStrobePulse ) );
	SetParameter( GetCameraDeviceParam( DeviceParamAcquisitionTriggerType ) );
	return S_OK;
}

CString SVDCamStandardDriver::GetFirmwareVersion()
{
	return CString();
}

unsigned long SVDCamStandardDriver::GetDCAMTriggerPolarity() const
{
	unsigned long l_Result = 0;  // default trigger on 0=Falling Edge; 1=Rising Edge

	SVDeviceParamWrapper wTriggerEdge = GetCameraDeviceParam( DeviceParamTriggerEdge );
	if ( (SVDeviceParam*) wTriggerEdge != NULL )
	{
		l_Result = LongValue( wTriggerEdge );
	}

	return l_Result;
}

unsigned long SVDCamStandardDriver::GetDCAMTriggerOnOff() const
{
	return IsCameraOnline() ? 1 : 0;
}

unsigned long SVDCamStandardDriver::GetDCAMTriggerMode() const
{
	unsigned long l_Result = 0;  // default triggering mode; 0=External Trigger

	SVDeviceParamWrapper l_wTriggerMode = GetCameraDeviceParam( DeviceParamCameraTriggerMode );
	if ( (SVDeviceParam*) l_wTriggerMode != NULL )
	{
		l_Result = LongValue( l_wTriggerMode );
	}

	return l_Result;
}

unsigned long SVDCamStandardDriver::GetTriggerEdge() const
{
	unsigned long l_Result = 0;  // default trigger on 0=Falling Edge; 1=Rising Edge

	SVDeviceParamWrapper wTriggerEdge = GetCameraDeviceParam( DeviceParamTriggerEdge );
	if ( (SVDeviceParam*) wTriggerEdge != NULL )
	{
		l_Result = LongValue( wTriggerEdge );
	}

	return l_Result;
}

HRESULT SVDCamStandardDriver::SetDeviceParamAcquisitionTriggerEdge()
{
	HRESULT l_Status = S_OK;

	bool l_IsRising = ( GetTriggerEdge() == 1 ); // 0=Falling Edge; 1=Rising Edge

	if ( CameraDeviceParamExists( DeviceParamAcquisitionTriggerEdge ) )
	{
		SVBoolValueDeviceParam* pParam = GetCameraDeviceParamNonConst( DeviceParamAcquisitionTriggerEdge ).DerivedValue( pParam );

		pParam->bValue = l_IsRising;
	}
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

		l_Edge.SetType( DeviceParamAcquisitionTriggerEdge );
		l_Edge.SetDetailLevel( 99 );
		l_Edge.bValue = l_IsRising;

		SVDeviceParamWrapper w = l_Edge;

		SetCameraDeviceParam( w ); // must always have this parameter.
	}

	return l_Status;
}

