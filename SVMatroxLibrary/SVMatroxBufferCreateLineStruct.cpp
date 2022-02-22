// ******************************************************************************
// * COPYRIGHT (c) 2007 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVBufferCreateLineStruct
// * .File Name       : $Workfile:   SVMatroxBufferCreateLineStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:01:36  $
// ******************************************************************************


#include "stdafx.h"
#include "SVMatroxBufferCreateLineStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxBufferCreateLineStruct::SVMatroxBufferCreateLineStruct()
{
	m_lSizeX		= 10;
	m_eType			= SVUnknownBufferType;
	m_eAttribute	= SVBufAttUnknown;

}

SVMatroxBufferCreateLineStruct::~SVMatroxBufferCreateLineStruct()
{

}

bool SVMatroxBufferCreateLineStruct::SetImageDepth( long p_lDepth)
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

