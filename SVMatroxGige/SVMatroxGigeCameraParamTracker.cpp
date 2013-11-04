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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeCameraParamTracker.cpp_v  $
 * 
 *    Rev 1.1   22 Apr 2013 15:36:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed "Description" label from header.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 14:42:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Jun 2012 16:43:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add the needed functionality to track parameter information and reset the camera to the desired parameters on reconnect.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/