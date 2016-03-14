//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxBufferArray
//* .File Name       : $Workfile:   SVMatroxBufferArray.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:59:58  $
//******************************************************************************
#include "stdafx.h"
#include "SVMatroxBufferArray.h"
#include "SVMatroxBufferInterface.h"

SVMatroxBufferArray::SVMatroxBufferArray()
{
}

SVMatroxBufferArray::SVMatroxBufferArray(const SVMatroxBufferArray& p_rBufArray)
{
	m_BufferIdentifierList = p_rBufArray.m_BufferIdentifierList;
}

SVMatroxBufferArray::~SVMatroxBufferArray()
{
	// Destroy(); // can this be called here or is the Framework too far gone at this point ?
}

SVMatroxBufferArray::SVStatusCode SVMatroxBufferArray::Create(const SVMatroxSystem& rSystem, const SVMatroxBufferCreateStruct& p_rCreateStruct, long count)
{
	LocalClear();
	
	SVStatusCode statusCode( SVMEE_STATUS_OK );
	m_BufferIdentifierList.resize(count);	// reserve storage

	SVMatroxBufferInterface l_Interface;
	SVMatroxBuffer l_Buffer;
	for (int i = 0;i < count;i++)
	{
		SVMatroxBufferInterface::SVStatusCode l_Code = l_Interface.Create(rSystem, l_Buffer, p_rCreateStruct);
		if (l_Code == SVMEE_STATUS_OK)
		{
			m_BufferIdentifierList[i] = l_Buffer;
		}
		else
		{
			statusCode = l_Code;
			break;
		}
	}
	
	return statusCode;
}

void SVMatroxBufferArray::Destroy()
{
	LocalClear();
}

bool SVMatroxBufferArray::empty() const
{
	return (m_BufferIdentifierList.size()) ? false : true;
}

SVMatroxBufferArray::BufferIDList SVMatroxBufferArray::GetList() const
{
	BufferIDList l_List( m_BufferIdentifierList.size() );

	for( size_t i = 0; i < m_BufferIdentifierList.size(); ++i )
	{
		l_List[ i ] = m_BufferIdentifierList[ i ].GetIdentifier();
	}

	return l_List;
}

void SVMatroxBufferArray::LocalClear()
{
	m_BufferIdentifierList.clear();		// release elements
	m_BufferIdentifierList.resize(0);	// release storage
}

void SVMatroxBufferArray::swap( SVMatroxBufferArray& p_rBufArray )
{
	BufferList l_IDList = m_BufferIdentifierList;
	m_BufferIdentifierList = p_rBufArray.m_BufferIdentifierList;
	p_rBufArray.m_BufferIdentifierList = l_IDList;
}

SVMatroxBufferArray& SVMatroxBufferArray::operator=( const SVMatroxBufferArray& p_rBufArray )
{
	m_BufferIdentifierList = p_rBufArray.m_BufferIdentifierList;

	return *this;
}

