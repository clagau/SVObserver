//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDigitizerProcessingClass
//* .File Name       : $Workfile:   SVDigitizerProcessingClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   04 Aug 2014 07:24:08  $
//******************************************************************************

#pragma region Includes
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
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVConfigurationObject.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

SVDigitizerProcessingClass& SVDigitizerProcessingClass::Instance()
{
	static SVDigitizerProcessingClass l_Object;

	return l_Object;
}

#pragma region Constructor
SVDigitizerProcessingClass::SVDigitizerProcessingClass()
{
}

SVDigitizerProcessingClass::~SVDigitizerProcessingClass()
{
	Shutdown();
}
#pragma endregion Constructor

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

	l_FullName += _T( ".Ch_0" );

	
	SVNameDigitizerMap::iterator l_Iter = m_AcquisitionDevices.find( l_FullName );

	if( l_Iter != m_AcquisitionDevices.end() && nullptr != l_Iter->second  )
	{
		l_Iter->second->m_hDigitizer = p_Handle;

		m_DigitizerSubsystems[ p_szName ] = p_pDigitizerSubsystem;

		m_Digitizers[ p_szName ] = l_Iter->second;
	}
	else
	{
		l_Status = E_FAIL;
	}
	
	return l_Status;
}

HRESULT SVDigitizerProcessingClass::SelectDigitizer( LPCTSTR AcquisitionName )
{
	HRESULT Status = S_OK;
	SVAcquisitionClassPtr pAcquisitionDevice;
	bool SwitchDigitizer = false;

	SVNameDigitizerMap::iterator Iter = m_AcquisitionDevices.find( AcquisitionName );

	if( Iter != m_AcquisitionDevices.end() && nullptr != Iter->second )
	{
		pAcquisitionDevice = Iter->second;
		if( NULL == pAcquisitionDevice->m_hDigitizer)
		{
			SwitchDigitizer = true;
		}
	}
	else
	{
		Status = E_FAIL;
	}

	if( SwitchDigitizer )
	{
		SVString Name( AcquisitionName );
		SVString::size_type pos = Name.rfind('.');
		if( SVString::npos != pos )
		{
			SVString DigitizerName;
			SVString PartnerAcquisitionName;

			DigitizerName = Name.Left( pos );
			PartnerAcquisitionName = DigitizerName;
			if( SVString::npos != Name.find(_T("Ch_0")) )
			{
				PartnerAcquisitionName += _T( ".Ch_All" );
			}
			else
			{
				PartnerAcquisitionName += _T( ".Ch_0" );
			}
			Iter = m_AcquisitionDevices.find( PartnerAcquisitionName );

			if( Iter != m_AcquisitionDevices.end() && nullptr != Iter->second )
			{
				pAcquisitionDevice->m_hDigitizer = Iter->second->m_hDigitizer;
				Iter->second->m_hDigitizer = NULL;
			}
			m_Digitizers[ DigitizerName ] = pAcquisitionDevice;
		}
	}

	return Status;
}

HRESULT SVDigitizerProcessingClass::UpdateIntekDevices()
{
	HRESULT Status = S_OK;

	SV1394CameraStructSet Cameras;

	SVString deviceName = _T( "Matrox_1394.Dig_0" ); // just use the first one

	if( IsValidDigitizerSubsystem( deviceName.ToString() ) )
	{
		SVDigitizerLoadLibraryClass* pLibrary = GetDigitizerSubsystem( deviceName.ToString() );

		unsigned long Count = 0;

		if( pLibrary->GetCount( &Count ) == S_OK )
		{
			for( unsigned long i = 0; i < Count; i++ )
			{
				SV1394CameraStruct Camera;

				unsigned long Handle = NULL;

				if( pLibrary->GetHandle( &Handle, i ) == S_OK )
				{
					_variant_t Value;

					Camera.iPosition = i;
					Camera.m_ulHandle = Handle;

					if( pLibrary->ParameterGetValue( Handle, 100, 0, &Value ) == S_OK )
					{
						Camera.m_csVendorId = Value.bstrVal;
					}

					if( pLibrary->ParameterGetValue( Handle, 101, 0, &Value ) == S_OK )
					{
						Camera.strVendorName = Value.bstrVal;
					}

					if( pLibrary->ParameterGetValue( Handle, 102, 0, &Value ) == S_OK )
					{
						Camera.strModelName = Value.bstrVal;
					}

					if( pLibrary->ParameterGetValue( Handle, 103, 0, &Value ) == S_OK )
					{
						unsigned __int64 l_ui64SerialNumber = ( (unsigned __int64)Value.ulVal ) << 32;

						if( pLibrary->ParameterGetValue( Handle, 104, 0, &Value ) == S_OK )
						{
							Camera.m_ui64SerialNumber = l_ui64SerialNumber + Value.ulVal;
						}
					}

					if( pLibrary->ParameterGetValue( Handle, 105, 0, &Value ) == S_OK )
					{
						Camera.strSerialNum = Value.bstrVal;
					}
				}

				Cameras.Add( Camera );
			}
		}
	}

	TheSV1394CameraManager.UpdateConnectedCameras( Cameras );
	
	Cameras = TheSV1394CameraManager.GetCameraOrder();

	int Count = Cameras.GetSize();

	for( int j = 0; j < Count; j++ )
	{
		SV1394CameraStruct& Camera = Cameras.ElementAt( j );

		if( Camera.m_ulHandle != NULL )
		{
			SVString DigitizerName;

			DigitizerName.Format( _T("Matrox_1394.Dig_%d"), j );

			SVAcquisitionClassPtr pAcquisitionDevice = GetDigitizer( DigitizerName.ToString() );

			if( !( pAcquisitionDevice.empty() ) )
			{
				SVDeviceParamCollection DeviceParams;

				pAcquisitionDevice->m_hDigitizer = Camera.m_ulHandle;

				DeviceParams.SetParameter( DeviceParamSerialNumber, SVi64ValueDeviceParam( Camera.m_ui64SerialNumber ) );
				DeviceParams.SetParameter( DeviceParamSerialNumberString, SVStringValueDeviceParam( Camera.strSerialNum ) );

				pAcquisitionDevice->SetDeviceParameters( DeviceParams );

				//Modify the configuration copy
				SVConfigurationObject* pConfig = nullptr;
				SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
				if( nullptr != pConfig )
				{
					pConfig->ModifyAcquisitionDevice( pAcquisitionDevice->GetRootDeviceName(), &DeviceParams );
				}
			}
		}
	}

	return Status;
}

SVString SVDigitizerProcessingClass::GetReOrderedCamera( LPCTSTR Name ) const
{
	SVString strRet=Name;
	// if the system is Matrox 
	SVString::size_type pos = strRet.find("Matrox_GIGE.Dig_");
	if( pos != SVString::npos )
	{
		SVGigeCameraStructSet GigECamSet = TheSVGigeCameraManager.GetCameraOrder();
		size_t index = strRet[16]-'0';
		if( index < GigECamSet.size() )
		{
			strRet[16] = '0' + GigECamSet[index].iPosition;
		}
	}
	else
	{
		// if the system is Intek
		pos = strRet.find("Matrox_1394.Dig_");
		if( pos != SVString::npos )
		{
			SVGigeCameraStructSet GigECamSet = TheSVGigeCameraManager.GetCameraOrder();
			size_t index = strRet[16]-'0';
			if( index < GigECamSet.size() )
			{
				strRet[16] = '0' + GigECamSet[index].iPosition;
			}
		}
	}
	return strRet;
}

HRESULT SVDigitizerProcessingClass::UpdateMatroxDevices()
{
	HRESULT Status = S_OK;

	SVGigeCameraStructSet Cameras;

	SVString deviceName = _T("Matrox_GIGE.Dig_0"); // just use the first one

	if( IsValidDigitizerSubsystem( deviceName.ToString() ) )
	{
		SVDigitizerLoadLibraryClass* pLibrary = GetDigitizerSubsystem( deviceName.ToString() );

		unsigned long Count = 0;

		if( pLibrary->GetCount( &Count ) == S_OK )
		{
			for( unsigned long i = 0; i < Count; i++ )
			{
				SVGigeCameraStruct Camera;

				unsigned long Handle = NULL;

				if( pLibrary->GetHandle( &Handle, i ) == S_OK )
				{
					_variant_t Value("");

					Camera.iPosition = i;
					Camera.m_ulHandle = Handle;

					if( pLibrary->ParameterGetValue( Handle, SVGigeParameterVendorName, 0, &Value ) == S_OK )
					{
						Camera.strVendorName = Value.bstrVal;
					}

					if( pLibrary->ParameterGetValue( Handle, SVGigeParameterModelName, 0, &Value ) == S_OK )
					{
						Camera.strModelName = Value.bstrVal;
					}

					if( pLibrary->ParameterGetValue( Handle, SVGigeParameterSerialNumber, 0, &Value ) == S_OK )
					{
						Camera.strSerialNum = Value.bstrVal;
					}

					if( pLibrary->ParameterGetValue( Handle, SVGigeParameterIPAddress, 0, &Value ) == S_OK )
					{
						Camera.strIPAddress = Value.bstrVal;
					}
				}

				Cameras.Add( Camera );
			}
		}
	}

	TheSVGigeCameraManager.UpdateConnectedCameras( Cameras );
	Cameras = TheSVGigeCameraManager.GetCameraOrder();

	int Count = Cameras.GetSize();

	for( int j = 0; j < Count; j++ )
	{
		SVGigeCameraStruct& Camera = Cameras.ElementAt( j );

		if( Camera.m_ulHandle != NULL )
		{
			SVString DigitizerName;

			DigitizerName.Format( _T("Matrox_GIGE.Dig_%d"), j );

			SVAcquisitionClassPtr pAcquisitionDevice = GetDigitizer( DigitizerName.ToString() );

			if( !( pAcquisitionDevice.empty() ) )
			{
				SVDeviceParamCollection DeviceParams;

				pAcquisitionDevice->m_hDigitizer = Camera.m_ulHandle;

				DeviceParams.SetParameter( DeviceParamSerialNumberString, SVStringValueDeviceParam( Camera.strSerialNum ) );
				DeviceParams.SetParameter( DeviceParamIPAddress, SVStringValueDeviceParam( Camera.strIPAddress ) );

				// Set packetSize if specified via hardware.ini file
				if (TheSVObserverApp.getGigePacketSize() != 0)
				{
					TheSVObserverApp.SetGigePacketSizeDeviceParam(&DeviceParams);
				}

				pAcquisitionDevice->SetDeviceParameters( DeviceParams );

				//Modify the configuration copy
				SVConfigurationObject* pConfig = nullptr;
				SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
				if( nullptr != pConfig )
				{
					pConfig->ModifyAcquisitionDevice( pAcquisitionDevice->GetRootDeviceName(), &DeviceParams );
				}
			}
		}
	}

	return Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVDigitizerProcessingClass.cpp_v  $
 * 
 *    Rev 1.5   04 Aug 2014 07:24:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  893
 * SCR Title:  Fix Camera Index Issue for Digital Cameras (e115)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added GetRe-ordered Cameras function to insure the correct camera order when they are re-ordered with the camera manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Mar 2014 15:21:16   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Made UpdateIntekDevices and UpdateMatroxDevices public and calling them from their respective Acquisition classes when LoadFile is called instead of in ConnectDevices
 * (this is due to the Device parameters being deleted when configuration loaded)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Mar 2014 18:14:10   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 *   Added DEBUG_NEW.
 *   Various code changes to better follow coding guidelines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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
