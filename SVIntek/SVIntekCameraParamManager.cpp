// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekCameraParamManager
// * .File Name       : $Workfile:   SVIntekCameraParamManager.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   26 Apr 2013 14:51:30  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIntekCameraParamManager.h"

void SVIntekCameraParamManager::Clear()
{
	m_cameraParamCollection.clear();
}

void SVIntekCameraParamManager::TrackCameraParams(__int64 cameraID)
{
	SVIntekCameraParamCollection::iterator it = m_cameraParamCollection.find(cameraID);
	if (it == m_cameraParamCollection.end())
	{
		SVIntekCameraParamList list;
		m_cameraParamCollection[cameraID] = list;
	}
	else
	{
		(*it).second.clear();
	}
}

void SVIntekCameraParamManager::UnTrackCameraParams(__int64 cameraID)
{
	SVIntekCameraParamCollection::iterator it = m_cameraParamCollection.find(cameraID);
	if (it != m_cameraParamCollection.end())
	{
		m_cameraParamCollection.erase(it);
	}
}

void SVIntekCameraParamManager::SetMainListLocked(__int64 cameraID, bool bLock)
{
	SVIntekCameraParamCollection::iterator it = m_cameraParamCollection.find(cameraID);
	if (it != m_cameraParamCollection.end())
	{
		SVIntekCameraParamList& list = (*it).second;
		list.m_bUseTransient = bLock;
	}
}

HRESULT SVIntekCameraParamManager::AddCameraParam(__int64 cameraID, const SVIntekCameraParam& param)
{
	HRESULT lhrOk = S_FALSE;

	SVIntekCameraParamCollection::iterator it = m_cameraParamCollection.find(cameraID);
	if (it != m_cameraParamCollection.end())
	{
		SVIntekCameraParamList& list = (*it).second;
		// if not locked use the main list
		if (!list.m_bUseTransient)
		{
			list.m_mainList.push_back(param);
			lhrOk = S_OK;
		}
		// otherwise use the transient list
		else
		{
			list.m_transientList.push_back(param);
			lhrOk = S_OK;
		}
	}
	return lhrOk;
}

HRESULT SVIntekCameraParamManager::ReloadCameraParams(__int64 cameraID, CCamera* p_pCamera)
{
	HRESULT lhrOk = S_OK;

	SVIntekCameraParamCollection::iterator it = m_cameraParamCollection.find(cameraID);
	if (it != m_cameraParamCollection.end())
	{
		const SVIntekCameraParamList& list = (*it).second;
		
		lhrOk = ReloadCameraParamList(list.m_mainList, p_pCamera);

		if (lhrOk == S_OK)
		{
			lhrOk = ReloadCameraParamList(list.m_transientList, p_pCamera);
		}
	}
	return lhrOk;
}

HRESULT SVIntekCameraParamManager::ReloadCameraParamList(const SVIntekCameraParamDeque& list, CCamera* p_pCamera)
{
	HRESULT lhrOk = S_OK;

	for ( unsigned int i = 0;i < list.size();i++)
	{
		SVIntekCameraParam param = list[i];

		if (param.m_paramType == SVIntekCameraParam::Block)
		{
			p_pCamera->WriteBlock(param.m_block.OffsetHigh, param.m_block.OffsetLow, param.m_block.pData, param.m_block.DataLen);
		}
		else if (param.m_paramType == SVIntekCameraParam::Quad)
		{
			p_pCamera->WriteQuad(param.m_quad.OffsetHigh, param.m_quad.OffsetLow, param.m_quad.Quadlet);
		}
		else if (param.m_paramType == SVIntekCameraParam::FeatureSet)
		{
			p_pCamera->Lock();
			p_pCamera->SetFeature(param.m_featureId, &param.m_featureSet);
			p_pCamera->Unlock();
		}
		else // Unknown type...
		{
			lhrOk = S_FALSE;
			break;
		}
	}
	return lhrOk;
}

HRESULT SVIntekCameraParamManager::GetIntekCameraParams(__int64 cameraID, CAMPARMS& params)
{
	HRESULT hr = S_FALSE;
	SVIntekCameraParamCollection::iterator it = m_cameraParamCollection.find(cameraID);
	if (it != m_cameraParamCollection.end())
	{
		SVIntekCameraParamList& list = (*it).second;
		params = list.m_intekCameraParams;
		hr = S_OK;
	}
	return hr;
}

HRESULT SVIntekCameraParamManager::SetIntekCameraParams(__int64 cameraID, const CAMPARMS& params)
{
	HRESULT hr = S_FALSE;
	SVIntekCameraParamCollection::iterator it = m_cameraParamCollection.find(cameraID);
	if (it != m_cameraParamCollection.end())
	{
		SVIntekCameraParamList& list = (*it).second;
		list.m_intekCameraParams = params;
		hr = S_OK;
	}
	return hr;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVIntek\SVIntekCameraParamManager.cpp_v  $
 * 
 *    Rev 1.0   26 Apr 2013 14:51:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 Jun 2007 12:54:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   28 Nov 2006 17:33:52   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  582
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.15
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
