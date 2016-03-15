// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIODigitalStruct
// * .File Name       : $Workfile:   SVIODigitalStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 12:46:48  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIODigitalStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVIODigitalStruct::SVIODigitalStruct()
{
	m_bIsInverted = false;
	m_bIsForced = false;
	m_bForcedValue = false;
	m_bValue = false;
}

SVIODigitalStruct::SVIODigitalStruct( const SVIODigitalStruct &p_rOriginal )
{
	m_bIsInverted = false;
	m_bIsForced = false;
	m_bForcedValue = false;
	m_bValue = false;

	*this = p_rOriginal;
}

SVIODigitalStruct::~SVIODigitalStruct()
{
}

const SVIODigitalStruct &SVIODigitalStruct::operator=( const SVIODigitalStruct &p_rOriginal )
{
	m_bIsInverted = p_rOriginal.m_bIsInverted;
	m_bIsForced = p_rOriginal.m_bIsForced;
	m_bForcedValue = p_rOriginal.m_bForcedValue;
	m_bValue = p_rOriginal.m_bValue;

	return *this;
}

