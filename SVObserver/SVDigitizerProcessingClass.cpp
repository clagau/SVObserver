//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDigitizerProcessingClass
//* .File Name       : $Workfile:   SVDigitizerProcessingClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   31 Jan 2014 17:16:28  $
//******************************************************************************

#include "stdafx.h"
#include "SVDigitizerProcessingClass.h"
#include "SVGigeCameraParametersLibrary/SVGigeEnums.h"
#include "SVImageLibrary/SVDigitizerLoadLibraryClass.h"
#include "SVImageLibrary/SVLut.h"
#include "SVOMFCLibrary/SVi64ValueDeviceParam.h"
#include "SVCorecoAcquisitionClass.h"
#include "SVFileAcquisitionClass.h"
#include "SVHardwareManifest.h"
#include "SVIntekAcquisitionClass.h"
#include "SVMatroxGigeAcquisitionClass.h"
#include "SV1394CameraStruct.h"
#include "SV1394CameraManager.h"
#include "SVGigeCameraStruct.h"
#include "SVGigeCameraManager.h"
#include "SVObserver.h"

SVDigitizerProcessingClass& SVDigitizerProcessingClass::Instance()
{
	static SVDigitizerProcessingClass l_Object;

	return l_Object;
}

SVDigitizerProcessingClass::SVDigitizerProcessingClass()
{
}

SVDigitizerProcessingClass::~SVDigitizerProcessingClass()
{
	Shutdown();
}

void SVDigitizerProcessingClass::Startup()
{
	const SVAcquisitionConstructParamsVector& l_rAcqParams = SVHardwareManifest::Instance().GetAcquisitionDeviceParams();

	SVAcquisitionConstructParamsVector::const_iterator l_Iter = l_rAcqParams.begin();

	while( l_Iter != l_rAcqParams.end() )
	{
		SVAcquisitionClassPtr l_AcqPtr;

		if( l_Iter->m_DigitizerName.find( _T( "Viper" ) ) == 0 )
		{
			l_AcqPtr = new SVCorecoAcquisitionClass( *l_Iter );
		}
		else if( l_Iter->m_DigitizerName.find( _T( "Matrox_1394" ) ) == 0 )
		{
			l_AcqPtr = new SVIntekAcquisitionClass( *l_Iter );
		}
		else if( l_Iter->m_DigitizerName.find( _T( "Matrox_GIGE" ) ) == 0 )
		{
			l_AcqPtr = new SVMatroxGigeAcquisitionClass( *l_Iter );
		}
		else if( l_Iter->m_DigitizerName.find( _T( "File" ) ) == 0 )
		{
			l_AcqPtr = new SVFileAcquisitionClass( *l_Iter );
		}

		if( !( l_AcqPtr.empty() ) )
		{
			if( l_AcqPtr->Create() == S_OK )
			{
				m_AcquisitionDevices[ l_Iter->m_DeviceName ] = l_AcqPtr;
			}
		}

		++l_Iter;
	}
}

void SVDigitizerProcessingClass::Shutdown()
{
	clear();

	if( !( m_Digitizers.empty() ) )
	{
		m_Digitizers.clear();
	}

	if( !( m_AcquisitionDevices.empty() ) )
	{
		SVNameDigitizerMap::iterator l_Iter = m_AcquisitionDevices.begin();

		while( l_Iter != m_AcquisitionDevices.end() )
		{
			l_Iter->second.clear();

			l_Iter = m_AcquisitionDevices.erase( l_Iter );
		}

		m_AcquisitionDevices.clear();
	}
}

void SVDigitizerProcessingClass::clear()
{
	DisconnectDevices();

	if( !( m_Subsystems.empty() ) )
	{
		m_Subsystems.clear();
	}

	if( !( m_DigitizerSubsystems.empty() ) )
	{
		m_DigitizerSubsystems.clear();
	}
}

void SVDigitizerProcessingClass::ClearDevices()
{
	if( !( m_AcquisitionDevices.empty() ) )
	{
		SVNameDigitizerMap::iterator l_Iter = m_AcquisitionDevices.begin();

		while( l_Iter != m_AcquisitionDevices.end() )
		{
			if( l_Iter->second.empty() )
			{
				l_Iter = m_AcquisitionDevices.erase( l_Iter );
			}
			else
			{
				l_Iter->second->ClearDevice();

				++l_Iter;
			}
		}
	}
}

HRESULT SVDigitizerProcessingClass::UpdateDigitizerSubsystem( SVDigitizerLoadLibraryClass* p_pDigitizerSubsystem )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pDigitizerSubsystem != NULL )
	{
		unsigned long l_ulSize = 0;

		m_Subsystems.insert( p_pDigitizerSubsystem );
		
		l_hrOk = p_pDigitizerSubsystem->GetCount( &l_ulSize );

		for ( unsigned long i = 0; l_hrOk == S_OK && i < l_ulSize; i++ )
		{
			SVString l_Name;

			unsigned long l_ulHandle = NULL;

			l_hrOk = p_pDigitizerSubsystem->GetHandle( &l_ulHandle, i );

			if ( l_hrOk == S_OK )
			{
				BSTR l_bstrName = NULL;

				l_hrOk = p_pDigitizerSubsystem->GetName( l_ulHandle, &l_bstrName );

				if ( l_hrOk == S_OK )
				{
					l_Name = l_bstrName;

					if ( l_bstrName != NULL )
					{
						::SysFreeString( l_bstrName );

						l_bstrName = NULL;
					}
				}
			}

			if ( l_hrOk == S_OK )
			{
				l_hrOk = AddDigitizer( l_Name.ToString(), p_pDigitizerSubsystem, l_ulHandle );
			}
		}
	}

	return l_hrOk;
}

HRESULT SVDigitizerProcessingClass::GetDigitizerList( CStringArray& rList ) const
{
	HRESULT l_Status = S_OK;

	SVNameDigitizerMap::const_iterator l_Iter = m_Digitizers.begin();

	while( l_Iter != m_Digitizers.end() )
	{
		HRESULT l_Temp = S_OK;

		SVAcquisitionClassPtr l_AcqDevicePtr = l_Iter->second;

		if( !( l_AcqDevicePtr.empty() ) && l_AcqDevicePtr->m_hDigitizer != NULL )
		{
			rList.Add( l_AcqDevicePtr->DigName() );
		}
		else
		{
			l_Temp = E_FAIL;
		}

		if( l_Temp != S_OK && l_Status == S_OK )
		{
			l_Status = l_Temp;
		}

		++l_Iter;
	}

	return l_Status;
}

HRESULT SVDigitizerProcessingClass::GetAcquisitionDeviceList( CStringArray& rList ) const
{
	HRESULT l_Status = S_OK;

	SVNameDigitizerMap::const_iterator l_Iter = m_Digitizers.begin();

	while( l_Iter != m_Digitizers.end() )
	{
		SVAcquisitionClassPtr l_AcqDevicePtr = l_Iter->second;

		if( !( l_AcqDevicePtr.empty() ) && l_AcqDevicePtr->m_hDigitizer != NULL )
		{
			rList.Add( l_AcqDevicePtr->DeviceName() );
		}

		++l_Iter;
	}

	return l_Status;
}

bool SVDigitizerProcessingClass::IsValidDigitizerSubsystem( LPCTSTR digitizerName ) const
{
	return ( GetDigitizerSubsystem( digitizerName ) != NULL );
}

SVDigitizerLoadLibraryClass* SVDigitizerProcessingClass::GetDigitizerSubsystem( LPCTSTR digitizerName ) const
{
	SVDigitizerLoadLibraryClass* l_pSubsystem = NULL;

	SVNameDigitizerSubsystemMap::const_iterator l_Iter = m_DigitizerSubsystems.find( digitizerName );

	if( l_Iter != m_DigitizerSubsystems.end() )
	{
		l_pSubsystem = l_Iter->second;
	}

	return l_pSubsystem;
}

SVAcquisitionClassPtr SVDigitizerProcessingClass::GetDigitizer( LPCTSTR szName ) const
{
	SVAcquisitionClassPtr l_AcqPtr;

	SVNameDigitizerMap::const_iterator l_Iter = m_Digitizers.find( szName );

	if( l_Iter != m_Digitizers.end() )
	{
		l_AcqPtr = l_Iter->second;
	}

	return l_AcqPtr;
}

SVAcquisitionClassPtr SVDigitizerProcessingClass::GetAcquisitionDevice( LPCTSTR szName ) const
{
	SVAcquisitionClassPtr l_AcqPtr;

	SVNameDigitizerMap::const_iterator l_Iter = m_AcquisitionDevices.find( szName );

	if( l_Iter != m_AcquisitionDevices.end() )
	{
		l_AcqPtr = l_Iter->second;
	}

	return l_AcqPtr;
}

HRESULT SVDigitizerProcessingClass::DestroyBuffers()
{
	HRESULT l_Status = S_OK;

	SVNameDigitizerMap::const_iterator l_Iter = m_Digitizers.begin();

	while( l_Iter != m_Digitizers.end() )
	{
		HRESULT l_Temp = S_OK;

		SVAcquisitionClassPtr l_AcqDevicePtr = l_Iter->second;

		if( !( l_AcqDevicePtr.empty() ) && l_AcqDevicePtr->m_hDigitizer != NULL )
		{
			l_Temp = l_AcqDevicePtr->DestroyBuffers();
		}

		if( l_Temp != S_OK && l_Status == S_OK )
		{
			l_Status = l_Temp;
		}

		++l_Iter;
	}

	return l_Status;
}

HRESULT SVDigitizerProcessingClass::ConnectDevices()
{
	HRESULT l_Status = S_OK;
	HRESULT l_Temp = S_OK;

	m_DigitizerSubsystems.clear();

	SVDigitizerSubsystemSet::iterator l_Iter = m_Subsystems.begin();

	while( l_Iter != m_Subsystems.end() )
	{
		l_Temp = S_OK;

		if( *l_Iter != NULL )
		{
			l_Temp = UpdateDigitizerSubsystem( *l_Iter );
		}
		else
		{
			l_Temp = E_FAIL;
		}

		if( l_Temp != S_OK && l_Status == S_OK )
		{
			l_Status = l_Temp;
		}

		++l_Iter;
	}

	l_Temp = UpdateIntekDevices();

	if( l_Temp != S_OK && l_Status == S_OK )
	{
		l_Status = l_Temp;
	}

	l_Temp = UpdateMatroxDevices();

	if( l_Temp != S_OK && l_Status == S_OK )
	{
		l_Status = l_Temp;
	}

	return l_Status;
}

HRESULT SVDigitizerProcessingClass::DisconnectDevices()
{
	HRESULT l_Status = S_OK;

	if( !( m_Digitizers.empty() ) )
	{
		SVNameDigitizerMap::const_iterator l_Iter = m_Digitizers.begin();

		while( l_Iter != m_Digitizers.end() )
		{
			HRESULT l_Temp = S_OK;

			SVAcquisitionClassPtr l_AcqDevicePtr = l_Iter->second;

			if( !( l_AcqDevicePtr.empty() ) )
			{
				l_AcqDevicePtr->m_hDigitizer = NULL;
				l_AcqDevicePtr->ClearDeviceIdentifier();
			}
			else
			{
				l_Temp = E_FAIL;
			}

			if( l_Temp != S_OK && l_Status == S_OK )
			{
				l_Status = l_Temp;
			}

			++l_Iter;
		}
	}

	return l_Status;
}

HRESULT SVDigitizerProcessingClass::ScanForCameras()
{
	HRESULT l_Status = S_OK;

	SVDigitizerSubsystemSet::iterator l_Iter = m_Subsystems.begin();

	while( l_Iter != m_Subsystems.end() )
	{
		HRESULT l_Temp = S_OK;

		if( *l_Iter != NULL )
		{
			l_Temp = ( *l_Iter )->ScanForCameras();
		}
		else
		{
			l_Temp = E_FAIL;
		}

		if( l_Temp != S_OK && l_Status == S_OK )
		{
			l_Status = l_Temp;
		}

		++l_Iter;
	}

	return l_Status;
}

HRESULT SVDigitizerProcessingClass::StoreLastCameraImage()
{
	HRESULT l_Status = S_OK;

	SVNameDigitizerMap::const_iterator l_Iter = m_Digitizers.begin();

	while( l_Iter != m_Digitizers.end() )
	{
		HRESULT l_Temp = S_OK;

		SVAcquisitionClassPtr l_AcqDevicePtr = l_Iter->second;

		if( !( l_AcqDevicePtr.empty() ) && l_AcqDevicePtr->m_hDigitizer != NULL )
		{
			l_Temp = l_AcqDevicePtr->StoreLastImage();
		}

		if( l_Temp != S_OK && l_Status == S_OK )
		{
			l_Status = l_Temp;
		}

		++l_Iter;
	}

	return l_Status;
}

HRESULT SVDigitizerProcessingClass::RestoreLastCameraImage()
{
	HRESULT l_Status = S_OK;

	SVNameDigitizerMap::const_iterator l_Iter = m_Digitizers.begin();

	while( l_Iter != m_Digitizers.end() )
	{
		HRESULT l_Temp = S_OK;

		SVAcquisitionClassPtr l_AcqDevicePtr = l_Iter->second;

		if( !( l_AcqDevicePtr.empty() ) && l_AcqDevicePtr->m_hDigitizer != NULL )
		{
			l_Temp = l_AcqDevicePtr->RestoreLastImage();
		}

		if( l_Temp != S_OK && l_Status == S_OK )
		{
			l_Status = l_Temp;
		}

		++l_Iter;
	}

	return l_Status;
}

HRESULT SVDigitizerProcessingClass::AddDigitizer( LPCTSTR p_szName, SVDigitizerLoadLibraryClass* p_pDigitizerSubsystem, unsigned long p_Handle )
{
	HRESULT l_Status = S_OK;

	SVString l_FullName = p_szName;

	if( TheSVObserverApp.IsColorSVIM() )
	{
		l_FullName += _T( ".Ch_All" );
	}
	else
	{
		l_FullName += _T( ".Ch_0" );
	}

	SVNameDigitizerMap::iterator l_Iter = m_AcquisitionDevices.find( l_FullName );

	if( l_Iter != m_AcquisitionDevices.end() && l_Iter->second != NULL )
	{
		l_Iter->second->m_hDigitizer = p_Handle;

		SVNameDigitizerSubsystemMap::iterator l_SubsystemIter = m_DigitizerSubsystems.find( p_szName );

		if( l_SubsystemIter != m_DigitizerSubsystems.end() )
		{
			l_SubsystemIter->second = p_pDigitizerSubsystem;
		}
		else
		{
			m_DigitizerSubsystems[ p_szName ] = p_pDigitizerSubsystem;
		}

		m_Digitizers[ p_szName ] = l_Iter->second;
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVDigitizerProcessingClass::UpdateIntekDevices()
{
	HRESULT l_Status = S_OK;

	SV1394CameraStructSet l_svCameras;

	SVString deviceName = _T( "Matrox_1394.Dig_0" ); // just use the first one

	if( IsValidDigitizerSubsystem( deviceName.ToString() ) )
	{
		SVDigitizerLoadLibraryClass* l_psvLibrary = GetDigitizerSubsystem( deviceName.ToString() );

		unsigned long l_ulCount = 0;

		if( l_psvLibrary->GetCount( &l_ulCount ) == S_OK )
		{
			for( unsigned long i = 0; i < l_ulCount; i++ )
			{
				SV1394CameraStruct camera;

				unsigned long l_ulHandle = NULL;

				if( l_psvLibrary->GetHandle( &l_ulHandle, i ) == S_OK )
				{
					_variant_t l_oValue;

					camera.iPosition = i;
					camera.m_ulHandle = l_ulHandle;

					if( l_psvLibrary->ParameterGetValue( l_ulHandle, 100, 0, &l_oValue ) == S_OK )
					{
						camera.m_csVendorId = l_oValue.bstrVal;
					}

					if( l_psvLibrary->ParameterGetValue( l_ulHandle, 101, 0, &l_oValue ) == S_OK )
					{
						camera.strVendorName = l_oValue.bstrVal;
					}

					if( l_psvLibrary->ParameterGetValue( l_ulHandle, 102, 0, &l_oValue ) == S_OK )
					{
						camera.strModelName = l_oValue.bstrVal;
					}

					if( l_psvLibrary->ParameterGetValue( l_ulHandle, 103, 0, &l_oValue ) == S_OK )
					{
						unsigned __int64 l_ui64SerialNumber = ( (unsigned __int64)l_oValue.ulVal ) << 32;

						if( l_psvLibrary->ParameterGetValue( l_ulHandle, 104, 0, &l_oValue ) == S_OK )
						{
							camera.m_ui64SerialNumber = l_ui64SerialNumber + l_oValue.ulVal;
						}
					}

					if( l_psvLibrary->ParameterGetValue( l_ulHandle, 105, 0, &l_oValue ) == S_OK )
					{
						camera.strSerialNum = l_oValue.bstrVal;
					}
				}

				l_svCameras.Add( camera );
			}
		}
	}

	TheSV1394CameraManager.UpdateConnectedCameras( l_svCameras );

	l_svCameras = TheSV1394CameraManager.GetCameraOrder();
	
	int l_iCount = l_svCameras.GetSize();

	for( int j = 0; j < l_iCount; j++ )
	{
		SV1394CameraStruct& l_rsvItem = l_svCameras.ElementAt( j );

		if( l_rsvItem.m_ulHandle != NULL )
		{
			SVString l_csDigName;
			SVAcquisitionClassPtr l_AcqDevicePtr;

			l_csDigName.Format( _T("Matrox_1394.Dig_%d"), j );

			l_AcqDevicePtr = GetDigitizer( l_csDigName.ToString() );

			if( !( l_AcqDevicePtr.empty() ) )
			{
				SVDeviceParamCollection params;

				l_AcqDevicePtr->m_hDigitizer = l_rsvItem.m_ulHandle;

				l_AcqDevicePtr->GetDeviceParameters( params );

				params.SetParameter( DeviceParamSerialNumber, SVi64ValueDeviceParam( l_rsvItem.m_ui64SerialNumber ) );
				params.SetParameter( DeviceParamSerialNumberString, SVStringValueDeviceParam( l_rsvItem.strSerialNum ) );

				l_AcqDevicePtr->SetDeviceParameters( params );
			}
		}
	}

	return l_Status;
}

HRESULT SVDigitizerProcessingClass::UpdateMatroxDevices()
{
	HRESULT l_Status = S_OK;

	SVGigeCameraStructSet l_svCameras;

	CString deviceName = _T("Matrox_GIGE.Dig_0"); // just use the first one

	if( IsValidDigitizerSubsystem( deviceName ) )
	{
		SVDigitizerLoadLibraryClass* l_psvLibrary = GetDigitizerSubsystem( deviceName );

		unsigned long l_ulCount = 0;

		if( l_psvLibrary->GetCount( &l_ulCount ) == S_OK )
		{
			for( unsigned long i = 0; i < l_ulCount; i++ )
			{
				SVGigeCameraStruct camera;

				unsigned long l_ulHandle = NULL;

				if( l_psvLibrary->GetHandle( &l_ulHandle, i ) == S_OK )
				{
					_variant_t l_oValue("");

					camera.iPosition = i;
					camera.m_ulHandle = l_ulHandle;

					if( l_psvLibrary->ParameterGetValue( l_ulHandle, SVGigeParameterVendorName, 0, &l_oValue ) == S_OK )
					{
						camera.strVendorName = l_oValue.bstrVal;
					}

					if( l_psvLibrary->ParameterGetValue( l_ulHandle, SVGigeParameterModelName, 0, &l_oValue ) == S_OK )
					{
						camera.strModelName = l_oValue.bstrVal;
					}

					if( l_psvLibrary->ParameterGetValue( l_ulHandle, SVGigeParameterSerialNumber, 0, &l_oValue ) == S_OK )
					{
						camera.strSerialNum = l_oValue.bstrVal;
					}

					if( l_psvLibrary->ParameterGetValue( l_ulHandle, SVGigeParameterIPAddress, 0, &l_oValue ) == S_OK )
					{
						camera.strIPAddress = l_oValue.bstrVal;
					}

				}

				l_svCameras.Add( camera );
			}
		}
	}

	TheSVGigeCameraManager.UpdateConnectedCameras( l_svCameras );

	l_svCameras = TheSVGigeCameraManager.GetCameraOrder();
	
	int l_iCount = l_svCameras.GetSize();

	for( int j = 0; j < l_iCount; j++ )
	{
		SVGigeCameraStruct& l_rsvItem = l_svCameras.ElementAt( j );

		if( l_rsvItem.m_ulHandle != NULL )
		{
			CString l_csDigName;

			l_csDigName.Format( _T("Matrox_GIGE.Dig_%d"), j );

			SVAcquisitionClassPtr l_AcqDevicePtr = GetDigitizer( l_csDigName );

			if( !( l_AcqDevicePtr.empty() ) )
			{
				SVDeviceParamCollection params;

				l_AcqDevicePtr->m_hDigitizer = l_rsvItem.m_ulHandle;

				l_AcqDevicePtr->GetDeviceParameters( params );

				params.SetParameter( DeviceParamSerialNumberString, SVStringValueDeviceParam( l_rsvItem.strSerialNum ) );
				params.SetParameter( DeviceParamIPAddress, SVStringValueDeviceParam( l_rsvItem.strIPAddress ) );

				// Set packetSize if specified via hardware.ini file
				if (TheSVObserverApp.getGigePacketSize() != 0)
				{
					TheSVObserverApp.SetGigePacketSizeDeviceParam(&params);
				}

				l_AcqDevicePtr->SetDeviceParameters( params );
			}
		}
	}

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDigitizerProcessingClass.cpp_v  $
 * 
 *    Rev 1.2   31 Jan 2014 17:16:28   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to follow guidelines more closely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Jun 2013 15:19:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   22 May 2013 10:18:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  833
 * SCR Title:  Change GigE Camera Manager to be based on IP Address
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed UpdateMatroxDevices to get/set SVGigeParameterIPAddress
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:11:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   30 Mar 2011 16:00:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   22 Mar 2011 07:50:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   03 Mar 2010 11:55:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated camera manager to fix issues with changing cameras.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   24 Feb 2010 10:30:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to handle storing and restoring the image buffers when the system is going on and off line.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   18 Feb 2010 14:17:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use new construction functionality via the hardware manifest object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   16 Feb 2010 10:12:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated add digitizer methodology to find acquisition device using SVObserver color information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   15 Feb 2010 12:25:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes to remove the generations of board image processing classes from the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   15 Dec 2009 15:56:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   30 Jul 2009 11:24:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Jun 2009 08:42:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised LocalCreate to support Matrox Gige
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   04 Feb 2009 13:36:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised LocalCreate method to correct camera manager problem when no 1394 cameras are connected at startup.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   23 Oct 2008 17:55:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added AddAvailableDigitizer method
 * Revised LocalCreate to add all available 1394 digitizers
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Sep 2008 13:46:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GetDigitizerSubsystem, GetImageSubsystem, IsValidDigitizerSubsystem, and IsValidImageSubsystem to use device name to support multiple acquisition devices.
 * 
 * Added map of Digitizer Devcie Name, Digitizer Device Interface and Image Interface pairs to support mulitple Digitizer Devices.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3.1.0   14 Jul 2009 13:00:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Aug 2006 09:50:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  567
 * SCR Title:  Update SVObserver to accept Intek driver for D-series SVIMs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated code to remove that one or more digitizer must be present.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 Nov 2005 15:47:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated class to include a method to give the digitizer count.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Oct 2003 14:14:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  389
 * SCR Title:  Present Error Messages for Invalid Hardware using User Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated LocalCreate method to add an error condition when the number of digitizers are zero.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2003 13:02:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   New class added to complete the new I/O interface methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
