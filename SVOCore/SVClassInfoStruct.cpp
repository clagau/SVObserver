//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVClassInfoStruct
//* .File Name       : $Workfile:   SVClassInfoStruct.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:53:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVClassInfoStruct.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "ObjectInterfaces/ITaskObject.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVClassInfoStruct
////////////////////////////////////////////////////////////////////////////////
SVClassInfoStruct::SVClassInfoStruct()
{
	m_ClassId = SV_GUID_NULL;
}

SVClassInfoStruct::SVClassInfoStruct( const SVClassInfoStruct& rRhs )
{
	m_ClassId					= rRhs.m_ClassId;
	m_ClassName				= rRhs.m_ClassName;
	m_ObjectTypeInfo			= rRhs.m_ObjectTypeInfo;
	m_DesiredInputInterface	= rRhs.m_DesiredInputInterface;
}

SVClassInfoStruct::~SVClassInfoStruct()
{
}

SVClassInfoStruct SVClassInfoStruct::operator=( const SVClassInfoStruct& rRhs )
{
	m_ClassId					= rRhs.m_ClassId;
	m_ClassName				= rRhs.m_ClassName;
	m_ObjectTypeInfo			= rRhs.m_ObjectTypeInfo;
	m_DesiredInputInterface	= rRhs.m_DesiredInputInterface;
	return( *this );
}

SVObjectClass* SVClassInfoStruct::Construct()
{
	SVObjectClass* pObject = nullptr;

	SVObjectManagerClass::Instance().ConstructObject( m_ClassId, pObject );

	if( nullptr != pObject )
	{
		pObject->SetName( m_ClassName.c_str() );

		if( SvOi::ITaskObject* pTaskObject = dynamic_cast<SvOi::ITaskObject*>(pObject) )
		{
			pTaskObject->ResolveDesiredInputs(m_DesiredInputInterface);
		}
	}
	return pObject;
}

SVClassInfoStructListClass::SVClassInfoStructListClass()
{
}

SVClassInfoStructListClass::~SVClassInfoStructListClass() 
{
	RemoveAll();
}

int SVClassInfoStructListClass::Find( const SVGUID& rGuid ) 
{ 
	for( int i = 0; i < GetSize(); i++ )
	{
		if( rGuid == GetAt( i ).m_ClassId )
		{
			return i;
		}
	}
	return -1;
}

