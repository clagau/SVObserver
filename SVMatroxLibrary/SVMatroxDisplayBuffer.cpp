// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxDisplayBuffer
// * .File Name       : $Workfile:   SVMatroxDisplayBuffer.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:12:38  $
// ******************************************************************************


#include "stdafx.h"
#include "SVMatroxDisplayBuffer.h"
#include "SVMatroxDisplayInterface.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVMatroxDisplayBuffer::SVMatroxDisplayBuffer()
: m_DisplayBufferPtr()
{
}

SVMatroxDisplayBuffer::~SVMatroxDisplayBuffer()
{
}

bool SVMatroxDisplayBuffer::empty() const
{
	return m_DisplayBufferPtr.empty() || m_DisplayBufferPtr->empty();
}

void SVMatroxDisplayBuffer::clear()
{
	m_DisplayBufferPtr.clear();
}

bool SVMatroxDisplayBuffer::operator<( const SVMatroxDisplayBuffer& p_rBuf ) const
{
	bool l_Status = true;

	if( m_DisplayBufferPtr.empty() )
	{
		l_Status = !( p_rBuf.m_DisplayBufferPtr.empty() );
	}
	else
	{
		l_Status = ( p_rBuf.m_DisplayBufferPtr.empty() );

		if( !l_Status )
		{
			l_Status = ( m_DisplayBufferPtr->GetIdentifier() < p_rBuf.m_DisplayBufferPtr->GetIdentifier() );
		}
	}

	return l_Status;
}

SVMatroxIdentifier SVMatroxDisplayBuffer::GetIdentifier() const
{
	SVMatroxIdentifier l_Identifier = 0;

	if( !( m_DisplayBufferPtr.empty() ) )
	{
		l_Identifier = m_DisplayBufferPtr->GetIdentifier();
	}

	return l_Identifier;
}

