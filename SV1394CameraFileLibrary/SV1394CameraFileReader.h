//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV1394CameraFileReader
//* .File Name       : $Workfile:   SV1394CameraFileReader.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:32:56  $
//******************************************************************************

#ifndef SV1394CAMERAFILEREADER_H
#define SV1394CAMERAFILEREADER_H

#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"


struct SV1394CameraFileInfoStruct
{
	// input params
	CString sFilename;
	bool bColorSystem;

	// output params
	CString sVersion;
	CString sCameraType;

	SV1394CameraFileInfoStruct() : bColorSystem(false) {}
};


class SV1394CameraFileReader
{
public:
	SV1394CameraFileReader(SV1394CameraFileInfoStruct& rInfo);
	HRESULT ReadParams( SVDeviceParamCollection& rParams );

protected:

	WORD GetChecksum(CStdioFile& rFile);
	HRESULT ReadCameraFileStringParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const CString& sSection );
	HRESULT ReadCameraFileBoolParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const CString& sSection );
	HRESULT ReadCameraFileLongParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const CString& sSection );
	HRESULT ReadCameraFileCameraFormatsParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const CString& sSection );
	HRESULT ReadCameraFileLutParam( SVDeviceParamCollection& rParams, SVDeviceParamEnum e, const CString& sSection );

	SV1394CameraFileInfoStruct& m_rInfo;
};

#endif

