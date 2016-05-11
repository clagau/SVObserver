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

#pragma region Includes
#include "stdafx.h"
#include "SVInputObject.h"
#pragma endregion Includes

SVInputObject::SVInputObject( LPCSTR strObjectName )
				:SVObjectClass( strObjectName )
{
	m_isCreated = false;
}

SVInputObject::SVInputObject( SVObjectClass* POwner, int StringResourceID )
				:SVObjectClass( POwner, StringResourceID )
{
	m_isCreated = false;
}

SVInputObject::~SVInputObject()
{
	if( m_isCreated )
	{
		Destroy();
	}
}

BOOL SVInputObject::Create()
{
	m_isCreated = true;

	return m_isCreated;
}

BOOL SVInputObject::Destroy()
{
	m_isCreated = false;

	return !m_isCreated;
}
