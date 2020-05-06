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

HRESULT SVCameraDataInputObject::Read(_variant_t& rValue) const
{
	HRESULT l_Status = S_OK;

	rValue = 0L;

	return l_Status;
}

void SVCameraDataInputObject::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVIoObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVCameraDataInputObjectType;
}
