//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVObject
//* .File Name       : $Workfile:   SVObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.10  $
//* .Check In Date   : $Date:   08 Jan 2015 05:41:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVObjectClass.h"
#include "JoinType.h"
#include "SVObjectAttributeClass.h"
#include "SVObjectLevelCreateStruct.h"
#include "SVObjectManagerClass.h"
#include "SVOResource/resource.h"
#include "SVToolsetScriptTags.h"
#include "Definitions/Color.h"
#include "Definitions/StringTypeDef.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "SVProtoBuf/InspectionCommands.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations
SV_IMPLEMENT_CLASS(SVObjectClass, SvPb::NoObjectClassId)

//This is the default constructor for this object.  This constructor initializes the name objects, clears all owner information, and calls the init method.
SVObjectClass::SVObjectClass()
	: m_resourceID(0)
	, m_ObjectName (_T("Base Class: SVResearch Base Object"))
	, m_Name(m_ObjectName)
{
	SetObjectOwner(nullptr);
	init();
}

/*
This constructor initializes the name objects from the provided parameter, clears all owner information, and calls the init method.
*/
SVObjectClass::SVObjectClass(LPCTSTR ObjectName)
	: m_resourceID(0)
	, m_ObjectName(ObjectName)
	, m_Name(ObjectName)
{
	init();
}

/*
This constructor initializes the name objects from the provided parameter, sets the owner information from the provided parameter, and calls the init method.
*/
SVObjectClass::SVObjectClass(SVObjectClass* pOwner /* = nullptr */, int StringResourceID /* = IDS_CLASSNAME_SVOBJECT */)
	: m_resourceID(StringResourceID)
	, m_ObjectName(SvUl::LoadStdString(StringResourceID))
	, m_Name(m_ObjectName)
{
	SetObjectOwner(pOwner);
	init();
}

/*
This method initializes all the local attributes of the object and registers this object with the object manager.

The object depth gives you the possibility to generate an object member data table. That means, if you want to have arrays of member variables to keep access to earlier results, use the objectDepth as max size of these arrays. ( Sample: SVBoolValueObjectClass ! )

NOTE: Each class derived from this class have to manage its own object data table.

If this Object is valid, the validationReferenceID refers to the last object on which this object valid state is proofed.  It should normally refers to itself.

If this Object is NOT valid, the validationReferenceID refers to the object on that the proofing fails.
*/
void SVObjectClass::init()
{
	m_isCreated = false;
	m_embeddedID = SvPb::NoEmbeddedId;

	// Set object Info...
	SVObjectManagerClass::Instance().CreateObjectID(this);

	m_outObjectInfo.SetObject(this);

	m_ObjectAttributesAllowed = SvPb::noAttributes;
	m_ObjectAttributesSet.resize(1);
	m_ObjectAttributesSet[0] = SvPb::noAttributes;
}

/*
This destructor unregisters itself from the object manager and frees all of the allocated names.
*/
SVObjectClass::~SVObjectClass()
{
	disconnectAllInputs();

	SVObjectManagerClass::Instance().CloseUniqueObjectID(this);
}

bool SVObjectClass::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	if (!m_isCreated)
	{
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NotCreated, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
			pErrorMessages->push_back(Msg);
		}
		return false;
	}
	return true;
}

HRESULT SVObjectClass::RefreshObject(const SVObjectClass* const , RefreshObjectType )
{
	HRESULT Result = S_OK;

	return Result;
}

void SVObjectClass::connectObject(uint32_t objectId)
{
	if (SvDef::InvalidObjectId != objectId)
	{
		std::lock_guard<std::mutex> Autolock(m_inputMutex);
		SVObjectManagerClass::Instance().connectDependency(getObjectId(), objectId, SvOl::JoinType::Dependent);
		m_connectedSet.emplace(objectId);
	}
}

void SVObjectClass::disconnectObject(uint32_t objectId)
{
	if (SvDef::InvalidObjectId != objectId)
	{
		SVObjectClass* pObject{ nullptr };
		{
			std::lock_guard<std::mutex> Autolock(m_inputMutex);
			auto iter = m_connectedSet.find(objectId);
			if (m_connectedSet.end() != iter)
			{
				SVObjectManagerClass::Instance().disconnectDependency(getObjectId(), objectId, SvOl::JoinType::Dependent);
				m_connectedSet.erase(iter);
				pObject = SVObjectManagerClass::Instance().GetObject(objectId);
			}
		}
				 
		if (nullptr != pObject)
		{
			pObject->disconnectObjectInput(getObjectId());
		}		
	}
}

void SVObjectClass::disconnectObjectInput(uint32_t objectId)
{
	auto* pObject = SVObjectManagerClass::Instance().GetObject(objectId);
	if (pObject)
	{
		pObject->disconnectObject(getObjectId());
	}
}

void SVObjectClass::disconnectAllInputs()
{
	auto tmpSet = m_connectedSet;
	m_connectedSet.clear();
	for (auto id : tmpSet)
	{
		auto* pObject = SVObjectManagerClass::Instance().GetObject(id);
		if (pObject)
		{
			pObject->disconnectObjectInput(getObjectId());
		}
	}
}

/*
After the object construction, the object must be created using this function with an object level depending create structure.
*/
bool SVObjectClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool Result(false);

	if (rCreateStructure.OwnerObjectInfo.getObject() != this && rCreateStructure.OwnerObjectInfo.getObjectId() != getObjectId())
	{
		SetObjectOwner(rCreateStructure.OwnerObjectInfo.getObject());
		Result = true;
	}
	else
	{
		assert(false);
	}

	m_isCreated = Result;

	return Result;
}

void SVObjectClass::ConnectObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	if (rCreateStructure.OwnerObjectInfo.getObject() != this && rCreateStructure.OwnerObjectInfo.getObjectId() != getObjectId())
	{
		SetObjectOwner(rCreateStructure.OwnerObjectInfo.getObject());
	}
}

/*
This method executes the close object method on all objects that use this object that are not closed.
*/
bool SVObjectClass::CloseObject()
{
	m_isCreated = false;

	return true;
}

/*
Reset user changeable name to the NOT user changeable object instance name.
*/
void SVObjectClass::ResetName()
{
	// Resets the changeable names to the name given at construction time
	// or set with SVObjectClass::SetObjectName()
	m_Name = m_ObjectName;
}

#pragma region virtual method (IObjectClass)
/*
This method return the object name.  This name is changeable by the user.
*/
LPCTSTR SVObjectClass::GetName() const
{
	return m_Name.c_str();
}

std::string SVObjectClass::GetCompleteName() const
{
	std::string Result;

	if (nullptr != m_ownerObjectInfo.getObject() && m_ownerObjectInfo.getObject() != this)
	{
		Result = m_ownerObjectInfo.getObject()->GetCompleteName();
	}

	if (0 < Result.size())
	{
		Result += _T(".");
	}

	Result += GetName();

	return Result;
}

HRESULT SVObjectClass::GetCompleteNameToType(SvPb::SVObjectTypeEnum objectType, std::string& rName) const
{
	HRESULT hr = S_OK;

	if (GetObjectType() == objectType)
	{
		rName = GetCompleteName();
	}
	else
	{
		const SvOi::IObjectClass* pObject = GetAncestorInterface(objectType);
		if (nullptr != pObject)
		{
			rName = pObject->GetCompleteName();
		}
		else
		{
			hr = E_POINTER;
		}
	}
	return hr;
}

SvOi::IObjectClass* SVObjectClass::GetAncestorInterface(SvPb::SVObjectTypeEnum ancestorObjectType, bool topLevel /*= false*/)
{
	return static_cast<SvOi::IObjectClass*> (GetAncestor(ancestorObjectType, topLevel));
}

const SvOi::IObjectClass* SVObjectClass::GetAncestorInterface(SvPb::SVObjectTypeEnum ancestorObjectType, bool topLevel /*= false*/) const
{
	return static_cast<SvOi::IObjectClass*> (GetAncestor(ancestorObjectType, topLevel));
}

SvPb::SVObjectSubTypeEnum SVObjectClass::GetObjectSubType() const
{
	return static_cast<SvPb::SVObjectSubTypeEnum>(m_outObjectInfo.m_ObjectTypeInfo.m_SubType);
}

bool SVObjectClass::is_Created() const
{
	return IsCreated();
}

SvUl::NameClassIdList SVObjectClass::GetCreatableObjects(const SvDef::SVObjectTypeInfoStruct& ) const
{
	return {};
}

void SVObjectClass::SetName(LPCTSTR Name)
{
	m_Name = Name;
}

SvOi::IObjectClass* SVObjectClass::getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool , const SvOi::IObjectClass* pRequestor) const
{
	// check the owner of this class
	if (nullptr != pRequestor && (pRequestor == this || pRequestor == GetParent()))
	{
		// Do not reference self or owner
		return nullptr;
	}

	// Find best match....m_EmbeddedID, Type, SubType...
	if ((SvPb::NoEmbeddedId == rObjectTypeInfo.m_EmbeddedID || rObjectTypeInfo.m_EmbeddedID == GetEmbeddedID()) &&
		(SvPb::SVNotSetObjectType == rObjectTypeInfo.m_ObjectType || rObjectTypeInfo.m_ObjectType == GetObjectType()) &&
		(SvPb::SVNotSetSubObjectType == rObjectTypeInfo.m_SubType || rObjectTypeInfo.m_SubType == GetObjectSubType())
		)
	{
		if (SvPb::NoEmbeddedId != rObjectTypeInfo.m_EmbeddedID ||
			SvPb::SVNotSetObjectType != rObjectTypeInfo.m_ObjectType ||
			SvPb::SVNotSetSubObjectType != rObjectTypeInfo.m_SubType
			)
		{
			// But object must be specified!
			return const_cast<SVObjectClass*>(this);
		}
	}

	return nullptr;
}

void SVObjectClass::fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType) const
{
	SVObjectReference ObjectRef = GetObjectInfo().GetObjectReference();
	if (isCorrectType(requiredType))
	{
		SvPb::TreeItem insertItem;

		if (nullptr != ObjectRef.getValueObject())
		{
			insertItem.set_type(ObjectRef.getValueObject()->getTypeName());
		}

		if (ObjectRef.isArray())
		{
			if (wholeArray && pFunctor(ObjectRef.getObject(), attribute, 0))
			{
				ObjectRef.SetEntireArray();
				insertItem.set_name(ObjectRef.GetName(true));
				UINT AttributesSet = ObjectRef.ObjectAttributesSet();
				insertItem.set_location(ObjectRef.GetObjectNameToObjectType(nameToType, true, true));
				insertItem.set_objectidindex(ObjectRef.GetObjectIdAndIndexOneBased());
				insertItem.set_selected((AttributesSet & attribute) == attribute);
				// cppcheck-suppress unreadVariable symbolName=treeInserter ; cppCheck doesn't know back_insert_iterator
				treeInserter = insertItem;
			}

			// add array elements
			int iArraySize = ObjectRef.getValueObject()->getArraySize();
			for (int i = 0; i < iArraySize; i++)
			{
				if (pFunctor(ObjectRef.getObject(), attribute, i))
				{
					ObjectRef.SetArrayIndex(i);
					insertItem.set_name(ObjectRef.GetName(true));
					UINT AttributesSet = ObjectRef.ObjectAttributesSet();
					insertItem.set_location(ObjectRef.GetObjectNameToObjectType(nameToType, true, true));
					insertItem.set_objectidindex(ObjectRef.GetObjectIdAndIndexOneBased());
					insertItem.set_selected((AttributesSet & attribute) == attribute);
					// cppcheck-suppress unreadVariable symbolName=treeInserter ; cppCheck doesn't know back_insert_iterator
					treeInserter = insertItem;
				}
			}
		}
		else if (pFunctor(ObjectRef.getObject(), attribute, 0))
		{
			insertItem.set_name(ObjectRef.GetName());
			UINT AttributesSet = ObjectRef.ObjectAttributesSet();
			insertItem.set_location(ObjectRef.GetObjectNameToObjectType(nameToType, true));
			insertItem.set_objectidindex(ObjectRef.GetObjectIdAndIndexOneBased());
			insertItem.set_selected((AttributesSet & attribute) == attribute);
			// cppcheck-suppress unreadVariable symbolName=treeInserter ; cppCheck doesn't know back_insert_iterator
			treeInserter = insertItem;
		}
	}
}

void SVObjectClass::fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool /*addHidden = false*/)
{
	if ((SvPb::NoEmbeddedId == rObjectInfo.m_EmbeddedID || rObjectInfo.m_EmbeddedID == GetEmbeddedID()) &&
		(SvPb::SVNotSetObjectType == rObjectInfo.m_ObjectType || rObjectInfo.m_ObjectType == GetObjectType()) &&
		(SvPb::SVNotSetSubObjectType == rObjectInfo.m_SubType || rObjectInfo.m_SubType == GetObjectSubType())
		)
	{
		// cppcheck-suppress unreadVariable symbolName=inserter ; cppCheck doesn't know back_insert_iterator
		inserter = this;
	}
}
#pragma endregion virtual method (IObjectClass)

/*
This method return the internal object name.  This name is not changeable by the user.
*/
LPCTSTR SVObjectClass::GetObjectName() const
{
	return m_ObjectName.c_str();
}

/*
Set the NOT user changeable object instance name.
*/
void SVObjectClass::SetObjectName(LPCTSTR ObjectName)
{
	m_ObjectName = ObjectName;
	ResetName();
}

/*
Set new object owner using owner pointer.
*/
bool SVObjectClass::SetObjectOwner(SVObjectClass* pNewOwner)
{
	// Check if object exists...
	if (nullptr != pNewOwner)
	{
		assert(pNewOwner != this); // can't own yourself...

		//First disconnect the previous owner
		uint32_t objectId = m_ownerObjectInfo.getObjectId();
		if (SvDef::InvalidObjectId != objectId)
		{
			SVObjectManagerClass::Instance().disconnectDependency(objectId, getObjectId(), SvOl::JoinType::Owner);
		}

		m_ownerObjectInfo.SetObject(pNewOwner);
		objectId = m_ownerObjectInfo.getObjectId();
		SVObjectManagerClass::Instance().connectDependency(objectId, getObjectId(), SvOl::JoinType::Owner);
		return true;
	}
	return false;
}

/*
Set new object owner using owner ID.  Uses TheSVObjectManager to get the owner pointer.
*/
bool SVObjectClass::SetObjectOwner(uint32_t newOwnerID)
{
	SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(newOwnerID);

	return SetObjectOwner(pObject);
}

/*
Set embedded object info.  Use this only for real embedded objects.
*/
void SVObjectClass::SetObjectEmbedded(SvPb::EmbeddedIdEnum embeddedID, SVObjectClass* pOwner, LPCTSTR NewObjectName)
{
	m_embeddedID = embeddedID;
	m_outObjectInfo.m_ObjectTypeInfo.m_EmbeddedID = embeddedID;
	SetObjectName(NewObjectName);
	SetObjectOwner(pOwner);
}

HRESULT SVObjectClass::SetValuesForAnObject(uint32_t aimObjectID, SVObjectAttributeClass* pDataObject)
{
	// check if it is for us
	if (getObjectId() == aimObjectID)
	{
		return SetObjectValue(pDataObject);
	}
	else
	{
		assert(false);
		return E_FAIL;
	}
}

/*
Must override this in the derived class if you wish to set any values upon restoration from a script.
*/
HRESULT SVObjectClass::SetObjectValue(SVObjectAttributeClass* pDataObject)
{
	HRESULT hr = S_FALSE;
	bool bOk = false;

	if (nullptr != pDataObject)
	{
		SvCl::SVObjectDWordArrayClass svDWordArray;
		SvCl::SVObjectStdStringArrayClass StringArray;

		if( true == (bOk = pDataObject->GetAttributeData(_T("AttributesSet"), svDWordArray)))
		{
			int iSize = static_cast<int> (svDWordArray.size());
			{
				m_ObjectAttributesSet.resize(iSize);
				for (int i = 0; i < iSize; i++)
				{
					m_ObjectAttributesSet.at(i) = svDWordArray[i];
				}
			}
		}
		else if( true == (bOk = pDataObject->GetAttributeData(_T("AttributesAllowed"), svDWordArray)))
		{
			; // Do nothing as it's obsolete
		}
	}

	hr = bOk ? S_OK : S_FALSE;
	return hr;
}

// Override this function to implement object behavior...
void SVObjectClass::SetDisabled()
{
}

/*
This method is a placeholder for derived functionality.  This method performs no operation.
*/
SVObjectClass* SVObjectClass::UpdateObject(uint32_t , SVObjectClass* , SVObjectClass* )
{
	return nullptr;
}

bool SVObjectClass::isCorrectType(SvPb::ObjectSelectorType requiredType, const SVObjectClass* pTestObject) const
{
	const SVObjectClass* pObject = (nullptr != pTestObject) ? pTestObject : this;
	switch (requiredType)
	{
	case SvPb::allValueObjects:
		return (SvPb::SVValueObjectType == pObject->GetObjectType() || SvPb::SVBasicValueObjectType == pObject->GetObjectType());
	case SvPb::allNumberValueObjects:
	{
		auto pValueObject = dynamic_cast<const SvOi::IValueObject*>(pObject);
		if (nullptr != pValueObject)
		{
			DWORD type = pValueObject->GetType();
			constexpr std::array<DWORD, 11> filter{ VT_I2, VT_I4, VT_I8, VT_R4, VT_R8, VT_UI2, VT_UI4, VT_UI8, VT_INT, VT_UINT, VT_BOOL };
			return (filter.end() != std::find(filter.begin(), filter.end(), type));
		}
		return false;
	}
	case SvPb::realNumberValueOjects:
	{
		if (SvPb::SVValueObjectType == pObject->GetObjectType())
		{
			constexpr std::array<SvPb::SVObjectSubTypeEnum, 11> filter{ SvPb::SVDWordValueObjectType, SvPb::SVLongValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::SVBoolValueObjectType, SvPb::SVByteValueObjectType, SvPb::SVInt64ValueObjectType, SvPb::DoubleSortValueObjectType };
			return (filter.end() != std::find(filter.begin(), filter.end(), pObject->GetObjectSubType()));
		}
		else if (SvPb::SVBasicValueObjectType == pObject->GetObjectType())
		{
			auto pValueObject = dynamic_cast<const SvOi::IValueObject*>(pObject);
			if (nullptr != pValueObject)
			{
				DWORD type = pValueObject->GetType();
				constexpr std::array<DWORD, 11> filter{ VT_I2, VT_I4, VT_I8, VT_R4, VT_R8, VT_UI2, VT_UI4, VT_UI8, VT_INT, VT_UINT, VT_BOOL };
				return (filter.end() != std::find(filter.begin(), filter.end(), type));
			}
		}
		return false;
	}
	case SvPb::stringValueObjects:
	{
		auto pValueObject = dynamic_cast<const SvOi::IValueObject*>(pObject);
		if (nullptr != pValueObject)
		{
			return (VT_BSTR == pValueObject->GetType());
		}
		return false;
	}
	case SvPb::tableObjects:
		return (SvPb::TableObjectType == pObject->GetObjectType());
	case SvPb::allImageObjects:
		return (SvPb::SVImageObjectType == pObject->GetObjectType());
	case SvPb::grayImageObjects:
		return (SvPb::SVImageObjectType == pObject->GetObjectType() && SvPb::SVImageMonoType == pObject->GetObjectSubType());
	case SvPb::colorImageObjects:
		return (SvPb::SVImageObjectType == pObject->GetObjectType() && SvPb::SVImageColorType == pObject->GetObjectSubType());
	default:
		return false;
	}
}

/*
This method returns Ancestor Object of specified Object Type of this Object, if any.  Otherwise it returns NULL.
*/
SVObjectClass* SVObjectClass::GetAncestor(SvPb::SVObjectTypeEnum AncestorObjectType, bool topLevel /*= false*/) const
{
	SVObjectClass* pResult {nullptr};

	SVObjectClass* pOwner = this->GetParent();

	while (nullptr != pOwner)
	{
		if (pOwner->GetObjectType() == AncestorObjectType)
		{
			pResult = pOwner;
			if (!topLevel)
			{
				break;
			}
		}
		pOwner = pOwner->GetParent();
	}
	return pResult;
}

// Get the local object color...
DWORD SVObjectClass::GetObjectColor() const
{
	// Not defined here!
	return SvDef::DefaultInactiveColor;
}

/*
Get the complete object name including selected SVObjectTypeEnum value.
*/
std::string SVObjectClass::GetObjectNameToObjectType(SvPb::SVObjectTypeEnum objectTypeToInclude, bool withOwnName) const
{
	std::string Result;

	SvPb::SVObjectTypeEnum objectType = GetObjectType();
	if (objectType != objectTypeToInclude)
	{
		SVObjectClass* pObject = m_ownerObjectInfo.getObject();
		if (nullptr != pObject && pObject != this)
		{
			Result = pObject->GetObjectNameToObjectType(objectTypeToInclude);
		}
	}

	if (withOwnName)
	{
		if (!Result.empty())
		{
			Result += _T(".");
		}
		Result += GetName();
	}

	return Result;
}

/*
Get the complete object name before selected SVObjectTypeEnum value.
*/
std::string SVObjectClass::GetObjectNameBeforeObjectType(SvPb::SVObjectTypeEnum objectTypeToInclude) const
{
	std::string Result;

	SvPb::SVObjectTypeEnum objectType = GetObjectType();
	if (objectType != objectTypeToInclude)
	{
		SVObjectClass* pObject = m_ownerObjectInfo.getObject();
		if (nullptr != pObject && pObject != this && pObject->GetObjectType() != objectTypeToInclude)
		{
			Result = pObject->GetObjectNameBeforeObjectType(objectTypeToInclude);
		}
	}
	if (!Result.empty())
	{
		Result += _T(".");
	}
	Result += GetName();

	return Result;
}

bool SVObjectClass::createAllObjects(const SVObjectLevelCreateStruct& rCreateStructure)
{
	if (!IsCreated() && !CreateObject(rCreateStructure))
	{
		assert(false);

		SvDef::StringVector msgList;
		msgList.push_back(GetName());
		msgList.push_back(GetCompleteName());
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CreationOf2Failed, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10209);

		return false;
	}

	return true;
}

SVObjectClass* SVObjectClass::OverwriteEmbeddedObject(uint32_t uniqueID, SvPb::EmbeddedIdEnum embeddedID)
{
	if (GetEmbeddedID() == embeddedID && SvPb::NoEmbeddedId != embeddedID)
	{
		SVObjectManagerClass::Instance().ChangeUniqueObjectID(this, uniqueID);
		return this;
	}
	return nullptr;
}

SVObjectReference SVObjectClass::ConvertStringInObject(const std::string& rValue) const
{
	const std::string ToolSetName = SvUl::LoadStdString(IDS_CLASSNAME_SVTOOLSET);
	std::string ObjectName;

	//If the tool set name is at the start then add the inspection name at the beginning
	if (0 == rValue.find(ToolSetName.c_str()))
	{
		const SvOi::IObjectClass* pInspection = GetAncestorInterface(SvPb::SVInspectionObjectType);
		if (nullptr != pInspection)
		{
			ObjectName = pInspection->GetName();
			ObjectName += _T(".");
			ObjectName += rValue;
		}
	}
	else
	{
		ObjectName = rValue;
	}

	SVObjectReference objectRef;
	SVObjectManagerClass::Instance().GetObjectByDottedName(ObjectName, objectRef);
	return objectRef;
}

HRESULT SVObjectClass::RemoveObjectConnection(uint32_t )
{
	return S_OK;
}

void SVObjectClass::Persist(SvOi::IObjectWriter& rWriter)
{
	_variant_t value;
	value.SetString(GetName()); // use user editable name for Data Element ObjectName attribute
	rWriter.WriteAttribute(scObjectNameTag, value);
	value.Clear();

	value = GetClassID();
	rWriter.WriteAttribute(scClassIDTag, value);
	value.Clear();

	value = convertObjectIdToVariant(m_outObjectInfo.getObjectId());
	rWriter.WriteAttribute(scUniqueReferenceIDTag, value);
	value.Clear();

	// Set up object definition...
	if (SvPb::NoEmbeddedId != GetEmbeddedID())
	{
		value = GetEmbeddedID();
		rWriter.WriteAttribute(scEmbeddedIDTag, value);
		value.Clear();
	}
}

HRESULT SVObjectClass::GetChildObject(SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index) const
{
	HRESULT l_Status = S_OK;

	if (Index == (rNameInfo.m_NameArray.size() - 1))
	{
		if (rNameInfo.m_NameArray[Index] == GetName())
		{
			rpObject = const_cast<SVObjectClass*>(this);
		}
		else
		{
			l_Status = S_FALSE;
		}
	}
	else
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

/*
This method returns the allowed attributes of this object.
*/
UINT SVObjectClass::ObjectAttributesAllowed() const
{
	return m_ObjectAttributesAllowed;
}

/*
This method returns a reference to the allowed attributes of this object.
*/
UINT SVObjectClass::SetObjectAttributesAllowed(UINT Attributes, SvOi::SetAttributeType Type)
{
	switch (Type)
	{
		case SvOi::SetAttributeType::AddAttribute:
			m_ObjectAttributesAllowed |= Attributes;
			break;
		case SvOi::SetAttributeType::RemoveAttribute:
			m_ObjectAttributesAllowed &= ~Attributes;
			break;
		case SvOi::SetAttributeType::OverwriteAttribute:
			m_ObjectAttributesAllowed = Attributes;
			break;
	}
	return m_ObjectAttributesAllowed;
}

/*
This method returns the set attributes of this object.
*/
UINT SVObjectClass::ObjectAttributesSet(int iIndex) const
{
	if (static_cast<int> (m_ObjectAttributesSet.size()) > iIndex)
	{
		return m_ObjectAttributesSet.at(iIndex);
	}
	else
	{
		assert(false);
		return 0;
	}
}

/*
This method sets attributes of this object.
*/
UINT SVObjectClass::SetObjectAttributesSet(UINT Attributes, SvOi::SetAttributeType Type, int Index)
{
	if (static_cast<int> (m_ObjectAttributesSet.size()) > Index)
	{

		UINT& rAttributesSet = m_ObjectAttributesSet.at(Index);
		switch (Type)
		{
			case SvOi::SetAttributeType::AddAttribute:
				rAttributesSet |= Attributes;
				break;
			case SvOi::SetAttributeType::RemoveAttribute:
				rAttributesSet &= ~Attributes;
				break;
			case SvOi::SetAttributeType::OverwriteAttribute:
				rAttributesSet = Attributes;
				break;
		}
		return rAttributesSet;
	}
	else
	{
		assert(false);
		return 0;
	}
}

/*
This method must be overridden if the derived class desires this functionality
*/
HRESULT SVObjectClass::RegisterSubObject(SVObjectClass*)
{
	return S_FALSE;
}

/*
This method must be overridden if the derived class desires this functionality
*/
HRESULT SVObjectClass::UnregisterSubObject(SVObjectClass*)
{
	return S_FALSE;
}