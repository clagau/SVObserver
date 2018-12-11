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

#pragma region Includes
#include "stdafx.h"
#include "SVOutputObject.h"
#pragma endregion Includes

SVOutputObject::SVOutputObject( LPCSTR strObjectName )
				:SVObjectClass( strObjectName )
{
	m_isCreated = false;
}

SVOutputObject::SVOutputObject( SVObjectClass* POwner, int StringResourceID )
				:SVObjectClass( POwner, StringResourceID )
{
	m_isCreated = false;
}

SVOutputObject::~SVOutputObject()
{
	if( m_isCreated )
	{
		Destroy();
	}
}

bool SVOutputObject::Create()
{
	m_isCreated = true;
	
	return m_isCreated;
}

bool SVOutputObject::Destroy()
{
	m_isCreated = false;

	return !m_isCreated;
}
