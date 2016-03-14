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
	if( ( m_ObjectInfo.EmbeddedID == SVInvalidGUID         || m_ObjectInfo.EmbeddedID == p_rElement.GetEmbeddedID() ) &&
		( m_ObjectInfo.ObjectType == SVNotSetObjectType    || m_ObjectInfo.ObjectType == p_rElement.GetObjectType() ) &&
		( m_ObjectInfo.SubType    == SVNotSetSubObjectType || m_ObjectInfo.SubType    == p_rElement.GetObjectSubType() )
	  )
	{
		if( m_ObjectInfo.EmbeddedID != SVInvalidGUID         ||
			m_ObjectInfo.ObjectType != SVNotSetObjectType    ||
			m_ObjectInfo.SubType    != SVNotSetSubObjectType
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

