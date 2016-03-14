//******************************************************************************
//* COPYRIGHT (c) 2011 by SVSeidenader Vision, Inc.
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
: SVMatroxBufferFacade(), m_StartIdentifier( 0 ), m_Identifier( 0 ), m_CreatorName( "Unknown" )
{
}

SVMatroxBufferTemplate::SVMatroxBufferTemplate( SVMatroxIdentifier p_Identifier, const SVString& p_rCreatorName )
: SVMatroxBufferFacade(), m_StartIdentifier( p_Identifier ), m_Identifier( p_Identifier ), m_CreatorName( p_rCreatorName )
{
}

SVMatroxBufferTemplate::~SVMatroxBufferTemplate()
{
	m_Identifier = 0;
}

bool SVMatroxBufferTemplate::empty() const
{
	return ( m_Identifier == 0 );
}

SVMatroxIdentifier SVMatroxBufferTemplate::GetIdentifier() const
{
	return m_Identifier;
}

