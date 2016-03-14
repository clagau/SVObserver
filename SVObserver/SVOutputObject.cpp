//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOutputObject
//* .File Name       : $Workfile:   SVOutputObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:15:38  $
//******************************************************************************

#include "stdafx.h"
#include "SVOutputObject.h"

SVOutputObject::SVOutputObject( LPCSTR strObjectName )
				:SVObjectClass( strObjectName )
{
	m_bCreated = FALSE;
}

SVOutputObject::SVOutputObject( SVObjectClass* POwner, int StringResourceID )
				:SVObjectClass( POwner, StringResourceID )
{
	m_bCreated = FALSE;
}

SVOutputObject::~SVOutputObject()
{
	if( m_bCreated )
		Destroy();
}

BOOL SVOutputObject::Create()
{
	m_bCreated = TRUE;
	
	return TRUE;
}

BOOL SVOutputObject::Destroy()
{
	m_bCreated = FALSE;

	return TRUE;
}

BOOL SVOutputObject::IsCreated()
{
	return m_bCreated;
}

