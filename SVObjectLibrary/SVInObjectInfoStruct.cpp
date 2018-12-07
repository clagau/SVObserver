//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInObjectInfoStruct
//* .File Name       : $Workfile:   SVInObjectInfoStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:35:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVInObjectInfoStruct.h"
#include "SVObjectClass.h"
#include "SVObjectManagerClass.h"

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvOl
{
SVInObjectInfoStruct::SVInObjectInfoStruct()
: SVObjectInfoStruct(), m_InputObjectInfo(), m_IsConnected( false ), m_InputName()
{
}

SVInObjectInfoStruct::SVInObjectInfoStruct( const SVInObjectInfoStruct& rSrc )
: SVObjectInfoStruct( rSrc ), m_InputObjectInfo( rSrc.m_InputObjectInfo ), m_IsConnected( rSrc.m_IsConnected ), m_InputName( rSrc.m_InputName )
{
}

SVInObjectInfoStruct::~SVInObjectInfoStruct()
{
}

const SVInObjectInfoStruct& SVInObjectInfoStruct::operator = ( const SVInObjectInfoStruct& rhs )
{
	if ( this != &rhs )
	{
		SVObjectInfoStruct::operator =( rhs );

		m_InputObjectInfo = rhs.m_InputObjectInfo;
		m_IsConnected = rhs.m_IsConnected;
		m_InputName = rhs.m_InputName;
	}
	return *this;
}

const SVObjectInfoStruct& SVInObjectInfoStruct::GetInputObjectInfo() const
{
	return m_InputObjectInfo;
}

bool SVInObjectInfoStruct::IsConnected() const
{
	return m_IsConnected;
}

void SVInObjectInfoStruct::SetInputObjectType(SvPb::SVObjectTypeEnum ObjectType, SvPb::SVObjectSubTypeEnum SubType, const SVGUID& rEmbeddedID)
{
	SetInputObjectType( SvDef::SVObjectTypeInfoStruct( ObjectType, SubType, rEmbeddedID) );
}

void SVInObjectInfoStruct::SetInputObjectType( const SvDef::SVObjectTypeInfoStruct& rTypeInfo )
{
	m_IsConnected = false;
	m_InputObjectInfo.SetObject( rTypeInfo );
}

void SVInObjectInfoStruct::SetInputObject( const SVGUID& rObjectID )
{
	if( rObjectID != m_InputObjectInfo.getUniqueObjectID() )
	{
		m_IsConnected = false;
		m_InputObjectInfo.SetObject( rObjectID );
	}
}

void SVInObjectInfoStruct::SetInputObject( SVObjectClass* pObject )
{
	m_InputObjectInfo.SetObject( pObject );
	m_IsConnected = ( nullptr != m_InputObjectInfo.getObject() );
}

void SVInObjectInfoStruct::SetInputObject( const SVObjectReference& rObject )
{
	m_InputObjectInfo.SetObject(rObject);
	m_IsConnected = (nullptr != m_InputObjectInfo.getObject());
}

const std::string& SVInObjectInfoStruct::GetInputName() const
{
	return m_InputName;
}

void SVInObjectInfoStruct::SetInputName(const std::string& rInputName)
{
	m_InputName = rInputName;
}

void ValidateInput(SVInObjectInfoStruct& rInputObject)
{
	// Check if the input object is still valid otherwise the pointer is invalid
	if (rInputObject.IsConnected() && !rInputObject.GetInputObjectInfo().CheckExistence())
	{
		rInputObject.SetInputObject(nullptr);
	}
	
	//Check if input object and owner object on the same inspection
	SVObjectClass* pOwnerObject{rInputObject.m_ObjectRef.getObject()};
	SVObjectClass* pInputObject {rInputObject.GetInputObjectInfo().m_ObjectRef.getObject()};
	if (nullptr != pOwnerObject && nullptr != pInputObject)
	{
		SVObjectClass* pOwnerInspection = pOwnerObject->GetAncestor(SvDef::SVInspectionObjectType);
		SVObjectClass* pInputInspection = pInputObject->GetAncestor(SvDef::SVInspectionObjectType);
		if (nullptr != pOwnerInspection && nullptr != pInputInspection)
		{
			if (pOwnerInspection->GetUniqueObjectID() != pInputInspection->GetUniqueObjectID())
			{
				rInputObject.SetInputObject(nullptr);
			}
		}
	}
}

void ValidateInputList(SVInObjectInfoStructPtrVector& rInputObjectList)
{
	for(auto pEntry : rInputObjectList)
	{
		if(nullptr != pEntry)
		{
			ValidateInput(*pEntry);
		}
	}
}
} //namespace SvOl