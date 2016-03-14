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

#ifndef SVMATROXGIGECAMERAPARAMTRACKER_H
#define SVMATROXGIGECAMERAPARAMTRACKER_H

#include <map>
#include "SVUtilityLibrary/SVString.h"
#include "SVMatroxGigeCameraParam.h"
#include "SVMatroxGigeCameraParamList.h"

class SVMatroxGigeDigitizer;

class SVMatroxGigeCameraParamTracker
{
// List of Camera Parameters by cameraID (serialNo)
typedef std::map<SVString, SVMatroxGigeCameraParamList> SVMatroxGigeCameraParamCollection;

private:
	SVMatroxGigeCameraParamCollection m_cameraParamCollection;
	
public:
	void Clear();

	void TrackCameraParams(const SVString& cameraID);
	void UnTrackCameraParams(const SVString& cameraID);
	void SetMainListLocked(const SVString& cameraID, bool bLock=true);

	HRESULT AddCameraParam(const SVString& cameraID, const SVMatroxGigeCameraParam& param);
	HRESULT ReloadCameraParams(const SVString& cameraID, SVMatroxGigeDigitizer& p_rCamera);
	
private:	
	HRESULT ReloadCameraParamList(const SVMatroxGigeCameraParamDeque& list, SVMatroxGigeDigitizer& p_rCamera);
};

#endif

