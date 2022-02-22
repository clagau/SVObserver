//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraFileReader
//* .File Name       : $Workfile:   SVGigeCameraFileReader.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:42:02  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <iostream>
#include "CameraLibrary/SVDeviceParam.h"
#pragma endregion Includes

class SVDeviceParamCollection;
class SVLongValueDeviceParam;


class SVGigeCameraFileReader
{
public:
	SVGigeCameraFileReader(const std::string& rFilename,bool bColorSystem);

	HRESULT ReadParams(SVDeviceParamCollection& rParams);
	HRESULT ReadGigeFeatureOverrides();
	HRESULT ReadCameraFileImpl( SVDeviceParamCollection& rParams );

	const std::string& GetFeatureOverrides() const {return m_FeatureOverrides;}


private:
	HRESULT ReadCustomParams(const std::string& filename, SVDeviceParamCollection& rParams);

	HRESULT ReadCameraFileStringParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const std::string& sSection );
	HRESULT ReadCameraFileBoolParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const std::string& sSection );
	HRESULT ReadCameraFileLongParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const std::string& sSection );
	HRESULT ReadCameraFileCameraFormatsParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const std::string& sSection );
	HRESULT ReadCameraFileLutParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const std::string& sSection );

	HRESULT ReadCameraFileCommonParams(const std::string& sSection, const std::string& sKey, const std::string& sFilename, SVDeviceParam* pParam);
//	HRESULT ReadCameraFileOptions(const std::string& sSection, const std::string& sKey, const std::string& sFilename, SVDeviceParamOptionInsertor& rInsertor);
	HRESULT ReadCameraFileLongValueInfo(const std::string& sSection, const std::string& sKey, const std::string& sFilename, SVLongValueDeviceParam* pParam);


	std::string m_Filename;
	bool m_isColorSystem;

	std::string m_FeatureOverrides;
};
