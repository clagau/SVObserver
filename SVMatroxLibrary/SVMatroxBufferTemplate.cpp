//******************************************************************************
//* COPYRIGHT (c) 2011 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxBufferTemplate
//* .File Name       : $Workfile:   SVMatroxBufferTemplate.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:02:22  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxBufferTemplate.h"

SVMatroxBufferTemplate::SVMatroxBufferTemplate()
: m_Identifier( 0 ), m_CreatorName( "Unknown" )
{
}

SVMatroxBufferTemplate::SVMatroxBufferTemplate( __int64 p_Identifier, const std::string& p_rCreatorName )
: m_Identifier( p_Identifier ), m_CreatorName( p_rCreatorName )
{
}

SVMatroxBufferTemplate::~SVMatroxBufferTemplate()
{
	freeBuffer();
}

void SVMatroxBufferTemplate::freeBuffer()
{
	if (0 != m_Identifier)
	{
		MbufFree(m_Identifier);
		m_Identifier = 0;
	}
}

