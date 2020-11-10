//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVExtentPropertyInfoStruct.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVObjectLibrary/SVObjectAttributeClass.h"
#include "SVObjectLibrary/SVOutputInfoListClass.h"
#include "SVObjectLibrary/SVToolsetScriptTags.h"
#include "SVLibrary/SVTemplate.h"
#include "Definitions/Color.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/RunStatus.h"
#include "ObjectInterfaces/IInspectionProcess.h"
#include "ObjectInterfaces/ITaskObjectListClass.h"
#include "ObjectInterfaces/IToolSet.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVObjectLibrary/DependencyManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVUtilityLibrary/SVNameVariantList.h"
#include "SVValueObjectLibrary/LinkedValue.h"
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

	// Add out Default Inputs and Outputs 
	// Used only from constructor and Dynamic binding can not used.
	SVTaskObjectClass::addDefaultInputObjects();

	return l_hrOk;
}

SVTaskObjectClass::~SVTaskObjectClass()
{
	SVTaskObjectClass::DestroyFriends();

	// empty the Embedded List
	for (SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = *Iter;
		if (nullptr != pObject && pObject->IsCreated())
		{
			pObject->CloseObject();
		}
	}
	m_embeddedList.clear();

	SVTaskObjectClass::CloseObject();
}

bool SVTaskObjectClass::resetAllObjects(SvStl::MessageContainerVector *pErrorMessages/*=nullptr */)
{
	clearTaskMessages();

	bool Result = (S_OK == updateImageExtent());

	// Notify friends...
	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		const SVObjectInfoStruct& rfriend = m_friendList[i];
		if (nullptr != rfriend.getObject())
		{
			Result &= rfriend.getObject()->resetAllObjects(&m_ResetErrorMessages);
		}
	}

	Result &= resetAllOutputListObjects(&m_ResetErrorMessages);

	Result &= __super::resetAllObjects(&m_ResetErrorMessages);

	if (nullptr != pErrorMessages && !m_ResetErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_ResetErrorMessages.begin(), m_ResetErrorMessages.end());
	}

	return Result;
}

HRESULT SVTaskObjectClass::GetOutputList(SVOutputInfoListClass& p_rOutputInfoList) const
{
	HRESULT l_Status(S_OK);

	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];

		// Check if Friend is alive...
		SVTaskObjectClass* l_pObject(nullptr);

		l_pObject = dynamic_cast<SVTaskObjectClass*>(SVObjectManagerClass::Instance().GetObject(rFriend.getObjectId()));

		if (nullptr != l_pObject)
		{
			HRESULT l_Temp = l_pObject->GetOutputList(p_rOutputInfoList);

			if (S_OK == l_Status)
			{
				l_Status = l_Temp;
			}
		}
	}

	for (SVObjectPtrVector::const_iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = *Iter;
		if (nullptr != pObject)
		{
			p_rOutputInfoList.Add(&(pObject->GetObjectOutputInfo()));
		}
		SVImageClass* pImage = dynamic_cast<SVImageClass*>(pObject);
		if (nullptr != pImage)
		{
			pImage->GetOutputList(p_rOutputInfoList);
		}
	}

	return l_Status;
}

HRESULT SVTaskObjectClass::GetOutputListFiltered(SVObjectReferenceVector& rObjectList, UINT uiAttributes, bool bAND)
{
	rObjectList.clear();
	SVOutputInfoListClass l_OutputList;

	GetOutputList(l_OutputList);

	for (auto pInfoItem : l_OutputList)
	{
		if (pInfoItem)
		{
			SVObjectReference ObjectRef = pInfoItem->GetObjectReference();
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
	return S_OK;
}

HRESULT SVTaskObjectClass::IsAuxInputImage(const SvOl::SVInObjectInfoStruct*) const
{
	return S_FALSE;
}

HRESULT SVTaskObjectClass::FindNextInputImageInfo(SvOl::SVInObjectInfoStruct*& p_rpsvFoundInfo, const SvOl::SVInObjectInfoStruct* p_psvLastInfo) const
{
	HRESULT l_svOk = S_FALSE;

	long l_lCount = static_cast<long> (m_svToolInputList.size());

	bool l_bFoundLast = nullptr != p_psvLastInfo && 0 < l_lCount &&
		0 <= m_lLastToolInputListIndex && m_lLastToolInputListIndex < l_lCount &&
		p_psvLastInfo == m_svToolInputList[m_lLastToolInputListIndex];

	if (!l_bFoundLast)
	{
		l_bFoundLast = (nullptr == p_psvLastInfo);

		m_lLastToolInputListIndex = -1;

		m_svToolInputList.clear();

		// Try to get input interface...
		GetInputInterface(m_svToolInputList, true);

		l_lCount = static_cast<long> (m_svToolInputList.size());
	}

	p_rpsvFoundInfo = nullptr;

	for (int i = m_lLastToolInputListIndex + 1; nullptr == p_rpsvFoundInfo && i < l_lCount; ++i)
	{
		SvOl::SVInObjectInfoStruct* l_psvInputInfo = m_svToolInputList[i];

		if (l_bFoundLast)
		{
			if (nullptr != l_psvInputInfo &&
				SvPb::SVImageObjectType == l_psvInputInfo->GetInputObjectInfo().m_ObjectTypeInfo.m_ObjectType)
			{
				if (S_FALSE == IsAuxInputImage(l_psvInputInfo))
				{
					p_rpsvFoundInfo = l_psvInputInfo;

					m_lLastToolInputListIndex = i;

					l_svOk = S_OK;
				}
			}
		}
		else
		{
			l_bFoundLast = p_psvLastInfo == l_psvInputInfo;
		}
	}

	if (S_OK != l_svOk)
	{
		m_lLastToolInputListIndex = -1;

		m_svToolInputList.clear();
	}

	return l_svOk;
}


void SVTaskObjectClass::removeTaskMessage(DWORD MessageCode, SvStl::MessageTextEnum AdditionalTextId)
{
	auto it = std::find_if(m_ResetErrorMessages.begin(), m_ResetErrorMessages.end(),
		[&](SvStl::MessageContainer &mc)
	{
		return ((mc.getMessage().m_MessageCode == MessageCode) && (mc.getMessage().m_AdditionalTextId == AdditionalTextId));
		
	});
	if (it != m_ResetErrorMessages.end())
	{
		m_ResetErrorMessages.erase(it);
	}
	auto it2 = std::find_if(m_RunErrorMessages.begin(), m_RunErrorMessages.end(),
		[&](SvStl::MessageContainer &mc)
	{
		return ((mc.getMessage().m_MessageCode == MessageCode) && (mc.getMessage().m_AdditionalTextId == AdditionalTextId));
	});
	if (it2 != m_RunErrorMessages.end())
	{
		m_RunErrorMessages.erase(it2);
	}
}

void SVTaskObjectClass::addResetErrorMessage(SvStl::MessageContainer& rErrorMessage)
{
	m_ResetErrorMessages.push_back(rErrorMessage);
}

HRESULT SVTaskObjectClass::SetValuesForAnObject(uint32_t aimObjectID, SVObjectAttributeClass* pDataObject)
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

	return __super::SetValuesForAnObject(aimObjectID, pDataObject);
}

void SVTaskObjectClass::DestroyFriends()
{
	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.getObjectId());
		if (pFriend)
		{
			auto* pOwner = dynamic_cast<SVTaskObjectClass*>(pFriend->GetParent());
			if (pOwner)
			{
				// Close, Disconnect and Delete Friend...
				pOwner->DestroyFriend(pFriend);
			}
			else
			{
				// Friend has no owner...
				pFriend->CloseObject();
				delete pFriend;
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
		const SvDef::SVObjectTypeInfoStruct* pInfoStruct = &(m_friendList[i].m_ObjectTypeInfo);
		if (pInfoStruct->m_ObjectType == rObjectType.m_ObjectType && pInfoStruct->m_SubType == rObjectType.m_SubType)
		{
			if (SvPb::NoEmbeddedId == pInfoStruct->m_EmbeddedID || pInfoStruct->m_EmbeddedID == rObjectType.m_EmbeddedID)
			{
				return 	m_friendList[i].getObject();
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
				const SVObjectInfoStruct& rfriend = m_friendList[i];
				if (nullptr != rfriend.getObject())
				{
					l_Status = rfriend.getObject()->GetChildObject(rpObject, rNameInfo, Index + 1);
				}
			}

			if (S_OK != l_Status)
			{
				for (SVObjectPtrVector::const_iterator Iter = m_embeddedList.begin(); nullptr == rpObject && m_embeddedList.end() != Iter; ++Iter)
				{
					SVObjectClass* pObject = *Iter;
					if (nullptr != pObject && (Index + 1 == rNameInfo.m_NameArray.size() - 1 || SvPb::SVImageObjectType == pObject->GetObjectType()))
					{
						l_Status = pObject->GetChildObject(rpObject, rNameInfo, Index + 1);
					}
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

void SVTaskObjectClass::fillSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, SvOi::IsObjectAllowedFunc pFunctor, UINT attribute, bool wholeArray, SvPb::SVObjectTypeEnum nameToType, SvPb::ObjectSelectorType requiredType) const
{
	nameToType = (SvPb::SVNotSetObjectType == nameToType) ? GetObjectType() : nameToType;
	__super::fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, nameToType, requiredType);

	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		// Check if Friend is alive...
		auto* pObject = SVObjectManagerClass::Instance().GetObject(m_friendList[i].getObjectId());
		if (nullptr != pObject)
		{
			pObject->fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, nameToType, requiredType);
		}
	}

	for (auto* pObject : m_embeddedList)
	{
		if (nullptr != pObject)
		{
			pObject->fillSelectorList(treeInserter, pFunctor, attribute, wholeArray, nameToType, requiredType);
		}
	}
}

void SVTaskObjectClass::fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden /*= false*/)
{
	__super::fillObjectList(inserter, rObjectInfo, addHidden);

	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		// Check if Friend is alive...
		auto* pObject = SVObjectManagerClass::Instance().GetObject(m_friendList[i].getObjectId());
		if (nullptr != pObject)
		{
			pObject->fillObjectList(inserter, rObjectInfo, addHidden);
		}
	}

	for (auto* pObject : m_embeddedList)
	{
		if (nullptr != pObject)
		{
			pObject->fillObjectList(inserter, rObjectInfo, addHidden);
		}
	}
}

#pragma region virtual method (ITaskObject)
void SVTaskObjectClass::GetInputs(SvUl::InputNameObjectIdPairList& rList, const SvDef::SVObjectTypeInfoStruct& typeInfo, SvPb::SVObjectTypeEnum objectTypeToInclude, bool shouldExcludeFirstObjectName, int maxNumbers)
{
	SvOl::SVInputInfoListClass toolInputList;
	// Try to get input interface...
	GetInputInterface(toolInputList, true);
	long lCount = static_cast<long> (toolInputList.size());

	for (int i = 0; i < lCount; ++i)
	{
		SvOl::SVInObjectInfoStruct* pInputInfo = toolInputList[i];

		if (nullptr != pInputInfo && (SvPb::SVNotSetObjectType == typeInfo.m_ObjectType ||
			(typeInfo.m_ObjectType == pInputInfo->GetInputObjectInfo().m_ObjectTypeInfo.m_ObjectType &&
			(SvPb::SVNotSetSubObjectType == typeInfo.m_SubType || typeInfo.m_SubType == pInputInfo->GetInputObjectInfo().m_ObjectTypeInfo.m_SubType)))
			&& S_FALSE == pInputInfo->isAuxInputImage())
		{
			SvOi::IObjectClass* pObject = dynamic_cast<SvOi::IObjectClass*> (pInputInfo->GetInputObjectInfo().getObject());
			std::string name = "";
			uint32_t objectID = SvDef::InvalidObjectId;
			if (pInputInfo->IsConnected() && nullptr != pObject)
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
			rList.insert(std::make_pair(pInputInfo->GetInputName(), std::make_pair(name, objectID)));
			if (0 < maxNumbers && rList.size() >= maxNumbers)
			{
				return;
			}
		}
	}
}

SvStl::MessageContainerVector SVTaskObjectClass::validateAndSetEmbeddedValues(const SvOi::SetValueStructVector& rValueVector, bool shouldSet)
{
	SvStl::MessageContainerVector messages;
	HRESULT Result(S_OK);

	for (auto const& rEntry : rValueVector)
	{
		try
		{
			if (nullptr != rEntry.m_pValueObject)
			{
				rEntry.m_pValueObject->validateValue(rEntry.m_Value, rEntry.m_DefaultValue);
			}
			else
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_NULL_POINTER, SvStl::Tid_Default, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				throw Msg;
			}
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			messages.push_back(rSvE);
			Result = rSvE.getMessage().m_MessageCode;
		}

	}

	if (shouldSet)
	{
		for (auto const& rEntry : rValueVector)
		{
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
				messages.push_back(Msg.getMessageContainer());
			}
		}
	}

	return messages;
}

void SVTaskObjectClass::ResolveDesiredInputs(const SvDef::SVObjectTypeInfoVector& rDesiredInputs)
{
	// Get Input Interface...
	SvOl::SVInputInfoListClass inputInterface;
	GetPrivateInputList(inputInterface);

	// Apply desired input interface...
	for (int i = 0; i < rDesiredInputs.size() && i < inputInterface.size(); ++i)
	{
		SvOl::SVInObjectInfoStruct* pInInfo = inputInterface[i];

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

std::vector<uint32_t> SVTaskObjectClass::getEmbeddedList() const
{
	std::vector<uint32_t> Result(m_embeddedList.size());
	std::transform(m_embeddedList.begin(), m_embeddedList.end(), Result.begin(), [](const auto& rEntry) { return rEntry->getObjectId(); });
	return Result;
}

bool SVTaskObjectClass::AddFriend(uint32_t friendId, uint32_t addPreId)
{
	size_t position = m_friendList.size();
	// Check ID...
	if (SvDef::InvalidObjectId == friendId)
	{
		return false;
	}
	// Check if friend is already applied...
	if (m_friendList.size())
	{
		for (int i = static_cast<int>(m_friendList.size()) - 1; i >= 0; --i)
		{
			if (m_friendList[i].getObjectId() == friendId)
			{
				return false;
			}

			if (m_friendList[i].getObjectId() == addPreId)
			{
				position = i;
			}
		}
	}

	SVObjectInfoStruct newFriendInfo;
	// Check if Friend is alive...
	SVObjectClass* pNewFriend = SVObjectManagerClass::Instance().GetObject(friendId);
	if (pNewFriend)
	{
		// Check if we are the Owner
		// Note:: Special hack for friend scripting
		// if we are the owner - it's not from the script
		SVObjectClass* l_psvOwner = pNewFriend->GetParent();

		if (l_psvOwner == this)
		{
			newFriendInfo.SetObject(pNewFriend);
		}
		else
		{
			auto* pTaskOwner = dynamic_cast<SVTaskObjectClass*>(l_psvOwner);
			if (nullptr != pTaskOwner)
			{
				SVObjectClass* l_psvNewObject = pTaskOwner->UpdateObject(friendId, pNewFriend, this);

				assert(nullptr != l_psvNewObject);

				newFriendInfo.SetObject(l_psvNewObject);
			}
		}
	}
	else
	{
		newFriendInfo.GetObjectReference().setObjectId(friendId);
	}

	return (m_friendList.Insert(position, newFriendInfo) >= 0);
}

void SVTaskObjectClass::moveFriendObject(uint32_t objectToMoveId, uint32_t preObjectId)
{
	int currentPos = -1;
	int newPos = -1;
	for (int i = 0; i <= m_friendList.size(); i++)
	{
		if (m_friendList[i].getObjectId() == objectToMoveId)
		{
			currentPos = i;
		}
		if (m_friendList[i].getObjectId() == preObjectId)
		{
			newPos = i;
		}
	}

	if (0 <= currentPos && m_friendList.size() > currentPos)
	{
		auto object = m_friendList[currentPos];
		//change first object which is later in the list.
		if (currentPos > newPos)
		{
			m_friendList.RemoveAt(currentPos);
			m_friendList.Insert(newPos, object);
		}
		else
		{
			m_friendList.Insert(newPos, object);
			m_friendList.RemoveAt(currentPos);
		}
	}
}
#pragma endregion virtual method (ITaskObject)

SVObjectClass* SVTaskObjectClass::OverwriteEmbeddedObject(uint32_t uniqueID, SvPb::EmbeddedIdEnum embeddedID)
{
	// Check here all embedded members ( embedded objects could be only identified by embeddedID!!!! )... 
	for (SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = *Iter;
		if (nullptr != pObject)
		{
			if (pObject->GetEmbeddedID() == embeddedID)
			{
				return pObject->OverwriteEmbeddedObject(uniqueID, embeddedID);
			}
		}
	}
	return __super::OverwriteEmbeddedObject(uniqueID, embeddedID);
}

void SVTaskObjectClass::GetInputInterface(SvOl::SVInputInfoListClass& rInputList, bool bAlsoFriends) const
{
	if (bAlsoFriends)
	{
		for (size_t i = 0; i < m_friendList.size(); ++i)
		{
			if (nullptr != m_friendList[i].getObject())
			{
				m_friendList[i].getObject()->GetInputInterface(rInputList, bAlsoFriends);
			}
		}
	}

	// Local input list...
	SvOl::SVInputInfoListClass localInputList;
	GetPrivateInputList(localInputList);
	std::copy(localInputList.begin(), localInputList.end(), std::back_inserter(rInputList));
}

void SVTaskObjectClass::DestroyFriend(SVObjectClass* pObject)
{
	// This was Added because SVTaskObjectClasses can also have Friends
	SVTaskObjectClass* pTaskObject = dynamic_cast<SVTaskObjectClass*>(pObject);

	// Close the Friend Object
	if (pTaskObject)
	{
		// Notify the Owner(s) of our Friends inputs that they are not needed anymore
		pTaskObject->Disconnect();

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
			const SVObjectInfoStruct& rfriend = m_friendList[i];
			if (nullptr != rfriend.getObject())
			{
				Result = rfriend.getObject()->getFirstObject(rObjectTypeInfo, true, pRequestor);
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

	Result = __super::getFirstObject(rObjectTypeInfo, useFriends, pRequestor);
	if (nullptr != Result)
	{
		return Result;
	}

	for (SVObjectPtrVector::const_iterator Iter = m_embeddedList.begin(); nullptr == Result && m_embeddedList.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = *Iter;

		if (nullptr != pObject)
		{
			Result = pObject->getFirstObject(rObjectTypeInfo, useFriends, pRequestor);
		}
	}
	return Result;
}

void SVTaskObjectClass::OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName)
{
	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		const SVObjectInfoStruct& rfriend = m_friendList[i];
		if (nullptr != rfriend.getObject())
		{
			rfriend.getObject()->OnObjectRenamed(rRenamedObject, rOldName);
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
			const SVObjectInfoStruct &rFriend = m_friendList[i];

			if (nullptr != rFriend.getObject() && rFriend.getObject()->GetParent() == this)
			{
				Result = rFriend.getObject()->isInputImage(imageId);
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

HRESULT SVTaskObjectClass::SetImageExtent(const SVImageExtentClass& rImageExtent)
{
	m_imageExtent = rImageExtent;
	return S_OK;
}

HRESULT SVTaskObjectClass::SetImageExtentToParent()
{
	return S_FALSE;
}

HRESULT SVTaskObjectClass::SetImageExtentToFit(const SVImageExtentClass& )
{
	return S_FALSE;
}

HRESULT SVTaskObjectClass::GetPropertyInfo(SvPb::SVExtentPropertyEnum, SVExtentPropertyInfoStruct& ) const
{
	return S_FALSE;
}

// Must and will be called if ObjectID of this object has been changed.
// ( Refer to SVObjectManagerClass::ChangeUniqueObjectID() )
void SVTaskObjectClass::ResetPrivateInputInterface()
{
	for (int i = 0; i < static_cast<int> (m_inputInterfaceList.size()); ++i)
	{
		SvOl::SVInObjectInfoStruct* pInInfo = m_inputInterfaceList[i];
		if (pInInfo)
		{
			const SVObjectInfoStruct &l_rsvInfo = GetObjectInfo();

			SvDef::SVObjectTypeInfoStruct info = pInInfo->GetInputObjectInfo().m_ObjectTypeInfo;

			if (SvDef::InvalidObjectId == pInInfo->GetInputObjectInfo().getObjectId() &&
				SvPb::NoEmbeddedId == info.m_EmbeddedID &&
				SvPb::SVNotSetObjectType == info.m_ObjectType &&
				SvPb::SVNotSetSubObjectType == info.m_SubType)
			{
				//assert( false );
			}

			pInInfo->SetObject(l_rsvInfo);
			info = pInInfo->GetInputObjectInfo().m_ObjectTypeInfo;

			if (SvDef::InvalidObjectId == pInInfo->GetInputObjectInfo().getObjectId() &&
				SvPb::NoEmbeddedId == info.m_EmbeddedID &&
				SvPb::SVNotSetObjectType == info.m_ObjectType &&
				SvPb::SVNotSetSubObjectType == info.m_SubType)
			{
				//assert( false );
			}
		}
	}
}

bool SVTaskObjectClass::ConnectAllInputs()
{
	bool Result(true);

	SvOl::SVInputInfoListClass inputList;

	// Add the defaults
	addDefaultInputObjects(&inputList);

	// tell friends to connect...
	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[j];
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(rFriend.getObjectId());
		if (nullptr != pObject)
		{
			pObject->ConnectAllInputs();
		}
	}

	// find our inputs
	for (auto pInputInfo : inputList)
	{
		if (nullptr != pInputInfo)
		{
			// Is not yet connected...
			if (!pInputInfo->IsConnected())
			{
				// if Connect to Default
				if (SvDef::InvalidObjectId == pInputInfo->GetInputObjectInfo().getObjectId())
				{
					// Input Object is not set...Try to get one...
					SvDef::SVObjectTypeInfoStruct info = pInputInfo->GetInputObjectInfo().m_ObjectTypeInfo;
					// At least one item from the SvDef::SVObjectTypeInfoStruct is required, but not all
					if (SvPb::NoEmbeddedId != info.m_EmbeddedID || SvPb::SVNotSetObjectType != info.m_ObjectType || SvPb::SVNotSetSubObjectType != info.m_SubType)
					{
						SVObjectClass* pOwner = GetParent();
						SVObjectClass* pRequestor = pInputInfo->getObject();
						const SvOi::IObjectClass* pObject(nullptr);
						bool bSuccess = false;

						if (hasToAskFriendForConnection(info, pOwner))
						{
							// Ask first friends...
							for (size_t j = 0; j < m_friendList.size(); ++j)
							{
								pObject = SVObjectManagerClass::Instance().getFirstObject(m_friendList[j].getObjectId(), info);
								if (pObject)
								{
									// Connect input ...
									pInputInfo->SetInputObject(pObject->getObjectId());
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
								pObject = nullptr;
								// if color system & pOwner == SVToolSet
								const SVObjectInfoStruct& ownerInfo = pOwner->GetObjectInfo();
								if (isColorInspection && SvPb::SVToolSetObjectType == ownerInfo.m_ObjectTypeInfo.m_ObjectType && SvPb::SVImageObjectType == info.m_ObjectType && SvPb::SVImageMonoType == info.m_SubType)
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
									pInputInfo->SetInputObject(pObject->getObjectId());
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

				// Finally try to connect...
				if (SvDef::InvalidObjectId != pInputInfo->GetInputObjectInfo().getObjectId())
				{
					Result = SVObjectManagerClass::Instance().ConnectObjectInput(pInputInfo->GetInputObjectInfo().getObjectId(), pInputInfo);
				}
			}
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

struct CompareInputName
{
	std::string m_Name;
	explicit CompareInputName(const std::string& rName) : m_Name(rName) {}
	bool operator()(const SvOl::SVInObjectInfoStruct* pInfo) const { return pInfo->GetInputName() == m_Name; }
};

HRESULT SVTaskObjectClass::ConnectToObject(const std::string& rInputName, uint32_t newID, SvPb::SVObjectTypeEnum objectType)
{
	HRESULT hr = S_OK;

	SvOl::SVInputInfoListClass toolInputList;
	GetInputInterface(toolInputList, true);
	// Find SVInObjectInfoStruct that has this name
	SvOl::SVInputInfoListClass::const_iterator it = std::find_if(toolInputList.begin(), toolInputList.end(), CompareInputName(rInputName));
	if (it != toolInputList.end())
	{
		SVObjectClass* pNewObject = SVObjectManagerClass::Instance().GetObject(newID);
		if (nullptr != pNewObject)
		{
			if (SvPb::SVNotSetObjectType == objectType || pNewObject->GetObjectType() == objectType)
			{
				hr = ConnectToObject((*it), pNewObject);
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
							hr = ConnectToObject((*it), pNewObject);
						}
						else
						{
							hr = E_FAIL;
						}
					}
					catch (const SvStl::MessageContainer& rExp)
					{
						SvStl::MessageManager Exception(SvStl::MsgType::Log);
						Exception.setMessage(rExp.getMessage());
						hr = E_FAIL;
					}
				}
				else
				{
					hr = E_POINTER;
				}
			}
		}
		else
		{
			hr = E_POINTER;
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}
	return hr;
}

HRESULT SVTaskObjectClass::ConnectToObject(SvOl::SVInObjectInfoStruct* p_psvInputInfo, SVObjectClass* pNewObject)
{
	HRESULT l_svOk = S_OK;

	if (nullptr != p_psvInputInfo)
	{
		const uint32_t oldInputObjectID = p_psvInputInfo->GetInputObjectInfo().getObjectId();
		SVObjectClass* pOldObject = dynamic_cast<SVObjectClass*>(SVObjectManagerClass::Instance().GetObject(oldInputObjectID));

		// Disconnect input info of input object...
		if (p_psvInputInfo->IsConnected())
		{
			// Send to the Object we are using
			SVObjectManagerClass::Instance().DisconnectObjectInput(oldInputObjectID, p_psvInputInfo);
		}

		// Set new input...
		p_psvInputInfo->SetInputObject(pNewObject);

		if (nullptr != pNewObject)
		{
			// Connect input info to new input object...
			if (!pNewObject->ConnectObjectInput(p_psvInputInfo))
			{
				// Unable to connect to new input object....
				SvDef::StringVector msgList;
				msgList.push_back(pNewObject->GetName());

				// Should we really be doing this here?
				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CriticalUnableToConnectTo, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10203);

				// Try to recover old state...
				if (nullptr != pOldObject)
				{
					p_psvInputInfo->SetInputObject(pOldObject);
					pOldObject->ConnectObjectInput(p_psvInputInfo);
				}

				l_svOk = S_FALSE;
			}
		}
		else
		{
			l_svOk = S_FALSE;
		}

		SVObjectClass* pTool = GetTool();
		if (nullptr != pTool)
		{
			pTool->ConnectAllInputs();
			pTool->resetAllObjects();
		}
	}
	else
	{
		l_svOk = S_FALSE;
	}

	return l_svOk;
}

bool SVTaskObjectClass::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool Result = SVObjectAppClass::CreateObject(rCreateStructure);

	Result &= (S_OK == updateImageExtent());

	// Create our friends
	for (size_t j = 0; j < m_friendList.size(); ++j)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[j];
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.getObjectId());
		if (pFriend)
		{
			Result = CreateChildObject(pFriend) && Result;

			assert(Result);
		}
		else
		{
			Result = false;
			assert(Result);
		}
	}

	// Create the embeddeds...
	// Save the owner and set the owner of our embeddeds to us!
	for (SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = *Iter;
		if (nullptr != pObject)
		{
			Result = CreateChildObject(pObject) && Result;

			assert(Result);
		}
	}

	constexpr UINT cAttribute {SvDef::selectableAttributes | SvPb::audittrail};
	m_statusTag.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);
	m_statusColor.SetObjectAttributesAllowed(SvPb::audittrail, SvOi::SetAttributeType::RemoveAttribute);

	m_isCreated = Result;

	return Result;
}

void SVTaskObjectClass::addDefaultInputObjects(SvOl::SVInputInfoListClass* PInputListToFill)
{
	int l_iCount = static_cast<int> (m_inputInterfaceList.size());
	int i(0);


	for (i = 0; i < l_iCount; i++)
	{
		SvOl::SVInObjectInfoStruct* pInInfo = m_inputInterfaceList[i];
		if (pInInfo)
		{
			SvDef::SVObjectTypeInfoStruct info = pInInfo->GetInputObjectInfo().m_ObjectTypeInfo;

			if (SvDef::InvalidObjectId == pInInfo->GetInputObjectInfo().getObjectId() &&
				SvPb::NoEmbeddedId == info.m_EmbeddedID &&
				SvPb::SVNotSetObjectType == info.m_ObjectType &&
				SvPb::SVNotSetSubObjectType == info.m_SubType)
			{
				//assert( false );
			}

			if (PInputListToFill)
			{
				PInputListToFill->push_back(pInInfo);
			}
			else
			{
				m_InputObjectList.push_back(pInInfo);
			}
		}
	}
}

bool SVTaskObjectClass::RegisterEmbeddedObject(SVImageClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, int StringResourceID)
{
	std::string Name = SvUl::LoadStdString(StringResourceID);
	return RegisterEmbeddedObject(pEmbeddedObject, embeddedID, Name.c_str());
}

bool SVTaskObjectClass::RegisterEmbeddedObject(SVImageClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, LPCTSTR newString)
{
	bool l_bOk = nullptr != pEmbeddedObject && RegisterEmbeddedObjectAsClass(pEmbeddedObject, embeddedID, newString);

	return l_bOk;
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
bool SVTaskObjectClass::RegisterEmbeddedObject(SVObjectClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, int StringResourceID, bool ResetAlways, SvOi::SVResetItemEnum RequiredReset)
{
	std::string Name = SvUl::LoadStdString(StringResourceID);
	return RegisterEmbeddedObject(pEmbeddedObject, embeddedID, Name.c_str(), ResetAlways, RequiredReset);
}

bool SVTaskObjectClass::RegisterEmbeddedObject(SVObjectClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, LPCTSTR Name, bool ResetAlways, SvOi::SVResetItemEnum RequiredReset)
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

bool SVTaskObjectClass::RegisterEmbeddedObjectAsClass(SVObjectClass* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, LPCTSTR ObjectName)
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

bool SVTaskObjectClass::resetAllOutputListObjects(SvStl::MessageContainerVector *pErrorMessages/*=nullptr */)
{
	bool Result = true;

	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];

		// Check if Friend is alive...
		SVTaskObjectClass* pObject = dynamic_cast<SVTaskObjectClass*> (SVObjectManagerClass::Instance().GetObject(rFriend.getObjectId()));

		if (nullptr != pObject)
		{
			//return-value and error-messages do not be saved here, because this object will call resetAllOutputListObjects by its own and return error message to the parents.
			//this call here is important to reset (resize) the embedded images, so the parents can use it for its reset.
			pObject->resetAllOutputListObjects();
		}
	}

	// Try to send message to outputObjectList members
	for (SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = *Iter;

		if (nullptr != pObject)
		{
			//the error of this embedded objects must be saved by this object.
			Result = pObject->resetAllObjects(pErrorMessages) && Result;
		}
	}

	return Result;
}

bool SVTaskObjectClass::RegisterInputObject(SvOl::SVInObjectInfoStruct* PInObjectInfo, const std::string& p_rInputName)
{
	if (PInObjectInfo)
	{
		if (m_inputInterfaceList.push_back(PInObjectInfo) >= 0)
		{
			PInObjectInfo->SetInputName(p_rInputName);
			PInObjectInfo->setAuxInputImageFlag(IsAuxInputImage(PInObjectInfo));
			return true;
		}

		PInObjectInfo->SetInputName(std::string());
	}
	return false;
}

void SVTaskObjectClass::GetPrivateInputList(SvOl::SVInputInfoListClass& rInputInterface) const
{
	rInputInterface = m_inputInterfaceList;
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
	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		if (rFriend.getObject())
		{
			rFriend.getObject()->SetDisabled();
		}
	}
}

DWORD SVTaskObjectClass::GetObjectColor() const
{
	DWORD dwColor;
	m_statusColor.GetValue(dwColor);
	return dwColor;
}

////////////////////////////////////////////////////////////////////////////////
// .Title       : GetInputOutputObjects
// -----------------------------------------------------------------------------
// .Description : Use this function to get the in- and outputs of this object.
//				: Normally used from an overlaying TaskListObject or TaskObject.
////////////////////////////////////////////////////////////////////////////////
void SVTaskObjectClass::GetInputObjects(SvOl::SVInputInfoListClass& RInputObjectList)
{
	// Add our Friends first

	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.getObjectId());
		if (SVTaskObjectClass* pTaskObjectFriend = dynamic_cast<SVTaskObjectClass*>(pFriend))
		{
			pTaskObjectFriend->GetInputObjects(RInputObjectList);
		}
	}

	int j(0);

	for (j = 0; j < static_cast<int> (m_InputObjectList.size()); ++j)
	{
		if (nullptr != m_InputObjectList[j])
		{
			RInputObjectList.push_back(m_InputObjectList[j]);
		}
	}
}

void SVTaskObjectClass::GetAllInputObjects()
{
	// Tell Friends to rebuild their lists
	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.getObjectId());
		if (SVTaskObjectClass* pTaskObjectFriend = dynamic_cast<SVTaskObjectClass*>(pFriend))
		{
			pTaskObjectFriend->GetAllInputObjects();
		}
	}
}

void SVTaskObjectClass::Persist(SvOi::IObjectWriter& rWriter)
{
	SVObjectAppClass::Persist(rWriter);

	PersistFriends(rWriter);
	PersistEmbeddeds(rWriter);
	PersistInputs(rWriter);
}

void SVTaskObjectClass::PersistFriends(SvOi::IObjectWriter& rWriter)
{
	// Get script of close friends list members...
	if (m_friendList.size())
	{
		rWriter.StartElement(scFriendsTag);

		for (size_t i = 0; i < m_friendList.size(); ++i)
		{
			const SVObjectInfoStruct& rFriend = m_friendList[i];
			// Check if Friend is alive...
			SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.getObjectId());
			if (SVTaskObjectClass* pTaskObjectFriend = dynamic_cast<SVTaskObjectClass*>(pFriend))
			{
				rWriter.StartElement(pTaskObjectFriend->GetObjectName()); // use internal name for node name
				pTaskObjectFriend->Persist(rWriter);
				rWriter.EndElement();
			}
		}
		rWriter.EndElement();
	}
}

void SVTaskObjectClass::PersistInputs(SvOi::IObjectWriter& rWriter)
{
	// Set up input list...
	SvOl::SVInputInfoListClass inputList;
	addDefaultInputObjects(&inputList);

	if (0 < inputList.size())
	{
		SVNameVariantList list;
		for (int i = 0; i < static_cast<int> (inputList.size()); ++i)
		{
			if (nullptr != inputList[i])
			{
				_bstr_t name;
				_variant_t value;

				SvOl::ValidateInput(*inputList[i]);
				const SvOl::SVInObjectInfoStruct& rInputInfo = *inputList[i];
				value.SetString(rInputInfo.GetInputObjectInfo().GetObjectReference().GetObjectIdAndIndexOneBased().c_str());
				name = _bstr_t(rInputInfo.GetInputName().c_str());
				// hold name value pair...
				list.insert(std::make_pair(name, value));
				value.Clear();
			}
		}
		rWriter.StartElement(scInputsTag);
		if (list.size())
		{
			for (SVNameVariantList::const_iterator it = list.begin(); it != list.end(); ++it)
			{
				rWriter.StartElement(scInputTag);
				rWriter.WriteAttribute(scNameTag, it->first);
				rWriter.WriteAttribute(scValueTag, it->second);
				rWriter.EndElement();
			}
		}
		rWriter.EndElement();
	}
}

void SVTaskObjectClass::PersistEmbeddeds(SvOi::IObjectWriter& rWriter)
{
	// Set up embedded object definitions...
	if (0 < m_embeddedList.size())
	{
		rWriter.StartElement(scEmbeddedsTag);
		// Get embedded object script...
		for (SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
		{
			SVObjectClass* pObject = *Iter;
			if (nullptr != pObject)
			{
				pObject->Persist(rWriter);
			}
		}
		rWriter.EndElement();
	}
}

bool SVTaskObjectClass::Run(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	clearRunErrorMessages();

	// Run yourself...
	bool bRetVal = onRun(rRunStatus, &m_RunErrorMessages);

	// Get Status Color...
	DWORD dwValue = rRunStatus.GetStatusColor();
	m_statusColor.SetValue(dwValue);

	// Get Status...
	dwValue = rRunStatus.GetState();
	m_statusTag.SetValue(dwValue);

	if (nullptr != pErrorMessages && !m_RunErrorMessages.empty())
	{
		pErrorMessages->insert(pErrorMessages->end(), m_RunErrorMessages.begin(), m_RunErrorMessages.end());
	}

	return bRetVal;
}

bool SVTaskObjectClass::onRun(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool bRetVal = (S_OK == updateImageExtent());

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

bool SVTaskObjectClass::runFriends(RunStatus& rRunStatus, SvStl::MessageContainerVector *pErrorMessages)
{
	bool bRetVal = true;

	// Run your friends
	size_t j = m_bSkipFirstFriend ? 1 : 0;
	for (; j < m_friendList.size(); ++j)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[j];
		if (SVTaskObjectClass* pTaskObject = dynamic_cast<SVTaskObjectClass*>(rFriend.getObject()))
		{
			bRetVal = pTaskObject->Run(rRunStatus, pErrorMessages) && bRetVal;
		}
		else
		{
			bRetVal = false;
		}
	}
	return bRetVal;
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
void SVTaskObjectClass::Disconnect()
{
	for (int inIndex = static_cast<int> (m_InputObjectList.size() - 1); inIndex >= 0; inIndex--)
	{
		SvOl::SVInObjectInfoStruct* pInObjectInfo = m_InputObjectList[inIndex];
		if (pInObjectInfo)
		{
			if (pInObjectInfo->IsConnected() && SvDef::InvalidObjectId != pInObjectInfo->getObjectId())
			{
				// Send to the Object we are using
				SVObjectManagerClass::Instance().DisconnectObjectInput(pInObjectInfo->GetInputObjectInfo().getObjectId(), pInObjectInfo);
			}
		}
		// remove it from the list
	}
	m_InputObjectList.clear();

	SVOutputInfoListClass l_OutputInfoList;

	GetOutputList(l_OutputInfoList);

	// Notify all Objects dependent on our Outputs that 
	// the outputs are not available anymore
	for (int outIndex = l_OutputInfoList.GetSize() - 1; outIndex >= 0; outIndex--)
	{
		SVOutObjectInfoStruct* pOutObjectInfo = l_OutputInfoList.GetAt(outIndex);
		if (pOutObjectInfo)
		{
			if (SvOi::ITaskObjectListClass* pTaskObjectList = dynamic_cast <SvOi::ITaskObjectListClass*> (GetParent()))
			{
				pTaskObjectList->RemoveOutputObject(pOutObjectInfo);
			}
			pOutObjectInfo->DisconnectAllInputs();
		}
	}

	// Disconnect our Friends
	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.getObjectId());
		if (SVTaskObjectClass* pTaskObjectFriend = dynamic_cast<SVTaskObjectClass*>(pFriend))
		{
			// Tell Friends to Disconnect...
			pTaskObjectFriend->Disconnect();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
bool SVTaskObjectClass::DisconnectObjectInput(SvOl::SVInObjectInfoStruct* pInObjectInfo)
{
	bool Result(false);

	if (nullptr != pInObjectInfo)
	{
		for (int i = 0; !Result && i < static_cast<int> (m_inputInterfaceList.size()); i++)
		{
			SvOl::SVInObjectInfoStruct* pInputObjectInfo = m_inputInterfaceList[i];

			if (pInputObjectInfo)
			{
				if (pInObjectInfo->GetInputObjectInfo().getObjectId() == pInputObjectInfo->GetInputObjectInfo().getObjectId())
				{
					pInputObjectInfo->SetInputObject(nullptr);

					Result = true;
				}
			}
		}

		for (SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
		{
			SVImageClass* pImage(dynamic_cast<SVImageClass*> (*Iter));

			if (nullptr != pImage)
			{
				if (pInObjectInfo->GetInputObjectInfo().getObjectId() == pImage->getObjectId())
				{
					pImage->UpdateImage(SvDef::InvalidObjectId, pImage->GetImageInfo());
				}
			}
		}
	}

	if (!Result)
	{
		Result = __super::DisconnectObjectInput(pInObjectInfo);
	}
	return Result;
}

// Add embedded object pointer to the embedded List.
// Use this only for real embedded objects.
void SVTaskObjectClass::AddEmbeddedObject(SVObjectClass* pObject)
{
	assert(nullptr != pObject);

	// Add to Owner's List of Embedded Objects
	m_embeddedList.push_back(pObject);
}

/////////////////////////////////////////////////////////////////////////////
//
//
void SVTaskObjectClass::RemoveEmbeddedObject(SVObjectClass* pObjectToRemove)
{
	if (nullptr != pObjectToRemove)
	{
		pObjectToRemove->SetObjectAttributesAllowed(SvDef::defaultValueObjectAttributes, SvOi::SetAttributeType::RemoveAttribute);
		// Reset any selection
		pObjectToRemove->SetObjectAttributesSet(0, SvOi::SetAttributeType::OverwriteAttribute);

		// Get this object's outputInfo
		SVOutObjectInfoStruct& rOutObjectInfo = pObjectToRemove->GetObjectOutputInfo();
		rOutObjectInfo.DisconnectAllInputs();
	}

	// iterate and remove object if in embedded list.
	for (SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = *Iter;
		if (nullptr != pObject && pObject == pObjectToRemove)
		{
			m_embeddedList.erase(Iter);
			break;
		}
	}

	SVOutputInfoListClass l_OutputInfoList;

	GetOutputList(l_OutputInfoList);

	// if object is on output list, remove it there as well!
	for (int outIndex = l_OutputInfoList.GetSize() - 1; outIndex >= 0; outIndex--)
	{
		SVOutObjectInfoStruct* pOutObjectInfo = l_OutputInfoList.GetAt(outIndex);
		if (pOutObjectInfo && pOutObjectInfo->getObject() == pObjectToRemove)
		{
			// remove from owner's list
			if (SvOi::ITaskObjectListClass* pTaskObjectList = dynamic_cast <SvOi::ITaskObjectListClass*> (GetParent()))
			{
				pTaskObjectList->RemoveOutputObject(pOutObjectInfo);
			}
		}
	}
}

// Added to process friends
bool SVTaskObjectClass::CloseObject()
{
	// Close ourself first
	bool Result = SVObjectAppClass::CloseObject();

	// Close our Friends
	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		const SVObjectInfoStruct& rFriend = m_friendList[i];
		// Check if Friend is alive...
		SVObjectClass* pFriend = SVObjectManagerClass::Instance().GetObject(rFriend.getObjectId());
		if (pFriend)
		{
			// Close Friend...
			Result = pFriend->CloseObject() && Result;
		}
	}

	// Close Embeddeds...
	for (SVObjectPtrVector::iterator Iter = m_embeddedList.begin(); m_embeddedList.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = *Iter;
		if (nullptr != pObject && pObject->IsCreated())
		{
			pObject->CloseObject();
		}
	}
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
	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		const SVObjectInfoStruct& rfriend = m_friendList[i];
		if (nullptr != rfriend.getObject())
		{
			rfriend.getObject()->setEditModeFreezeFlag(flag);
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

void SVTaskObjectClass::copiedSavedImage(SvTrc::ITriggerRecordRWPtr pTr)
{
	__super::copiedSavedImage(pTr);
	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		const SVObjectInfoStruct& rfriend = m_friendList[i];
		if (nullptr != rfriend.getObject())
		{
			rfriend.getObject()->copiedSavedImage(pTr);
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
	for (size_t i = 0; i < m_friendList.size(); ++i)
	{
		const SVObjectInfoStruct& rfriend = m_friendList[i];
		if (nullptr != rfriend.getObject())
		{
			rfriend.getObject()->goingOffline();
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

//namespace
//{
struct local
{
	struct IsInput
	{
		explicit IsInput(const SVObjectClass* pInput) : m_pInput(pInput) {}
		bool operator () (const SvOl::SVInObjectInfoStruct* pInput) { return pInput->getObject() == m_pInput; }
		const SVObjectClass* m_pInput;
	};
	struct IsOutput
	{
		explicit IsOutput(const SVObjectClass* pOutput) : m_pOutput(pOutput) {}
		bool operator () (const SVOutObjectInfoStruct* pOutput) { return pOutput->getObject() == m_pOutput; }
		const SVObjectClass* m_pOutput;
	};
};
//}

HRESULT SVTaskObjectClass::DisconnectInputsOutputs(SVObjectPtrVector& rListOfObjects)
{
	SVOutputInfoListClass l_OutputInfoList;
	GetOutputList(l_OutputInfoList);

	for (auto* pObject : rListOfObjects)
	{
		// check inputs
		SvOl::SVInputInfoListClass::iterator iterInput = std::find_if(m_InputObjectList.begin(), m_InputObjectList.end(), local::IsInput(pObject));
		if (iterInput != m_InputObjectList.end())
		{
			SvOl::SVInObjectInfoStruct* pInObjectInfo = *iterInput;
			if (nullptr != pInObjectInfo && pInObjectInfo->IsConnected() && SvDef::InvalidObjectId != pInObjectInfo->getObjectId())
			{
				// Send to the Object we are using
				SVObjectManagerClass::Instance().DisconnectObjectInput(pInObjectInfo->GetInputObjectInfo().getObjectId(), pInObjectInfo);
			}
		}

		// check outputs
		SVOutputInfoListClass::iterator iterOutput = std::find_if(l_OutputInfoList.begin(), l_OutputInfoList.end(), local::IsOutput(pObject));
		if (iterOutput != l_OutputInfoList.end())
		{
			SVOutObjectInfoStruct* pOutObjectInfo = *iterOutput;
			if (pOutObjectInfo)
			{
				pOutObjectInfo->DisconnectAllInputs();
			}
			l_OutputInfoList.erase(iterOutput);
		}
	}

	return S_OK;
}

HRESULT SVTaskObjectClass::HideInputsOutputs(SVObjectPtrVector& rListOfObjects)
{
	SVObjectPtrVector::iterator iter;
	for (iter = rListOfObjects.begin(); iter != rListOfObjects.end(); ++iter)
	{
		(*iter)->SetObjectAttributesAllowed(SvPb::viewable, SvOi::SetAttributeType::RemoveAttribute);
	}
	return S_OK;
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

HRESULT SVTaskObjectClass::CollectOverlays(SVImageClass* p_Image, SVExtentMultiLineStructVector &p_MultiLineArray)
{
	HRESULT hrRet = S_OK;

	if (m_bUseOverlays)
	{
		hrRet = onCollectOverlays(p_Image, p_MultiLineArray);
	}

	for (size_t j = 0; j < m_friendList.size(); j++)
	{
		const SVObjectInfoStruct& l_rFriend = m_friendList[j];

		SVTaskObjectClass* pObject = dynamic_cast<SVTaskObjectClass*>(l_rFriend.getObject());

		if (pObject)
		{
			HRESULT l_Temp = pObject->CollectOverlays(p_Image, p_MultiLineArray);

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

	for (size_t j = 0; j < m_friendList.size(); j++)
	{
		SVTaskObjectClass* pObject = dynamic_cast<SVTaskObjectClass*>(m_friendList[j].getObject());
		if (pObject)
		{
			pObject->collectOverlays(pImage, rOverlay);
		}
	}
}

HRESULT SVTaskObjectClass::onCollectOverlays(SVImageClass* p_Image, SVExtentMultiLineStructVector &p_MultiLineArray)
{
	HRESULT l_Status = E_FAIL;

	if (nullptr != p_Image)
	{
		if (GetImageExtent().hasFigure())
		{
			SVExtentMultiLineStruct l_MultiLine;

			UpdateOverlayIDs(l_MultiLine);

			UpdateOverlayName(l_MultiLine, GetImageExtent());

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

			l_MultiLine.AssignExtentFigure(GetImageExtent().GetFigure(), l_MultiLine.m_Color);

			p_MultiLineArray.push_back(l_MultiLine);
			l_Status = S_OK;
		}
	}

	return l_Status;
}

void SVTaskObjectClass::registerEmbeddedLinkedValue(SvVol::LinkedValue* pEmbeddedObject, SvPb::EmbeddedIdEnum embeddedID, SvPb::EmbeddedIdEnum embeddedLinkID, int StringResourceID, _variant_t defaultValue)
{
	RegisterEmbeddedObject(pEmbeddedObject, embeddedID, StringResourceID, true, SvOi::SVResetItemTool);
	pEmbeddedObject->SetDefaultValue(defaultValue, true);
	std::string objectName = SvUl::LoadStdString(StringResourceID);
	objectName += SvDef::cLinkName;
	RegisterEmbeddedObject(&(pEmbeddedObject->getLinkedName()), embeddedLinkID, objectName.c_str(), false, SvOi::SVResetItemNone);
	pEmbeddedObject->getLinkedName().SetDefaultValue(_T(""), false);
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

HRESULT SVTaskObjectClass::ResetObjectInputs()
{
	HRESULT l_Status = SVObjectAppClass::ResetObjectInputs();

	addDefaultInputObjects();

	return l_Status;
}

} //namespace SvIe
