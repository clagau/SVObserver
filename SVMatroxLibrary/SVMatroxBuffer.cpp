//******************************************************************************
//* COPYRIGHT (c) 2007 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxBuffer
//* .File Name       : $Workfile:   SVMatroxBuffer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:59:46  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxBuffer.h"
#include "SVMatroxBufferInterface.h"
#include "SVMatroxDisplayBuffer.h"

SVMatroxBuffer::SVMatroxBuffer()
: m_BufferPtr()
{
}

SVMatroxBuffer::SVMatroxBuffer(const SVMatroxBuffer& p_rBuf)
{
	m_BufferPtr = p_rBuf.m_BufferPtr;
}

SVMatroxBuffer::SVMatroxBuffer(const SVMatroxDisplayBuffer& p_rBuf)
{
	m_BufferPtr = p_rBuf.m_DisplayBufferPtr;
}

SVMatroxBuffer::~SVMatroxBuffer()
{
}

bool SVMatroxBuffer::operator<(const SVMatroxBuffer& p_rBuf) const 
{
	bool l_Status = true;

	if( m_BufferPtr.empty() )
	{
		l_Status = !( p_rBuf.m_BufferPtr.empty() );
	}
	else
	{
		l_Status = ( p_rBuf.m_BufferPtr.empty() );

		if( !l_Status )
		{
			l_Status = ( m_BufferPtr->GetIdentifier() < p_rBuf.m_BufferPtr->GetIdentifier() );
		}
	}

	return l_Status;
}

bool SVMatroxBuffer::empty() const
{
	return ( m_BufferPtr.empty() || m_BufferPtr->empty() );
}

void SVMatroxBuffer::clear()
{
	m_BufferPtr.clear();
}

void SVMatroxBuffer::swap( SVMatroxBuffer& p_rBuf )
{
	SVMatroxBufferPtr l_ID = m_BufferPtr;
	m_BufferPtr = p_rBuf.m_BufferPtr;
	p_rBuf.m_BufferPtr = l_ID;
}

const SVMatroxBuffer& SVMatroxBuffer::operator=( const SVMatroxBuffer& p_rBuf )
{
	if( this != &p_rBuf )
	{
		m_BufferPtr = p_rBuf.m_BufferPtr;
	}

	return *this;
}

SVMatroxIdentifier SVMatroxBuffer::GetIdentifier() const
{
	SVMatroxIdentifier l_Identifier = 0;

	if( !( m_BufferPtr.empty() ) )
	{
		l_Identifier = m_BufferPtr->GetIdentifier();
	}

	return l_Identifier;
}

bool SVMatroxBuffer::IsParent( const SVMatroxBuffer& p_rParent ) const
{
	bool l_Status = ( S_OK == SVMatroxBufferInterface::IsParent( p_rParent, *this ) );

	return l_Status;
}

