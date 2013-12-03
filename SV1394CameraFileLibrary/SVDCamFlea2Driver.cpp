//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDCamDragonflyDriver
//* .File Name       : $Workfile:   SVDCamFlea2Driver.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Sep 2013 14:15:00  $
//******************************************************************************

#include "stdafx.h"
#include "SVDCamFlea2Driver.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVOMFCLibrary/Regexp.h"
#include "SVOMFCLibrary/SVDeviceParamCollection.h"
#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVOMFCLibrary/SVBoolValueDeviceParam.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"

SVDCamFlea2Driver::SVDCamFlea2Driver()
: SVDCamStandardDriver()
{
	m_ulGPIO_XTRA = 0xBAADF00D;
	m_ulABS_CSR_SHUTTER_REGISTER_BASE = 0xDEADBEEF;
}

SVDCamFlea2Driver::~SVDCamFlea2Driver()
{
}

SVDCamDriver* SVDCamFlea2Driver::CreateNew()
{
	return new SVDCamFlea2Driver;
}

HRESULT SVDCamFlea2Driver::GetSoftwareTriggerRegisterAndValue(unsigned long& ulRegister, unsigned long& ulValue)
{
	ulRegister = DCAM_SOFTWARE_TRIGGER;
	ulValue = 0x80000000;
	return S_OK;
}

HRESULT SVDCamFlea2Driver::SetParameter( const SVDeviceParamWrapper& rw )
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
				SVLUTRegisterUnion ulInfo;

				ulInfo.m_Value = 0;

				ReadCameraRegister( Flea_LUT_INFO, ulInfo.m_Value );

				ulInfo.m_Bits.l_OnOff = 1;

				WriteCameraRegister( Flea_LUT_INFO, ulInfo.m_Value );

				ULONG ulOffset=0;

				ReadCameraRegister( Flea_LUT_OFFSET, ulOffset );

				size_t l_AdjOffset = ( ulOffset * 4 ) & 0x000FFFFF;

				const SVLutDeviceParam* pLutParam = rw.DerivedValue( pLutParam );
				ASSERT( pLutParam );

				int l_FileMaxValue = pLutParam->lut.Info().MaxValue() + 1;
				int l_CameraMaxValue = 1 << ulInfo.m_Bits.l_BitDepth;
				int l_FileBandSize = pLutParam->lut.Info().BandSize();

				ASSERT( l_FileMaxValue == l_CameraMaxValue );	// check correct output
				ASSERT( ulInfo.m_Bits.l_Entries == l_FileBandSize );	// check table size

				if( l_FileMaxValue == l_CameraMaxValue && ulInfo.m_Bits.l_Entries == l_FileBandSize )
				{
					SVLUTVector aData;

					NeutralLutToRawLut( pLutParam->lut, aData );
					
					if( aData.size() == ulInfo.m_Bits.l_Entries / 2 )
					{
						for( size_t i = 0; i < ulInfo.m_Bits.l_Entries / 2; ++i )
						{
							WriteCameraRegister( static_cast<unsigned long>(l_AdjOffset + i) * 4, aData[ i ] );
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

HRESULT SVDCamFlea2Driver::SetParameters( const SVDeviceParamCollection& rDeviceParams )
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

HRESULT SwapEndian( unsigned long& p_rDest, unsigned long p_Source )
{
	HRESULT l_Status = S_OK;

	p_rDest = 0;

	for( size_t i = 0; i < 32; ++i )
	{
		unsigned long l_FromValue = 1 << i;
		unsigned long l_ToValue = 1 << ( 31 - i );

		if( p_Source & l_FromValue )
		{
			p_rDest += l_ToValue;
		}
	}

	return l_Status;
}

HRESULT SVDCamFlea2Driver::NeutralLutToRawLut( const SVLut& rLut, SVLUTVector& raData )
{
	HRESULT hr = S_OK;

	size_t l_Size = rLut.Info().BandSize() / 2;

	raData.resize( l_Size );

	for( size_t i = 0; i < l_Size; ++i )
	{
		unsigned short* l_pValue = reinterpret_cast< unsigned short* >( &raData[ i ] );

		l_pValue[ 1 ] = rLut( 0, static_cast<unsigned long>(i) * 2 ) << 7;
		l_pValue[ 0 ] = rLut( 0, static_cast<unsigned long>(i) * 2 + 1 ) << 7;

		//SwapEndian( raData[ i ], raData[ i ] );
	}

	return hr;
}

SVDCamFlea2Driver::Firmware::Firmware( const CString& strFirmwareString )
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

bool SVDCamFlea2Driver::Firmware::operator < ( const Firmware& rhs ) const
{
	return m_Firmware.MajorRevision != rhs.m_Firmware.MajorRevision ? m_Firmware.MajorRevision < rhs.m_Firmware.MajorRevision : 
	       m_Firmware.MinorRevision != rhs.m_Firmware.MinorRevision ? m_Firmware.MinorRevision < rhs.m_Firmware.MinorRevision :
	       m_Firmware.BuildNumber   != rhs.m_Firmware.BuildNumber   ? m_Firmware.BuildNumber   < rhs.m_Firmware.BuildNumber   :
	       m_Firmware.TypeOfRelease != rhs.m_Firmware.TypeOfRelease ? m_Firmware.TypeOfRelease < rhs.m_Firmware.TypeOfRelease :
	       false;
}

bool SVDCamFlea2Driver::Firmware::operator == ( const Firmware& rhs ) const
{
	return   m_Firmware.MajorRevision == rhs.m_Firmware.MajorRevision
	      && m_Firmware.MinorRevision == rhs.m_Firmware.MinorRevision
	      && m_Firmware.BuildNumber   == rhs.m_Firmware.BuildNumber
	      && m_Firmware.TypeOfRelease == rhs.m_Firmware.TypeOfRelease;
}

unsigned long SVDCamFlea2Driver::GetDCAMTriggerPolarity() const
{
	unsigned long l_Result = 0;   // 0=Active Low(Rising Edge); 1=Active High(Falling Edge)

	return l_Result;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SV1394CameraFileLibrary\SVDCamFlea2Driver.cpp_v  $
 * 
 *    Rev 1.1   30 Sep 2013 14:15:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Apr 2013 17:36:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   04 Oct 2012 08:52:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Mar 2011 15:36:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construct functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Aug 2010 10:34:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  699
 * SCR Title:  Fix problem with AVT Guppy and Software Triggers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to specify DCAM software trigger or DCAM one shot functionality for software trigger.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Nov 2009 18:39:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated drivers to fix the software trigger functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Oct 2009 11:16:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated source code to include new functionality for the Flea2 camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
