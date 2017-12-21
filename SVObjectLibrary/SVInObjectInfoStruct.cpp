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

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

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

void SVInObjectInfoStruct::SetInputObjectType(SvDef::SVObjectTypeEnum ObjectType, SvDef::SVObjectSubTypeEnum SubType, const SVGUID& rEmbeddedID)
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

void SVInObjectInfoStruct::SetInputName( const std::string& rInputName )
{
	m_InputName = rInputName;
}

