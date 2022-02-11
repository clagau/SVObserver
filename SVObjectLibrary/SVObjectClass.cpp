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
#include "ObjectInterfaces/ILinkedObject.h"
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

	m_ObjectAttributesAllowed = SvPb::noAttributes;
	m_ObjectAttributesSet.resize(1);
	m_ObjectAttributesSet[0] = SvPb::noAttributes;
}

/*
This destructor unregisters itself from the object manager and frees all of the allocated names.
*/
SVObjectClass::~SVObjectClass()
{
	assert(0 == m_embeddedList.size());
	assert(0 == m_notificationList.size());
	sendChangeNotification(SvOi::ObjectNotificationType::Deleting, m_objectId);
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

	if (&rCreateStructure.m_rOwner != this  && rCreateStructure.m_rOwner.getObjectId() != getObjectId())
	{
		SetObjectOwner(&rCreateStructure.m_rOwner);
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
	if (&rCreateStructure.m_rOwner != this && rCreateStructure.m_rOwner.getObjectId() != getObjectId())
	{
		SetObjectOwner(&rCreateStructure.m_rOwner);
	}
}

/*
This method executes the close object method on all objects that use this object that are not closed.
*/
bool SVObjectClass::CloseObject()
{
	// Close Embeddeds...
	for (SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = *Iter;
		if (nullptr != pObject)
		{
			pObject->CloseObject();
		}
	}
	m_embeddedList.clear();
	sendChangeNotification(SvOi::ObjectNotificationType::Deleting, m_objectId);
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

	if (nullptr != m_pOwner && m_pOwner != this)
	{
		Result = m_pOwner->GetCompleteName();
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
	return static_cast<SvPb::SVObjectSubTypeEnum>(m_ObjectTypeInfo.m_SubType);
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

	for (SVObjectPtrVector::const_iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = *Iter;

		if (nullptr != pObject)
		{
			auto* pResult = pObject->getFirstObject(rObjectTypeInfo, false, pRequestor);
			if (pResult)
			{
				return pResult;
			}
		}
	}

	return nullptr;
}

void SVObjectClass::getOutputList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter) const
{
	for (auto* pObject : m_embeddedList)
	{
		if (nullptr != pObject)
		{
			inserter = pObject;
			pObject->getOutputList(inserter);
		}
	}
}

void SVObjectClass::fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType, bool stopIfClosed /*= false*/, bool /*firstObject = false*/) const
{
	if (isCorrectType(requiredType))
	{
		SVObjectReference ObjectRef{ getObjectId() };
		ObjectRef.fillSelectorList(treeInserter, wholeArray, pFunctor, attribute, nameToType);
	}

	if (SvPb::noAttributes == attribute || SvPb::noAttributes != ObjectAttributesAllowed())
	{
		for (auto* pObject : m_embeddedList)
		{
			if (nullptr != pObject)
			{
				pObject->fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, nameToType, requiredType, stopIfClosed);
			}
		}
	}
}

void SVObjectClass::fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden /*= false*/, bool stopIfClosed /*= false*/, bool /*firstObject = false*/)
{
	if ((SvPb::NoEmbeddedId == rObjectInfo.m_EmbeddedID || rObjectInfo.m_EmbeddedID == GetEmbeddedID()) &&
		(SvPb::SVNotSetObjectType == rObjectInfo.m_ObjectType || rObjectInfo.m_ObjectType == GetObjectType()) &&
		(SvPb::SVNotSetSubObjectType == rObjectInfo.m_SubType || rObjectInfo.m_SubType == GetObjectSubType())
		)
	{
		inserter = this;
	}

	for (auto* pObject : m_embeddedList)
	{
		if (nullptr != pObject)
		{
			pObject->fillObjectList(inserter, rObjectInfo, addHidden, stopIfClosed);
		}
	}

}

uint32_t SVObjectClass::getFirstClosedParent(uint32_t stopSearchAtObjectId) const
{
	if (getObjectId() == stopSearchAtObjectId || nullptr == m_pOwner)
	{
		return SvDef::InvalidObjectId;
	}
	
	return m_pOwner->getFirstClosedParent(stopSearchAtObjectId);
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
	assert(pNewOwner != this); // can't own yourself...

	//First disconnect the previous owner
	if (nullptr != m_pOwner)
	{
		SVObjectManagerClass::Instance().disconnectDependency(m_pOwner->getObjectId(), getObjectId(), SvOl::JoinType::Owner);
	}

	m_pOwner = pNewOwner;

	if (nullptr != m_pOwner)
	{
		SVObjectManagerClass::Instance().connectDependency(m_pOwner->getObjectId(), getObjectId(), SvOl::JoinType::Owner);
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
	m_ObjectTypeInfo.m_EmbeddedID = embeddedID;
	SetObjectName(NewObjectName);
	SetObjectOwner(pOwner);
}

HRESULT SVObjectClass::SetValuesForAnObject(uint32_t aimObjectID, SVObjectAttributeClass* pDataObject)
{
	for (SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = *Iter;
		if (nullptr != pObject)
		{
			// check if it's this object
			if (aimObjectID == pObject->getObjectId())
			{
				// Set the Object's Data Member Value
				if (S_OK == pObject->SetObjectValue(pDataObject))
				{
					return S_OK;
				}
			}
		}
	}

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

		if( true == (bOk = pDataObject->GetAttributeData(scAttributesSetTag, svDWordArray)))
		{
			int iSize = static_cast<int> (svDWordArray.size());
			{
				m_ObjectAttributesSet.resize(iSize);
				for (int i = 0; i < iSize; i++)
				{
					//attribute set infos are only in older configs
					// correct infos about variables in result view are in a seperate list in the config
					m_ObjectAttributesSet.at(i) = (svDWordArray[i] & ~SvPb::viewable);
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

bool SVObjectClass::isCorrectType(SvPb::ObjectSelectorType requiredType) const
{
	switch (requiredType)
	{
		case SvPb::allValueObjects:
			return (SvPb::SVValueObjectType == GetObjectType() || SvPb::SVBasicValueObjectType == GetObjectType());
		case SvPb::allNumberValueObjects:
		{
			auto pValueObject = dynamic_cast<const SvOi::IValueObject*>(this);
			if (nullptr != pValueObject)
			{
				DWORD type = pValueObject->GetType();
				constexpr std::array<DWORD, 11> filter {VT_I2, VT_I4, VT_I8, VT_R4, VT_R8, VT_UI2, VT_UI4, VT_UI8, VT_INT, VT_UINT, VT_BOOL};
				return (filter.end() != std::find(filter.begin(), filter.end(), type));
			}
			return false;
		}
		case SvPb::realNumberValueOjects:
		{
			if (SvPb::SVValueObjectType == GetObjectType())
			{
				constexpr std::array<SvPb::SVObjectSubTypeEnum, 11> filter {SvPb::SVDWordValueObjectType, SvPb::SVLongValueObjectType, SvPb::SVDoubleValueObjectType, SvPb::SVBoolValueObjectType, SvPb::SVByteValueObjectType, SvPb::SVInt64ValueObjectType, SvPb::DoubleSortValueObjectType};
				return (filter.end() != std::find(filter.begin(), filter.end(), GetObjectSubType()));
			}
			else if (SvPb::SVBasicValueObjectType == GetObjectType())
			{
				auto pValueObject = dynamic_cast<const SvOi::IValueObject*>(this);
				if (nullptr != pValueObject)
				{
					DWORD type = pValueObject->GetType();
					constexpr std::array<DWORD, 11> filter {VT_I2, VT_I4, VT_I8, VT_R4, VT_R8, VT_UI2, VT_UI4, VT_UI8, VT_INT, VT_UINT, VT_BOOL};
					return (filter.end() != std::find(filter.begin(), filter.end(), type));
				}
			}
			return false;
		}
		case SvPb::stringValueObjects:
		{
			auto pValueObject = dynamic_cast<const SvOi::IValueObject*>(this);
			if (nullptr != pValueObject)
			{
				return (VT_BSTR == pValueObject->GetType());
			}
			return false;
		}
		case SvPb::tableObjects:
			return (SvPb::TableObjectType == GetObjectType());
		case SvPb::allImageObjects:
			return (SvPb::SVImageObjectType == GetObjectType());
		case SvPb::grayImageObjects:
			return (SvPb::SVImageObjectType == GetObjectType() && SvPb::SVImageMonoType == GetObjectSubType());
		case SvPb::colorImageObjects:
			return (SvPb::SVImageObjectType == GetObjectType() && SvPb::SVImageColorType == GetObjectSubType());
		case SvPb::toolObjects:
			return (SvPb::SVToolObjectType == GetObjectType());
		default:
			return false;
	}	
}

bool SVObjectClass::checkIfValidDependency(const SVObjectClass* pObject) const
{
	if (pObject)
	{
		auto namePair = make_pair(pObject->GetObjectNameToObjectType(SvPb::SVToolSetObjectType), GetObjectNameToObjectType(SvPb::SVToolSetObjectType));
		std::initializer_list<const SVObjectClass*> objectList = { GetParent(), pObject };
		for (auto* pTestObject : objectList)
		{
			auto* pParent = pTestObject;
			while (pParent)
			{
				if (false == pParent->isValidDependency(namePair))
				{
					return false;
				}
				pParent = pParent->GetParent();
			}
		}
		return true;
	}
	return false;
}

// bool p_bResetAlways - input 
//    true - means that the object will initiate a reset whenever the value is 
//        set. Even if set to the same value.
//    false - means that the object will only initiate a reset if its value 
//        changes.
//
// The only place that this is ever set to true is for the color HSI 
// conversion value (Color Tool) and it is probably not necessary there.
//
bool SVObjectClass::RegisterEmbeddedObject(SVObjectClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, int StringResourceID, bool ResetAlways, SvOi::SVResetItemEnum RequiredReset)
{
	std::string Name = SvUl::LoadStdString(StringResourceID);
	return RegisterEmbeddedObject(pEmbeddedObject, embeddedID, Name.c_str(), ResetAlways, RequiredReset);
}

bool SVObjectClass::RegisterEmbeddedObject(SVObjectClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, LPCTSTR Name, bool ResetAlways, SvOi::SVResetItemEnum RequiredReset)
{
	bool Result(false);

	SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*> (pEmbeddedObject);
	if (nullptr != pValueObject)
	{
		pValueObject->setResetOptions(ResetAlways, RequiredReset);

		Result = RegisterEmbeddedObjectAsClass(pEmbeddedObject, embeddedID, Name);
	}

	return Result;
}

HRESULT SVObjectClass::setIndirectStringToObject(SvPb::EmbeddedIdEnum embeddedId, const std::vector<_variant_t>& rValueString)
{
	auto iter = std::ranges::find_if(m_embeddedList, [embeddedId](const auto* pEntry) { return pEntry->GetEmbeddedID() == embeddedId; });
	if (m_embeddedList.end() != iter)
	{
		auto* pLinked = dynamic_cast<SvOi::ILinkedObject*>(*iter);
		if (pLinked)
		{
			return pLinked->setIndirectStringForOldStruct(rValueString);
		}
	}
	return E_FAIL;
}

SvOi::ObjectNotificationRAIIPtr SVObjectClass::registerNotification(SvOi::ObjectNotificationFunctionPtr pFunc)
{ 
	m_notificationList.AddIfNoExist(pFunc);

	return SvOi::ObjectNotificationRAIIPtr(new int, ([this, pFunc](auto* pValue)
	{
		delete pValue;
		this->m_notificationList.Remove(pFunc);
	}));
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
		if (nullptr != m_pOwner && m_pOwner != this &&
			//special code for Remote/Digital Input. If ToolSet is selected for this part the inspection name should not be added!
			(SvPb::SVToolSetObjectType != objectTypeToInclude || SvPb::SVInspectionObjectType != m_pOwner->GetObjectType()))
		{
			Result = m_pOwner->GetObjectNameToObjectType(objectTypeToInclude);
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
		if (nullptr != m_pOwner && m_pOwner != this && m_pOwner->GetObjectType() != objectTypeToInclude)
		{
			Result = m_pOwner->GetObjectNameBeforeObjectType(objectTypeToInclude);
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
	// Check here all embedded members ( embedded objects could be only identified by embeddedID!!!! )... 
	for (SVObjectClass* pObject : m_embeddedList)
	{
		if (nullptr != pObject)
		{
			if (pObject->GetEmbeddedID() == embeddedID)
			{
				return pObject->OverwriteEmbeddedObject(uniqueID, embeddedID);
			}
		}
	}

	if (GetEmbeddedID() == embeddedID && SvPb::NoEmbeddedId != embeddedID)
	{
		SVObjectManagerClass::Instance().ChangeUniqueObjectID(this, uniqueID);
		return this;
	}
	
	return nullptr;
}

SVObjectReference SVObjectClass::GetObjectReferenceForDottedName(const std::string& rDottedName) const
{
	std::string fullDottedName;

	//If the tool set name is at the start then add the inspection name at the beginning
	if (rDottedName._Starts_with(SvUl::LoadedStrings::g_ToolSetName)
		|| SvUl::Left(rDottedName, strlen(SvDef::DioInputStr)) == SvDef::DioInputStr
		|| SvUl::Left(rDottedName, strlen(SvDef::RemoteInputStr)) == SvDef::RemoteInputStr)
	{
		const SvOi::IObjectClass* pInspection = GetAncestorInterface(SvPb::SVInspectionObjectType);
		if (nullptr != pInspection)
		{
			fullDottedName = pInspection->GetName();
			fullDottedName += _T(".");
			fullDottedName += rDottedName;
		}
	}
	else
	{
		fullDottedName = rDottedName;
	}

	SVObjectReference objectRef;
	SVObjectManagerClass::Instance().GetObjectByDottedName(fullDottedName, objectRef);
	return objectRef;
}

HRESULT SVObjectClass::RemoveObjectConnection(uint32_t )
{
	return S_OK;
}

bool SVObjectClass::RegisterEmbeddedObjectAsClass(SVObjectClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, LPCTSTR ObjectName)
{
	assert(nullptr != pEmbeddedObject);
	if (nullptr != pEmbeddedObject)
	{
		for (SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
		{
			SVObjectClass* pObject = *Iter;
			if (nullptr != pObject)
			{
				if (embeddedID == pObject->GetEmbeddedID())
				{
					SvStl::MessageManager Msg(SvStl::MsgType::Log);
					Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_Error_DuplicateEmbeddedId, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10204);
					assert(false);
					return false;
				}
			}
		}
		// Set object embedded to Setup the Embedded ID
		pEmbeddedObject->SetObjectEmbedded(embeddedID, this, ObjectName);

		// Add to embedded object to List of Embedded Objects
		AddEmbeddedObject(pEmbeddedObject);

		return true;
	}
	return false;
}

void SVObjectClass::AddEmbeddedObject(SVObjectClass* pObject)
{
	assert(nullptr != pObject);

	// Add to Owner's List of Embedded Objects
	m_embeddedList.push_back(pObject);
}

/////////////////////////////////////////////////////////////////////////////
//
//

void SVObjectClass::disableEmbeddedObject(SVObjectClass* pObjectToDisable)
{
	if (nullptr != pObjectToDisable)
	{
		pObjectToDisable->SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute);
		// Reset any selection
		pObjectToDisable->SetObjectAttributesSet(0, SvOi::SetAttributeType::OverwriteAttribute);

		// Get this object's outputInfo
		pObjectToDisable->disconnectAllInputs();
	}
}

void SVObjectClass::RemoveEmbeddedObject(SVObjectClass* pObjectToRemove)
{
	disableEmbeddedObject(pObjectToRemove);

	m_embeddedList.erase(std::remove(m_embeddedList.begin(), m_embeddedList.end(), pObjectToRemove), m_embeddedList.end());
}

bool SVObjectClass::createEmbeddedChildren()
{
	bool result = true;
	// Create the embeddeds...
// Save the owner and set the owner of our embeddeds to us!
	for (auto* pObject : m_embeddedList)
	{
		if (nullptr != pObject)
		{
			result = createAllObjectsFromChild(*pObject) && result;
			assert(result);
		}
		else
		{
			assert(false);
			result = false;
		}
	}
	return result;
}

void SVObjectClass::Persist(SvOi::IObjectWriter& rWriter) const
{
	PersistBaseData(rWriter);
	PersistEmbeddeds(rWriter);
}

void SVObjectClass::PersistBaseData(SvOi::IObjectWriter& rWriter) const
{
	_variant_t value;
	value.SetString(GetName()); // use user editable name for Data Element ObjectName attribute
	rWriter.WriteAttribute(scObjectNameTag, value);
	value.Clear();

	value = GetClassID();
	rWriter.WriteAttribute(scClassIDTag, value);
	value.Clear();

	value = convertObjectIdToVariant(m_objectId);
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

void SVObjectClass::PersistEmbeddeds(SvOi::IObjectWriter& rWriter) const
{
	// Set up embedded object definitions...
	if (0 < m_embeddedList.size())
	{
		rWriter.StartElement(scEmbeddedsTag);
		// Get embedded object script...
		for (const auto* pObject : m_embeddedList)
		{
			if (nullptr != pObject)
			{
				pObject->Persist(rWriter);
			}
		}
		rWriter.EndElement();
	}
}

HRESULT SVObjectClass::GetChildObject(SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index) const
{
	if (Index < rNameInfo.m_NameArray.size() && rNameInfo.m_NameArray[Index] == GetName())
	{
		if (Index == (rNameInfo.m_NameArray.size() - 1))
		{
			rpObject = const_cast<SVObjectClass*>(this);
			return S_OK;
		}
		else if (0 != ObjectAttributesAllowed())
		{
			for (const auto* pObject : m_embeddedList)
			{
				if (nullptr != pObject)
				{
					HRESULT l_Status = pObject->GetChildObject(rpObject, rNameInfo, Index + 1);
					if (S_OK == l_Status && nullptr != rpObject)
					{
						return S_OK;
					}
				}
			}
		}
	}

	return S_FALSE;
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

void SVObjectClass::setObjectId(uint32_t objectId) 
{ 
	uint32_t oldId = m_objectId;
	m_objectId = objectId; 
	sendChangeNotification(SvOi::ObjectNotificationType::ObjectIdChange, oldId);
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

bool SVObjectClass::allowExtensionCopy() const 
{
	return false;
}

void SVObjectClass::sendChangeNotification(SvOi::ObjectNotificationType type, uint32_t objectId)
{
	if (m_notificationList.size())
	{
		//onChangeNotification can deregister the notification and this will decrease the m_notifactionList. For this reason the list have to be copied before use it in a for loop
		auto tmpList = m_notificationList.getContainerCopy();
		for (auto func : tmpList)
		{
			if (func && func.get() && (*func.get()))
			{
				(*func.get())(type, objectId);
			}
		}
	}
}