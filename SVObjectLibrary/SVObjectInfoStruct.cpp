//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectInfoStruct
//* .File Name       : $Workfile:   SVObjectInfoStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:50:34  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObjectInfoStruct.h"
#include "SVObjectClass.h"
#include "SVObjectManagerClass.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVObjectInfoStruct::SVObjectInfoStruct( const SVObjectReference& rObjectRef )
: m_ObjectRef(rObjectRef)
, m_ObjectTypeInfo()
{
}

SVObjectInfoStruct::~SVObjectInfoStruct()
{
}

void SVObjectInfoStruct::clear()
{
	m_ObjectRef.clear();
	m_ObjectTypeInfo = SvDef::SVObjectTypeInfoStruct();
}

HRESULT SVObjectInfoStruct::SetObject(uint32_t objectID)
{
	HRESULT l_hrOk = S_OK;

	if( objectID != m_ObjectRef.getObjectId() || nullptr == m_ObjectRef.getObject() )
	{
		m_ObjectRef = SVObjectReference(objectID);
	}
	return l_hrOk;
}

HRESULT SVObjectInfoStruct::SetObject( const SvDef::SVObjectTypeInfoStruct& rTypeInfo )
{
	HRESULT l_hrOk = S_OK;
	clear();
	m_ObjectTypeInfo = rTypeInfo;
	return l_hrOk;
}

HRESULT SVObjectInfoStruct::SetObject( SVObjectClass* pObject )
{
	HRESULT l_hrOk = S_OK;

	if( nullptr != pObject )
	{
		SetObject(SVObjectReference{ pObject });		
	}
	else
	{
		m_ObjectRef.clear();
	}
	return l_hrOk;
}

HRESULT SVObjectInfoStruct::SetObject( const SVObjectReference& rObjectRef )
{
	m_ObjectRef = rObjectRef;
	if (SvPb::SVNotSetObjectType == m_ObjectTypeInfo.m_ObjectType &&
		SvPb::SVNotSetSubObjectType == m_ObjectTypeInfo.m_SubType && nullptr != m_ObjectRef.getFinalObject())
	{
		m_ObjectTypeInfo = m_ObjectRef.getFinalObject()->getObjectTypeInfo();
	}
	return S_OK;
}

HRESULT SVObjectInfoStruct::SetObject( const SVObjectInfoStruct& rObjectInfo )
{
	HRESULT l_hrOk = S_OK;

	if( this != &rObjectInfo )
	{
		m_ObjectRef = rObjectInfo.m_ObjectRef;
		m_ObjectTypeInfo = rObjectInfo.m_ObjectTypeInfo;
	}
	return l_hrOk;
}

bool SVObjectInfoStruct::operator == ( const SVObjectInfoStruct& rRhs ) const
{
	return (m_ObjectRef == rRhs.m_ObjectRef);
}

bool SVObjectInfoStruct::operator == ( const SVObjectReference& rRhs ) const
{
	return (m_ObjectRef == rRhs);
}

bool SVObjectInfoStruct::CheckExistence() const
{
	bool Result = (SvDef::InvalidObjectId != m_ObjectRef.getObjectId());

	if(Result)
	{
		const SVObjectClass* const pObject = SVObjectManagerClass::Instance().GetObject(m_ObjectRef.getObjectId());

		Result = (pObject == m_ObjectRef.getObject());
	}

	return Result && nullptr != m_ObjectRef.getFinalObject();
}