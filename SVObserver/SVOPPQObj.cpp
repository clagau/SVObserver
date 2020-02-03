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

void SVOPPQObj::SetPPQName(LPCTSTR PPQName)
{
	m_PPQName = PPQName;
}

const std::string& SVOPPQObj::GetPPQName()
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

const std::string& SVOPPQObj::GetAttachedTriggerName() const
{
	return m_AttachedTrigger;
}

bool SVOPPQObj::AttachCameraToPPQ( LPCTSTR CameraName )
{
	bool Result( false );
	//check to see if it is already in list
	//if not add camera to list
	std::string AttachedCamera( CameraName );
	SvDef::StringVector::const_iterator Iter = std::find( m_AttachedCameraList.begin(), m_AttachedCameraList.end(), AttachedCamera );
	if( m_AttachedCameraList.end() == Iter )
	{ 
		m_AttachedCameraList.push_back( AttachedCamera );
		Result = true;
	}
	return Result;
}

bool SVOPPQObj::DetachCameraFromPPQ(LPCTSTR CameraName)
{
	bool Result( false );
	
	if (!m_AttachedCameraList.empty())
	{
		std::string AttachedCamera( CameraName );
		SvDef::StringVector::const_iterator Iter = std::find( m_AttachedCameraList.begin(), m_AttachedCameraList.end(), AttachedCamera );
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

std::string SVOPPQObj::GetAttachedCamera(int iPos) const
{
	SvDef::StringVector::const_iterator Iter = m_AttachedCameraList.begin() + iPos;
	
	if( m_AttachedCameraList.end() != Iter )
	{
		return *Iter;
	}

	return std::string();
}

bool SVOPPQObj::AttachInspectionToPPQ(LPCTSTR InspectName)
{
	bool Result( false );
	
	std::string AttachedInspection( InspectName );
	SvDef::StringVector::const_iterator Iter = std::find( m_AttachedInspectList.begin(), m_AttachedInspectList.end(), AttachedInspection );
	if( m_AttachedInspectList.end() == Iter )
	{ 
		m_AttachedInspectList.push_back( AttachedInspection );
		Result = true;
	}
	return Result;
}

bool SVOPPQObj::DetachInspectionFromPPQ(LPCTSTR InspectName)
{
	bool Result( false );
	
	if (!m_AttachedInspectList.empty())
	{
		std::string AttachedInspection( InspectName );
		SvDef::StringVector::const_iterator Iter = std::find( m_AttachedInspectList.begin(), m_AttachedInspectList.end(), AttachedInspection );
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

std::string SVOPPQObj::GetAttachedInspection(int iPos)
{
	SvDef::StringVector::const_iterator Iter = m_AttachedInspectList.begin() + iPos;

	if( m_AttachedInspectList.end() != Iter )
	{
		return *Iter;
	}

	return std::string();
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

void SVOPPQObj::SetMaintainSourceImageProperty(bool bValue)
{
	m_bMaintainSrcImg = bValue;
}

void SVOPPQObj::SetMaxProcessingOffsetProperty(int value)
{
	m_MaxProcessingOffset = value;
}

bool SVOPPQObj::GetMaintainSourceImageProperty() const
{
	return m_bMaintainSrcImg;
}

int SVOPPQObj::GetMaxProcessingOffsetProperty() const
{
	return m_MaxProcessingOffset;
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

void SVOPPQObj::SetConditionalOutputName(const std::string& name)
{
	m_conditionalOutputName = name;
}

const std::string& SVOPPQObj::GetConditionalOutputName() const
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

