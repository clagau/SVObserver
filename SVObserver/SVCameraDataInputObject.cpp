//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraDataInputObject
//* .File Name       : $Workfile:   SVCameraDataInputObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:44:38  $
//******************************************************************************

#include "stdafx.h"
#include "SVCameraDataInputObject.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"

SVCameraDataInputObject::SVCameraDataInputObject(LPCSTR strObjectName)
: SVInputObject(strObjectName)
{
	LocalInitialize();
}

SVCameraDataInputObject::SVCameraDataInputObject(SVObjectClass *pOwner, int StringResourceID)
: SVInputObject(pOwner, StringResourceID)
{
	LocalInitialize();
}

SVCameraDataInputObject::~SVCameraDataInputObject()
{
	Destroy();
}

HRESULT SVCameraDataInputObject::Read( _variant_t& p_rValue )
{
	HRESULT l_Status = S_OK;
	return l_Status;
}

void SVCameraDataInputObject::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVIoObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVCameraDataInputObjectType;
}
