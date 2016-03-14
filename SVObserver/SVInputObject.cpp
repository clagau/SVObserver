//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInputObject
//* .File Name       : $Workfile:   SVInputObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:59:26  $
//******************************************************************************

#include "stdafx.h"
#include "SVInputObject.h"

SVInputObject::SVInputObject( LPCSTR strObjectName )
				:SVObjectClass( strObjectName )
{
	m_bCreated = FALSE;
}

SVInputObject::SVInputObject( SVObjectClass* POwner, int StringResourceID )
				:SVObjectClass( POwner, StringResourceID )
{
	m_bCreated = FALSE;
}

SVInputObject::~SVInputObject()
{
	if( m_bCreated )
		Destroy();
}

BOOL SVInputObject::Create()
{
	m_bCreated = TRUE;

	return TRUE;
}

BOOL SVInputObject::Destroy()
{
	m_bCreated = FALSE;

	return TRUE;
}

BOOL SVInputObject::IsCreated()
{
	return m_bCreated;
}

