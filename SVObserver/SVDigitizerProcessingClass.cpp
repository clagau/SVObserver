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
#include "TriggerInformation/SVHardwareManifest.h"
#include "SVMatroxGigeAcquisitionClass.h"
#include "SVGigeCameraFileLibrary/SVGigeCameraStruct.h"
#include "SVGigeCameraManager.h"
#include "SVObserver.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVConfigurationObject.h"
#include "TextDefinesSvO.h"
#include "SVGlobal.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const TCHAR* const cMatroxGigeDigitizer = _T("Matrox_GIGE.Dig_%d");

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
	const SvTi::SVAcquisitionConstructParamsVector& l_rAcqParams = SvTi::SVHardwareManifest::Instance().GetAcquisitionDeviceParams();

	SvTi::SVAcquisitionConstructParamsVector::const_iterator l_Iter = l_rAcqParams.begin();

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
			if( S_OK == l_AcqPtr->Create() )
			{
				m_AcquisitionDevices[ l_Iter->m_DigitizerName ] = l_AcqPtr;
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

HRESULT SVDigitizerProcessingClass::UpdateDigitizerSubsystem( SVDigitizerLoadLibraryClass* pDigitizerSubsystem )
{
	HRESULT Result = S_FALSE;

	if ( nullptr != pDigitizerSubsystem )
	{
		unsigned long l_ulSize = 0;

		m_Subsystems.insert( pDigitizerSubsystem );
		
		Result = pDigitizerSubsystem->GetCount( &l_ulSize );

		for ( unsigned long i = 0; S_OK == Result && i < l_ulSize; i++ )
		{
			SVString AcquisitionName;
			SVString IPAddress;

			unsigned long Handle = 0;

			Result = pDigitizerSubsystem->GetHandle( &Handle, i );

			if ( S_OK == Result )
			{
				_variant_t Value;
				_bstr_t StringValue;

				Result = pDigitizerSubsystem->GetName( Handle, StringValue.GetAddress() );
				if( S_OK == Result )
				{
					AcquisitionName = static_cast<LPCTSTR> (StringValue);
				}
				//Not all DLL's support IP address so do not place in Result value
				if( S_OK == pDigitizerSubsystem->ParameterGetValue( Handle, SVGigeParameterIPAddress, 0, &Value ) )
				{
					StringValue =  Value.bstrVal;
					IPAddress = static_cast<LPCTSTR> (StringValue);
				}
			}

			if ( S_OK == Result )
			{
				SVString CameraName;
				if( AcquisitionName.find( _T( "Matrox_GIGE" ) ) == 0 )
				{
					CameraName =  SVGigeCameraManager::Instance().getCameraName( IPAddress );
				}
				else
				{
					CameraName = AcquisitionName;
				}
				if( !CameraName.empty() )
				{
					Result = AddDigitizer( CameraName.c_str(), AcquisitionName.c_str(), pDigitizerSubsystem, Handle );
				}
			}
		}
	}

	return Result;
}

HRESULT SVDigitizerProcessingClass::GetAcquisitionDeviceList( SVStringArray& rList ) const
{
	HRESULT l_Status = S_OK;

	SVNameDigitizerMap::const_iterator l_Iter = m_Digitizers.begin();

	while( l_Iter != m_Digitizers.end() )
	{
		SVAcquisitionClassPtr l_AcqDevicePtr = l_Iter->second;

		if( !( l_AcqDevicePtr.empty() ) && 0 != l_AcqDevicePtr->m_hDigitizer )
		{
			rList.push_back( SVString( l_AcqDevicePtr->DeviceName() ) );
		}

		++l_Iter;
	}

	return l_Status;
}

bool SVDigitizerProcessingClass::IsValidDigitizerSubsystem( LPCTSTR digitizerName ) const
{
	return ( nullptr != GetDigitizerSubsystem( digitizerName ) );
}

SVDigitizerLoadLibraryClass* SVDigitizerProcessingClass::GetDigitizerSubsystem( LPCTSTR digitizerName ) const
{
	SVDigitizerLoadLibraryClass* l_pSubsystem = nullptr;

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

		if( !( l_AcqDevicePtr.empty() ) && 0 != l_AcqDevicePtr->m_hDigitizer )
		{
			l_Temp = l_AcqDevicePtr->DestroyBuffers();
		}

		if( S_OK != l_Temp && S_OK == l_Status )
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
	m_Digitizers.clear();

	SVDigitizerSubsystemSet::iterator l_Iter = m_Subsystems.begin();

	while( l_Iter != m_Subsystems.end() )
	{
		l_Temp = S_OK;

		if( nullptr != *l_Iter )
		{
			l_Temp = UpdateDigitizerSubsystem( *l_Iter );
		}
		else
		{
			l_Temp = E_FAIL;
		}

		if( S_OK != l_Temp && S_OK == l_Status )
		{
			l_Status = l_Temp;
		}

		++l_Iter;
	}

	l_Temp = UpdateMatroxDevices();

	if( S_OK != l_Temp && S_OK == l_Status )
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
				l_AcqDevicePtr->m_hDigitizer = 0;
				l_AcqDevicePtr->ClearDeviceIdentifier();
			}
			else
			{
				l_Temp = E_FAIL;
			}

			if( S_OK != l_Temp && S_OK == l_Status )
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

		if( nullptr != *l_Iter )
		{
			l_Temp = ( *l_Iter )->ScanForCameras();
		}
		else
		{
			l_Temp = E_FAIL;
		}

		if( S_OK != l_Temp && S_OK == l_Status )
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

		if( !( l_AcqDevicePtr.empty() ) && 0 < l_AcqDevicePtr->m_hDigitizer )
		{
			l_Temp = l_AcqDevicePtr->StoreLastImage();
		}

		if( S_OK != l_Temp && S_OK == l_Status )
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

		if( !( l_AcqDevicePtr.empty() ) && 0 != l_AcqDevicePtr->m_hDigitizer )
		{
			l_Temp = l_AcqDevicePtr->RestoreLastImage();
		}

		if( S_OK != l_Temp && S_OK == l_Status )
		{
			l_Status = l_Temp;
		}

		++l_Iter;
	}

	return l_Status;
}

HRESULT SVDigitizerProcessingClass::AddDigitizer( LPCTSTR Name, LPCTSTR AcquisitionName, SVDigitizerLoadLibraryClass* pDigitizerSubsystem, unsigned long p_Handle )
{
	HRESULT Result( S_OK );
	SVString DigitizerName( Name );
	SVString AcqName( AcquisitionName );

	SVNameDigitizerMap::iterator l_Iter = m_AcquisitionDevices.find( AcqName );

	if( l_Iter != m_AcquisitionDevices.end() && nullptr != l_Iter->second  )
	{
		l_Iter->second->m_hDigitizer = p_Handle;

		m_DigitizerSubsystems[ AcqName ] = pDigitizerSubsystem;

		m_Digitizers[ DigitizerName ] = l_Iter->second;
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

HRESULT SVDigitizerProcessingClass::SetDigitizerColor( LPCTSTR DigitizerName, bool isColor )
{
	HRESULT Result = S_OK;
	SVAcquisitionClassPtr pAcquisitionDevice;

	pAcquisitionDevice = GetAcquisitionDevice( DigitizerName );

	if( !pAcquisitionDevice.empty() )
	{
		int NumberOfBands( 1 );
		if( isColor )
		{
			NumberOfBands = 3;
		}
		pAcquisitionDevice->SetNumberOfBands( NumberOfBands );
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

SVString SVDigitizerProcessingClass::GetReOrderedCamera( const int CameraID, bool NewOrder ) const
{
	SVString Result;
	const SVGigeCameraStructVector& rGigeCameraOrder = SVGigeCameraManager::Instance().GetCameraOrder();
	SVGigeCameraStructVector::const_iterator Iter( rGigeCameraOrder.begin() );
	for(; rGigeCameraOrder.end() != Iter && Result.empty(); ++Iter )
	{
		if( CameraID == Iter->m_CameraID )
		{
			int DigitizerID = Iter->m_DigitizerID;
			//! If Digitizer not set then use the camera ID offset by maximum physical cameras as default
			if( -1 == DigitizerID )
			{
				DigitizerID = Iter->m_CameraID;
				if( NewOrder )
				{
					DigitizerID += cMaximumCameras;
				}
			}
			Result = SvUl_SF::Format( cMatroxGigeDigitizer, DigitizerID );
		}
	}
	return Result;
}

SVString SVDigitizerProcessingClass::GetReOrderedCamera( LPCTSTR CameraIPAddress ) const
{
	SVString Result;

	SVString CameraName = SVGigeCameraManager::Instance().getCameraName( SVString( CameraIPAddress ) );
	if( !CameraName.empty() )
	{
		//Zero based camera ID, note camera name is one based!
		int CameraID = atoi( SvUl_SF::Mid( CameraName, SVString(SvO::cCameraFixedName).length() ).c_str() );
		CameraID--;
		Result = GetReOrderedCamera( CameraID );
	}
	return Result;
}

int SVDigitizerProcessingClass::getDigitizerID( const int CameraID ) const
{
	int Result( -1 );

	const SVGigeCameraStructVector& rGigeCameraOrder = SVGigeCameraManager::Instance().GetCameraOrder();
	SVGigeCameraStructVector::const_iterator Iter( rGigeCameraOrder.begin() );
	for(; rGigeCameraOrder.end() != Iter; ++Iter )
	{
		if( CameraID == Iter->m_CameraID )
		{
			//! If Digitizer not set then use the camera ID offset by maximum physical cameras as default
			Result = Iter->m_DigitizerID;
			if( -1 == Result )
			{
				Result = Iter->m_CameraID + cMaximumCameras;
			}
			break;
		}
	}
	return Result;
}


HRESULT SVDigitizerProcessingClass::UpdateMatroxDevices()
{
	HRESULT Status = S_OK;

	SVGigeCameraStructVector Cameras;

	SVString deviceName = _T("Matrox_GIGE.Dig_0"); // just use the first one

	if( IsValidDigitizerSubsystem( deviceName.c_str() ) )
	{
		SVDigitizerLoadLibraryClass* pLibrary = GetDigitizerSubsystem( deviceName.c_str() );

		unsigned long Count = 0;

		if( S_OK == pLibrary->GetCount( &Count ) )
		{
			for( unsigned long i = 0; i < Count; i++ )
			{
				SVGigeCameraStruct Camera;

				unsigned long Handle = 0;

				if( S_OK == pLibrary->GetHandle( &Handle, i ) )
				{
					_variant_t Value("");

					Camera.m_AcquisitionHandle = Handle;

					if( S_OK == pLibrary->ParameterGetValue( Handle, SVGigeParameterVendorName, 0, &Value ) )
					{
						_bstr_t StringValue( Value.bstrVal );
						Camera.m_VendorName = static_cast< LPCTSTR >( StringValue );
					}

					if( S_OK == pLibrary->ParameterGetValue( Handle, SVGigeParameterModelName, 0, &Value ) )
					{
						_bstr_t StringValue( Value.bstrVal );
						Camera.m_ModelName = static_cast< LPCTSTR >( StringValue );
					}

					if( S_OK == pLibrary->ParameterGetValue( Handle, SVGigeParameterSerialNumber, 0, &Value ) )
					{
						_bstr_t StringValue( Value.bstrVal );
						Camera.m_SerialNum = static_cast< LPCTSTR >( StringValue );
					}

					if( S_OK == pLibrary->ParameterGetValue( Handle, SVGigeParameterIPAddress, 0, &Value ) )
					{
						_bstr_t StringValue( Value.bstrVal );
						Camera.m_IPAddress = static_cast< LPCTSTR >( StringValue );
					}
				}

				Cameras.Add( Camera );
			}
		}
	}
	std::sort( Cameras.begin(), Cameras.end());
	SVGigeCameraStructVector::iterator Iter( Cameras.begin() );
	int DigitizerID( 0 );
	for( ; Cameras.end() != Iter; ++Iter )
	{
		Iter->m_DigitizerID = DigitizerID;
		DigitizerID++;
	}

	SVGigeCameraManager::Instance().UpdateConnectedCameras( Cameras );
	Cameras = SVGigeCameraManager::Instance().GetCameraOrder();

	int Count = Cameras.GetSize();

	for( int j = 0; j < Count; j++ )
	{
		SVGigeCameraStruct& rCamera = Cameras.ElementAt( j );

		if( 0 != rCamera.m_AcquisitionHandle )
		{
			SVString AcquisitionName = SvUl_SF::Format( cMatroxGigeDigitizer, rCamera.m_DigitizerID );

			SVAcquisitionClassPtr pAcquisitionDevice = GetAcquisitionDevice( AcquisitionName.c_str() );

			if( !( pAcquisitionDevice.empty() ) )
			{
				SVDeviceParamCollection DeviceParams;

				pAcquisitionDevice->m_hDigitizer = rCamera.m_AcquisitionHandle;

				DeviceParams.SetParameter( DeviceParamSerialNumberString, SVStringValueDeviceParam( rCamera.m_SerialNum.c_str() ) );
				SVStringValueDeviceParam IP_Address( DeviceParamIPAddress );
				IP_Address = rCamera.m_IPAddress.c_str();
				DeviceParams.SetParameter( DeviceParamIPAddress, IP_Address );

				// Set packetSize if specified via hardware.ini file
				if (0 != TheSVObserverApp.getGigePacketSize())
				{
					TheSVObserverApp.SetGigePacketSizeDeviceParam(&DeviceParams);
				}

				pAcquisitionDevice->SetDeviceParameters( DeviceParams );

				//Modify the configuration copy
				SVConfigurationObject* pConfig = nullptr;
				SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
				if( nullptr != pConfig )
				{
					pConfig->ModifyAcquisitionDevice( pAcquisitionDevice->DeviceName(), &DeviceParams );
				}
			}
		}
	}

	return Status;
}

