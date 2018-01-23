// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxBufferCreateStruct
// * .File Name       : $Workfile:   SVMatroxBufferCreateStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:01:48  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxBufferCreateStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxBufferCreateStruct::SVMatroxBufferCreateStruct()
{
	m_lSizeBand		= 1;
	m_lSizeX		= 10;
	m_lSizeY		= 10;
	m_eType			= SVUnknownBufferType;
	m_eAttribute	= SVBufAttUnknown;

}

SVMatroxBufferCreateStruct::~SVMatroxBufferCreateStruct()
{

}

bool SVMatroxBufferCreateStruct::SetImageDepth( long p_lDepth)
{
	bool l_bRet = true;
	if( p_lDepth == 1 )
	{
		m_eType = SV1BitUnsigned;
	}
	else if( p_lDepth <= 8 )
	{
		m_eType = SV8BitUnsigned;
	}
	else if( p_lDepth <= 16 )
	{
		m_eType = SV16BitUnsigned;
	}
	else if( p_lDepth <= 32 )
	{
		m_eType = SV32BitUnsigned;
	}
	else
	{
		l_bRet = false;
	}
	return l_bRet;
}

bool SVMatroxBufferCreateStruct::operator ==(const SVMatroxBufferCreateStruct& other) const
{
	return m_lSizeBand == other.m_lSizeBand && m_lSizeX == other.m_lSizeX && m_lSizeY == other.m_lSizeY	&& m_eType == other.m_eType && m_eAttribute == other.m_eAttribute;
}
