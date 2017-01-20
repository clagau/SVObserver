//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObjectInterfaceInputRequestStruct
//* .File Name       : $Workfile:   SVTaskObjectInterfaceInputRequestStruct.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 11:34:06  $
//******************************************************************************

#include "SVUtilityLibrary/SVUtilityGlobals.h"

inline SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct()
{
	m_Guid = SV_GUID_NULL;
}

inline SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( const SVValueObjectReference& rObjectRef, const SVGUID& rGuid, const SVString& rName )
{
	m_ObjectRef = rObjectRef;
	m_Guid = rGuid;
	m_Name = rName;
}

inline SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( const SVValueObjectReference& rObjectRef )
{
	m_ObjectRef = rObjectRef;
	if ( m_ObjectRef.Object() )
	{
		m_Guid = m_ObjectRef.Guid();
		m_Name = m_ObjectRef.GetCompleteName();
	}
}

inline bool SVTaskObjectInterfaceInputRequestStruct::operator < ( const SVTaskObjectInterfaceInputRequestStruct& rhs ) const
{
	if ( m_ObjectRef.Object() && rhs.m_ObjectRef.Object() )
	{
		return m_ObjectRef < rhs.m_ObjectRef;
	}
	else if ( SV_GUID_NULL != m_Guid && SV_GUID_NULL != rhs.m_Guid )
	{
		return m_Guid < rhs.m_Guid;
	}
	else
	{
		return m_Name < rhs.m_Name;
	}
}

