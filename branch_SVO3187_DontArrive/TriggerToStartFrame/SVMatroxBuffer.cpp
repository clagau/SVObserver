//******************************************************************************
//* COPYRIGHT (c) 2007 by Körber Pharma Inspection GmbH. All Rights Reserved
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

SVMatroxBuffer::SVMatroxBuffer()
: m_BufferPtr()
{
}

SVMatroxBuffer::SVMatroxBuffer(const SVMatroxBuffer& p_rBuf)
: m_BufferPtr{p_rBuf.m_BufferPtr}
{
}

SVMatroxBuffer::~SVMatroxBuffer()
{
}

bool SVMatroxBuffer::operator<(const SVMatroxBuffer& p_rBuf) const 
{
	bool l_Status = true;

	if( nullptr == m_BufferPtr )
	{
		l_Status = nullptr != p_rBuf.m_BufferPtr;
	}
	else
	{
		l_Status = nullptr == p_rBuf.m_BufferPtr;

		if( !l_Status )
		{
			l_Status = ( m_BufferPtr->GetIdentifier() < p_rBuf.m_BufferPtr->GetIdentifier() );
		}
	}

	return l_Status;
}

bool SVMatroxBuffer::empty() const
{
	return (nullptr == m_BufferPtr || m_BufferPtr->empty());
}

void SVMatroxBuffer::clear()
{
	m_BufferPtr.reset();
}

void SVMatroxBuffer::swap( SVMatroxBuffer& p_rBuf )
{
	std::swap(m_BufferPtr, p_rBuf.m_BufferPtr);
}

SVMatroxBuffer& SVMatroxBuffer::operator=( const SVMatroxBuffer& p_rBuf )
{
	if( this != &p_rBuf )
	{
		m_BufferPtr = p_rBuf.m_BufferPtr;
	}

	return *this;
}

__int64 SVMatroxBuffer::GetIdentifier() const
{
	__int64 l_Identifier = 0;

	if(nullptr != m_BufferPtr)
	{
		l_Identifier = m_BufferPtr->GetIdentifier();
	}

	return l_Identifier;
}

bool SVMatroxBuffer::IsParent( const SVMatroxBuffer& ) const
{
	return false;
}

