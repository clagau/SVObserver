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
#pragma region Includes
#include "stdafx.h"
#include "SVMatroxBufferArray.h"
#include "SVMatroxBufferCreateStruct.h"
#include "SVMatroxBufferInterface.h"
#include "SVMatroxSystem.h"
#pragma endregion Includes

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

HRESULT SVMatroxBufferArray::Create(const SVMatroxSystem& rSystem, const SVMatroxBufferCreateStruct& p_rCreateStruct, long count)
{
	LocalClear();
	
	HRESULT Result(S_OK);
	m_BufferIdentifierList.resize(count);	// reserve storage

	SVMatroxBufferInterface l_Interface;
	SVMatroxBuffer l_Buffer;
	for (int i = 0;i < count;i++)
	{
		HRESULT MatroxCode = l_Interface.Create(rSystem, l_Buffer, p_rCreateStruct);
		if (S_OK == MatroxCode)
		{
			m_BufferIdentifierList[i] = l_Buffer;
		}
		else
		{
			Result = MatroxCode;
			break;
		}
	}
	
	return Result;
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

