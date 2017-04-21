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
//Moved to precompiled header: #include <algorithm>
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
: m_PPQName(_T(""))
, m_AttachedTrigger(_T(""))
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
	m_AttachedCameraList.clear();
	m_AttachedCameraList.clear();
}

void SVOPPQObj::SetPPQName(LPCTSTR PPQName)
{
	m_PPQName = PPQName;
}

const SVString& SVOPPQObj::GetPPQName()
{
	return m_PPQName;
}

void SVOPPQObj::AttachTriggerToPPQ( LPCTSTR TriggerName )
{
	m_AttachedTrigger = TriggerName;
}

void SVOPPQObj::DetachTriggerFromPPQ()
{
	m_AttachedTrigger.clear();
}

const SVString& SVOPPQObj::GetAttachedTriggerName() const
{
	return m_AttachedTrigger;
}

BOOL SVOPPQObj::AttachCameraToPPQ( LPCTSTR CameraName )
{
	bool Result( false );
	//check to see if it is already in list
	//if not add camera to list
	SVString AttachedCamera( CameraName );
	SVStringVector::const_iterator Iter = std::find( m_AttachedCameraList.begin(), m_AttachedCameraList.end(), AttachedCamera );
	if( m_AttachedCameraList.end() == Iter )
	{ 
		m_AttachedCameraList.push_back( AttachedCamera );
		Result = true;
	}
	return Result;
}

BOOL SVOPPQObj::DetachCameraFromPPQ(LPCTSTR CameraName)
{
	bool Result( false );
	
	if (!m_AttachedCameraList.empty())
	{
		SVString AttachedCamera( CameraName );
		SVStringVector::const_iterator Iter = std::find( m_AttachedCameraList.begin(), m_AttachedCameraList.end(), AttachedCamera );
		if( m_AttachedCameraList.end() != Iter)
		{ //camera is in list.  now delete
			m_AttachedCameraList.erase(Iter);
			Result = true;
		}
	}

	return Result;
}

int SVOPPQObj::GetAttachedCameraCount() const
{
	return static_cast<int> (m_AttachedCameraList.size());
}

SVString SVOPPQObj::GetAttachedCamera(int iPos) const
{
	SVStringVector::const_iterator Iter = m_AttachedCameraList.begin() + iPos;
	
	if( m_AttachedCameraList.end() != Iter )
	{
		return *Iter;
	}

	return SVString();
}

BOOL SVOPPQObj::AttachInspectionToPPQ(LPCTSTR InspectName)
{
	bool Result( false );
	
	SVString AttachedInspection( InspectName );
	SVStringVector::const_iterator Iter = std::find( m_AttachedInspectList.begin(), m_AttachedInspectList.end(), AttachedInspection );
	if( m_AttachedInspectList.end() == Iter )
	{ 
		m_AttachedInspectList.push_back( AttachedInspection );
		Result = true;
	}
	return Result;
}

BOOL SVOPPQObj::DetachInspectionFromPPQ(LPCTSTR InspectName)
{
	bool Result( false );
	
	if (!m_AttachedInspectList.empty())
	{
		SVString AttachedInspection( InspectName );
		SVStringVector::const_iterator Iter = std::find( m_AttachedInspectList.begin(), m_AttachedInspectList.end(), AttachedInspection );
		if( m_AttachedInspectList.end() != Iter)
		{ //camera is in list.  now delete
			m_AttachedInspectList.erase( Iter );
			Result = true;
		}
	}

	return Result;
}

int SVOPPQObj::GetAttachedInspectionCount()
{
	return static_cast<int> (m_AttachedInspectList.size());
}

SVString SVOPPQObj::GetAttachedInspection(int iPos)
{
	SVStringVector::const_iterator Iter = m_AttachedInspectList.begin() + iPos;

	if( m_AttachedInspectList.end() != Iter )
	{
		return *Iter;
	}

	return SVString();
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

SVOPPQObj &SVOPPQObj::operator =(const SVOPPQObj &rRhs)
{
	if (this != &rRhs)
	{
		m_PPQName = rRhs.m_PPQName;
		m_AttachedTrigger = rRhs.m_AttachedTrigger;
		m_AttachedCameraList.clear();
		m_AttachedCameraList = rRhs.m_AttachedCameraList;
		m_AttachedInspectList.clear();
		m_AttachedInspectList = rRhs.m_AttachedInspectList;
		m_iPPQMode = rRhs.m_iPPQMode;
		m_lPPQLength = rRhs.m_lPPQLength;
		m_lPPQOutputResetDelay = rRhs.m_lPPQOutputResetDelay;
		m_lPPQOutputDelayTime = rRhs.m_lPPQOutputDelayTime;
		m_bMaintainSrcImg = rRhs.m_bMaintainSrcImg;
		m_lInspectionTimeout = rRhs.m_lInspectionTimeout;
		m_conditionalOutputName = rRhs.m_conditionalOutputName;
		m_availableInputs = rRhs.m_availableInputs;
		m_importedInputList = rRhs.m_importedInputList;
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
	return ( 0 == m_conditionalOutputName.find("Camera") );
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

