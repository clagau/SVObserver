// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVBufferResource
// * .File Name       : $Workfile:   SVBufferResource.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 14:56:08  $
// ******************************************************************************
#include "stdafx.h"
#include "SVBufferResource.h"

SVBufferResource::SVBufferResource(void)
{
	m_pBuffer = NULL;
}

SVBufferResource::~SVBufferResource(void)
{
	Destroy();
}


void SVBufferResource::Destroy(void)
{
	if( m_pBuffer )
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
	}
}
HRESULT SVBufferResource::Resize( long l_lSize)
{
	Destroy();
	m_pBuffer = new char[l_lSize];
	if( m_pBuffer )
		return S_OK;
	else
		return S_FALSE;
}

HRESULT SVBufferResource::GetBuffer( MIL_TEXT_CHAR*& p_rpMilChar )
{
	if( m_pBuffer )
	{
		p_rpMilChar = m_pBuffer;
		return S_OK;
	}
	else
	{
		p_rpMilChar = NULL;
		return S_FALSE;
	}
}


