// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVCallbackStruct
// * .File Name       : $Workfile:   SVCallbackStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 13:00:44  $
// ******************************************************************************

#include "stdafx.h"
#include "SVCallbackStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVCallbackStruct::SVCallbackStruct()
{
	m_pCallback = nullptr;

	m_pOwner = nullptr;
	m_pData = nullptr;
}

SVCallbackStruct::SVCallbackStruct( const SVCallbackStruct &p_rOriginal )
{
	m_pCallback = nullptr;

	m_pOwner = nullptr;
	m_pData = nullptr;

	*this = p_rOriginal;
}

SVCallbackStruct::~SVCallbackStruct()
{
}

const SVCallbackStruct &SVCallbackStruct::operator=( const SVCallbackStruct &p_rOriginal )
{
	m_pCallback = p_rOriginal.m_pCallback;

	m_pOwner = p_rOriginal.m_pOwner;
	m_pData = p_rOriginal.m_pData;

	return *this;
}

bool operator==( const SVCallbackStruct &p_rLeft,
                 const SVCallbackStruct &p_rRight )
{
	bool l_bOk = true;

	l_bOk = p_rLeft.m_pCallback == p_rRight.m_pCallback && l_bOk;
	l_bOk = p_rLeft.m_pOwner == p_rRight.m_pOwner && l_bOk;
	l_bOk = p_rLeft.m_pData == p_rRight.m_pData && l_bOk;

	return l_bOk;
}

