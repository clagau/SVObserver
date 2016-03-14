//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV1394CameraManager
//* .File Name       : $Workfile:   SV1394CameraManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 11:54:38  $
//******************************************************************************

#include "stdafx.h"
#include "SV1394CameraManager.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVImageProcessingClass.h"
#include "SVDigitizerProcessingClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


SV1394CameraManager::SV1394CameraManager()
: m_OrderedCamaras()
{
	GetCameraOrder( m_OrderedCamaras );
}

HRESULT SV1394CameraManager::Refresh()
{
	// Refresh will get information from the system and update the
	// SV1394CurrentCameraArray.

	HRESULT hr = S_OK;

	SVDigitizerProcessingClass::Instance().ConnectDevices();
	SVDigitizerProcessingClass::Instance().DisconnectDevices();

	return hr;
}

SV1394CameraManager& SV1394CameraManager::Instance()
{
	static SV1394CameraManager l_Instance;

	return l_Instance;
}

const SV1394CameraStructSet& SV1394CameraManager::GetCameraOrder() const
{
	return m_OrderedCamaras;
}

HRESULT SV1394CameraManager::UpdateCameraOrder( const SV1394CameraStructSet& rSVCameraList, bool ignoreHandles )
{
	HRESULT l_Status = S_OK;

	int iSize = rSVCameraList.GetSize();

	// Write Camera serial numbers to Camera Mapping
	for ( int x = 0; x < iSize; x++ )
	{
		CString strKey;
		CString l_Number;
		
		if( x < iSize )
		{
			if( (rSVCameraList[ x ].m_ulHandle != NULL) || ignoreHandles )
			{
				l_Number = rSVCameraList[ x ].strSerialNum;
			}
		}

		strKey.Format("Matrox_1394.Dig_%d", x);
		WritePrivateProfileString("Camera Mapping", strKey, l_Number, "c:\\SVObserver\\Bin\\SVIM.ini");
	}

	// Write Num Cameras to Camera Mapping
	CString sTmp;
	sTmp.Format( "%d", iSize ); 
	WritePrivateProfileString( "Camera Mapping",
		"Count",
		sTmp,
		"c:\\SVObserver\\Bin\\SVIM.ini" );

	SVConnectedCameraMap l_ConnectedCameras;

	GetConnectedCameras( l_ConnectedCameras );

	return OrderCameras( l_ConnectedCameras );
}

HRESULT SV1394CameraManager::UpdateConnectedCameras( const SV1394CameraStructSet& rSVCameraList )
{
	HRESULT l_Status = S_OK;

	SVConnectedCameraMap l_ConnectedCameras;

	for( size_t i = 0; i < rSVCameraList.size(); ++i )
	{
		const SV1394CameraStruct& l_rCameraInfo = rSVCameraList[ i ];
		if( ! l_rCameraInfo.strSerialNum.IsEmpty() )
		{
			l_ConnectedCameras[ l_rCameraInfo.strSerialNum ] = l_rCameraInfo;
		}
	}

	return OrderCameras( l_ConnectedCameras );
}

void SV1394CameraManager::GetCameraOrder( SV1394CameraStructSet& rSVCameraList ) const
{
	int iSize = GetPrivateProfileInt( "Camera Mapping",
		"Count",
		3,
		"c:\\SVObserver\\Bin\\SVIM.ini" );

	rSVCameraList.clear();

	for ( int x = 0; x < iSize; x++ )
	{
		SV1394CameraStruct SVcs;
		char szGetBuf[128];
		SVString strKey = SvUl_SF::Format( "Matrox_1394.Dig_%d", x );
		GetPrivateProfileString( "Camera Mapping", strKey.c_str(), "", szGetBuf, 128, "c:\\SVObserver\\Bin\\SVIM.ini" );
		SVcs.strSerialNum = szGetBuf;

		SVcs.iPosition = x;
		SVcs.eChangeType = SV1394CameraStruct::SVNoChange;
		
		rSVCameraList.Add( SVcs );
	}
}

void SV1394CameraManager::GetConnectedCameras( SVConnectedCameraMap& rSVCameraMap ) const
{
	rSVCameraMap.clear();

	for( size_t i = 0; i < m_OrderedCamaras.size(); ++i )
	{
		const SV1394CameraStruct& l_rCameraInfo = m_OrderedCamaras[ i ];
		if( ! l_rCameraInfo.strSerialNum.IsEmpty() )
		{
			rSVCameraMap[ l_rCameraInfo.strSerialNum ] = l_rCameraInfo;
		}
	}
}

HRESULT SV1394CameraManager::OrderCameras( SVConnectedCameraMap& rSVCameraList )
{
	HRESULT l_Status = S_OK;

	GetCameraOrder( m_OrderedCamaras );

	size_t l_Size = std::max< size_t >( m_OrderedCamaras.size(), rSVCameraList.size() );

	m_OrderedCamaras.resize( l_Size );

	for( size_t i = 0; i < l_Size; ++i )
	{
		SV1394CameraStruct& l_rCamera = m_OrderedCamaras[ i ];

		if( l_rCamera.HasSerialNumber() )
		{
			SVConnectedCameraMap::iterator l_Iter = rSVCameraList.find( l_rCamera.strSerialNum );

			if( l_Iter != rSVCameraList.end() )
			{
				l_rCamera = l_Iter->second;
				l_rCamera.eChangeType = SVCameraStructInterface::SVNoChange;

				rSVCameraList.erase( l_Iter );
			}
			else
			{
				l_rCamera.strModelName.Empty();
				l_rCamera.strVendorName.Empty();
				l_rCamera.eChangeType = SVCameraStructInterface::SVCameraRemoved;
				l_rCamera.m_ulHandle = NULL;
			}
		}

		l_rCamera.iPosition = static_cast<int>(i);
	}

	for( size_t i = 0; i < l_Size && !( rSVCameraList.empty() ); ++i )
	{
		SV1394CameraStruct& l_rCamera = m_OrderedCamaras[ i ];

		if( l_rCamera.m_ulHandle == NULL )
		{
			SVConnectedCameraMap::iterator l_Iter = rSVCameraList.begin();

			if( l_Iter != rSVCameraList.end() )
			{
				if( l_rCamera.HasSerialNumber() )
				{
					l_rCamera = l_Iter->second;
					l_rCamera.eChangeType = SVCameraStructInterface::SVReplacedCamera;
				}
				else
				{
					l_rCamera = l_Iter->second;
					l_rCamera.eChangeType = SVCameraStructInterface::SVNewCamera;
				}

				rSVCameraList.erase( l_Iter );
			}
		}

		l_rCamera.iPosition = static_cast<int>(i);
	}

	return l_Status;
}

