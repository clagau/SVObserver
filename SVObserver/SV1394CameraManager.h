//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV1394CameraManager
//* .File Name       : $Workfile:   SV1394CameraManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:18:00  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <map>
#include "SV1394CameraStruct.h"

class SV1394CameraManager  
{
public:
	static SV1394CameraManager& Instance();

	HRESULT Refresh();

	const SV1394CameraStructSet& GetCameraOrder() const;

	HRESULT UpdateCameraOrder( const SV1394CameraStructSet& rSVCameraList, bool ignoreHandles=false );
	HRESULT UpdateConnectedCameras( const SV1394CameraStructSet& rSVCameraList );

protected:
	typedef std::map< CString, SV1394CameraStruct > SVConnectedCameraMap;

	SV1394CameraManager();

	// For a singleton object, this constructor is here to warn of a invalid copy construction.  Do not implement this constructor.
	SV1394CameraManager( const SV1394CameraManager& p_rObject );

	// For a singleton object, this operator is here to warn of a invalid assignement operator.  Do not implement this operator.
	const SV1394CameraManager& operator=( const SV1394CameraManager& p_rObject );

	void GetCameraOrder( SV1394CameraStructSet& rSVCameraList ) const;
	void GetConnectedCameras( SVConnectedCameraMap& rSVCameraMap ) const;

	HRESULT OrderCameras( SVConnectedCameraMap& rSVCameraMap );

	SV1394CameraStructSet m_OrderedCamaras;

};

#define TheSV1394CameraManager SV1394CameraManager::Instance()

