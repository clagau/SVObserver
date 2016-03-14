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

SVActiveXLockStruct::SVActiveXLockStruct()
: strName( _T( "" ) ), lProcessCount( -1 ), m_ImageHandlePtr()
{
}

SVActiveXLockStruct::SVActiveXLockStruct( const SVActiveXLockStruct &rvalue )
: strName( rvalue.strName ), lProcessCount( rvalue.lProcessCount ), m_ImageHandlePtr( rvalue.m_ImageHandlePtr )
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
		strName = rvalue.strName;
		lProcessCount = rvalue.lProcessCount;
		m_ImageHandlePtr = rvalue.m_ImageHandlePtr;
	}

	return *this;
}

void SVActiveXLockStruct::clear()
{
	strName.Empty();
	lProcessCount = -1;
	m_ImageHandlePtr.clear();
}

BOOL SVActiveXLockStruct::Valid()
{
	if ( strName.IsEmpty() || m_ImageHandlePtr.empty() || m_ImageHandlePtr->empty() )
		return FALSE;
	else
		return TRUE;
}

