//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGetObjectDequeByTypeVisitor
//* .File Name       : $Workfile:   SVGetObjectDequeByTypeVisitor.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:38:06  $
//******************************************************************************

#include "stdafx.h"
#include "SVGetObjectDequeByTypeVisitor.h"
#include "SVObjectLibrary/SVObjectClass.h"

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SVGetObjectDequeByTypeVisitor::SVGetObjectDequeByTypeVisitor( const SVObjectTypeInfoStruct& p_rObjectInfo )
: m_ObjectInfo( p_rObjectInfo ), m_Objects()
{
}

SVGetObjectDequeByTypeVisitor::~SVGetObjectDequeByTypeVisitor()
{
}

HRESULT SVGetObjectDequeByTypeVisitor::VisitElement( SVObjectClass& p_rElement )
{
	HRESULT l_Status = S_OK;

	// Find best match....EmbeddedID, Type, SubType...
	if( ( SV_GUID_NULL          == m_ObjectInfo.EmbeddedID || m_ObjectInfo.EmbeddedID == p_rElement.GetEmbeddedID() ) &&
		( SVNotSetObjectType    == m_ObjectInfo.ObjectType || m_ObjectInfo.ObjectType == p_rElement.GetObjectType() ) &&
		( SVNotSetSubObjectType == m_ObjectInfo.SubType    || m_ObjectInfo.SubType    == p_rElement.GetObjectSubType() )
	  )
	{
		if( SV_GUID_NULL          != m_ObjectInfo.EmbeddedID ||
			SVNotSetObjectType    != m_ObjectInfo.ObjectType ||
			SVNotSetSubObjectType != m_ObjectInfo.SubType 
		  )
		{
			m_Objects.push_back( &p_rElement );
		}
	}

	return l_Status;
}

const SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque& SVGetObjectDequeByTypeVisitor::GetObjects() const
{
	return m_Objects;
}

