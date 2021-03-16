//*****************************************************************************
/// \copyright (c) 2020,2020 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \Author	Marc Ziegler
//*****************************************************************************
/// This class should managed the inputs of a task.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "InputObject.h"
#include "SVObjectClass.h"
#include "SVObjectLibrary.h"
#include "SVObjectManagerClass.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVToolsetScriptTags.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvOl
{
	SV_IMPLEMENT_CLASS(InputObject, SvPb::InputConnectedClassId);

	InputObject::InputObject(LPCSTR objectName)
		: SVObjectClass(objectName)
	{
		m_ObjectTypeInfo.m_ObjectType = SvPb::InputConnectedType;
		SetObjectAttributesAllowed(SvPb::audittrail | SvPb::embedable, SvOi::SetAttributeType::OverwriteAttribute);
	}

	InputObject::InputObject(SVObjectClass* pOwner, int stringResourceID)
		: SVObjectClass(pOwner, stringResourceID)
	{
		m_ObjectTypeInfo.m_ObjectType = SvPb::InputConnectedType;
		SetObjectAttributesAllowed(SvPb::audittrail | SvPb::embedable, SvOi::SetAttributeType::OverwriteAttribute);
	}


	InputObject::~InputObject()
	{
	}

const SVObjectInfoStruct& InputObject::GetInputObjectInfo() const
{
	return m_InputObjectInfo;
}

bool InputObject::IsConnected() const
{
	return m_IsConnected;
}

void InputObject::SetInputObjectType(SvPb::SVObjectTypeEnum ObjectType, SvPb::SVObjectSubTypeEnum SubType, SvPb::EmbeddedIdEnum embeddedID)
{
	SetInputObjectType( SvDef::SVObjectTypeInfoStruct( ObjectType, SubType, embeddedID) );
}

void InputObject::SetInputObjectType( const SvDef::SVObjectTypeInfoStruct& rTypeInfo )
{
	m_IsConnected = false;
	m_InputObjectInfo.SetObject( rTypeInfo );
}

void InputObject::SetInputObject(uint32_t objectID )
{
	SetInputObject(SVObjectReference(objectID));
}

void InputObject::SetInputObject( SVObjectClass* pObject )
{
	SetInputObject(SVObjectReference(pObject));
}

void InputObject::SetInputObject( const SVObjectReference& rObject )
{
	uint32_t oldObjectId = m_InputObjectInfo.getObjectId();

	bool shouldNewConnect = rObject.getObjectId() != oldObjectId || (SvDef::InvalidObjectId != rObject.getObjectId() && nullptr == m_InputObjectInfo.getObject());
	if (shouldNewConnect)
	{
		auto* pOldObject = SVObjectManagerClass::Instance().GetObject(oldObjectId);
		if (pOldObject)
		{
			pOldObject->disconnectObject(getObjectId());
		}
	}

	m_InputObjectInfo.SetObject(rObject);

	auto* pNewObject = m_InputObjectInfo.getObject();
	if (pNewObject)
	{
		m_IsConnected = true;
		if (shouldNewConnect)
		{
			pNewObject->connectObject(getObjectId());
			correctDependencies();
		}
	}
	else
	{
		m_IsConnected = false;
	}
}

void InputObject::validateInput()
{
	// Check if the input object is still valid otherwise the pointer is invalid
	if (IsConnected() && (false == m_InputObjectInfo.CheckExistence() || false == checkIfValidDependency(m_InputObjectInfo.getObject())))
	{
		SetInputObject(nullptr);
	}

	//Check if input object and owner object on the same inspection
	SVObjectClass* pInputObject{ GetInputObjectInfo().m_ObjectRef.getObject() };
	if (nullptr != pInputObject)
	{
		SVObjectClass* pOwnerInspection = GetAncestor(SvPb::SVInspectionObjectType);
		SVObjectClass* pInputInspection = pInputObject->GetAncestor(SvPb::SVInspectionObjectType);
		if (nullptr != pOwnerInspection && nullptr != pInputInspection)
		{
			if (pOwnerInspection->getObjectId() != pInputInspection->getObjectId())
			{
				SetInputObject(nullptr);
			}
		}
	}
}

void InputObject::disconnectObjectInput(uint32_t objectId)
{
	if (GetInputObjectInfo().getObjectId() == objectId)
	{
		SetInputObject(nullptr);
		__super::disconnectObjectInput(objectId);
	}
}

void InputObject::disconnectAllInputs() 
{ 
	__super::disconnectAllInputs();
	SetInputObject(nullptr); 
}

UINT InputObject::SetObjectAttributesAllowed(UINT Attributes, SvOi::SetAttributeType Type)
{
	auto tmpValue = m_ObjectAttributesAllowed;
	auto retValue = __super::SetObjectAttributesAllowed(Attributes, Type);
	if (tmpValue != m_ObjectAttributesAllowed)
	{
		correctDependencies();
	}
	return retValue;
}

void InputObject::Persist(SvOi::IObjectWriter& rWriter)
{
	//@TODO[MZA][10.10][10.12.2020] wie kann man das wieder reinnehmen?
	//if (SvPb::noAttributes == ObjectAttributesAllowed())
	//{
	//	return;
	//}

	uint32_t connectId{ GetInputObjectInfo().getObjectId() };
	if (SvDef::InvalidObjectId == connectId)
	{
		return;
	}
	rWriter.StartElement(GetObjectName()); // use internal name for node name

	__super::Persist(rWriter);

	_variant_t value = convertObjectIdToVariant( connectId );
	rWriter.WriteAttribute(scConntectedIDTag, value);

	rWriter.EndElement();
}

void InputObject::correctDependencies()
{
	if (IsConnected() && m_InputObjectInfo.getObject())
	{
		if (SvPb::noAttributes == ObjectAttributesAllowed())
		{
			SVObjectManagerClass::Instance().disconnectDependency(m_InputObjectInfo.getObject()->getObjectId(), getObjectId(), SvOl::JoinType::Dependent);
		}
		else
		{
			SVObjectManagerClass::Instance().connectDependency(m_InputObjectInfo.getObject()->getObjectId(), getObjectId(), SvOl::JoinType::Dependent);
		}
	}
}
} //namespace SvOl