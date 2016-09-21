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
	guid = SV_GUID_NULL;
}

inline SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( const SVValueObjectReference& p_ref, GUID p_guid, const CString& p_strName )
{
	ref = p_ref;
	guid = p_guid;
	strName = p_strName;
}

inline SVTaskObjectInterfaceInputRequestStruct::SVTaskObjectInterfaceInputRequestStruct( const SVValueObjectReference& p_ref )
{
	ref = p_ref;
	if ( ref.Object() )
	{
		guid = ref.Guid();
		strName = ref.GetCompleteObjectName();
	}
}

inline bool SVTaskObjectInterfaceInputRequestStruct::operator < ( const SVTaskObjectInterfaceInputRequestStruct& rhs ) const
{
	if ( ref.Object() && rhs.ref.Object() )
	{
		return ref < rhs.ref;
	}
	else if ( SV_GUID_NULL != guid && SV_GUID_NULL != rhs.guid )
	{
		return guid < rhs.guid;
	}
	else
	{
		return strName < rhs.strName;
	}
}

