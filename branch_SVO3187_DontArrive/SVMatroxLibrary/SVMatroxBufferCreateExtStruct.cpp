// ******************************************************************************
// * COPYRIGHT (c) 2007 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVBufferCreateExtStruct
// * .File Name       : $Workfile:   SVMatroxBufferCreateExtStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:00:22  $
// ******************************************************************************


#include "stdafx.h"
#include "SVMatroxBufferCreateExtStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxBufferCreateExtStruct::SVMatroxBufferCreateExtStruct()
: m_lSizeBand( 1 ), m_lSizeX(10), m_lSizeY(10),
 m_PitchWidth(0), m_ppArrayOfDataPtr(nullptr)
{
	m_eType = SV8BitUnsigned;
	m_eAttribute = SVBufAttUnknown;
	m_eControlFlag = SVBufInfoUnknown;
}

SVMatroxBufferCreateExtStruct::~SVMatroxBufferCreateExtStruct()
{

}

bool SVMatroxBufferCreateExtStruct::SetImageDepth( long p_lDepth)
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

