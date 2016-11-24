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

#include "SVOCore/SVImageProcessingClass.h"
#include "SVDigitizerProcessingClass.h"
#include "SVStatusLibrary\GlobalPath.h"
#include "TextDefinesSvO.h"
#include "SVOMFCLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error
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

	SVDigitizerProcessingClass::Instance().ScanForCameras();
	SVDigitizerProcessingClass::Instance().ConnectDevices();
	SVDigitizerProcessingClass::Instance().DisconnectDevices();

	return Result;
}

HRESULT SVGigeCameraManager::UpdateConnectedCameras( const SVGigeCameraStructVector& rCameraList )
{
	HRESULT Result( S_OK );
	m_OrderedCameras.clear();

	SVGigeCameraStructVector::const_iterator IterIni( m_iniCameras.begin() );
	for( ; m_iniCameras.end() != IterIni; ++IterIni )
	{
		SVGigeCameraStruct GigeCamera( *IterIni );
		SVGigeCameraStructVector::const_iterator Iter( rCameraList.begin() );
		for( ; rCameraList.end() != Iter; ++Iter )
		{
			if( IterIni->GetIPAddress() == Iter->GetIPAddress() )
			{
				GigeCamera = *Iter;
				GigeCamera.m_CameraID = IterIni->m_CameraID;
				break;
			}
		}
		m_OrderedCameras.Add( GigeCamera );
	}

	return Result;
}

SVString SVGigeCameraManager::getCameraName( const SVString& rIPAddress ) const
{
	SVString Result;

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
		SVString CameraName;
		TCHAR pBuffer[128];
		memset(pBuffer, 0, 128 );

		CameraName =  SvUl_SF::Format( _T("%s%d"), SvO::cCameraFixedName, i + 1 );
		GetPrivateProfileString( cCameraMapping, CameraName.c_str(), "", pBuffer, 128, SvStl::GlobalPath::Inst().GetSVIMIniPath() );
		
		SVString IPAddress( pBuffer );
		if( !IPAddress.empty() )
		{
			m_CameraIPtoName[IPAddress] = CameraName;
		}

		SVGigeCameraStruct GigeCamera;
		GigeCamera.m_IPAddress = IPAddress;
		GigeCamera.m_CameraID = i;
		m_iniCameras.Add( GigeCamera );
	}
	//! After initializing set the ordered cameras to the ini settings
	m_OrderedCameras.clear();
	m_OrderedCameras = m_iniCameras;
}

