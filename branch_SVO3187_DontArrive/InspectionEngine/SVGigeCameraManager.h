//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraManager
//* .File Name       : $Workfile:   SVGigeCameraManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   05 Jul 2013 09:11:02  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVGigeCameraFileLibrary/SVGigeCameraStruct.h"
#pragma endregion Includes

class SVGigeCameraManager  
{
public:
	static SVGigeCameraManager& Instance();

	HRESULT Refresh();

	const SVGigeCameraStructVector& GetCameraOrder() const { return m_OrderedCameras; };
	HRESULT UpdateConnectedCameras( const SVGigeCameraStructVector& rCameraList );

	std::string getCameraName( const std::string& rIPAddress ) const;

protected:
	typedef std::map<std::string, std::string> CameraIP_NameMap;

	SVGigeCameraManager();

	// For a singleton object, this constructor is here to warn of a invalid copy construction.  Do not implement this constructor.
	SVGigeCameraManager( const SVGigeCameraManager& p_rObject );

	// For a singleton object, this operator is here to warn of a invalid assignement operator.  Do not implement this operator.
	const SVGigeCameraManager& operator=( const SVGigeCameraManager& p_rObject );

	void ReadCameraMapping();

	SVGigeCameraStructVector m_OrderedCameras;
	SVGigeCameraStructVector m_iniCameras;

	CameraIP_NameMap m_CameraIPtoName;
};
