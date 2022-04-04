//*****************************************************************************
/// \copyright (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "ObjectInterfaces/ITaskObject.h"
#include "ObjectInterfaces/ITool.h"
#include "SVToolsetScriptTags.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace
{
bool checkType(const SvDef::SVObjectTypeInfoStruct& rRequiredType, const SvDef::SVObjectTypeInfoStruct& rTestType)
{
	return ((SvPb::NoEmbeddedId == rRequiredType.m_EmbeddedID || rTestType.m_EmbeddedID == rRequiredType.m_EmbeddedID) &&
			(SvPb::SVNotSetObjectType == rRequiredType.m_ObjectType || rTestType.m_ObjectType == rRequiredType.m_ObjectType) &&
			(SvPb::SVNotSetSubObjectType == rRequiredType.m_SubType || rTestType.m_SubType == rRequiredType.m_SubType));
}
}

namespace SvOl
{
	SV_IMPLEMENT_CLASS(InputObject, SvPb::InputConnectedClassId);

	InputObject::InputObject(LPCSTR objectName)
		: SVObjectAppClass(objectName)
	{
		m_ObjectTypeInfo.m_ObjectType = SvPb::InputConnectedType;
		SetObjectAttributesAllowed(SvPb::audittrail | SvPb::embedable, SvOi::SetAttributeType::OverwriteAttribute);
	}

	InputObject::InputObject(SVObjectClass* pOwner, int stringResourceID)
		: SVObjectAppClass(pOwner, stringResourceID)
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

HRESULT InputObject::checkAndSetInput(uint32_t connnectedInput)
{
	SVObjectReference refObject {connnectedInput};
	if (nullptr != refObject.getFinalObject())
	{
		if (checkType(m_InputObjectInfo.m_ObjectTypeInfo, refObject.getFinalObject()->getObjectTypeInfo()))
		{
			SetInputObject(refObject);
			return S_OK;
		}
	}
	return E_FAIL;
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
	if (IsConnected() && 
			(false == m_InputObjectInfo.CheckExistence() || 
			false == checkIfAllowedObject() || 
			false == checkIfValidDependency(m_InputObjectInfo.getObject()))
		)
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

std::tuple<SvDef::SVObjectTypeInfoStruct, uint32_t, SvOi::InputAllowedMode, uint32_t> InputObject::getAvailableObjectParameter() const
{
	uint32_t startSearchId = m_startSearchId;
	if (SvDef::InvalidObjectId == startSearchId)
	{
		auto* pInsp = GetAncestor(SvPb::SVInspectionObjectType);
		if (nullptr != pInsp)
		{
			startSearchId = pInsp->getObjectId();
		}
	}
	if (SvOi::InputAllowedMode::IsBeforeTool == m_allowedMode)
	{
		auto* pTool = GetAncestor(SvPb::SVToolObjectType);
		if (nullptr != pTool)
		{
			return {m_InputObjectInfo.m_ObjectTypeInfo, startSearchId, m_allowedMode, pTool->getObjectId()};
		}
	}

	return {m_InputObjectInfo.m_ObjectTypeInfo, startSearchId, m_allowedMode, SvDef::InvalidObjectId};
}

SvPb::GetInputDataResponse InputObject::getInputData(SvPb::SVObjectTypeEnum desiredFirstObjectTypeForConnectedName, bool excludeFirstObjectNameInConntectedName) const
{
	SvPb::GetInputDataResponse retValue;
	retValue.mutable_data()->set_inputid(getObjectId());
	retValue.mutable_data()->set_inputname(GetName());
	if (nullptr != m_InputObjectInfo.getObject())
	{
		std::string name;
		if (excludeFirstObjectNameInConntectedName)
		{
			name = m_InputObjectInfo.getObject()->GetObjectNameBeforeObjectType(desiredFirstObjectTypeForConnectedName);
		}
		else
		{
			name = m_InputObjectInfo.getObject()->GetObjectNameToObjectType(desiredFirstObjectTypeForConnectedName);
		}
		retValue.mutable_data()->set_connected_objectdottedname(name);
	}
	retValue.mutable_data()->set_connected_objectid(m_InputObjectInfo.getObjectId());
	retValue.mutable_typeinfo()->set_objecttype(m_InputObjectInfo.m_ObjectTypeInfo.m_ObjectType);
	retValue.mutable_typeinfo()->set_subtype(m_InputObjectInfo.m_ObjectTypeInfo.m_SubType);
	retValue.mutable_typeinfo()->set_embeddedid(m_InputObjectInfo.m_ObjectTypeInfo.m_EmbeddedID);
	return retValue;
}

void InputObject::Persist(SvOi::IObjectWriter& rWriter) const
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
	auto* pConnectObject = GetInputObjectInfo().getObject();
	if (nullptr != pConnectObject)
	{
		value = pConnectObject->GetObjectNameToObjectType().c_str();
		rWriter.WriteAttribute(scConntectedDotnameTag, value);
	}

	rWriter.EndElement();
}

void InputObject::fixInvalidInputs(std::back_insert_iterator<std::vector<SvPb::FixedInputData>> inserter)
{
	if (SvPb::noAttributes != ObjectAttributesAllowed())
	{
		validateInput();
		if (false == IsConnected())
		{
			SvPb::FixedInputData data;
			data.set_name(GetObjectNameToObjectType());
			data.set_objectid(getObjectId());
			data.set_parentid(GetParentID());
			data.set_oldinputvalue(m_DottedNameOfFailedLoadInput);
			data.set_islinkedvalue(false);
			// cppcheck-suppress unreadVariable symbolName=inserter ; cppCheck doesn't know back_insert_iterator
			inserter = data;

			SVObjectReference objectRef {GetObjectReferenceForDottedName(m_DottedNameOfFailedLoadInput)};
			if (nullptr != objectRef.getFinalObject() && checkType(m_InputObjectInfo.m_ObjectTypeInfo, objectRef.getFinalObject()->getObjectTypeInfo()))
			{
				SetInputObject(objectRef);
				validateInput();
			}

			if (false == IsConnected())
			{
				auto* pOwner = dynamic_cast<SvOi::ITaskObject*>(GetParent());
				if (nullptr != pOwner)
				{
					pOwner->connectInput(this);
					validateInput();
				}
			}
		}
	}
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

bool InputObject::checkIfAllowedObject()
{
	switch (m_allowedMode)
	{
		case SvOi::InputAllowedMode::IsBeforeTool:
		{
			auto* pTool = GetToolInterface();
			if (nullptr == pTool)
			{
				return true;
			}
			auto* pObject = getInputObject<SVObjectClass>();
			if (nullptr != pObject)
			{
				auto* pObjectTool = dynamic_cast<SvOi::ITool*>(pObject->GetAncestor(SvPb::SVToolObjectType));
				if (nullptr != pObjectTool)
				{
					auto toolPos = pTool->getToolPosition();
					auto toolObjectPos = pObjectTool->getToolPosition();
					//If toolPos < 0 then toolPos is not set yet. Ignore then the check.
					return (0 > toolPos || toolObjectPos < toolPos);
				}
				return true;
			}
			return false;
		}
		default:
			return true;
	}
}
} //namespace SvOl