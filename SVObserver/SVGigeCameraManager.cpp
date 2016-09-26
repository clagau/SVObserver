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

#include "SVImageProcessingClass.h"
#include "SVDigitizerProcessingClass.h"
#include "SVStatusLibrary\GlobalPath.h"
#include "SVOMFCLibrary/SVDeviceParams.h" //Arvid added to avoid VS2015 compile Error


#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

SVGigeCameraManager::SVGigeCameraManager()
: m_OrderedCamaras()
{
	GetCameraOrder( m_OrderedCamaras );
	GetCameraOrder(m_iniCameras);
}

HRESULT SVGigeCameraManager::Refresh()
{
	// Refresh will get information from the system and update the
	// SVGigeCurrentCameraArray.
	// 

	HRESULT hr = S_OK;

	SVDigitizerProcessingClass::Instance().ScanForCameras();
	SVDigitizerProcessingClass::Instance().ConnectDevices();
	SVDigitizerProcessingClass::Instance().DisconnectDevices();

	return hr;
}

SVGigeCameraManager& SVGigeCameraManager::Instance()
{
	static SVGigeCameraManager l_Instance;

	return l_Instance;
}

const SVGigeCameraStructSet& SVGigeCameraManager::GetCameraOrder() const
{
	return m_OrderedCamaras;
}

const SVGigeCameraStructSet& SVGigeCameraManager::GetOriginalCameraIniOrder() const
{
	return m_iniCameras;
}

void SVGigeCameraManager::UpdateCameraIniList()
{
	GetCameraOrder(m_iniCameras);
}

HRESULT SVGigeCameraManager::UpdateCameraOrder( const SVGigeCameraStructSet& rSVCameraList, bool ignoreHandles )
{
	HRESULT l_Status = S_OK;

	int iSize = rSVCameraList.GetSize();

	// Write Camera IP Address to Camera Mapping
	for ( int x = 0; x < iSize; x++ )
	{
		CString strKey;
		CString l_Number;
		if( (0 != rSVCameraList[x].m_triggerchannel) || ignoreHandles )
		{
			l_Number = rSVCameraList[ x ].strIPAddress;
		}

		strKey.Format("Matrox_GIGE.Dig_%d", x);
		WritePrivateProfileString("Camera Mapping", strKey, l_Number,SvStl::GlobalPath::Inst().GetSVIMIniPath());
	}

	// Write Num Cameras to Camera Mapping
	CString sTmp;
	sTmp.Format( "%d", iSize ); 

	WritePrivateProfileString( "Camera Mapping",
		"GigeCameraCount",
		sTmp,
		SvStl::GlobalPath::Inst().GetSVIMIniPath());

	SVConnectedCameraMap l_ConnectedCameras;

	GetConnectedCameras( l_ConnectedCameras );

	return OrderCameras( l_ConnectedCameras );
}

HRESULT SVGigeCameraManager::UpdateConnectedCameras( const SVGigeCameraStructSet& rSVCameraList )
{
	HRESULT l_Status = S_OK;

	SVConnectedCameraMap l_ConnectedCameras;

	for( size_t i = 0; i < rSVCameraList.size(); ++i )
	{
		const SVGigeCameraStruct& l_rCameraInfo = rSVCameraList[ i ];
		if( ! l_rCameraInfo.strIPAddress.IsEmpty() )
		{
			l_ConnectedCameras[ l_rCameraInfo.strIPAddress ] = l_rCameraInfo;
		}
	}

	return OrderCameras( l_ConnectedCameras );
}

void SVGigeCameraManager::GetCameraOrder( SVGigeCameraStructSet& rSVCameraList ) const
{
	int iSize = GetPrivateProfileInt( "Camera Mapping",
		"GigeCameraCount",
		1,
		SvStl::GlobalPath::Inst().GetSVIMIniPath() );

	rSVCameraList.clear();

	for ( int x = 0; x < iSize; x++ )
	{
		SVGigeCameraStruct SVcs;

		CString strKey;
		char szGetBuf[128];

		strKey.Format( "Matrox_GIGE.Dig_%d", x );
		GetPrivateProfileString( "Camera Mapping", strKey, "", szGetBuf, 128, SvStl::GlobalPath::Inst().GetSVIMIniPath() );
		SVcs.strIPAddress = szGetBuf;

		SVcs.iPosition = x;
		SVcs.eChangeType = SVGigeCameraStruct::SVNoChange;
		
		rSVCameraList.Add( SVcs );
	}
}

void SVGigeCameraManager::GetConnectedCameras( SVConnectedCameraMap& rSVCameraMap ) const
{
	rSVCameraMap.clear();

	for( size_t i = 0; i < m_OrderedCamaras.size(); ++i )
	{
		const SVGigeCameraStruct& l_rCameraInfo = m_OrderedCamaras[ i ];
		if( ! l_rCameraInfo.strIPAddress.IsEmpty() )
		{
			rSVCameraMap[ l_rCameraInfo.strIPAddress ] = l_rCameraInfo;
		}
	}
}

HRESULT SVGigeCameraManager::OrderCameras( SVConnectedCameraMap& rSVCameraList )
{
	HRESULT l_Status = S_OK;

	GetCameraOrder( m_OrderedCamaras );

	size_t l_Size = std::max< size_t >( m_OrderedCamaras.size(), rSVCameraList.size() );

	m_OrderedCamaras.resize( l_Size );

	for( size_t i = 0; i < l_Size; ++i )
	{
		SVGigeCameraStruct& l_rCamera = m_OrderedCamaras[ i ];

		if( l_rCamera.HasIPAddress() )
		{
			SVConnectedCameraMap::iterator l_Iter = rSVCameraList.find( l_rCamera.strIPAddress );

			if( l_Iter != rSVCameraList.end() )
			{
				l_rCamera = l_Iter->second;
				l_rCamera.eChangeType = SvOi::SVGigeCameraStructInterface::SVNoChange;

				rSVCameraList.erase( l_Iter );
			}
			else
			{
				l_rCamera.strModelName.Empty();
				l_rCamera.strVendorName.Empty();
				l_rCamera.eChangeType = SvOi::SVGigeCameraStructInterface::SVCameraRemoved;
				l_rCamera.m_triggerchannel = 0;
			}
		}
		l_rCamera.iPosition = static_cast<int>(i);
	}

	for( size_t i = 0; i < l_Size && !( rSVCameraList.empty() ); ++i )
	{
		SVGigeCameraStruct& l_rCamera = m_OrderedCamaras[ i ];

		if( 0 == l_rCamera.m_triggerchannel )
		{
			SVConnectedCameraMap::iterator l_Iter = rSVCameraList.begin();

			if( l_Iter != rSVCameraList.end() )
			{
				if( l_rCamera.HasIPAddress() )
				{
					l_rCamera = l_Iter->second;
					l_rCamera.eChangeType = SvOi::SVGigeCameraStructInterface::SVReplacedCamera;
				}
				else
				{
					l_rCamera = l_Iter->second;
					l_rCamera.eChangeType = SvOi::SVGigeCameraStructInterface::SVNewCamera;
				}

				rSVCameraList.erase( l_Iter );
			}
		}

		l_rCamera.iPosition = static_cast<int>(i);
	}

	return l_Status;
}
