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
This method returns the object's Global Unique Identifier of this object instance.
*/
inline uint32_t SVObjectClass::getObjectId() const
{
	return m_outObjectInfo.getObjectId();
}

/*
This method returns the embedded Identifier if the object is embedded in another object.
*/
inline SvPb::EmbeddedIdEnum SVObjectClass::GetEmbeddedID() const
{
	return m_embeddedID;
}

/*
This method returns the object's Global Unique Identifier of this object's parent
*/
inline uint32_t SVObjectClass::GetParentID() const
{
	return m_ownerObjectInfo.getObjectId();
}

#pragma region virtual method (IObjectClass)
/*
This method returns the object's type.
*/
inline const SvPb::SVObjectTypeEnum& SVObjectClass::GetObjectType() const
{
	return m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType;
}
#pragma endregion virtual method (IObjectClass)

/*
This method return the pointer to the owner object.
*/
inline SVObjectClass* SVObjectClass::GetParent() const
{
	return m_ownerObjectInfo.getObject();
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
