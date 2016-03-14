//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCameraParamTracker
//* .File Name       : $Workfile:   SVMatroxGigeCameraParamTracker.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   22 Apr 2013 15:36:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxGigeCameraParamTracker.h"
#include "SVMatroxGigeDigitizer.h"
#include "SVMatroxGigeDeviceParameterManager.h"

void SVMatroxGigeCameraParamTracker::Clear()
{
	m_cameraParamCollection.clear();
}

void SVMatroxGigeCameraParamTracker::TrackCameraParams(const SVString& cameraID)
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

void SVMatroxGigeCameraParamTracker::UnTrackCameraParams(const SVString& cameraID)
{
	SVMatroxGigeCameraParamCollection::iterator it = m_cameraParamCollection.find(cameraID);
	if (it != m_cameraParamCollection.end())
	{
		m_cameraParamCollection.erase(it);
	}
}

void SVMatroxGigeCameraParamTracker::SetMainListLocked(const SVString& cameraID, bool bLock)
{
	SVMatroxGigeCameraParamCollection::iterator it = m_cameraParamCollection.find(cameraID);
	if (it != m_cameraParamCollection.end())
	{
		SVMatroxGigeCameraParamList& list = (*it).second;
		list.m_bUseTransient = bLock;
	}
}

HRESULT SVMatroxGigeCameraParamTracker::AddCameraParam(const SVString& cameraID, const SVMatroxGigeCameraParam& param)
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

HRESULT SVMatroxGigeCameraParamTracker::ReloadCameraParams(const SVString& cameraID, SVMatroxGigeDigitizer& p_rCamera)
{
	HRESULT hr = S_FALSE;

	SVMatroxGigeCameraParamCollection::iterator it = m_cameraParamCollection.find(cameraID);
	if (it != m_cameraParamCollection.end())
	{
		const SVMatroxGigeCameraParamList& list = (*it).second;
		
		hr = ReloadCameraParamList(list.m_mainList, p_rCamera);

		if (hr == S_OK)
		{
			hr = ReloadCameraParamList(list.m_transientList, p_rCamera);
		}
	}
	return hr;
}
	
HRESULT SVMatroxGigeCameraParamTracker::ReloadCameraParamList(const SVMatroxGigeCameraParamDeque& list, SVMatroxGigeDigitizer& p_rCamera)
{
	HRESULT hr = S_OK;

	for (size_t i = 0;i < list.size();i++)
	{
		const SVMatroxGigeCameraParam& param = list[i];
		_variant_t value( param.m_value );
		hr = SVMatroxGigeDeviceParameterManager::SetParameter(p_rCamera, param.m_ParameterID, param.m_ParameterTypeID, &(value.GetVARIANT()));
	}
	return hr;
}

