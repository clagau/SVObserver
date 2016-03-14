//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOPPQObj
//* .File Name       : $Workfile:   SVOPPQObj.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 06:48:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVOPPQObj.h"
#include "SVPPQConstants.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVOPPQObj::SVOPPQObj()
: m_sPPQName(_T(""))
, m_sAttachedTrigger(_T(""))
, m_iPPQMode(0)
, m_lPPQLength(2)
, m_lPPQOutputResetDelay(0)
, m_lPPQOutputDelayTime(100)
, m_bMaintainSrcImg(false)
, m_lInspectionTimeout(0)
, m_conditionalOutputName(PPQ_CONDITIONAL_OUTPUT_ALWAYS)
{
}

SVOPPQObj::~SVOPPQObj()
{
	m_slAttachedCameraList.RemoveAll();
	m_slAttachedCameraList.RemoveAll();
}

void SVOPPQObj::SetPPQName(CString sPPQName)
{
	m_sPPQName = sPPQName;
}

CString SVOPPQObj::GetPPQName()
{
	return m_sPPQName;
}

void SVOPPQObj::AttachTriggerToPPQ(CString sTriggerName)
{
	m_sAttachedTrigger = sTriggerName;
}

void SVOPPQObj::DetachTriggerFromPPQ()
{
	m_sAttachedTrigger = "";
}

CString SVOPPQObj::GetAttachedTriggerName() const
{
	return m_sAttachedTrigger;
}

BOOL SVOPPQObj::AttachCameraToPPQ(CString sCameraName)
{
	BOOL bRet = FALSE;
	//check to see if it is already in list
	//if not add camera to list
	if (m_slAttachedCameraList.Find(sCameraName) == NULL)
	{ 
		m_slAttachedCameraList.AddTail(sCameraName);
		bRet = TRUE;
	}
	return bRet;
}

BOOL SVOPPQObj::DetachCameraFromPPQ(CString sCameraName)
{
	BOOL bRet = FALSE;
	POSITION pos;
	
	if (!m_slAttachedCameraList.IsEmpty())
	{
		pos = m_slAttachedCameraList.Find(sCameraName);
		if (pos != NULL)
		{ //camera is in list.  now delete
			m_slAttachedCameraList.RemoveAt(pos);
			bRet = TRUE;
		}
	}
	return bRet;
}

int SVOPPQObj::GetAttachedCameraCount() const
{
	return static_cast<int>(m_slAttachedCameraList.GetCount());
}

CString SVOPPQObj::GetAttachedCamera(int iPos) const
{
	CString sReturnStr = "";
	POSITION pos = m_slAttachedCameraList.FindIndex(iPos);
	
	if (pos != NULL)
	{
		sReturnStr = m_slAttachedCameraList.GetAt(pos);
	}
	return sReturnStr;
}

BOOL SVOPPQObj::AttachInspectionToPPQ(CString sInspectName)
{
	BOOL bRet = FALSE;
	
	if (m_slAttachedInspectList.Find(sInspectName) == NULL)
	{
		//was not found, add to list
		m_slAttachedInspectList.AddTail(sInspectName);
		bRet = TRUE;
	}
	return bRet;
}

BOOL SVOPPQObj::DetachInspectionFromPPQ(CString sInspectName)
{
	BOOL bRet = FALSE;
	POSITION pos;
	
	if (!m_slAttachedInspectList.IsEmpty())
	{
		pos = m_slAttachedInspectList.Find(sInspectName);
		
		if (pos != NULL)
		{
			m_slAttachedInspectList.RemoveAt(pos);
			bRet = TRUE;
		}
	}
	return bRet;
}

int SVOPPQObj::GetAttachedInspectionCount()
{
	return static_cast<int>(m_slAttachedInspectList.GetCount());
}

CString SVOPPQObj::GetAttachedInspection(int iPos)
{
	POSITION pos = m_slAttachedInspectList.FindIndex(iPos);
	
	return m_slAttachedInspectList.GetAt(pos);
}

//property methods
void SVOPPQObj::SetPPQMode(int iPPQMode)
{
	m_iPPQMode = iPPQMode;
}

void SVOPPQObj::SetPPQLength(long lLength)
{
	m_lPPQLength = lLength;
}

void SVOPPQObj::SetPPQOutputResetDelay(long lResetDelay)
{
	m_lPPQOutputResetDelay = lResetDelay;
}

void SVOPPQObj::SetPPQOutputDelayTime(long lDelayTime)
{
	m_lPPQOutputDelayTime = lDelayTime;
}

int SVOPPQObj::GetPPQMode() const
{
	return m_iPPQMode;
}

long SVOPPQObj::GetPPQLength() const
{
	return m_lPPQLength;
}

long SVOPPQObj::GetPPQOutputResetDelay() const
{
	return m_lPPQOutputResetDelay;
}

long SVOPPQObj::GetPPQOutputDelayTime() const
{
	return m_lPPQOutputDelayTime;
}

SVOPPQObj &SVOPPQObj::operator =(const SVOPPQObj &source)
{
	if (this != &source)
	{
		m_sPPQName = source.m_sPPQName;
		m_sAttachedTrigger = source.m_sAttachedTrigger;
		m_slAttachedCameraList.RemoveAll();
		//The MFC method AddHead only excepts a CStringList pointer that is why const_cast is necessary!
		m_slAttachedCameraList.AddHead( const_cast<CStringList*> (&source.m_slAttachedCameraList) );
		m_slAttachedInspectList.RemoveAll();
		//The MFC method AddHead only excepts a CStringList pointer that is why const_cast is necessary!
		m_slAttachedInspectList.AddHead( const_cast<CStringList*> (&source.m_slAttachedInspectList) );
		m_iPPQMode = source.m_iPPQMode;
		m_lPPQLength = source.m_lPPQLength;
		m_lPPQOutputResetDelay = source.m_lPPQOutputResetDelay;
		m_lPPQOutputDelayTime = source.m_lPPQOutputDelayTime;
		m_bMaintainSrcImg = source.m_bMaintainSrcImg;
		m_lInspectionTimeout = source.m_lInspectionTimeout;
		m_conditionalOutputName = source.m_conditionalOutputName;
		m_availableInputs = source.m_availableInputs;
		m_importedInputList = source.m_importedInputList;
	}
	return (*this);
}

void SVOPPQObj::SetMaintainSourceImageProperty(bool bValue)
{
	m_bMaintainSrcImg = bValue;
}

bool SVOPPQObj::GetMaintainSourceImageProperty() const
{
	return m_bMaintainSrcImg;
}

void SVOPPQObj::SetInspectionTimeout( long lTimeoutMillisec )
{
	m_lInspectionTimeout = lTimeoutMillisec;
}

long SVOPPQObj::GetInspectionTimeout() const
{
	return m_lInspectionTimeout;
}

void SVOPPQObj::SetAvailableInputsForConditionalOutput(const SVNameGuidPairList& list)
{
	m_availableInputs = list;
}

const SVNameGuidPairList& SVOPPQObj::GetAvailableInputsForConditionalOutput() const
{
	return m_availableInputs;
}

void SVOPPQObj::SetConditionalOutputName(const SVString& name)
{
	m_conditionalOutputName = name;
}

const SVString& SVOPPQObj::GetConditionalOutputName() const
{
	return m_conditionalOutputName;
}

bool SVOPPQObj::IsConditionalOutputCameraInput() const
{
	return (m_conditionalOutputName.find("Camera") == 0);
}

void SVOPPQObj::RemoveCameraInputConditionalOutput()
{
	if (IsConditionalOutputCameraInput())
	{
		m_conditionalOutputName = PPQ_CONDITIONAL_OUTPUT_ALWAYS;
	}
}

void SVOPPQObj::SetImportedInputList(const SVImportedInputList& list)
{
	m_importedInputList = list;
}

const SVImportedInputList& SVOPPQObj::GetImportedInputList() const
{
	return m_importedInputList;
}

void SVOPPQObj::ClearImportedInputList()
{
	m_importedInputList.clear();
}

