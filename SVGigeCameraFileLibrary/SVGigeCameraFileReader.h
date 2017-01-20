//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraFileReader
//* .File Name       : $Workfile:   SVGigeCameraFileReader.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:42:02  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <iostream>
#include "SVUtilityLibrary/SVString.h"
#include "CameraLibrary/SVDeviceParams.h"
#include "CameraLibrary/SVLongValueDeviceParam.h"

class SVGigeCameraFileReader
{
public:
	SVGigeCameraFileReader(const SVString& rFilename,bool bColorSystem);

	HRESULT ReadParams(SVDeviceParamCollection& rParams);
	HRESULT ReadGigeFeatureOverrides();
	HRESULT ReadCameraFileImpl( SVDeviceParamCollection& rParams );

	const SVString& GetFeatureOverrides() const {return m_FeatureOverrides;}


private:
	HRESULT ReadCustomParams(const SVString& filename, SVDeviceParamCollection& rParams);

	HRESULT ReadCameraFileStringParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection );
	HRESULT ReadCameraFileBoolParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection );
	HRESULT ReadCameraFileLongParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection );
	HRESULT ReadCameraFileCameraFormatsParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection );
	HRESULT ReadCameraFileLutParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const SVString& sSection );

	HRESULT ReadCameraFileCommonParams(const SVString& sSection, const SVString& sKey, const SVString& sFilename, SVDeviceParam* pParam);
//	HRESULT ReadCameraFileOptions(const SVString& sSection, const SVString& sKey, const SVString& sFilename, SVDeviceParamOptionInsertor& rInsertor);
	HRESULT ReadCameraFileLongValueInfo(const SVString& sSection, const SVString& sKey, const SVString& sFilename, SVLongValueDeviceParam* pParam);


	SVString m_Filename;
	bool m_isColorSystem;

	SVString m_FeatureOverrides;
};
