//******************************************************************************
//* COPYRIGHT (c) 2007 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOBoardCapabilities
//* .File Name       : $Workfile:   SVIOBoardCapabilities.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   :     $Date:   23 Apr 2013 11:08:10  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVIOBoardCapabilities.h"
#pragma endregion Includes

SVIOBoardCapabilities::SVIOBoardCapabilities()
:m_iNumTriggers(4),
m_iNumInputs(8),
m_iNumOutputs(16),
m_iStrobeInverters(4),
m_iTriggerInverters(4)
{
}

SVIOBoardCapabilities::SVIOBoardCapabilities( const SVIOBoardCapabilities& p_rCapable )
{
	if (this != &p_rCapable)
	{
		*this = p_rCapable;
	}
}

const SVIOBoardCapabilities& SVIOBoardCapabilities::operator=(const SVIOBoardCapabilities& p_rCapable )
{
	m_iNumInputs = p_rCapable.m_iNumInputs;
	m_iNumOutputs = p_rCapable.m_iNumOutputs;
	m_iNumTriggers = p_rCapable.m_iNumTriggers;
	m_iStrobeInverters = p_rCapable.m_iStrobeInverters;
	m_iTriggerInverters = p_rCapable.m_iTriggerInverters;

	return *this;
}

SVIOBoardCapabilities::~SVIOBoardCapabilities()
{
}

int SVIOBoardCapabilities::GetTriggerInverterCount() const
{
	return m_iTriggerInverters;
}

int SVIOBoardCapabilities::GetStrobeInverterCount() const
{
	return m_iStrobeInverters;
}

int SVIOBoardCapabilities::GetTriggerCount() const
{
	return m_iNumTriggers;
}

int SVIOBoardCapabilities::GetInputCount() const
{
	return m_iNumInputs;
}

int SVIOBoardCapabilities::GetOutputCount() const
{
	return m_iNumOutputs;
}

void SVIOBoardCapabilities::SetInputCount( int p_iInputs )
{
	m_iNumInputs = p_iInputs;
}

void SVIOBoardCapabilities::SetOutputCount( int p_iOutputs )
{
	m_iNumOutputs = p_iOutputs;
}

void SVIOBoardCapabilities::SetTriggerCount( int p_iTriggers )
{
	m_iNumTriggers = p_iTriggers;
}

void SVIOBoardCapabilities::SetTriggerInverters( int p_iTrigInverters )
{
	m_iTriggerInverters = p_iTrigInverters;
}

void SVIOBoardCapabilities::SetStrobeInverters( int p_iStrobeInverters )
{
	m_iStrobeInverters = p_iStrobeInverters;
}

bool SVIOBoardCapabilities::IsNonIOSVIM() const
{
	return (m_iNumInputs == 0 && m_iNumOutputs == 0);
}

void SVIOBoardCapabilities::SetNonIOSVIM(int numTriggers)
{
	m_iTriggerInverters = 0;
	m_iStrobeInverters = 0;
	m_iNumInputs = 0;
	m_iNumOutputs = 0;
	m_iNumTriggers = numTriggers;
}

