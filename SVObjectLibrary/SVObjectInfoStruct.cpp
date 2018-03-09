//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVObjectLibrary.h"
#include "SVObjectManagerClass.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVObjectInfoStruct::SVObjectInfoStruct()
{
}

SVObjectInfoStruct::SVObjectInfoStruct( const SVObjectInfoStruct& rObjectInfo )
: m_ObjectRef( rObjectInfo.m_ObjectRef )
, m_ObjectTypeInfo( rObjectInfo.m_ObjectTypeInfo )
{
}

SVObjectInfoStruct::SVObjectInfoStruct( const SVObjectReference& rObjectRef )
: m_ObjectRef(rObjectRef)
, m_ObjectTypeInfo()
{
}

SVObjectInfoStruct::~SVObjectInfoStruct()
{
}

const SVObjectInfoStruct& SVObjectInfoStruct::operator = ( const SVObjectInfoStruct& rObjectInfo )
{
	if( this != &rObjectInfo )
	{
		m_ObjectRef = rObjectInfo.m_ObjectRef;
		m_ObjectTypeInfo = rObjectInfo.m_ObjectTypeInfo;
	}
	return *this;
}

void SVObjectInfoStruct::clear()
{
	m_ObjectRef.clear();
	m_ObjectTypeInfo = SvDef::SVObjectTypeInfoStruct();
}

HRESULT SVObjectInfoStruct::SetObject( const SVGUID& rObjectID )
{
	HRESULT l_hrOk = S_OK;

	if( rObjectID != m_ObjectRef.Guid() )
	{
		m_ObjectRef = SVObjectReference(rObjectID);
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
		m_ObjectRef = SVObjectReference{ pObject };
		m_ObjectTypeInfo = pObject->GetObjectInfo().m_ObjectTypeInfo;
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
	bool Result = (GUID_NULL != m_ObjectRef.Guid());

	if(Result)
	{
		const SVObjectClass* const pObject = SVObjectManagerClass::Instance().GetObject(m_ObjectRef.Guid());

		Result = (pObject == m_ObjectRef.getObject());
	}

	return Result;
}

