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
#include "SVFileAcquisitionClass.h"
#include "SVHardwareManifest.h"
#include "SVMatroxGigeAcquisitionClass.h"
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

		if( l_Iter->m_DigitizerName.find( _T( "Matrox_GIGE" ) ) == 0 )
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
					l_Name = SvUl_SF::createSVString(l_bstrName);

					if ( l_bstrName != NULL )
					{
						::SysFreeString( l_bstrName );

						l_bstrName = NULL;
					}
				}
			}

			if ( l_hrOk == S_OK )
			{
				l_hrOk = AddDigitizer( l_Name.c_str(), p_pDigitizerSubsystem, l_ulHandle );
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

			DigitizerName = Name.substr( 0, pos );
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
	return strRet;
}

HRESULT SVDigitizerProcessingClass::UpdateMatroxDevices()
{
	HRESULT Status = S_OK;

	SVGigeCameraStructSet Cameras;

	SVString deviceName = _T("Matrox_GIGE.Dig_0"); // just use the first one

	if( IsValidDigitizerSubsystem( deviceName.c_str() ) )
	{
		SVDigitizerLoadLibraryClass* pLibrary = GetDigitizerSubsystem( deviceName.c_str() );

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
			SVString DigitizerName = SvUl_SF::Format( _T("Matrox_GIGE.Dig_%d"), j );

			SVAcquisitionClassPtr pAcquisitionDevice = GetDigitizer( DigitizerName.c_str() );

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

