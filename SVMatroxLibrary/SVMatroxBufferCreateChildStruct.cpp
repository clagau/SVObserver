// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVBufferCreateChildStruct
// * .File Name       : $Workfile:   SVMatroxBufferCreateChildStruct.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:00:10  $
// ******************************************************************************


#include "stdafx.h"
#include "SVMatroxBufferCreateChildStruct.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxBufferCreateChildStruct::SVMatroxBufferCreateChildStruct(const SVMatroxBuffer& p_rBuf)
: m_lBand(0), m_lParentBandCount(1),	m_lOffX(0), m_lOffY(0), m_lSizeX(10), m_lSizeY(10), m_ParentBufId( p_rBuf )
{

}


SVMatroxBufferCreateChildStruct::~SVMatroxBufferCreateChildStruct()
{

}

