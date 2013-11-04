// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekCameraParamManager
// * .File Name       : $Workfile:   SVIntekCameraParamManager.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   26 Apr 2013 14:51:44  $
// ******************************************************************************

#ifndef INCL_SVINTEKCAMERAPARAMMANAGER_H
#define INCL_SVINTEKCAMERAPARAMMANAGER_H

#include <map>
#include <deque>
#include "camera.h"
#include <fctlArgs.h>

struct SVIntekCameraParam
{
	enum CameraParamType
	{
		Unknown		= 0,
		FeatureSet	= 1,
		Quad		= 2,
		Block		= 3
	};

	CameraParamType m_paramType;
	unsigned char m_featureId;
	FEATSET m_featureSet;
	FCTLWRITEQUAD m_quad;
	FCTLWRITEBLK m_block;

	SVIntekCameraParam() : m_paramType(Unknown)
	{
		m_featureId = 0;
		memset(&m_featureSet, 0, sizeof(m_featureSet));
		memset(&m_quad, 0, sizeof(m_quad));
		memset(&m_block, 0, sizeof(m_block));
	}

	SVIntekCameraParam(const SVIntekCameraParam& rParam)
	{ 
		m_featureId = 0;
		memset(&m_featureSet, 0, sizeof(m_featureSet));
		memset(&m_quad, 0, sizeof(m_quad));
		memset(&m_block, 0, sizeof(m_block));
		Copy(rParam);
	}
	
	~SVIntekCameraParam()
	{
		if (m_paramType == Block)
		{
			if (m_block.pData)
			{
				delete [] m_block.pData;
				m_block.pData = NULL;
			}
		}
	}
	
	SVIntekCameraParam& operator=(const SVIntekCameraParam& rParam)
	{
		Copy(rParam);
		return *this;
	}

	void Copy(const SVIntekCameraParam& rParam)
	{
		m_paramType = rParam.m_paramType;

		if (rParam.m_paramType == Block)
		{
			m_block.pData = new unsigned char[rParam.m_block.DataLen];
			memcpy(m_block.pData, rParam.m_block.pData, rParam.m_block.DataLen);
			m_block.DataLen = rParam.m_block.DataLen;
			m_block.OffsetLow = rParam.m_block.OffsetLow;
			m_block.OffsetHigh = rParam.m_block.OffsetHigh;
		}
		else if (rParam.m_paramType == Quad)
		{
			m_quad = rParam.m_quad;
		}
		else
		{
			m_featureSet = rParam.m_featureSet;
			m_featureId = rParam.m_featureId;
		}
	}
};

// List of Camera Parameters (SetPArameters, WriteQuad, And WriteBlock
typedef std::deque<SVIntekCameraParam> SVIntekCameraParamDeque;

// There are 2 lists, one main list and one transient
// The transient list is for test mode support of LUT and Light Reference
struct SVIntekCameraParamList
{
	CAMPARMS m_intekCameraParams;
	bool m_bUseTransient;
	SVIntekCameraParamDeque m_mainList;
	SVIntekCameraParamDeque m_transientList;

	SVIntekCameraParamList () : m_bUseTransient(false) 
	{
		memset(&m_intekCameraParams, 0, sizeof(m_intekCameraParams));
	}

	void clear() 
	{
		m_mainList.clear();
		m_transientList.clear();
		m_bUseTransient = false;
	}
};

// List of Camera Parameters by cameraID (verndorID, serialNo)
typedef std::map<__int64, SVIntekCameraParamList> SVIntekCameraParamCollection;

class SVIntekCameraParamManager
{
private:
	SVIntekCameraParamCollection m_cameraParamCollection;
	
public:
	void Clear();

	void TrackCameraParams(__int64 cameraID);
	void UnTrackCameraParams(__int64 cameraID);
	void SetMainListLocked(__int64 cameraID, bool bLock=true);

	HRESULT AddCameraParam(__int64 cameraID, const SVIntekCameraParam& param);
	HRESULT ReloadCameraParams(__int64 cameraID, CCamera* p_pCamera);
	HRESULT ReloadCameraParamList(const SVIntekCameraParamDeque& list, CCamera* p_pCamera);
	
	HRESULT SetIntekCameraParams(__int64 cameraID, const CAMPARMS& params);
	HRESULT GetIntekCameraParams(__int64 cameraID, CAMPARMS& params);
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVIntek\SVIntekCameraParamManager.h_v  $
 * 
 *    Rev 1.0   26 Apr 2013 14:51:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   31 Jan 2007 10:58:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  582
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.15
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SVIntekCameraParam Copy constructor to clear the member variables.
 * Revised SVIntekCameraParam Copy method to set the datalen for the block parameter.
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