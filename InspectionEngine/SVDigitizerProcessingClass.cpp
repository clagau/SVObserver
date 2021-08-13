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
#include "Definitions/SVGigeEnums.h"
#include "ObjectInterfaces/ISVOApp_Helper.h"
#include "Triggering/SVDigitizerLoadLibraryClass.h"
#include "SVOLibrary/SVLut.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"
#include "CameraLibrary/SVStringValueDeviceParam.h"
#include "SVFileAcquisitionClass.h"
#include "SVOLibrary/SVHardwareManifest.h"
#include "SVMatroxGigeAcquisitionClass.h"
#include "SVGigeCameraFileLibrary/SVGigeCameraStruct.h"
#include "SVGigeCameraManager.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/TextDefineSvDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvIe
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr const char* cMatroxGigeDigitizer = _T("Matrox_GIGE.Dig_%d");
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
		SVAcquisitionClassPtr pAcqPtr;

		if( 0 == l_Iter->m_DigitizerName.find( _T( "Matrox_GIGE" ) ) )
		{
			pAcqPtr = std::make_shared<SVMatroxGigeAcquisitionClass>(*l_Iter);
		}
		else if( 0 == l_Iter->m_DigitizerName.find( _T( "File" ) ) )
		{
			pAcqPtr = std::make_shared<SVFileAcquisitionClass>(*l_Iter);
		}

		if(nullptr != pAcqPtr)
		{
			if (S_OK == pAcqPtr->Create())
			{
				m_AcquisitionDevices[l_Iter->m_DigitizerName] = pAcqPtr;
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
			l_Iter->second.reset();

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
			if(nullptr == l_Iter->second)
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

HRESULT SVDigitizerProcessingClass::UpdateDigitizerSubsystem( SvTrig::SVDigitizerLoadLibraryClass* pDigitizerSubsystem )
{
	HRESULT Result{ E_FAIL };

	if ( nullptr != pDigitizerSubsystem )
	{
		m_Subsystems.insert( pDigitizerSubsystem );
		
		Result = S_OK;
		unsigned long count = pDigitizerSubsystem->GetCount();

		for ( unsigned long i = 0; S_OK == Result && i < count; i++ )
		{
			std::string AcquisitionName;
			std::string IPAddress;

			unsigned long Handle = pDigitizerSubsystem->GetHandle(i);

			_variant_t value = pDigitizerSubsystem->GetName(Handle);
			AcquisitionName = SvUl::createStdString(value);

			value = pDigitizerSubsystem->ParameterGetValue(Handle, SvDef::SVGigeParameterIPAddress);
			//Not all DLL's support IP address so do not place in Result value
			if(VT_EMPTY != value.vt)
			{
				IPAddress = SvUl::createStdString(value);
			}

			if ( S_OK == Result )
			{
				std::string CameraName;
				constexpr LPCTSTR cMGige = _T("Matrox_GIGE");
				if( 0 == AcquisitionName.compare(0,  strlen(cMGige), cMGige) )
				{
					CameraName =  SVGigeCameraManager::Instance().getCameraName(IPAddress);
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

HRESULT SVDigitizerProcessingClass::GetAcquisitionDeviceList( SvDef::StringVector& rList ) const
{
	HRESULT l_Status = S_OK;

	SVNameDigitizerMap::const_iterator l_Iter = m_Digitizers.begin();

	while( l_Iter != m_Digitizers.end() )
	{
		SVAcquisitionClassPtr l_AcqDevicePtr = l_Iter->second;

		if( nullptr != l_AcqDevicePtr && 0 != l_AcqDevicePtr->m_hDigitizer )
		{
			rList.push_back( std::string( l_AcqDevicePtr->DeviceName() ) );
		}

		++l_Iter;
	}

	return l_Status;
}

bool SVDigitizerProcessingClass::IsValidDigitizerSubsystem( LPCTSTR digitizerName ) const
{
	return ( nullptr != GetDigitizerSubsystem( digitizerName ) );
}

SvTrig::SVDigitizerLoadLibraryClass* SVDigitizerProcessingClass::GetDigitizerSubsystem( LPCTSTR digitizerName ) const
{
	SvTrig::SVDigitizerLoadLibraryClass* l_pSubsystem = nullptr;

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

		if(nullptr != l_AcqDevicePtr && 0 != l_AcqDevicePtr->m_hDigitizer )
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

			if(nullptr != l_AcqDevicePtr)
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
			(*l_Iter)->ScanForCameras();
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

HRESULT SVDigitizerProcessingClass::AddDigitizer( LPCTSTR Name, LPCTSTR AcquisitionName, SvTrig::SVDigitizerLoadLibraryClass* pDigitizerSubsystem, unsigned long p_Handle )
{
	HRESULT Result( S_OK );
	std::string DigitizerName( Name );
	std::string AcqName( AcquisitionName );

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

	if(nullptr != pAcquisitionDevice)
	{
		int NumberOfBands( 1 );
		if( isColor )
		{
			NumberOfBands = 3;
		}
		SVLightReference LightReference;
		pAcquisitionDevice->GetLightReference(LightReference);
		if (NumberOfBands != LightReference.NumBands())
		{
			pAcquisitionDevice->SetNumberOfBands(NumberOfBands);
		}
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

std::string SVDigitizerProcessingClass::GetReOrderedCamera( const int CameraID ) const
{
	std::string Result;
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
				DigitizerID = Iter->m_CameraID + SvDef::cMaximumCameras;
			}
			Result = SvUl::Format( cMatroxGigeDigitizer, DigitizerID );
		}
	}
	return Result;
}

std::string SVDigitizerProcessingClass::GetReOrderedCamera( LPCTSTR CameraIPAddress ) const
{
	std::string Result;

	std::string CameraName = SVGigeCameraManager::Instance().getCameraName( std::string( CameraIPAddress ) );
	if( !CameraName.empty() )
	{
		//Zero based camera ID, note camera name is one based!
		int CameraID = atoi( SvUl::Mid( CameraName, std::string(SvDef::cCameraFixedName).length() ).c_str() );
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
				Result = Iter->m_CameraID + SvDef::cMaximumCameras;
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

	std::string deviceName = _T("Matrox_GIGE.Dig_0"); // just use the first one

	if( IsValidDigitizerSubsystem( deviceName.c_str() ) )
	{
		SvTrig::SVDigitizerLoadLibraryClass* pLibrary = GetDigitizerSubsystem( deviceName.c_str() );

		if(nullptr != pLibrary)
		{
			unsigned long count = pLibrary->GetCount();
			for( unsigned long i = 0; i < count; i++ )
			{
				SVGigeCameraStruct Camera;

				unsigned long Handle = pLibrary->GetHandle(i);
				if(0 != Handle)
				{
					Camera.m_AcquisitionHandle = Handle;
					_variant_t value;
					value = pLibrary->ParameterGetValue(Handle, SvDef::SVGigeParameterVendorName);
					if(VT_EMPTY != value.vt)
					{
						Camera.m_VendorName = SvUl::createStdString(value).c_str();
					}
					
					value = pLibrary->ParameterGetValue(Handle, SvDef::SVGigeParameterModelName);
					if(VT_EMPTY != value.vt)
					{
						Camera.m_ModelName = SvUl::createStdString(value).c_str();
					}

					value = pLibrary->ParameterGetValue(Handle, SvDef::SVGigeParameterSerialNumber);
					if(VT_EMPTY != value.vt)
					{
						Camera.m_SerialNum = SvUl::createStdString(value).c_str();
					}

					value = pLibrary->ParameterGetValue(Handle, SvDef::SVGigeParameterIPAddress);
					if(VT_EMPTY != value.vt)
					{
						Camera.m_IPAddress = SvUl::createStdString(value).c_str();
					}
				}

				Cameras.push_back( Camera );
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

	int Count = static_cast<int> (Cameras.size());

	for( int j = 0; j < Count; j++ )
	{
		SVGigeCameraStruct& rCamera = Cameras[j];

		if( 0 != rCamera.m_AcquisitionHandle )
		{
			std::string AcquisitionName = SvUl::Format( cMatroxGigeDigitizer, rCamera.m_DigitizerID );

			SVAcquisitionClassPtr pAcquisitionDevice = GetAcquisitionDevice( AcquisitionName.c_str() );

			if(nullptr != pAcquisitionDevice)
			{
				SVDeviceParamCollection DeviceParams;

				pAcquisitionDevice->m_hDigitizer = rCamera.m_AcquisitionHandle;

				DeviceParams.SetParameter(DeviceParamSerialNumberString, SVStringValueDeviceParam{rCamera.m_SerialNum});
				DeviceParams.SetParameter(DeviceParamIPAddress, SVStringValueDeviceParam{rCamera.m_IPAddress});

				long gigePacketSize = SvOi::getGigePacketSize();
				if(0 != gigePacketSize)
				{
					DeviceParams.SetParameter(DeviceParamGigePacketSize, SVLongValueDeviceParam{gigePacketSize});
					SVDeviceParam* pParam = DeviceParams.GetParameter(DeviceParamGigePacketSize);
					if (nullptr != pParam)
					{
						pParam->SetName(DeviceParamGigePacketSize_String);
					}
				}

				pAcquisitionDevice->SetDeviceParameters( DeviceParams );

				//Modify the configuration copy
				SvOi::modifyAcquisitionDevice(pAcquisitionDevice->DeviceName().c_str(), &DeviceParams);
			}
		}
	}

	return Status;
}

} //namespace SvIe
