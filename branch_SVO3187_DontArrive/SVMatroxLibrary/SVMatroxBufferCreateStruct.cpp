// ******************************************************************************
// * COPYRIGHT (c) 2007 by Körber Pharma Inspection GmbH. All Rights Reserved
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

bool SVMatroxBufferCreateStruct::operator ==(const SVMatroxBufferCreateStruct& other) const
{
	return m_lSizeBand == other.m_lSizeBand && m_lSizeX == other.m_lSizeX && m_lSizeY == other.m_lSizeY	&& m_eType == other.m_eType && m_eAttribute == other.m_eAttribute;
}

bool SVMatroxBufferCreateStruct::operator <(const SVMatroxBufferCreateStruct& other) const
{
	bool retValue = m_lSizeBand < other.m_lSizeBand;
	if (m_lSizeBand == other.m_lSizeBand)
	{
		retValue = m_lSizeX < other.m_lSizeX;
		if (m_lSizeX == other.m_lSizeX)
		{
			retValue = m_lSizeY < other.m_lSizeY;
			if (m_lSizeY == other.m_lSizeY)
			{
				retValue = m_eType < other.m_eType;
				if (m_eType == other.m_eType)
				{
					retValue = m_eAttribute < other.m_eAttribute;
				}
			}
		}
	}

	return retValue;
}

bool SetImageDepth(SVMatroxBufferCreateStruct& rBuffer, long p_lDepth)
{
	bool l_bRet = true;
	if( p_lDepth == 1 )
	{
		rBuffer.m_eType = SV1BitUnsigned;
	}
	else if( p_lDepth <= 8 )
	{
		rBuffer.m_eType = SV8BitUnsigned;
	}
	else if( p_lDepth <= 16 )
	{
		rBuffer.m_eType = SV16BitUnsigned;
	}
	else if( p_lDepth <= 32 )
	{
		rBuffer.m_eType = SV32BitUnsigned;
	}
	else
	{
		l_bRet = false;
	}
	return l_bRet;
}

__int64 getBufferSize(const SVMatroxBufferCreateStruct& rBuffer)
{
	return rBuffer.m_lSizeBand * rBuffer.m_lSizeX * rBuffer.m_lSizeY;
}

