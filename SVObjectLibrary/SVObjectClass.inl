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

#ifndef SVOBJECTCLASS_INL
#define SVOBJECTCLASS_INL

/*
This method returns the object's Global Unique Identifier (GUID) of this object instance.
*/
inline const SVGUID& SVObjectClass::GetUniqueObjectID() const
{
	return outObjectInfo.UniqueObjectID;
}

/*
This method returns the embedded Global Unique Identifier (GUID) if the object is embedded in another object.
*/
inline const SVGUID& SVObjectClass::GetEmbeddedID() const
{
	return embeddedID;
}

/*
This method returns the object's Global Unique Identifier (GUID) of this object's owner.
*/
inline const SVGUID& SVObjectClass::GetOwnerID() const
{
	return ownerObjectInfo.UniqueObjectID;
}

#pragma region virtual method (IObjectClass)
/*
This method returns the object's type.
*/
inline const SVObjectTypeEnum& SVObjectClass::GetObjectType() const
{
	return outObjectInfo.ObjectTypeInfo.ObjectType;
}
#pragma endregion virtual method (IObjectClass)

/*
This metthod return the pointer to the owner object.
*/
inline SVObjectClass* SVObjectClass::GetOwner() const
{
	return ownerObjectInfo.PObject;
}

/*
This method return the owner object info.
*/
inline const SVObjectInfoStruct& SVObjectClass::GetOwnerInfo() const
{
	return ownerObjectInfo;
}

/*
This method returns the object info of this object instance.
*/
inline const SVObjectInfoStruct& SVObjectClass::GetObjectInfo() const
{
	return outObjectInfo;
}

/*
This method returns the out object info of this object instance.  This method is only valid for data object's used as task output.
*/
inline SVOutObjectInfoStruct& SVObjectClass::GetObjectOutputInfo()
{
	return outObjectInfo;
}

/*
This method returns the object's image depth.
*/
inline long SVObjectClass::GetImageDepth() const
{
	return mlImageDepth;
}

/*
This method return the object's data depth.
*/
inline int SVObjectClass::GetObjectDepth() const
{
	return objectDepth;
}

/*
This method returns the creation state of the object.
*/
inline BOOL SVObjectClass::IsCreated() const
{
	return isCreated;
}

/*
This method returns the object's valid state.
*/
inline BOOL SVObjectClass::IsValid()
{
	return isObjectValid;
}

template< typename SVObjectVisitor >
HRESULT SVObjectClass::Accept( SVObjectVisitor& p_rVisitor )
{
	HRESULT l_Status = S_OK;

	l_Status = p_rVisitor.VisitElement( *this );

	return l_Status;
}

#endif

