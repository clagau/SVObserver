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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SV1394CameraFileLibrary\SVDCamDragonflyDriver.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:32:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   04 Oct 2012 08:52:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Mar 2011 15:36:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construct functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   04 Nov 2009 18:39:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated drivers to fix the software trigger functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Oct 2009 19:19:30   jspila
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
