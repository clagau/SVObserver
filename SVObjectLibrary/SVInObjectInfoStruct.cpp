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
void SVInObjectInfoStruct::SetInputObjectType( SVObjectTypeEnum p_ObjectType, long p_SubType )
{
	SetInputObjectType( SVObjectTypeInfoStruct( p_ObjectType, p_SubType ) );
}

void SVInObjectInfoStruct::SetInputObjectType( const SVGUID& p_rEmbeddedID, SVObjectTypeEnum p_ObjectType, long p_SubType )
{
	SetInputObjectType( SVObjectTypeInfoStruct( p_rEmbeddedID, p_ObjectType, p_SubType ) );
}

void SVInObjectInfoStruct::SetInputObjectType( const SVObjectTypeInfoStruct& p_rTypeInfo )
{
	m_IsConnected = false;
	m_InputObjectInfo.SetObject( p_rTypeInfo );
}

void SVInObjectInfoStruct::SetInputObject( const SVString& p_rName )
{
	if( p_rName != m_InputObjectInfo.GetObjectReference().GetCompleteName() )
	{
		m_IsConnected = false;
		m_InputObjectInfo.SetObject( p_rName );
	}
}

void SVInObjectInfoStruct::SetInputObject( const SVGUID& p_rObjectID )
{
	if( p_rObjectID != m_InputObjectInfo.m_UniqueObjectID )
	{
		m_IsConnected = false;
		m_InputObjectInfo.SetObject( p_rObjectID );
	}
}

void SVInObjectInfoStruct::SetInputObject( SVObjectClass* p_pObject )
{
	m_InputObjectInfo.SetObject( p_pObject );

	m_IsConnected = ( nullptr != m_InputObjectInfo.m_pObject );
}

void SVInObjectInfoStruct::SetInputObject( const SVObjectReference& p_rObject )
{
	m_InputObjectInfo.SetObject( p_rObject );

	m_IsConnected = ( nullptr != m_InputObjectInfo.m_pObject );
}

const SVString& SVInObjectInfoStruct::GetInputName() const
{
	return m_InputName;
}

void SVInObjectInfoStruct::SetInputName( const SVString& p_rInputName )
{
	m_InputName = p_rInputName;
}

