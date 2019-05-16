//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraManager
//* .File Name       : $Workfile:   SVGigeCameraManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   01 Oct 2013 14:19:30  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVGigeCameraManager.h"

#include "InspectionEngine/SVDigitizerProcessingClass.h"
#include "SVStatusLibrary\GlobalPath.h"
#include "Definitions/TextDefineSvDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int cDefaultCameraCount			= 4;
const TCHAR* const cCameraMapping		= _T( "Camera Mapping" );
const TCHAR* const cGigeCameraCount		= _T( "GigeCameraCount" );
#pragma endregion Declarations

SVGigeCameraManager::SVGigeCameraManager()
{
	ReadCameraMapping();
}

SVGigeCameraManager& SVGigeCameraManager::Instance()
{
	static SVGigeCameraManager Instance;

	return Instance;
}

HRESULT SVGigeCameraManager::Refresh()
{
	HRESULT Result( S_OK );

	SvIe::SVDigitizerProcessingClass::Instance().ScanForCameras();
	SvIe::SVDigitizerProcessingClass::Instance().ConnectDevices();
	SvIe::SVDigitizerProcessingClass::Instance().DisconnectDevices();

	return Result;
}

HRESULT SVGigeCameraManager::UpdateConnectedCameras( const SVGigeCameraStructVector& rCameraList )
{
	HRESULT Result( S_OK );
	m_OrderedCameras.clear();

	for(auto const& rIniCamera :  m_iniCameras)
	{
		SVGigeCameraStruct GigeCamera(rIniCamera);
		for(auto const& rCamera : rCameraList)
		{
			if(rIniCamera.GetIPAddress() == rCamera.GetIPAddress() )
			{
				GigeCamera = rCamera;
				GigeCamera.m_CameraID = rIniCamera.m_CameraID;
				break;
			}
		}
		m_OrderedCameras.push_back(GigeCamera);
	}

	return Result;
}

std::string SVGigeCameraManager::getCameraName( const std::string& rIPAddress ) const
{
	std::string Result;

	CameraIP_NameMap::const_iterator Iter( m_CameraIPtoName.find(rIPAddress) );
	if( m_CameraIPtoName.end() != Iter )
	{
		Result = Iter->second;
	}
	return Result;
}

void SVGigeCameraManager::ReadCameraMapping()
{
	int CameraCount = GetPrivateProfileInt( cCameraMapping, cGigeCameraCount, cDefaultCameraCount, SvStl::GlobalPath::Inst().GetSVIMIniPath() );

	for ( int i = 0; i < CameraCount; i++ )
	{
		std::string CameraName;
		TCHAR pBuffer[128];
		memset(pBuffer, 0, 128 );

		CameraName =  SvUl::Format( _T("%s%d"), SvDef::cCameraFixedName, i + 1 );
		GetPrivateProfileString( cCameraMapping, CameraName.c_str(), "", pBuffer, 128, SvStl::GlobalPath::Inst().GetSVIMIniPath() );
		
		std::string IPAddress( pBuffer );
		if( !IPAddress.empty() )
		{
			m_CameraIPtoName[IPAddress] = CameraName;
		}

		SVGigeCameraStruct GigeCamera;
		GigeCamera.m_IPAddress = IPAddress;
		GigeCamera.m_CameraID = i;
		m_iniCameras.push_back( GigeCamera );
	}
	//! After initializing set the ordered cameras to the ini settings
	m_OrderedCameras.clear();
	m_OrderedCameras = m_iniCameras;
}

