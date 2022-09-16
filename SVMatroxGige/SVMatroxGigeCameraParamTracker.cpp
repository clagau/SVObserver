//******************************************************************************
//* COPYRIGHT (c) 2012 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCameraParamTracker
//* .File Name       : $Workfile:   SVMatroxGigeCameraParamTracker.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   22 Apr 2013 15:36:04  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVMatroxGigeCameraParamTracker.h"
#include "SVMatroxGigeDeviceParameterManager.h"
#pragma endregion Includes

void SVMatroxGigeCameraParamTracker::Clear()
{
	m_cameraParamCollection.clear();
}

void SVMatroxGigeCameraParamTracker::TrackCameraParams(const std::string& cameraID)
{
	SVMatroxGigeCameraParamCollection::iterator it = m_cameraParamCollection.find(cameraID);
	if (it == m_cameraParamCollection.end())
	{
		SVMatroxGigeCameraParamList list;
		m_cameraParamCollection[cameraID] = list;
	}
	else
	{
		(*it).second.clear();
	}
}

void SVMatroxGigeCameraParamTracker::UnTrackCameraParams(const std::string& cameraID)
{
	SVMatroxGigeCameraParamCollection::iterator it = m_cameraParamCollection.find(cameraID);
	if (it != m_cameraParamCollection.end())
	{
		m_cameraParamCollection.erase(it);
	}
}

void SVMatroxGigeCameraParamTracker::SetMainListLocked(const std::string& cameraID, bool bLock)
{
	SVMatroxGigeCameraParamCollection::iterator it = m_cameraParamCollection.find(cameraID);
	if (it != m_cameraParamCollection.end())
	{
		SVMatroxGigeCameraParamList& list = (*it).second;
		list.m_bUseTransient = bLock;
	}
}

HRESULT SVMatroxGigeCameraParamTracker::AddCameraParam(const std::string& cameraID, const SVMatroxGigeCameraParam& param)
{
	HRESULT hr = S_FALSE;

	SVMatroxGigeCameraParamCollection::iterator it = m_cameraParamCollection.find(cameraID);
	if (it != m_cameraParamCollection.end())
	{
		SVMatroxGigeCameraParamList& list = (*it).second;
		// if not locked use the main list
		if (!list.m_bUseTransient)
		{
			list.m_mainList.push_back(param);
			hr = S_OK;
		}
		// otherwise use the transient list
		else
		{
			list.m_transientList.push_back(param);
			hr = S_OK;
		}
	}
	return hr;
}

HRESULT SVMatroxGigeCameraParamTracker::ReloadCameraParams(const std::string& cameraID, const SVMatroxGigeDigitizer& p_rCamera)
{
	HRESULT hr = S_FALSE;

	SVMatroxGigeCameraParamCollection::iterator it = m_cameraParamCollection.find(cameraID);
	if (it != m_cameraParamCollection.end())
	{
		const SVMatroxGigeCameraParamList& list = (*it).second;
		
		hr = ReloadCameraParamList(list.m_mainList, p_rCamera);

		if (S_OK == hr)
		{
			hr = ReloadCameraParamList(list.m_transientList, p_rCamera);
		}
	}
	return hr;
}
	
HRESULT SVMatroxGigeCameraParamTracker::ReloadCameraParamList(const SVMatroxGigeCameraParamDeque& rList, const SVMatroxGigeDigitizer& rCamera)
{
	HRESULT result = S_OK;

	for (const auto& rParam : rList)
	{
		result = SVMatroxGigeDeviceParameterManager::SetParameter(rCamera, rParam.m_ParameterID, rParam.m_value);
		if (S_OK != result)
		{
			break;
		}
	}
	return result;
}

