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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SV1394CameraFileLibrary\SVDCamFleaDriver.cpp_v  $
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
 *    Rev 1.5   04 Oct 2012 08:52:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   30 Mar 2011 15:36:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construct functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Aug 2010 10:34:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  699
 * SCR Title:  Fix problem with AVT Guppy and Software Triggers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to specify DCAM software trigger or DCAM one shot functionality for software trigger.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Nov 2009 18:39:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated drivers to fix the software trigger functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   23 Oct 2009 14:40:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix trigger edge problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Oct 2009 19:19:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Split Dragonfly and Flea classes apart and fixed issues with functionality and const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Oct 2009 10:42:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  674
 * SCR Title:  Incorrect Strobe timing when using Software Trigger
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetSoftwareTriggerRegisterandValue method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Jun 2009 09:46:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Include file definitions due to changes in SVDeviceParams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Jun 2007 10:12:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 14:56:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   28 Mar 2006 14:07:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  555
 * SCR Title:  Implement new Flea features
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented firmware check, Absolute Value Shutter, bulb mode fix
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   21 Mar 2006 14:09:48   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  555
 * SCR Title:  Fix bulb mode for Flea
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added BulbModeTriggerEdge
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   08 Nov 2005 15:42:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   22 Feb 2005 10:59:20   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  472
 * SCR Title:  Fix problems with Basler cameras
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated poliarity issues in configurating camera registers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   10 Aug 2004 15:33:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  435
 * SCR Title:  Add Bulb Mode Operation to the Digital SVIM for Point Gray Dragonfly Cameras
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   reversed pin polarity for proper functionality
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   30 Jul 2004 13:24:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  435
 * SCR Title:  Add Bulb Mode Operation to the Digital SVIM for Point Gray Dragonfly Cameras
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   DeviceParamAcquisitionTriggerEdge
 * Added Flea handling of DeviceParamTriggerEdge
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   23 Jul 2004 08:29:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  435
 * SCR Title:  Add Bulb Mode Operation to the Digital SVIM for Point Gray Dragonfly Cameras
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SetParameter method to handle the new trigger mode correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   08 Jun 2004 11:20:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  421
 * SCR Title:  Allow SVObserver to get the Trigger Edge and Strobe Edge from the Camera File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SetParameter method to hardcode the trigger edge to falling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   25 May 2004 08:43:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  421
 * SCR Title:  Allow SVObserver to get the Trigger Edge and Strobe Edge from the Camera File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SetParameter method to either add or update the trigger edge parameter information used to configure the internal trigger signal to the camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   17 Feb 2004 16:58:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem of overwriting a register
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   10 Feb 2004 17:40:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed LUT upload problem on the Flea
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   10 Feb 2004 17:28:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed bug with strobe on flea
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   09 Feb 2004 09:30:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   implemented chnages for Flea
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   06 Feb 2004 06:56:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed blocksize setting
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   05 Feb 2004 15:58:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   implemented LUT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   21 Jan 2004 15:27:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   use correct param on StrobeStartMode and TriggerQueueMode
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   16 Jan 2004 07:53:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  399
 * SCR Title:  Increase 1394 SVIM Performance
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added support for StrobePulseDelay / StrobePulseDuration multiplier
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   12 Jan 2004 09:35:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   made change to strobe GPIO pin setting to work with Flea
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   31 Oct 2003 08:21:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CanSoftwareTrigger fn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   17 Sep 2003 09:01:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   general changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   27 Aug 2003 10:21:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed StrobePulse from Bool to Long
 * added enum for bits
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   14 Aug 2003 11:44:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved TriggerType handling to SVDCamStandardDriver
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Aug 2003 12:41:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed param names
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   31 Jul 2003 09:48:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added #include <SVImagingDeviceParams.h>
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Jul 2003 10:50:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added test for NULL
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Jul 2003 08:44:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
