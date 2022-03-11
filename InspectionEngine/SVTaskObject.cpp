//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTaskObject
//* .File Name       : $Workfile:   SVTaskObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   17 Jul 2014 20:39:34  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTaskObject.h"
#include "SVTaskObjectList.h"
#include "SVExtentPropertyInfoStruct.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVLibrary/SVTemplate.h"
#include "Definitions/Color.h"
#include "SVStatusLibrary/MessageManager.h"
#include "RunStatus.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/ITaskObjectListClass.h"
#include "ObjectInterfaces/IToolSet.h"
#include "SVObjectLibrary/DependencyManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVValueObjectLibrary/LinkedValue.h"
#include "SVObjectLibrary/InputObject.h"
#pragma endregion Includes

namespace SvIe
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

///For this class it is not necessary to call SV_IMPLEMENT_CLASS as it is a base class and only derived classes are instantiated.
//SV_IMPLEMENT_CLASS(SVTaskObjectClass, SVTaskObjectClassId)

SVTaskObjectClass::SVTaskObjectClass(LPCSTR ObjectName)
	: SVObjectAppClass(ObjectName)
	, m_bSkipFirstFriend(false)
{
	LocalInitialize();
}

SVTaskObjectClass::SVTaskObjectClass(SVObjectClass* pOwner, int StringResourceID)
	: SVObjectAppClass(pOwner, StringResourceID)
	, m_bSkipFirstFriend(false)
{
	LocalInitialize();
}

HRESULT SVTaskObjectClass::LocalInitialize()
{
	HRESULT l_hrOk = S_OK;

	m_bUseOverlays = true;	// most objects use overlays; must change if needed in derived classes

	SetObjectAttributesAllowed(SvPb::taskObject, SvOi::SetAttributeType::AddAttribute);

	// Register Embedded Objects
	RegisterEmbeddedObject(&m_statusTag, SvPb::StatusEId, IDS_OBJECTNAME_STATUS, false, SvOi::SVResetItemNone);
	RegisterEmbeddedObject(&m_statusColor, SvPb::ColorEId, IDS_OBJECTNAME_COLOR, false, SvOi::SVResetItemNone);

	// Set Embedded defaults
	m_statusColor.SetDefaultValue(static_cast<DWORD> (SvDef::DefaultInactiveColor), true);
	m_statusColor.setSaveValueFlag(false);
	m_statusTag.SetDefaultValue(DWORD(0), true);
	m_statusTag.setSaveValueFlag(false);

	return l_hrOk;
}

SVTaskObjectClass::~SVTaskObjectClass()
{
	SVTaskObjectClass::DestroyFriends();

	assert(0 == m_inputs.size());
}

bool SVTaskObjectClass::resetAllObjects(SvStl::MessageContainerVector* pErrorMessages/*=nullptr */)
{
	clearTaskMessages();

	bool Result = (S_OK == updateImageExtent(true));

	// Notify friends...
	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		if (nullptr != m_friendList[i])
		{
			Result &= m_friendList[i]->resetAllObjects(&m_ResetErrorMessages);
		}
	}

	Result &= resetAllOutputListObjects(&m_ResetErrorMessages);
	Result &= __super::resetAllObjects(&m_ResetErrorMessages);

	m_embeddedFormulaLinked.clear();
	for (auto* pObject : m_embeddedList)
	{
		if (nullptr != pObject && SvPb::LinkedValueClassId == pObject->GetClassID())
		{
			auto* pLinkedObject = dynamic_cast<SvVol::LinkedValue*>(pObject);
			if (nullptr != pLinkedObject && SvPb::LinkedSelectedOption::Formula == pLinkedObject->getSelectedOption())
			{
				m_embeddedFormulaLinked.push_back(pLinkedObject);
				Result = pLinkedObject->resetAllObjects(pErrorMessages) && Result;
			}
		}
	}

	if (nullptr != pErrorMessages && !m_ResetErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_ResetErrorMessages.begin(), m_ResetErrorMessages.end());
	}

	return Result;
}

void SVTaskObjectClass::getOutputList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter) const
{
	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		if (nullptr != m_friendList[i])
		{
			m_friendList[i]->getOutputList(inserter);
		}
	}

	__super::getOutputList(inserter);
}

void SVTaskObjectClass::fixInvalidInputs(std::back_insert_iterator<std::vector<SvPb::FixedInputData>> inserter)
{
	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		if (nullptr != m_friendList[i])
		{
			m_friendList[i]->fixInvalidInputs(inserter);
		}
	}

	for (auto* pObject : m_inputs)
	{
		if (nullptr != pObject)
		{
			pObject->fixInvalidInputs(inserter);
		}
	}

	__super::fixInvalidInputs(inserter);
}

std::vector<SvOi::IObjectClass*> SVTaskObjectClass::getOutputListFiltered(UINT uiAttributes, bool bAND) const
{
	std::vector<SvOi::IObjectClass*> outputList;
	getOutputList(std::back_inserter(outputList));
	outputList.erase(std::remove_if(outputList.begin(), outputList.end(), [bAND, uiAttributes](auto* pObject)
	{return bAND ? (pObject->ObjectAttributesSet() & uiAttributes) != uiAttributes // AND
		: 0 == (pObject->ObjectAttributesSet() & uiAttributes); }), outputList.end());

	return outputList;
}

void SVTaskObjectClass::GetOutputListFiltered(SVObjectReferenceVector& rObjectList, UINT uiAttributes, bool bAND) const
{
	std::vector<SvOi::IObjectClass*> outputList;
	getOutputList(std::back_inserter(outputList));
	for (auto* pIObject : outputList)
	{
		SVObjectReference ObjectRef {dynamic_cast<SVObjectClass*>(pIObject)};
		if (ObjectRef.getObject())
		{
			if (!ObjectRef.isArray())
			{
				bool bAttributesOK = bAND ? (ObjectRef.ObjectAttributesSet() & uiAttributes) == uiAttributes // AND
					: (ObjectRef.ObjectAttributesSet() & uiAttributes) > 0;            // OR
				if (bAttributesOK)
				{
					rObjectList.push_back(ObjectRef);
				}
			}
			else
			{
				int iArraySize = ObjectRef.getValueObject()->getArraySize();
				for (int j = 0; j < iArraySize; j++)
				{
					ObjectRef.SetArrayIndex(j);
					bool bAttributesOK = bAND ? (ObjectRef.ObjectAttributesSet() & uiAttributes) == uiAttributes // AND
						: (ObjectRef.ObjectAttributesSet() & uiAttributes) > 0;            // OR
					if (bAttributesOK)
					{
						rObjectList.push_back(ObjectRef);
					}
				}

			}
		}
	}
}

void SVTaskObjectClass::removeTaskMessage(DWORD MessageCode, SvStl::MessageTextEnum AdditionalTextId)
{
	auto it = std::find_if(m_ResetErrorMessages.begin(), m_ResetErrorMessages.end(),
		[&](SvStl::MessageContainer& mc)
	{
		return ((mc.getMessage().m_MessageCode == MessageCode) && (mc.getMessage().m_AdditionalTextId == AdditionalTextId));

	});
	if (it != m_ResetErrorMessages.end())
	{
		m_ResetErrorMessages.erase(it);
	}
	auto it2 = std::find_if(m_RunErrorMessages.begin(), m_RunErrorMessages.end(),
		[&](SvStl::MessageContainer& mc)
	{
		return ((mc.getMessage().m_MessageCode == MessageCode) && (mc.getMessage().m_AdditionalTextId == AdditionalTextId));
	});
	if (it2 != m_RunErrorMessages.end())
	{
		m_RunErrorMessages.erase(it2);
	}
}

HRESULT SVTaskObjectClass::SetValuesForAnObject(uint32_t aimObjectID, SVObjectAttributeClass* pDataObject)
{
	return __super::SetValuesForAnObject(aimObjectID, pDataObject);
}

void SVTaskObjectClass::DestroyFriends()
{
	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		if (nullptr != m_friendList[i])
		{
			auto* pOwner = dynamic_cast<SVTaskObjectClass*>(m_friendList[i]->GetParent());
			if (pOwner)
			{
				// Close, Disconnect and Delete Friend...
				pOwner->DestroyFriend(m_friendList[i]);
			}
			else
			{
				// Friend has no owner...
				m_friendList[i]->CloseObject();
				delete m_friendList[i];
			}
		}
	}
	m_friendList.RemoveAll();
}

SVObjectClass* SVTaskObjectClass::GetFriend(const SvDef::SVObjectTypeInfoStruct& rObjectType) const
{
	// Check if friend is already applied...
	for (int i = 0; i < static_cast<int>(m_friendList.size()); i++)
	{
		if (nullptr != m_friendList[i])
		{
			const SvDef::SVObjectTypeInfoStruct& rInfoStruct = m_friendList[i]->getObjectTypeInfo();
			if (rInfoStruct.m_ObjectType == rObjectType.m_ObjectType && rInfoStruct.m_SubType == rObjectType.m_SubType)
			{
				if (SvPb::NoEmbeddedId == rInfoStruct.m_EmbeddedID || rInfoStruct.m_EmbeddedID == rObjectType.m_EmbeddedID)
				{
					return m_friendList[i];
				}
			}
		}
	}
	return nullptr;
}

HRESULT SVTaskObjectClass::GetChildObject(SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index) const
{
	HRESULT l_Status = SVObjectAppClass::GetChildObject(rpObject, rNameInfo, Index);

	if (S_OK != l_Status)
	{
		if (static_cast<const size_t> (Index) < rNameInfo.m_NameArray.size() && rNameInfo.m_NameArray[Index] == GetName())
		{
			// Notify friends...
			for (size_t i = 0; nullptr == rpObject && i < m_friendList.size(); ++i)
			{
				if (nullptr != m_friendList[i])
				{
					l_Status = m_friendList[i]->GetChildObject(rpObject, rNameInfo, Index + 1);
				}
			}
		}
		else
		{
			l_Status = S_FALSE;
		}
	}

	return l_Status;
}

void SVTaskObjectClass::fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType, bool stopIfClosed /*= false*/, bool /*firstObject = false*/) const
{
	nameToType = (SvPb::SVNotSetObjectType == nameToType) ? GetObjectType() : nameToType;
	__super::fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, nameToType, requiredType, stopIfClosed);

	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		if (nullptr != m_friendList[i])
		{
			m_friendList[i]->fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, nameToType, requiredType, stopIfClosed);
		}
	}
}

void SVTaskObjectClass::fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden /*= false*/, bool stopIfClosed /*= false*/, bool /*firstObject = false*/)
{
	__super::fillObjectList(inserter, rObjectInfo, addHidden, stopIfClosed);

	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		if (nullptr != m_friendList[i])
		{
			m_friendList[i]->fillObjectList(inserter, rObjectInfo, addHidden, stopIfClosed);
		}
	}
}

#pragma region virtual method (ITaskObject)
void SVTaskObjectClass::GetInputs(SvPb::InputDataList& rList, const SvDef::SVObjectTypeInfoStruct& typeInfo, SvPb::SVObjectTypeEnum objectTypeToInclude, bool shouldExcludeFirstObjectName, int maxNumbers)
{
	std::vector<SvOl::InputObject*> inputList;
	getInputs(std::back_inserter(inputList));
	for (const auto* pInput : inputList)
	{
		if (nullptr != pInput && (SvPb::SVNotSetObjectType == typeInfo.m_ObjectType ||
			(typeInfo.m_ObjectType == pInput->GetInputObjectInfo().m_ObjectTypeInfo.m_ObjectType &&
			(SvPb::SVNotSetSubObjectType == typeInfo.m_SubType || typeInfo.m_SubType == pInput->GetInputObjectInfo().m_ObjectTypeInfo.m_SubType)))
			&& SvPb::AuxImageInputEId != pInput->GetEmbeddedID()) //@TODO[MZA][10.10][07.12.2020] This is needed because some GUI take the first input and this should not be the AuxImage. Maybe we can make it better than yet.
		{
			SvOi::IObjectClass* pObject = dynamic_cast<SvOi::IObjectClass*> (pInput->GetInputObjectInfo().getObject());
			std::string name = "";
			uint32_t objectID = SvDef::InvalidObjectId;
			if (pInput->IsConnected() && nullptr != pObject)
			{
				SvOi::ISVImage* pImage = dynamic_cast <SvOi::ISVImage*>(pObject);
				if (nullptr != pImage)
				{
					name = pImage->getDisplayedName();
				}
				else
				{
					if (SvPb::SVNotSetObjectType == objectTypeToInclude)
					{
						name = pObject->GetName();
					}
					else
					{
						if (shouldExcludeFirstObjectName)
						{
							name = pObject->GetObjectNameBeforeObjectType(objectTypeToInclude);
						}
						else
						{
							name = pObject->GetObjectNameToObjectType(objectTypeToInclude);
						}
					}
				}
				objectID = pObject->getObjectId();
			}
			auto* pData = rList.Add();
			pData->set_inputname(pInput->GetName());
			pData->set_inputid(pInput->getObjectId());
			pData->set_connected_objectid(objectID);
			pData->set_connected_objectdottedname(name);
			if (0 < maxNumbers && rList.size() >= maxNumbers)
			{
				return;
			}
		}
	}
}

SvStl::MessageContainerVector SVTaskObjectClass::validateAndSetEmbeddedValues(const SvOi::SetValueStructVector& rValueVector, bool shouldSet, SvOi::ResetParameter* pPar)
{
	SvStl::MessageContainerVector messages;
	HRESULT Result(S_OK);
	if (nullptr != pPar)
	{
		pPar->result = SvPb::RT_None;
	}
	SvOi::SVResetItemEnum ToReset {SvOi::SVResetItemNone};
	bool checkReset {false};
	if (nullptr != pPar)
	{
		switch (pPar->target)
		{
			case SvPb::RT_IP:
				ToReset = SvOi::SVResetItemIP; //0
				break;
			case SvPb::RT_Tool:
				ToReset = SvOi::SVResetItemTool; //1 
				break;
			case SvPb::RT_OWNER:
				ToReset = SvOi::SVResetItemOwner; //2 
				break;
			case SvPb::RT_FromObject:
				checkReset = true;
				break;
		}
	}
	

	for (auto const& rEntry : rValueVector)
	{
		try
		{
			switch (rEntry.index())
			{
				case SvOi::IVal:
				{
					auto& rData = std::get<SvOi::IVal>(rEntry);
					if (nullptr != rData.m_pValueObject)
					{
						rData.m_pValueObject->validateValue(rData.m_Value, rData.m_DefaultValue);
					}
					else
					{
						SvStl::MessageContainer Msg(SVMSG_SVO_NULL_POINTER, SvStl::Tid_Default, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
						throw Msg;
					}
					
					SvOi::IObjectClass* pObj = dynamic_cast<SvOi::IObjectClass*>(rData.m_pValueObject);
					if (pObj && pObj->GetParentID() != getObjectId())
					{
						SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_WrongParentForEmbeddetValue, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
						throw Msg;
					}
					break;
				}
				case SvOi::ILink:
					auto& rData = std::get<SvOi::ILink>(rEntry);
					if (nullptr != rData.m_pValueObject)
					{
						rData.m_pValueObject->validateValue(rData.m_linkedData);
					}
					else
					{
						SvStl::MessageContainer Msg(SVMSG_SVO_NULL_POINTER, SvStl::Tid_Default, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
						throw Msg;
					}
					//SvOi::IObjectClass* pObj = (SvOi::IObjectClass*)(rData.m_pValueObject);
					SvOi::IObjectClass* pObj = dynamic_cast<SvOi::IObjectClass*>(rData.m_pValueObject);
					if (pObj && pObj->GetParentID() != getObjectId())
					{
							
						SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_WrongParentForEmbeddetValue, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
						throw Msg;
					}
					
					break;
			}

		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			messages.push_back(rSvE);
			Result = rSvE.getMessage().m_MessageCode;
		}
	}

	if (shouldSet && S_OK == Result)
	{
		for (auto const& rEntry : rValueVector)
		{
			SvOi::IValueObject* pValueObject = nullptr;
			switch (rEntry.index())
			{
				case SvOi::IVal:
					Result = setEmbeddedValue(std::get<SvOi::IVal>(rEntry), std::back_inserter(messages));
					if (checkReset)
					{
						pValueObject = std::get<SvOi::IVal>(rEntry).m_pValueObject;
					}
					break;
				case SvOi::ILink:
					Result = setEmbeddedValue(std::get<SvOi::ILink>(rEntry), std::back_inserter(messages));
					if (checkReset)
					{
						pValueObject = dynamic_cast<SvOi::IValueObject*>(std::get<SvOi::ILink>(rEntry).m_pValueObject);
					}

					break;
			}

			if (checkReset && nullptr != pValueObject)
			{
				auto ResetItem = pValueObject->getResetItem();

				ToReset = (ResetItem < ToReset) ? ResetItem : ToReset;
				if (ToReset == SvOi::SVResetItemIP)
				{
					checkReset = false;
				}

			}
		}
	}

	if (shouldSet && S_OK == Result && pPar)
	{

		switch (ToReset)
		{
			case SvOi::SVResetItemIP:
				GetInspection()->resetAllObjects();
				pPar->result = SvPb::RT_IP;
				break;
			case SvOi::SVResetItemTool:
				GetTool()->resetAllObjects();
				pPar->result = SvPb::RT_Tool;
				break;
			case SvOi::SVResetItemOwner:
				resetAllObjects();
				pPar->result = SvPb::RT_OWNER;
				break;

			default:
				pPar->result = SvPb::RT_None;
				break;
		}
	}

	return messages;
}

void SVTaskObjectClass::ResolveDesiredInputs(const SvDef::SVObjectTypeInfoVector& rDesiredInputs)
{
	// Apply desired input interface...
	for (int i = 0; i < rDesiredInputs.size() && i < m_inputs.size(); ++i)
	{
		auto* pInInfo = *(m_inputs.begin() + i);

		const SvDef::SVObjectTypeInfoStruct& rDesiredInType = rDesiredInputs[i];

		if (pInInfo)
		{
			pInInfo->SetInputObjectType(rDesiredInType);
		}

		const SvDef::SVObjectTypeInfoStruct& rInfo = pInInfo->GetInputObjectInfo().m_ObjectTypeInfo;

		assert(SvDef::InvalidObjectId != pInInfo->GetInputObjectInfo().getObjectId() ||
			SvPb::NoEmbeddedId != rInfo.m_EmbeddedID ||
			SvPb::SVNotSetObjectType != rInfo.m_ObjectType ||
			SvPb::SVNotSetSubObjectType != rInfo.m_SubType);
		UNREFERENCED_PARAMETER(rInfo);
	}
}

SvStl::MessageContainer SVTaskObjectClass::getFirstTaskMessage() const
{
	if (0 < m_ResetErrorMessages.size())
	{
		return m_ResetErrorMessages[0];
	}

	return SvStl::MessageContainer();
}

bool SVTaskObjectClass::getImage(SvPb::EmbeddedIdEnum embeddedId, SvOi::SVImageBufferHandlePtr& rImagePtr) const
{
	auto iter = std::ranges::find_if(m_embeddedList, [embeddedId](auto* pObject) { return nullptr != pObject && embeddedId == pObject->GetEmbeddedID(); });
	if (m_embeddedList.end() != iter)
	{
		const SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*>(*iter);
		if (pImage)
		{
			if (SvPb::SVImageTypeEnum::SVImageTypePhysical == pImage->GetImageType())
			{
				rImagePtr = pImage->GetParentImageInterface()->getLastImage();
			}
			else
			{
				rImagePtr = pImage->getLastImage();
			}
			return true;
		}
	}
	return false;
}

std::vector<uint32_t> SVTaskObjectClass::getEmbeddedList() const
{
	std::vector<uint32_t> Result(m_embeddedList.size());
	std::transform(m_embeddedList.begin(), m_embeddedList.end(), Result.begin(), [](const auto& rEntry) { return rEntry->getObjectId(); });
	return Result;
}

bool SVTaskObjectClass::AddFriend(uint32_t friendId, uint32_t addPreId)
{
	// Check ID...
	if (SvDef::InvalidObjectId == friendId)
	{
		return false;
	}

	auto* pNewFriend = dynamic_cast<SVTaskObjectClass*>(SVObjectManagerClass::Instance().GetObject(friendId));
	if (nullptr != pNewFriend)
	{
		return addFriend(*pNewFriend, addPreId);
	}
	return false;
}

bool SVTaskObjectClass::AddFriend(SVTaskObjectClass* pFriend, uint32_t addPreId)
{
	if (nullptr != pFriend)
	{
		return addFriend(*pFriend, addPreId);
	}
	return false;
}

void SVTaskObjectClass::moveFriendObject(uint32_t objectToMoveId, uint32_t preObjectId)
{
	int currentPos = -1;
	int newPos = -1;
	for (int i = 0; i < m_friendList.size(); i++)
	{
		if (nullptr != m_friendList[i] && m_friendList[i]->getObjectId() == objectToMoveId)
		{
			currentPos = i;
		}
		if (nullptr != m_friendList[i] && m_friendList[i]->getObjectId() == preObjectId)
		{
			newPos = i;
		}
	}

	if (0 <= currentPos && m_friendList.size() > currentPos)
	{
		auto* pObject = m_friendList[currentPos];
		//change first object which is later in the list.
		if (currentPos > newPos)
		{
			m_friendList.RemoveAt(currentPos);
			m_friendList.Insert(newPos, pObject);
		}
		else
		{
			m_friendList.Insert(newPos, pObject);
			m_friendList.RemoveAt(currentPos);
		}
	}
}
#pragma endregion virtual method (ITaskObject)

void SVTaskObjectClass::DestroyFriend(SVObjectClass* pObject)
{
	// This was Added because SVTaskObjectClasses can also have Friends
	SVTaskObjectClass* pTaskObject = dynamic_cast<SVTaskObjectClass*>(pObject);

	// Close the Friend Object
	if (pTaskObject)
	{
		// Notify the Owner(s) of our Friends inputs that they are not needed anymore
		pTaskObject->disconnectAllInputs();

		// Close the Object
		pTaskObject->CloseObject();

		// Destroy our Friend's Friends
		pTaskObject->DestroyFriends();

		// Delete the Friend Object
		delete pTaskObject;
	}
}

SvOi::IObjectClass* SVTaskObjectClass::getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends, const SvOi::IObjectClass* pRequestor) const
{
	SvOi::IObjectClass* Result = nullptr;
	if (useFriends)
	{
		for (size_t i = 0; i < m_friendList.size(); ++i)
		{
			if (nullptr != m_friendList[i])
			{
				Result = m_friendList[i]->getFirstObject(rObjectTypeInfo, true, pRequestor);
				if (nullptr != Result)
				{
					return Result;
				}
			}
		}
	}

	if (pRequestor && pRequestor == this || pRequestor == GetParent())
	{
		// Do not reference self 
		return nullptr;
	}

	return __super::getFirstObject(rObjectTypeInfo, useFriends, pRequestor);
}

void SVTaskObjectClass::OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName)
{
	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		if (nullptr != m_friendList[i])
		{
			m_friendList[i]->OnObjectRenamed(rRenamedObject, rOldName);
		}
	}

	for (SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = *Iter;

		if (nullptr != pObject)
		{
			pObject->OnObjectRenamed(rRenamedObject, rOldName);
		}
	}
}

bool SVTaskObjectClass::isInputImage(uint32_t imageId) const
{
	bool Result(false);

	if (SvDef::InvalidObjectId != imageId)
	{
		// Notify friends...
		for (size_t i = 0; !Result && i < m_friendList.size(); ++i)
		{
			if (nullptr != m_friendList[i] && m_friendList[i]->GetParent() == this)
			{
				Result = m_friendList[i]->isInputImage(imageId);
			}
		}

		// Notify embeddeds...
		for (SVObjectPtrVector::const_iterator Iter = m_embeddedList.begin(); !Result && m_embeddedList.end() != Iter; ++Iter)
		{
			if (nullptr != *Iter)
			{
				Result = (*Iter)->isInputImage(imageId);
			}
		}
	}

	return Result;
}

bool SVTaskObjectClass::DoesObjectHaveExtents() const
{
	return false;
}


HRESULT SVTaskObjectClass::SetImageExtentToParent()
{
	return S_FALSE;
}

HRESULT SVTaskObjectClass::SetImageExtentToFit(const SVImageExtentClass&)
{
	return S_FALSE;
}

HRESULT SVTaskObjectClass::GetPropertyInfo(SvPb::SVExtentPropertyEnum, SVExtentPropertyInfoStruct&) const
{
	return S_FALSE;
}

void SVTaskObjectClass::connectInput(SvOi::IObjectClass* pInput)
{
	auto* pInputObject = dynamic_cast<SvOl::InputObject*>(pInput);
	if (nullptr != pInputObject)
	{
		connectInput(*pInputObject);
	}
}

void SVTaskObjectClass::connectInput(SvOl::InputObject& rInput)
{
	// Is not yet connected...
	if (!rInput.IsConnected())
	{
		// if Connect to Default
		if (SvDef::InvalidObjectId == rInput.GetInputObjectInfo().getObjectId())
		{
			// Input Object is not set...Try to get one...
			SvDef::SVObjectTypeInfoStruct info = rInput.GetInputObjectInfo().m_ObjectTypeInfo;
			// At least one item from the SvDef::SVObjectTypeInfoStruct is required, but not all
			if (SvPb::NoEmbeddedId != info.m_EmbeddedID || SvPb::SVNotSetObjectType != info.m_ObjectType || SvPb::SVNotSetSubObjectType != info.m_SubType)
			{
				SVObjectClass* pOwner = GetParent();
				SVObjectClass* pRequestor = this;
				bool bSuccess = false;

				// Ask first friends...
				for (size_t j = 0; j < m_friendList.size(); ++j)
				{
					if (nullptr != m_friendList[j])
					{
						const auto* pObject = SVObjectManagerClass::Instance().getFirstObject(m_friendList[j]->getObjectId(), info);
						if (nullptr != pObject)
						{
							// Connect input ...
							rInput.SetInputObject(pObject->getObjectId());
							bSuccess = true;
							break;
						}
					}
				}

				// Then ask owner...
				if (!bSuccess)
				{
					//GetInspection is still nullptr because in SVToolSet::createAllObjectsFromChild SetDefaultInputs is called before CreateObject !
					//To still get the appropriate inspection we call GetAncestor
					SvOi::IInspectionProcess* pInspection = dynamic_cast<SvOi::IInspectionProcess*> (GetAncestor(SvPb::SVInspectionObjectType));
					while (pOwner)
					{
						bool isColorInspection = (nullptr != pInspection) ? pInspection->IsColorCamera() : false;
						const SvOi::IObjectClass* pObject {nullptr};
						// if color system & pOwner == SVToolSet
						if (isColorInspection && SvPb::SVToolSetObjectType == pOwner->GetObjectType() && SvPb::SVImageObjectType == info.m_ObjectType && SvPb::SVImageMonoType == info.m_SubType)
						{
							SvOi::IToolSet* pToolSet(dynamic_cast<SvOi::IToolSet*> (pOwner));
							if (nullptr != pToolSet)
							{
								pObject = pToolSet->getBand0Image();
							}
						}
						else
						{
							pObject = pOwner->getFirstObject(info, true, pRequestor);
						}
						if (pObject)
						{
							// Connect input ...
							rInput.SetInputObject(pObject->getObjectId());
							break;
						}
						else
						{
							pOwner = pOwner->GetParent();
							pRequestor = pRequestor->GetParent();
						}
					}// end while (pOwner)
				}// end if (! bSuccess)
			}
		}// end if (SvDef::InvalidObjectId == pInInfo->InputObjectInfo.UniqueObjectID )
	}
}

bool SVTaskObjectClass::connectAllInputs()
{
	bool Result(true);

	// tell friends to connect...
	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		if (nullptr != m_friendList[j])
		{
			m_friendList[j]->connectAllInputs();
		}
	}

	// find our inputs
	for (auto pInput : m_inputs)
	{
		if (nullptr != pInput)
		{
			connectInput(*pInput);
		}
		else
		{
			Result = false;
		}
	}
	return Result;
}

SvStl::MessageContainerVector SVTaskObjectClass::getErrorMessages() const
{
	SvStl::MessageContainerVector list = m_ResetErrorMessages;
	list.insert(list.end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	return list;
};

HRESULT SVTaskObjectClass::ConnectToObject(const std::string& rInputName, uint32_t newID, SvPb::SVObjectTypeEnum objectType /*= SvPb::SVNotSetObjectType*/, bool shouldResetObject /*= false*/)
{
	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		if (nullptr != m_friendList[j])
		{
			HRESULT result = m_friendList[j]->ConnectToObject(rInputName, newID, objectType, shouldResetObject);
			if (S_OK == result)
			{
				return result;
			}
		}
	}

	HRESULT result = S_OK;
	auto it = std::find_if(m_inputs.begin(), m_inputs.end(), [rInputName](const auto* pEntry) { return rInputName == pEntry->GetName(); });
	if (it != m_inputs.end() && nullptr != *it)
	{
		SVObjectClass* pNewObject = SVObjectManagerClass::Instance().GetObject(newID);
		if (nullptr != pNewObject)
		{
			if (SvPb::SVNotSetObjectType == objectType || pNewObject->GetObjectType() == objectType)
			{
				(*it)->SetInputObject(newID);
			}
			else
			{
				auto* pLinkedValue = dynamic_cast<SvVol::LinkedValue*>(pNewObject);
				if (nullptr != pLinkedValue)
				{
					try
					{
						auto* pObject = pLinkedValue->getLinkedObject();
						if (nullptr != pObject && pObject->GetObjectType() == objectType)
						{
							(*it)->SetInputObject(newID);
						}
						else
						{
							result = E_FAIL;
						}
					}
					catch (const SvStl::MessageContainer& rExp)
					{
						SvStl::MessageManager Exception(SvStl::MsgType::Log);
						Exception.setMessage(rExp.getMessage());
						result = E_FAIL;
					}
				}
				else
				{
					result = E_POINTER;
				}
			}
			if (shouldResetObject && S_OK == result)
			{
				SVObjectClass* pTool = GetTool();
				if (nullptr != pTool)
				{
					pTool->resetAllObjects();
				}
			}
		}
		else
		{
			result = E_POINTER;
		}
	}
	else
	{
		result = E_INVALIDARG;
	}
	return result;
}

bool SVTaskObjectClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool Result = SVObjectAppClass::CreateObject(rCreateStructure);

	Result &= (S_OK == updateImageExtent(true));

	// Create our friends
	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		if (nullptr != m_friendList[j])
		{
			Result = CreateChildObject(m_friendList[j]) && Result;

			assert(Result);
		}
		else
		{
			Result = false;
			assert(Result);
		}
	}

	Result = createEmbeddedChildren() && Result;

	for (auto* pInput : m_inputs)
	{
		if (pInput)
		{
			pInput->SetInputObject(pInput->GetInputObjectInfo().getObjectId());
			Result = CreateChildObject(pInput) && Result;
			assert(Result);
		}
	}

	constexpr UINT cAttribute {SvDef::selectableAttributes | SvPb::audittrail};
	m_statusTag.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_statusColor.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);

	m_isCreated = Result;

	return Result;
}

bool SVTaskObjectClass::RegisterEmbeddedImage(SVImageClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, int StringResourceID)
{
	std::string Name = SvUl::LoadStdString(StringResourceID);
	return RegisterEmbeddedImage(pEmbeddedObject, embeddedID, Name.c_str());
}

bool SVTaskObjectClass::RegisterEmbeddedImage(SVImageClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, LPCTSTR newString)
{
	bool l_bOk = nullptr != pEmbeddedObject && RegisterEmbeddedObjectAsClass(pEmbeddedObject, embeddedID, newString);

	return l_bOk;
}

SVObjectClass* SVTaskObjectClass::overwriteInputObject(uint32_t uniqueId, SvPb::EmbeddedIdEnum embeddedId)
{
	auto iter = std::find_if(m_inputs.begin(), m_inputs.end(), [embeddedId](const auto* pInput) { return pInput && pInput->GetEmbeddedID() == embeddedId; });
	if (m_inputs.end() != iter)
	{
		if (SvDef::InvalidObjectId != uniqueId)
		{
			SVObjectManagerClass::Instance().ChangeUniqueObjectID(*iter, uniqueId);
		}
		return *iter;
	}
	return nullptr;
}

void SVTaskObjectClass::MovedEmbeddedObject(SVObjectClass* pToMoveObject, SVObjectClass* pPosObject)
{
	auto currentIter = std::find(m_embeddedList.begin(), m_embeddedList.end(), pToMoveObject);
	auto newPosIter = std::find(m_embeddedList.begin(), m_embeddedList.end(), pPosObject);
	if (currentIter != newPosIter && m_embeddedList.end() != currentIter && m_embeddedList.end() != newPosIter)
	{
		//change first object which is later in the list.
		if (currentIter > newPosIter)
		{
			m_embeddedList.erase(currentIter);
			m_embeddedList.insert(newPosIter, pToMoveObject);
		}
		else
		{
			m_embeddedList.insert(newPosIter, pToMoveObject);
			m_embeddedList.erase(currentIter);
		}
	}
}

bool SVTaskObjectClass::resetAllOutputListObjects(SvStl::MessageContainerVector* pErrorMessages/*=nullptr */)
{
	bool Result = true;

	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		if (nullptr != m_friendList[j])
		{
			//return-value and error-messages do not be saved here, because this object will call resetAllOutputListObjects by its own and return error message to the parents.
			//this call here is important to reset (resize) the embedded images, so the parents can use it for its reset.
			m_friendList[j]->resetAllOutputListObjects();
		}
	}

	// Try to send message to outputObjectList members
	for (SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = *Iter;

		if (nullptr != pObject && pObject->ObjectAttributesAllowed())
		{
			//the error of this embedded objects must be saved by this object.
			Result = pObject->resetAllObjects(pErrorMessages) && Result;
		}
	}

	return Result;
}

bool SVTaskObjectClass::registerInputObject(SvOl::InputObject* pInputObject, const std::string& rInputName, SvPb::EmbeddedIdEnum embeddedId)
{
	if (nullptr != pInputObject)
	{
		assert(m_inputs.end() == std::find_if(m_inputs.begin(), m_inputs.end(), [embeddedId](const auto* pInput) { return pInput->GetEmbeddedID() == embeddedId; }));
		pInputObject->SetObjectEmbedded(embeddedId, this, rInputName.c_str());
		if (m_inputs.end() == std::find(m_inputs.begin(), m_inputs.end(), pInputObject))
		{
			m_inputs.push_back(pInputObject);
		}
		return true;
	}
	return false;
}

// Override this function to implement object behavior...
void SVTaskObjectClass::SetDisabled()
{
	// Set all embeddeds ( outputs ) to disabled...
	for (SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = *Iter;
		if (nullptr != pObject)
		{
			pObject->SetDisabled();
		}
	}

	// Set yourself to disabled...

	// Special implementation here:
	// Set just your object color to disabled...
	m_statusColor.SetValue(SvDef::DefaultDisabledColor);

	// Set friends to disabled...
	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		if (nullptr != m_friendList[j])
		{
			m_friendList[j]->SetDisabled();
		}
	}
}

DWORD SVTaskObjectClass::GetObjectColor() const
{
	DWORD dwColor;
	m_statusColor.GetValue(dwColor);
	return dwColor;
}

DWORD SVTaskObjectClass::GetStatusTag() const
{
	DWORD dwState;
	m_statusTag.GetValue(dwState);
	return dwState;
}

void SVTaskObjectClass::Persist(SvOi::IObjectWriter& rWriter) const
{
	__super::Persist(rWriter);

	PersistFriends(rWriter);
	PersistInputs(rWriter);
}

void SVTaskObjectClass::PersistFriends(SvOi::IObjectWriter& rWriter) const
{
	// Get script of close friends list members...
	if (m_friendList.size())
	{
		rWriter.StartElement(scFriendsTag);

		for (size_t j = 0; j < m_friendList.size(); ++j)
		{
			if (nullptr != m_friendList[j])
			{
				rWriter.StartElement(m_friendList[j]->GetObjectName()); // use internal name for node name
				m_friendList[j]->Persist(rWriter);
				rWriter.EndElement();
			}
		}
		rWriter.EndElement();
	}
}

void SVTaskObjectClass::PersistInputs(SvOi::IObjectWriter& rWriter) const
{
	// Set up input list...
	if (0 < m_inputs.size())
	{
		rWriter.StartElement(scInputsTag);
		for (const auto* pInput : m_inputs)
		{
			if (pInput)
			{
				pInput->Persist(rWriter);
			}
		}
		rWriter.EndElement();
	}
}

bool SVTaskObjectClass::Run(RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages)
{
	clearRunErrorMessages();

	// Run yourself...
	bool bRetVal = onRun(rRunStatus, &m_RunErrorMessages);

	setStatus(rRunStatus);

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}

	return bRetVal;
}

bool SVTaskObjectClass::onRun(RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages)
{
	bool bRetVal = (S_OK == updateImageExtent(false));

	for (auto* pLinkedObject : m_embeddedFormulaLinked)
	{
		if (pLinkedObject)
		{
			bRetVal = pLinkedObject->runEmbedded(rRunStatus, pErrorMessages) && bRetVal;
		}
	}

	// Run first friends...
	bRetVal &= runFriends(rRunStatus, pErrorMessages);


	// Now Validate yourself...
	if (!bRetVal || !isErrorMessageEmpty())
	{
		rRunStatus.SetInvalid();
		bRetVal = false;
	}

	return bRetVal;
}

bool SVTaskObjectClass::runFriends(RunStatus& rRunStatus, SvStl::MessageContainerVector* pErrorMessages)
{
	bool bRetVal = true;

	// Run your friends
	size_t j = m_bSkipFirstFriend ? 1 : 0;
	for (; j < m_friendList.size(); ++j)
	{
		if (nullptr != m_friendList[j])
		{
			bRetVal = m_friendList[j]->Run(rRunStatus, pErrorMessages) && bRetVal;
		}
		else
		{
			bRetVal = false;
		}
	}
	return bRetVal;
}

void SVTaskObjectClass::disconnectAllInputs()
{
	__super::disconnectAllInputs();
	for (auto* pInput : m_inputs)
	{
		pInput->disconnectAllInputs();
	}

	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		if (nullptr != m_friendList[j])
		{
			m_friendList[j]->disconnectAllInputs();
		}
	}

	for (auto* pObject : m_embeddedList)
	{
		if (nullptr != pObject)
		{
			pObject->disconnectAllInputs();
		}
	}
}

void SVTaskObjectClass::getInputs(std::back_insert_iterator<std::vector<SvOl::InputObject*>> inserter) const
{
	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		if (nullptr != m_friendList[j])
		{
			m_friendList[j]->getInputs(inserter);
		}
	}

	// Local input list...
	std::copy(m_inputs.begin(), m_inputs.end(), inserter);
}

// Added to process friends
bool SVTaskObjectClass::CloseObject()
{
	// Close ourself first
	bool Result = SVObjectAppClass::CloseObject();

	// Close our Friends
	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		if (m_friendList[j])
		{
			// Close Friend...
			Result = m_friendList[j]->CloseObject() && Result;
		}
	}

	for (auto* pInput : m_inputs)
	{
		if (nullptr != pInput && pInput->IsCreated())
		{
			pInput->CloseObject();
		}
	}
	m_inputs.clear();

	return Result;
}

HRESULT SVTaskObjectClass::GetImageList(SVImageClassPtrVector& p_rImageList, UINT uiAttributes, bool bAND)
{
	HRESULT hr = S_OK;

	SvDef::SVObjectTypeInfoStruct  info;

	info.m_ObjectType = SvPb::SVImageObjectType;
	info.m_SubType = SvPb::SVNotSetSubObjectType;

	std::vector<SvOi::IObjectClass*> list;
	fillObjectList(std::back_inserter(list), info);

	for (auto* pObject : list)
	{
		SVImageClass* pImage = dynamic_cast<SVImageClass*>(pObject);

		if (nullptr != pImage)
		{
			bool bAttributesOK = bAND ? (pImage->ObjectAttributesSet() & uiAttributes) == uiAttributes // AND
				: (pImage->ObjectAttributesSet() & uiAttributes) > 0;            // OR
			if (bAttributesOK)
			{
				p_rImageList.push_back(pImage);
			}
		}
	}

	return hr;
}

HRESULT SVTaskObjectClass::RegisterSubObject(SVObjectClass* pObject)
{
	HRESULT Result(E_FAIL);
	SvOi::IValueObject* pValueObject(nullptr);

	if (nullptr != (pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject)))
	{
		m_ValueObjectSet.insert(pValueObject);
		Result = S_OK;
	}

	return Result;
}

HRESULT SVTaskObjectClass::UnregisterSubObject(SVObjectClass* pObject)
{
	HRESULT Result(E_FAIL);
	SvOi::IValueObject* pValueObject(nullptr);

	if (nullptr != (pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject)))
	{
		m_ValueObjectSet.erase(pValueObject);
		Result = S_OK;
	}

	return Result;
}

void SVTaskObjectClass::setEditModeFreezeFlag(bool flag)
{
	__super::setEditModeFreezeFlag(flag);
	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		if (nullptr != m_friendList[j])
		{
			m_friendList[j]->setEditModeFreezeFlag(flag);
		}
	}
	for (auto* pEmbedded : m_embeddedList)
	{
		if (nullptr != pEmbedded)
		{
			pEmbedded->setEditModeFreezeFlag(flag);
		}
	}
}

void SVTaskObjectClass::copiedSavedImage(SvOi::ITriggerRecordRWPtr pTr)
{
	__super::copiedSavedImage(pTr);
	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		if (nullptr != m_friendList[j])
		{
			m_friendList[j]->copiedSavedImage(pTr);
		}
	}
	for (auto* pEmbedded : m_embeddedList)
	{
		if (nullptr != pEmbedded)
		{
			pEmbedded->copiedSavedImage(pTr);
		}
	}
}

void SVTaskObjectClass::goingOffline()
{
	__super::goingOffline();
	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		if (nullptr != m_friendList[j])
		{
			m_friendList[j]->goingOffline();
		}
	}
	for (auto* pEmbedded : m_embeddedList)
	{
		if (nullptr != pEmbedded)
		{
			pEmbedded->goingOffline();
		}
	}
}

void SVTaskObjectClass::UpdateOverlayIDs(SVExtentMultiLineStruct& p_rMultiLine)
{
	p_rMultiLine.m_ObjectID = getObjectId();

	if (nullptr != GetAnalyzer())
	{
		p_rMultiLine.m_AnalyzerID = GetAnalyzer()->getObjectId();
	}
	else
	{
		p_rMultiLine.m_AnalyzerID = SvDef::InvalidObjectId;
	}

	if (nullptr != GetTool())
	{
		p_rMultiLine.m_ToolID = GetTool()->getObjectId();
	}
	else
	{
		p_rMultiLine.m_ToolID = SvDef::InvalidObjectId;
	}

	if (nullptr != GetInspection())
	{
		p_rMultiLine.m_InspectionID = GetInspection()->getObjectId();
	}
	else
	{
		p_rMultiLine.m_InspectionID = SvDef::InvalidObjectId;
	}
}

void SVTaskObjectClass::UpdateOverlayColor(SVExtentMultiLineStruct& p_rMultiLine) const
{
	p_rMultiLine.m_Color = GetObjectColor();
}

void SVTaskObjectClass::UpdateOverlayName(SVExtentMultiLineStruct& p_rMultiLine, const SVImageExtentClass& p_pImageExtents)
{
	SVPoint<double> point;

	if (S_OK == p_pImageExtents.GetTitlePoint(point))
	{
		p_rMultiLine.m_StringPoint = point;
		p_rMultiLine.m_csString = GetObjectNameBeforeObjectType(SvPb::SVToolSetObjectType);
	}
}

HRESULT SVTaskObjectClass::CollectOverlays(SVImageClass* p_Image, SVExtentMultiLineStructVector& p_MultiLineArray)
{
	HRESULT hrRet = S_OK;

	if (m_bUseOverlays)
	{
		hrRet = onCollectOverlays(p_Image, p_MultiLineArray);
	}

	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		if (nullptr != m_friendList[j])
		{
			HRESULT l_Temp = m_friendList[j]->CollectOverlays(p_Image, p_MultiLineArray);

			if (S_OK == hrRet)
			{
				hrRet = l_Temp;
			}
		}
	}

	return hrRet;
}

void SVTaskObjectClass::collectOverlays(const SVImageClass* pImage, SvPb::Overlay& rOverlay) const
{
	addOverlayGroups(pImage, rOverlay);

	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		if (nullptr != m_friendList[j])
		{
			m_friendList[j]->collectOverlays(pImage, rOverlay);
		}
	}
}

HRESULT SVTaskObjectClass::onCollectOverlays(SVImageClass* p_Image, SVExtentMultiLineStructVector& p_MultiLineArray)
{
	HRESULT l_Status = E_FAIL;
	// cppcheck-suppress nullPointer
	if (nullptr != p_Image && nullptr != GetImageExtentPtr() && GetImageExtentPtr()->hasFigure())
	{
		// cppcheck-suppress nullPointer
		const SVImageExtentClass& rImageExtents = *GetImageExtentPtr();
		SVExtentMultiLineStruct l_MultiLine;
		UpdateOverlayIDs(l_MultiLine);
		UpdateOverlayName(l_MultiLine, rImageExtents);
		SVTaskObjectClass* pTaskObject = dynamic_cast<SVTaskObjectClass*> (GetTool());
		if (nullptr != pTaskObject)
		{
			pTaskObject->UpdateOverlayColor(l_MultiLine);
			pTaskObject->GetDrawInfo(l_MultiLine);
		}
		else
		{
			UpdateOverlayColor(l_MultiLine);
			GetDrawInfo(l_MultiLine);
		}

		l_MultiLine.AssignExtentFigure(rImageExtents.GetFigure(), l_MultiLine.m_Color);
		p_MultiLineArray.push_back(l_MultiLine);
		l_Status = S_OK;

	}

	return l_Status;
}

SVObjectClass* SVTaskObjectClass::GetEmbeddedValueObject(SvPb::EmbeddedIdEnum embeddedID)
{
	SVObjectClass* pResult = nullptr;

	for (SVObjectPtrVector::const_iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = *Iter;
		if (nullptr != dynamic_cast<SvOi::IValueObject*> (pObject))
		{
			if (pObject->GetEmbeddedID() == embeddedID)
			{
				pResult = pObject;
				break;
			}
		}
	}

	return pResult;
}

void SVTaskObjectClass::setStatus(const RunStatus& state)
{
	setStatus(state.GetStatusColor(), state.GetState());
}

void SVTaskObjectClass::setStatus(DWORD color, DWORD state)
{
	m_statusColor.SetValue(color);
	m_statusTag.SetValue(state);
}

HRESULT SVTaskObjectClass::setEmbeddedValue(const SvOi::SetValueStruct& rEntry, std::back_insert_iterator<SvStl::MessageContainerVector> inserter)
{
	HRESULT Result = S_OK;
	if (nullptr != rEntry.m_pValueObject)
	{
		try
		{
			Result = rEntry.m_pValueObject->setDefaultValue(rEntry.m_DefaultValue);
			if (S_OK == Result)
			{
				Result = rEntry.m_pValueObject->setValue(rEntry.m_Value, rEntry.m_ArrayIndex);
			}
			else
			{
				assert(false);
			}
		}
		catch (...)
		{
			Result = E_FAIL;
		}
	}
	else
	{
		Result = E_POINTER;
	}
	if (S_OK != Result)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log);
		SvDef::StringVector msgList;
		SvOi::IObjectClass* pObject = dynamic_cast<SvOi::IObjectClass*> (rEntry.m_pValueObject);
		if (nullptr != pObject)
		{
			msgList.push_back(pObject->GetName());
		}
		//! Check if general error or specific error for detailed information
		if (E_FAIL == Result || S_FALSE == Result)
		{
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SetEmbeddedValueFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
		}
		else
		{
			Msg.setMessage(Result, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
		}
		// cppcheck-suppress unreadVariable symbolName=inserter ; cppCheck doesn't know back_insert_iterator
		inserter = Msg.getMessageContainer();
	}
	return Result;
}

HRESULT SVTaskObjectClass::setEmbeddedValue(const SvOi::SetLinkedStruct& rEntry, std::back_insert_iterator<SvStl::MessageContainerVector> inserter)
{
	HRESULT Result = S_OK;
	if (nullptr != rEntry.m_pValueObject)
	{
		try
		{
			Result = rEntry.m_pValueObject->setValue(rEntry.m_linkedData);
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			// cppcheck-suppress unreadVariable symbolName=inserter ; cppCheck doesn't know back_insert_iterator
			inserter = rSvE;
			return E_FAIL;
		}
		catch (...)
		{
			Result = E_FAIL;
		}
	}
	else
	{
		Result = E_POINTER;
	}
	if (S_OK != Result)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log);
		SvDef::StringVector msgList;
		SvOi::IObjectClass* pObject = dynamic_cast<SvOi::IObjectClass*> (rEntry.m_pValueObject);
		if (nullptr != pObject)
		{
			msgList.push_back(pObject->GetName());
		}
		//! Check if general error or specific error for detailed information
		if (E_FAIL == Result || S_FALSE == Result)
		{
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_SetEmbeddedValueFailed, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
		}
		else
		{
			Msg.setMessage(Result, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
		}
		// cppcheck-suppress unreadVariable symbolName=inserter ; cppCheck doesn't know back_insert_iterator
		inserter = Msg.getMessageContainer();
	}
	return Result;
}

void SVTaskObjectClass::addOrRemoveResetErrorMessage(SvStl::MessageContainer& rErrorMessage, bool add)
{
	if (add)
	{
		if (m_ResetErrorMessages.size() == 0)
		{
			m_ResetErrorMessages.push_back(rErrorMessage);
		}
		else
		{
			auto it = m_ResetErrorMessages.back();
			if (0 != std::string(rErrorMessage.what()).compare(it.what()))
			{
				m_ResetErrorMessages.push_back(rErrorMessage);
			}
		}
	}
	else if (m_ResetErrorMessages.size() > 0 && add == false)
	{
		auto it = m_ResetErrorMessages.back();
		if (0 == std::string(rErrorMessage.what()).compare(it.what()))
		{
			m_ResetErrorMessages.pop_back();
		}
	}

}

bool SVTaskObjectClass::addFriend(SVTaskObjectClass& rFriend, uint32_t addPreId)
{
	size_t position = m_friendList.size();
	// Check if friend is already applied...
	if (m_friendList.size())
	{
		for (int i = static_cast<int>(m_friendList.size()) - 1; i >= 0; --i)
		{
			if (nullptr != m_friendList[i] && m_friendList[i]->getObjectId() == rFriend.getObjectId())
			{
				return false;
			}

			if (nullptr != m_friendList[i] && m_friendList[i]->getObjectId() == addPreId)
			{
				position = i;
			}
		}
	}

	auto* pNewFriend = &rFriend;
	// Check if we are the Owner
// Note:: Special hack for friend scripting
// if we are the owner - it's not from the script
	SVObjectClass* pOwner = rFriend.GetParent();

	if (pOwner != this)
	{
		auto* pTaskOwner = dynamic_cast<SVTaskObjectListClass*>(pOwner);
		if (nullptr != pTaskOwner)
		{
			pNewFriend = pTaskOwner->UpdateObject(rFriend.getObjectId(), &rFriend, this);
		}
	}
	return (m_friendList.Insert(position, pNewFriend) >= 0);
}

} //namespace SvIe

