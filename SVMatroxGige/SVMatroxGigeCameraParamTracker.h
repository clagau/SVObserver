//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCameraParamTracker
//* .File Name       : $Workfile:   SVMatroxGigeCameraParamTracker.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   22 Apr 2013 15:36:04  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>

#include "SVMatroxGigeCameraParamList.h"
#pragma endregion Includes

class SVMatroxGigeDigitizer;

class SVMatroxGigeCameraParamTracker
{
// List of Camera Parameters by cameraID (serialNo)
typedef std::map<std::string, SVMatroxGigeCameraParamList> SVMatroxGigeCameraParamCollection;

private:
	SVMatroxGigeCameraParamCollection m_cameraParamCollection;
	
public:
	void Clear();

	void TrackCameraParams(const std::string& cameraID);
	void UnTrackCameraParams(const std::string& cameraID);
	void SetMainListLocked(const std::string& cameraID, bool bLock=true);

	HRESULT AddCameraParam(const std::string& cameraID, const SVMatroxGigeCameraParam& param);
	HRESULT ReloadCameraParams(const std::string& cameraID, SVMatroxGigeDigitizer& p_rCamera);
	
private:	
	HRESULT ReloadCameraParamList(const SVMatroxGigeCameraParamDeque& list, SVMatroxGigeDigitizer& p_rCamera);
};

