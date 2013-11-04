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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SV1394CameraFileLibrary\SVDCamStandardDriver.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:36:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   04 Oct 2012 08:52:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   30 Mar 2011 15:36:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construct functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   17 Aug 2010 10:34:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  699
 * SCR Title:  Fix problem with AVT Guppy and Software Triggers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to specify DCAM software trigger or DCAM one shot functionality for software trigger.
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
 *    Rev 1.4   23 Oct 2009 14:38:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix trigger edge problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Oct 2009 19:19:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Split Dragonfly and Flea classes apart and fixed issues with functionality and const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Oct 2009 10:42:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  674
 * SCR Title:  Incorrect Strobe timing when using Software Trigger
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetSoftwareTriggerRegisterandValue method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Jun 2009 09:46:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Include file definitions due to changes in SVDeviceParams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 14:57:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   23 Jun 2006 09:26:34   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  568
 * SCR Title:  Add Register/Value Parameter capability to 1394 camera files and SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CameraRegister parameter functionality
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   28 Mar 2006 14:04:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  555
 * SCR Title:  Implement new Flea features
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented camera file firmware
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   08 Nov 2005 15:42:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated 1394 code to allow for the use of the Intek driver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   23 Jul 2004 08:29:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  435
 * SCR Title:  Add Bulb Mode Operation to the Digital SVIM for Point Gray Dragonfly Cameras
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SetParameter method to handle the new trigger mode correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   08 Jun 2004 10:44:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  421
 * SCR Title:  Allow SVObserver to get the Trigger Edge and Strobe Edge from the Camera File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated comment
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   25 May 2004 08:43:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  421
 * SCR Title:  Allow SVObserver to get the Trigger Edge and Strobe Edge from the Camera File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SetParameter method to either add or update the trigger edge parameter information used to configure the internal trigger signal to the camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   09 Feb 2004 09:32:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added DCAM_TRIGGER_DELAY
 * added GetFirmwareVersion
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   31 Oct 2003 08:21:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added CanSoftwareTrigger fn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Sep 2003 14:56:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added gamma device param support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Sep 2003 09:00:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   general changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   27 Aug 2003 10:23:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added GoOnline and GoOffline
 * added TriggerEdge support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Aug 2003 11:45:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved TriggerType handling from SVDCamDragonflyDriver
 * fixed trigger edge (now hard-coded to rising edge)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Jul 2003 10:47:26   ebeyeler
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
