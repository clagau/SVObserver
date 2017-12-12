//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVActiveXLockStruct
//* .File Name       : $Workfile:   SVActiveXLockStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:26:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVActiveXLockStruct.h"

SVActiveXLockStruct::SVActiveXLockStruct() :
m_ProcessCount( -1 ), m_ImageHandlePtr()
{
}

SVActiveXLockStruct::SVActiveXLockStruct( const SVActiveXLockStruct &rValue )
: m_Name( rValue.m_Name ), m_ProcessCount( rValue.m_ProcessCount ), m_ImageHandlePtr( rValue.m_ImageHandlePtr )
{
}

SVActiveXLockStruct::~SVActiveXLockStruct()
{
	clear();
}

const SVActiveXLockStruct& SVActiveXLockStruct::operator =(const SVActiveXLockStruct &rvalue)
{
	if( this != &rvalue )
	{
		m_Name = rvalue.m_Name;
		m_ProcessCount = rvalue.m_ProcessCount;
		m_ImageHandlePtr = rvalue.m_ImageHandlePtr;
	}

	return *this;
}

void SVActiveXLockStruct::clear()
{
	m_Name.clear();
	m_ProcessCount = -1;
	m_ImageHandlePtr.reset();
}

BOOL SVActiveXLockStruct::Valid()
{
	if (m_Name.empty() || nullptr == m_ImageHandlePtr || m_ImageHandlePtr->empty())
	{
		return false;
	}
	return true;
}

