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

