//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxBoardImageProcessingClass
//* .File Name       : $Workfile:   SVMatroxBoardImageProcessingClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:30:50  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxBoardImageProcessingClass.h"

#include "SVDisplayImageProcessingClass.h"
#include "SVImageProcessingClass.h"
#include "SVMatroxAcquisitionClass.h"

#include "SVObserver.h"

//SVMatroxDeviceManager SVMatroxBoardImageProcessingClass::m_DeviceManager;

SVMatroxBoardImageProcessingClass::SVMatroxBoardImageProcessingClass( SVImageProcessingClass* pSubsystem, 
	                                                                    SVDigitizerProcessingClass *p_pDigitizerSubsystem )
	                               : SVBoardImageProcessingClass( pSubsystem, p_pDigitizerSubsystem, _T("SVMatroxBoardImageProcessingClass") )
{
	// Indentify your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVImageProcessingObjectType;
}

SVMatroxBoardImageProcessingClass::SVMatroxBoardImageProcessingClass( SVImageProcessingClass* pSubsystem, 
	                                                                    SVDigitizerProcessingClass *p_pDigitizerSubsystem,
																																		  LPCSTR ObjectName )
	                               : SVBoardImageProcessingClass( pSubsystem, p_pDigitizerSubsystem, ObjectName )
{
	// Indentify your type...
	outObjectInfo.ObjectTypeInfo.ObjectType = SVImageProcessingObjectType;
}

SVMatroxBoardImageProcessingClass::~SVMatroxBoardImageProcessingClass()
{
	LocalDestroy();
}

HRESULT SVMatroxBoardImageProcessingClass::Create()
{
	HRESULT l_hrOk = SVBoardImageProcessingClass::Create();

	if ( l_hrOk == S_OK )
	{
		l_hrOk = LocalCreate();

		if ( l_hrOk == S_OK )
		{
			
			SVLutInfo lutinfo;
			SVDefaultLutTransform luttransform;
			SVLutTransformOperationNormal lutoperation;
			luttransform.SetOperation( lutoperation );
			lutinfo.SetTransform( luttransform );

			bool bColorSystem = TheSVObserverApp.IsColorSVIM() ? true : false;

			for (int iDig=0; iDig < m_DeviceManager.GetMaxNumDigitizers(); iDig++)
			{
				CString sAcqDeviceName;
				CString sDigName;
				sDigName.Format( _T("Matrox_1394.Dig_%d"), iDig );

				SVMatroxDeviceParamCollection device;
				HRESULT hrFoundDevice = m_DeviceManager.GetDevice( sDigName, device);

				MIL_ID milDig = device.milDig;
				bool bValidDigitizer = device.bValidDigitizer;

				if ( hrFoundDevice != S_OK )
				{
					device.SetParameter(DeviceParamDeviceName, DeviceParam(sDigName) );
					device.SetParameter(DeviceParamDeviceDigNum, DeviceParam(iDig) );
				}

				bool bCameraSupportsColor = false;
				const SVCameraFormatsDeviceParam* pParam = device.Parameter( DeviceParamCameraFormats ).DerivedValue( pParam );
				if ( pParam )
				{
					bCameraSupportsColor = pParam->SupportsColor();
					/*
					SVCameraFormatsDeviceParam::OptionsType::const_iterator iter;
					for ( iter = pParam->options.begin(); iter != pParam->options.end(); iter++)
					{
						if ( iter->second.bColor )
							bCameraSupportsColor = true;
					}
					*/
				}


				SVMatroxAcquisitionClass* pAcqDevice = NULL;

				// always create mono and color versions

				bool bMakeColor = false;
				do
				{
					pAcqDevice = new SVMatroxAcquisitionClass;
					
					pAcqDevice->SetMatroxDeviceManagerPointer( &m_DeviceManager );
					pAcqDevice->SetImageProcessingClass( mpImageProcessingSubsystem );
					pAcqDevice->SetChannel( 0 );

					int iBands = bMakeColor ? 3 : 1;

					pAcqDevice->SetBandSize( iBands );
					pAcqDevice->SetBandMaxSize( iBands );
					lutinfo.SetBands( iBands );
					pAcqDevice->CreateLut( lutinfo );

					pAcqDevice->CreateLightReference( iBands, NULL, NULL );

					SVODeviceClass* pStart = NULL;
					SVODeviceClass* pEnd = NULL;

	//				if ( !(device.milDig && LongValue(device.Parameter( DeviceParamSizeBands )) == 3) )
	//				if ( device.milDig == M_NULL || !bCameraSupportsColor )
					if ( !bMakeColor )
					{
						sAcqDeviceName =  sDigName + ".Ch_0";
						pAcqDevice->GetStartDevice( &pStart );
						pStart->mDeviceName.SetValue( sAcqDeviceName );

						pAcqDevice->GetEndDevice( &pEnd );
						pEnd->mDeviceName.SetValue( sAcqDeviceName );
					}
					else
					{
						sAcqDeviceName =  sDigName + ".Ch_All";
						pAcqDevice->GetStartDevice( &pStart );
						pStart->mDeviceName.SetValue( sAcqDeviceName );

						pAcqDevice->GetEndDevice( &pEnd );
						pEnd->mDeviceName.SetValue( sAcqDeviceName );
					} // end if b&w else color

					device.milDig = (bMakeColor == bColorSystem) ? milDig : M_NULL;
					device.bValidDigitizer = (bMakeColor == bColorSystem) ? bValidDigitizer : false;
					
					//device.SetParameter(DeviceParamDeviceChannelName, DeviceParam(sAcqDeviceName) );	// this causes connectdevices / disconnectdevices to work incorrectly
					pAcqDevice->SetHardwareDeviceCapabilities( device );
					pAcqDevice->SetDeviceName(sAcqDeviceName);	// not a hardware capability; only for SVO use. 

					pAcqDevice->msvDeviceInterface.mDeviceName.SetValue( sAcqDeviceName );
					BOOL bCreated = pAcqDevice->Create();

					HRESULT hrAddAcq = AddAcquisitionDevice( sAcqDeviceName, pAcqDevice );

					if ( bMakeColor == bColorSystem )
					{
						HRESULT hrAddDig = AddDigitizer( sDigName, pAcqDevice );
					}
				} while ( !(bMakeColor++) );	// postincrement so this is true the first time through


			}// end for (int iDig=0; iDig < m_DeviceManager.GetMaxNumDigitizers(); iDig++)

			m_bDevicesConnected = true;
		}// end if
		else
		{
			SVBoardImageProcessingClass::Destroy();		
		}
	}

	return l_hrOk;
}

HRESULT SVMatroxBoardImageProcessingClass::Destroy()
{
	HRESULT l_hrOk = S_OK;

	m_bDevicesConnected = false;

	if ( LocalDestroy() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	if ( SVBoardImageProcessingClass::Destroy() != S_OK )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVMatroxBoardImageProcessingClass::LocalCreate()
{
	return m_DeviceManager.Create();
}

HRESULT SVMatroxBoardImageProcessingClass::LocalDestroy()
{
	return m_DeviceManager.Destroy();
}

HRESULT SVMatroxBoardImageProcessingClass::DisconnectDevices()
{
	if ( m_bDevicesConnected )
	{
		m_bDevicesConnected = false;

		SVAcquisitionClass* pAcqDevice = NULL;

		POSITION pos = mDigitizerMap.GetStartPosition();
		CString sDigName;

		while ( pos != NULL )
		{
			mDigitizerMap.GetNextAssoc( pos, sDigName, pAcqDevice );

			if ( pAcqDevice != NULL )
			{
				SVMatroxDeviceParamCollection device;
				HRESULT hrFoundDevice = m_DeviceManager.GetDevice( sDigName, device );

				if ( hrFoundDevice == S_OK )
				{
					device.milDig = M_NULL;
					device.milSystem = M_NULL;
					pAcqDevice->SetHardwareDeviceCapabilities( device );
				}
			}
		}

		m_DeviceManager.Destroy();
	}

	return S_OK;
}

HRESULT SVMatroxBoardImageProcessingClass::ConnectDevices()
{
	if ( !m_bDevicesConnected )
	{
		m_DeviceManager.Create();

		// check for changes in attached cameras???

		SVAcquisitionClass* pAcqDevice = NULL;

		POSITION pos = mDigitizerMap.GetStartPosition();
		CString sDigName;

		while ( pos != NULL )
		{
			mDigitizerMap.GetNextAssoc( pos, sDigName, pAcqDevice );

			if ( pAcqDevice != NULL )
			{
				SVMatroxDeviceParamCollection device;
				HRESULT hrFoundDevice = m_DeviceManager.GetDevice( sDigName, device );

				if ( hrFoundDevice == S_OK )
				{
					pAcqDevice->SetHardwareDeviceCapabilities( device );
					// reload all params
					SVDeviceParamCollection params;
					pAcqDevice->GetDeviceParameters( params );
					pAcqDevice->SetDeviceParameters( params );
				}
			}
		}

		m_bDevicesConnected = true;
	}

	return S_OK;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVMatroxBoardImageProcessingClass.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:30:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   26 Oct 2004 09:11:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  461
 * SCR Title:  Add 1394 Hub capability to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   get max digitizers from MatroxDeviceManager, not hard coded.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   18 Dec 2003 14:56:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed irrelevant parameter
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   18 Dec 2003 14:51:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed Connect / Disconnect problem that reset the DeviceChannelName to .Ch_0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   19 Nov 2003 11:53:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed color stuff
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   12 Nov 2003 09:40:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   always create both b/w and color versions of the acq devices
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   03 Nov 2003 16:24:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changes to allow unplugging and plugging cameras
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   31 Oct 2003 08:25:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added ConnectDevices / DisconnectDevices
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Jul 2003 10:00:58   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Matrox 1394 support
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 11:17:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   26 Feb 2003 09:03:44   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed stub constructors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   26 Feb 2003 08:47:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   stubbed out the functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   06 Feb 2003 10:04:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Initial iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/