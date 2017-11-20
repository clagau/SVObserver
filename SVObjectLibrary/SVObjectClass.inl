//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObjectClass
//* .File Name       : $Workfile:   SVObjectClass.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   19 Dec 2014 02:45:14  $
//******************************************************************************

#pragma once

/*
This method returns the object's Global Unique Identifier (GUID) of this object instance.
*/
inline const SVGUID& SVObjectClass::GetUniqueObjectID() const
{
	return m_outObjectInfo.m_UniqueObjectID;
}

/*
This method returns the embedded Global Unique Identifier (GUID) if the object is embedded in another object.
*/
inline const SVGUID& SVObjectClass::GetEmbeddedID() const
{
	return m_embeddedID;
}

/*
This method returns the object's Global Unique Identifier (GUID) of this object's owner.
*/
inline const SVGUID& SVObjectClass::GetOwnerID() const
{
	return m_ownerObjectInfo.m_UniqueObjectID;
}

#pragma region virtual method (IObjectClass)
/*
This method returns the object's type.
*/
inline const SvDef::SVObjectTypeEnum& SVObjectClass::GetObjectType() const
{
	return m_outObjectInfo.m_ObjectTypeInfo.ObjectType;
}
#pragma endregion virtual method (IObjectClass)

/*
This metthod return the pointer to the owner object.
*/
inline SVObjectClass* SVObjectClass::GetOwner() const
{
	return m_ownerObjectInfo.m_pObject;
}

/*
This method return the owner object info.
*/
inline const SVObjectInfoStruct& SVObjectClass::GetOwnerInfo() const
{
	return m_ownerObjectInfo;
}

/*
This method returns the object info of this object instance.
*/
inline const SVObjectInfoStruct& SVObjectClass::GetObjectInfo() const
{
	return m_outObjectInfo;
}

/*
This method returns the out object info of this object instance.  This method is only valid for data object's used as task output.
*/
inline SVOutObjectInfoStruct& SVObjectClass::GetObjectOutputInfo()
{
	return m_outObjectInfo;
}

/*
This method returns the object's image depth.
*/
inline long SVObjectClass::GetImageDepth() const
{
	return m_lImageDepth;
}

/*
This method return the object's data depth.
*/
inline int SVObjectClass::GetObjectDepth() const
{
	return m_objectDepth;
}

/*
This method returns the creation state of the object.
*/
inline bool SVObjectClass::IsCreated() const
{
	return m_isCreated;
}

template< typename SVObjectVisitor >
HRESULT SVObjectClass::Accept( SVObjectVisitor& rVisitor )
{
	HRESULT l_Status = S_OK;

	l_Status = rVisitor.VisitElement( *this );

	return l_Status;
}
