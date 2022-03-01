//*****************************************************************************
/// \copyright (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ParameterTask.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Class to contains parameters (LinkedValue and TypeParameter per Parameter), e.g. for Group Tool-Inputs and -Results.
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "ParameterTask.h"
#include "TableObject.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVValueObjectLibrary/SVEnumerateValueObjectClass.h"
#pragma endregion Includes

namespace SvOp
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

	namespace
	{
	constexpr long cMaxNumberOfObjects = 50;
	constexpr const char* cObjectTypeEnum = _T("Decimal=0,Text=1,Table=2,GrayImage=3,ColorImage=4,Image=5,States=6");
	constexpr const char* cTypeNamePostfix = _T(" Type");
	const std::array<std::string, 3> g_forbittenNames {"Number of Objects" /*IDS_OBJECTNAME_NUMBER_OF_OBJECTS*/, "State" /*IDS_OBJECTNAME_STATUS*/, "Color" /*IDS_OBJECTNAME_COLOR*/};

	SvPb::MessageContainerVector createMessages(uint32_t objectId, SvStl::MessageTextEnum textId, SvDef::StringVector additionalText = {})
	{
		SvStl::MessageContainerVector messages;
		SvStl::MessageContainer message;
		message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, textId, additionalText, SvStl::SourceFileParams(StdMessageParams), 0, objectId);
		messages.emplace_back(message);
		return SvPb::convertMessageVectorToProtobuf(messages);
	}

	void fillMessageToProtobuf(SvPb::MessageContainerVector* pMessageContainer, uint32_t objectId, SvStl::MessageTextEnum textId, const SvStl::SourceFileParams& messageParam, SvDef::StringVector additionalText = {})
	{
		if (pMessageContainer)
		{
			SvStl::MessageContainer message;
			message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, textId, additionalText, messageParam, 0, objectId);
			SvPb::convertMessageToProtobuf(message, pMessageContainer->add_messages());
		}
	}

	void addMessagesToProtobuf(SvPb::MessageContainerVector* pMessageContainer, const SvStl::MessageContainerVector& rMessageContainer)
	{
		if (pMessageContainer)
		{
			for (const auto& rMessage : rMessageContainer)
			{
				SvPb::convertMessageToProtobuf(rMessage, pMessageContainer->add_messages());
			}
		}
	}

	void checkNameForUnique(uint32_t objectId, SvPb::SetAndSortEmbeddedValueRequest request, SvPb::InspectionCmdResponse& rCmd)
	{
		auto* pResponse = rCmd.mutable_setandsortembeddedvalueresponse();
		for (auto iter = request.embeddedlist().begin(); request.embeddedlist().end() != iter; ++iter)
		{
			std::string name = iter->name();

			//check if forbitten name (already used by common objects)
			//if (std::any_of(g_forbittenNames.begin(), g_forbittenNames.end(), [name](const auto& rEntry) { return rEntry == name; }))
			if (std::ranges::any_of(g_forbittenNames, [name](const auto& rEntry) { return rEntry == name; }))
			{ //duplicate
				pResponse->mutable_errormessages()->CopyFrom(createMessages(objectId, SvStl::Tid_ForbittenNameForParameterName, {name}));
				pResponse->set_errorrow(static_cast<int>(std::distance(request.embeddedlist().begin(), iter)));
				rCmd.set_hresult(E_FAIL);
				return;
			}

			//check name
			if (1 < std::count_if(request.embeddedlist().begin(), request.embeddedlist().end(), [name](const auto& rEntry) { return rEntry.name() == name; }))
			{ //duplicate
				pResponse->mutable_errormessages()->CopyFrom(createMessages(objectId, SvStl::Tid_DuplicateParameterName, {name}));
				pResponse->set_errorrow(static_cast<int>(std::distance(request.embeddedlist().begin(), iter)));
				rCmd.set_hresult(E_FAIL);
				return;
			}
			//linked name
			name = iter->name() + SvDef::cLinkName;
			if (0 < std::count_if(request.embeddedlist().begin(), request.embeddedlist().end(), [name](const auto& rEntry) { return rEntry.name() == name; }))
			{ //duplicate
				pResponse->mutable_errormessages()->CopyFrom(createMessages(objectId, SvStl::Tid_DuplicateParameterNameWithLinked, {name}));
				pResponse->set_errorrow(static_cast<int>(std::distance(request.embeddedlist().begin(), iter)));
				rCmd.set_hresult(E_FAIL);
				return;
			}
			//Type name
			name = iter->name() + cTypeNamePostfix;
			if (0 < std::count_if(request.embeddedlist().begin(), request.embeddedlist().end(), [name](const auto& rEntry) { return rEntry.name() == name; }))
			{ //duplicate
				pResponse->mutable_errormessages()->CopyFrom(createMessages(objectId, SvStl::Tid_DuplicateParameterNameWithType, {name}));
				pResponse->set_errorrow(static_cast<int>(std::distance(request.embeddedlist().begin(), iter)));
				rCmd.set_hresult(E_FAIL);
				return;
			}
		}
	}

	template <typename T>
	const T* convertObject(const SvOi::IObjectClass* pObject)
	{
		try
		{
			const SvVol::LinkedValue* pLinked = dynamic_cast<const SvVol::LinkedValue*>(pObject);
			return dynamic_cast<const T*>(nullptr != pLinked ? pLinked->getLinkedObject() : pObject);
		}
		catch (...)
		{
			return nullptr;
		}
	}

	SvPb::SVObjectTypeEnum getObjectType(const SvOi::IObjectClass* pObject)
	{
		try
		{
			if (nullptr != pObject)
			{
				const SvVol::LinkedValue* pLinked = dynamic_cast<const SvVol::LinkedValue*>(pObject);
				if (nullptr != pLinked)
				{
					auto pLinkedObject = pLinked->getLinkedObject();
					return (nullptr != pLinkedObject) ? pLinkedObject->GetObjectType() : pObject->GetObjectType();
				}
				else
				{
					return pObject->GetObjectType();
				}
			}
		}
		catch (...)
		{
		}
		return SvPb::SVObjectTypeEnum::SVNotSetObjectType;
	}

	void swapValues(SVObjectPtrVector& rList, const SVObjectClass* pFirst, const SVObjectClass* pSecond)
	{
		auto firstIter = std::ranges::find(rList, pFirst);
		auto secondIter = std::ranges::find(rList, pSecond);
		if (rList.end() != firstIter && rList.end() != secondIter)
		{
			std::swap(*firstIter, *secondIter);
		}
	}

	std::string getFirstFreeName(SVObjectPtrVector& rList, const std::string& rName)
	{
		std::string retName {rName};
		while (std::any_of(rList.begin(), rList.end(), [retName](const auto* pEntry) { return pEntry && pEntry->GetName() == retName; }))
		{
			retName += "z";
		}
		return retName;
	}
}
#pragma endregion Declarations

	ParameterTask::ParameterTask(SvPb::EmbeddedIdEnum startEmbeddedIdValue, SvPb::EmbeddedIdEnum startEmbeddedIdType, LPCTSTR objectTypeEnumString,
		SVObjectClass* pOwner, int StringResourceID)
		: SVTaskObjectClass(pOwner, StringResourceID)
		, m_startEmbeddedIdValue(startEmbeddedIdValue)
		, m_startEmbeddedIdType(startEmbeddedIdType)
		, m_objectTypeEnumString(objectTypeEnumString)
	{
		init();
	}

	ParameterTask::~ParameterTask()
	{
	}

	bool ParameterTask::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
	{
		m_isCreated = __super::CreateObject(rCreateStructure);
		registerParameter();
		return m_isCreated;
	}

	bool ParameterTask::CloseObject()
	{
		for (auto& rObject : m_objects)
		{
			auto iter = std::find(m_embeddedList.begin(), m_embeddedList.end(), rObject.get());
			if (m_embeddedList.end() != iter)
			{
				m_embeddedList.erase(iter);
			}
			if (rObject.get())
			{
				rObject->CloseObject();
			}
		}
		m_objects.clear();
		for (auto& rObject : m_TypeObjects)
		{
			auto iter = std::find(m_embeddedList.begin(), m_embeddedList.end(), rObject.get());
			if (m_embeddedList.end() != iter)
			{
				m_embeddedList.erase(iter);
			}
			if (rObject.get())
			{
				rObject->CloseObject();
			}
		}
		m_TypeObjects.clear();
		return __super::CloseObject();
	}

	bool ParameterTask::ResetObject(SvStl::MessageContainerVector* pErrorMessages)
	{
		bool Result = __super::ResetObject(pErrorMessages);

		registerParameter();
		Result &= checkValue(pErrorMessages);

		return Result;
	}

	SvPb::InspectionCmdResponse ParameterTask::setAndSortEmbeddedValues(SvPb::SetAndSortEmbeddedValueRequest request)
	{
		SvPb::InspectionCmdResponse cmd;
		auto* pResponse = cmd.mutable_setandsortembeddedvalueresponse();
		pResponse->set_errorrow(-1);
		
		if (request.embeddedlist_size() > cMaxNumberOfObjects)
		{
			pResponse->mutable_errormessages()->CopyFrom(createMessages(getObjectId(), SvStl::Tid_TooManyVariables));
			cmd.set_hresult(E_FAIL);
			return cmd;
		}

		
		//First check for unique names.
		checkNameForUnique(getObjectId(), request, cmd);
		if (S_OK != cmd.hresult())
		{
			return cmd;
		}

		//Check values
		//std::vector<bool> isValuesOK;
		//isValuesOK.resize(request.embeddedlist_size());
		std::vector<variant_t> defaults;
		defaults.resize(request.embeddedlist_size());
		for (int i = 0; request.embeddedlist_size() > i; ++i)
		{
			setObject(i);
			bool isOk = (nullptr != m_objects[i]) && checkValueAndGetDefault(request.embeddedlist(i), *m_objects[i], defaults[i], pResponse->mutable_errormessages());
			if (false == isOk && S_OK == cmd.hresult())
			{	//only set the first errorrow
				pResponse->set_errorrow(i);
				cmd.set_hresult(E_FAIL);
			}
		}
		
		long newFreePos = cMaxNumberOfObjects;
		m_NumberOfObjects.GetValue(newFreePos);

		auto* pEmbeddedList = request.mutable_embeddedlist();
		assert(pEmbeddedList);

		//Set empty-"oldembeddedid" to the free position
		setEmptyOldEmbeddedIdToFreeId(*pEmbeddedList, newFreePos);

		//Sort values in embedded names and set values and rename.
		for (int i = 0; pEmbeddedList->size() > i; ++i)
		{
			swapElements(*pEmbeddedList, i, *pResponse->mutable_errormessages());

			bool isOk = (nullptr != m_objects[i]) && setElement(request.embeddedlist(i), i, defaults[i], pResponse->mutable_errormessages());
			if (false == isOk && S_OK == cmd.hresult())
			{	//only set the first errorrow
				pResponse->set_errorrow(i);
				cmd.set_hresult(E_FAIL);
			}
		}

		m_NumberOfObjects.SetValue(request.embeddedlist_size());
		registerParameter();
		//The next method is needed, because if an object will be deleted, but used in the ResultView, the ResultList has to rebuild, otherwise it can crash.
		GetInspectionInterface()->SetDefaultInputs();

		return cmd;
	}

	SVObjectClass* ParameterTask::OverwriteEmbeddedObject(uint32_t uniqueID, SvPb::EmbeddedIdEnum embeddedID)
	{
		if (int index = embeddedID - m_startEmbeddedIdValue; 0 <= index && index < cMaxNumberOfObjects)
		{
			setObject(index);
		}
		else if (index = embeddedID - m_startEmbeddedIdType; 0 <= index && index < cMaxNumberOfObjects)
		{
			setObject(index);			
		}
		return __super::OverwriteEmbeddedObject(uniqueID, embeddedID);
	}

	void ParameterTask::init()
	{
		// Identify our output type
		m_ObjectTypeInfo.m_ObjectType = SvPb::ParameterTaskObjectType;

		BuildEmbeddedObjectList();
	}

	void ParameterTask::BuildEmbeddedObjectList()
	{
		RegisterEmbeddedObject(&m_NumberOfObjects, SvPb::NumberOfObjectsEId, IDS_OBJECTNAME_NUMBER_OF_OBJECTS, false, SvOi::SVResetItemNone);
		m_NumberOfObjects.SetDefaultValue(0);

		m_objects.resize(cMaxNumberOfObjects);
		m_TypeObjects.resize(cMaxNumberOfObjects);
		
		m_statusTag.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
		m_statusColor.SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
	}

	void ParameterTask::registerParameter()
	{
		long number = 0;
		m_NumberOfObjects.GetValue(number);
		if (number < 0)
		{
			number = 0;
			m_NumberOfObjects.SetValue(number);
		}
		else if (cMaxNumberOfObjects < number)
		{
			number = cMaxNumberOfObjects;
			m_NumberOfObjects.SetValue(number);
		}

		for (int i = 0; number > i; ++i)
		{
			long type;
			setObject(i);
			m_TypeObjects[i]->GetValue(type);
			m_objects[i]->UpdateContent();
			UINT defaultStringValueAttributes = SvPb::viewable | SvPb::publishable | SvPb::archivable | SvPb::embedable | SvPb::dataDefinitionValue;
			UINT defaultValueValueAttributes = SvDef::defaultValueObjectAttributes & (~SvPb::audittrail);
			if (SvPb::ParameterInputObjectType == GetObjectSubType())
			{
				defaultStringValueAttributes |= SvPb::remotelySetable;
				defaultValueValueAttributes |= SvPb::remotelySetable;
			}
			SvPb::InputTypeEnum typeEnum{ type };
			switch (typeEnum)
			{
			case SvPb::InputTypeEnum::TypeDecimal:
				m_objects[i]->SetObjectAttributesAllowed(defaultValueValueAttributes, SvOi::SetAttributeType::AddAttribute);
				break;
			case SvPb::InputTypeEnum::TypeText:
				m_objects[i]->SetObjectAttributesAllowed(defaultStringValueAttributes, SvOi::SetAttributeType::AddAttribute);
				break;
			case SvPb::InputTypeEnum::TypeTable:
			case SvPb::InputTypeEnum::TypeStates:
				m_objects[i]->SetObjectAttributesAllowed(defaultValueValueAttributes | SvPb::taskObject, SvOi::SetAttributeType::AddAttribute);
				break;
			case SvPb::InputTypeEnum::TypeGrayImage:
			case SvPb::InputTypeEnum::TypeColorImage:
			case SvPb::InputTypeEnum::TypeImage:
				m_objects[i]->SetObjectAttributesAllowed(SvPb::archivableImage | SvPb::publishResultImage | SvPb::dataDefinitionImage, SvOi::SetAttributeType::AddAttribute);
				break;
			default:
				assert(false);
				break;
			}
			
			m_TypeObjects[i]->SetObjectAttributesAllowed(defaultStringValueAttributes & (~SvPb::dataDefinitionValue), SvOi::SetAttributeType::OverwriteAttribute);
		}
		for (int i = number; cMaxNumberOfObjects > i; ++i)
		{
			if (nullptr != m_objects[i])
			{
				m_objects[i]->DisconnectInput();
				RemoveEmbeddedObject(m_objects[i].get());
				m_objects[i]->CloseObject();
				m_objects[i] = nullptr;
			}
			if (nullptr != m_TypeObjects[i])
			{
				RemoveEmbeddedObject(m_TypeObjects[i].get());
				m_TypeObjects[i]->CloseObject();
				m_TypeObjects[i] = nullptr;
			}
		}
	}

	bool ParameterTask::checkObject(const std::string& name, const SvOi::IObjectClass* pObject, SvPb::InputTypeEnum type, SvStl::MessageContainerVector* pErrorMessages, uint32_t objectIdForErrorMessage)
	{
		bool bRet = true;
		switch (type)
		{
		case SvPb::InputTypeEnum::TypeTable:
		{
			const SvOp::TableObject* pTable = convertObject<SvOp::TableObject>(pObject);
			if (nullptr == pTable)
			{
				SvDef::StringVector msgList;
				msgList.emplace_back(GetName());
				msgList.emplace_back(name);
				msgList.emplace_back(SvStl::MessageData::convertId2AdditionalText(SvStl::Tid_aTable));
				SvStl::MessageManager Msg(SvStl::MsgType::Log);
				Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidInputObject, msgList, SvStl::SourceFileParams(StdMessageParams), 0, objectIdForErrorMessage);
				if (nullptr != pErrorMessages)
				{
					pErrorMessages->push_back(Msg.getMessageContainer());
				}
				bRet = false;
			}
			break;
		}
		case SvPb::InputTypeEnum::TypeGrayImage:
		case SvPb::InputTypeEnum::TypeColorImage:
		case SvPb::InputTypeEnum::TypeImage:
		{
			const SvIe::SVImageClass* pImage = convertObject<SvIe::SVImageClass>(pObject);
			if (nullptr == pImage)
			{
				SvDef::StringVector msgList;
				msgList.emplace_back(GetName());
				msgList.emplace_back(name);
				msgList.emplace_back(SvStl::MessageData::convertId2AdditionalText(SvStl::Tid_anImage));
				SvStl::MessageManager Msg(SvStl::MsgType::Log);
				Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidInputObject, msgList, SvStl::SourceFileParams(StdMessageParams), 0, objectIdForErrorMessage);
				if (nullptr != pErrorMessages)
				{
					pErrorMessages->push_back(Msg.getMessageContainer());
				}
				bRet = false;
			}
			else
			{
				if (SvPb::InputTypeEnum::TypeGrayImage == type && pImage->GetObjectSubType() != SvPb::SVImageMonoType)
				{
					SvDef::StringVector msgList;
					msgList.emplace_back(GetName());
					msgList.emplace_back(name);
					msgList.emplace_back(SvStl::MessageData::convertId2AdditionalText(SvStl::Tid_aGrayImage));
					SvStl::MessageManager Msg(SvStl::MsgType::Log);
					Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidInputObject, msgList, SvStl::SourceFileParams(StdMessageParams), 0, objectIdForErrorMessage);
					if (nullptr != pErrorMessages)
					{
						pErrorMessages->push_back(Msg.getMessageContainer());
					}
					bRet = false;
				}
				else if (SvPb::InputTypeEnum::TypeColorImage == type && pImage->GetObjectSubType() != SvPb::SVImageColorType)
				{
					SvDef::StringVector msgList;
					msgList.emplace_back(GetName());
					msgList.emplace_back(name);
					msgList.emplace_back(SvStl::MessageData::convertId2AdditionalText(SvStl::Tid_aColorImage));
					SvStl::MessageManager Msg(SvStl::MsgType::Log);
					Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidInputObject, msgList, SvStl::SourceFileParams(StdMessageParams), 0, objectIdForErrorMessage);
					if (nullptr != pErrorMessages)
					{
						pErrorMessages->push_back(Msg.getMessageContainer());
					}
					bRet = false;
				}
			}
			break;
		}
		case SvPb::InputTypeEnum::TypeStates:
		{
			if (SvPb::SVToolObjectType != getObjectType(pObject))
			{
				SvDef::StringVector msgList;
				msgList.emplace_back(GetName());
				msgList.emplace_back(name);
				msgList.emplace_back(SvStl::MessageData::convertId2AdditionalText(SvStl::Tid_aTool));
				SvStl::MessageManager Msg(SvStl::MsgType::Log);
				Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidInputObject, msgList, SvStl::SourceFileParams(StdMessageParams), 0, objectIdForErrorMessage);
				if (nullptr != pErrorMessages)
				{
					pErrorMessages->push_back(Msg.getMessageContainer());
				}
				bRet = false;
			}
			break;
		}
		default:
		{
			SvDef::StringVector msgList;
			msgList.emplace_back(GetName());
			msgList.emplace_back(name);
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidInputType, msgList, SvStl::SourceFileParams(StdMessageParams), 0, objectIdForErrorMessage);
			if (nullptr != pErrorMessages)
			{
				pErrorMessages->push_back(Msg.getMessageContainer());
			}
			bRet = false;
			break;
		}
		}
		return bRet;
	}

	bool ParameterTask::checkValue(SvStl::MessageContainerVector* pErrorMessages)
	{
		bool bRet = true;
		long number = 0;
		m_NumberOfObjects.GetValue(number);
		for (int i = 0; number > i; ++i)
		{
			if (nullptr == m_objects[i] || nullptr == m_TypeObjects[i])
			{
				SvStl::MessageManager Msg(SvStl::MsgType::Log);
				Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidData, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
				if (nullptr != pErrorMessages)
				{
					pErrorMessages->push_back(Msg.getMessageContainer());
				}
				return false;
			}
			auto* pObject = m_objects[i].get();
			long type;
			m_TypeObjects[i]->GetValue(type);
			SvPb::InputTypeEnum typeEnum{ type };
			switch (typeEnum)
			{
			case SvPb::InputTypeEnum::TypeDecimal:
			case SvPb::InputTypeEnum::TypeText:
			{
				_variant_t value;
				HRESULT hr = pObject->GetValue(value);
				if ((S_OK != hr && SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE != hr && E_BOUNDS != hr) || VT_EMPTY == value.vt)
				{
					SvStl::MessageManager Msg(SvStl::MsgType::Log);
					SvDef::StringVector msgList;
					msgList.emplace_back(GetName());
					msgList.emplace_back(pObject->GetName());
					Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidInputValue, msgList, SvStl::SourceFileParams(StdMessageParams), 0, pObject->getObjectId());
					if (nullptr != pErrorMessages)
					{
						pErrorMessages->push_back(Msg.getMessageContainer());
					}
					bRet = false;
				}
				break;
			}
			case SvPb::InputTypeEnum::TypeTable:
			case SvPb::InputTypeEnum::TypeGrayImage:
			case SvPb::InputTypeEnum::TypeColorImage:
			case SvPb::InputTypeEnum::TypeImage:
			case SvPb::InputTypeEnum::TypeStates:
			{
				bRet &= checkObject(pObject->GetName(), pObject, typeEnum, pErrorMessages, pObject->getObjectId());
				break;
			}
			default:
			{
				SvDef::StringVector msgList;
				msgList.emplace_back(GetName());
				msgList.emplace_back(std::to_string(i));
				SvStl::MessageManager Msg(SvStl::MsgType::Log);
				Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidInputType, msgList, SvStl::SourceFileParams(StdMessageParams), 0, pObject->getObjectId());
				if (nullptr != pErrorMessages)
				{
					pErrorMessages->push_back(Msg.getMessageContainer());
				}
				bRet = false;
				break;
			}
			}
		}		
		return bRet;
	}

	void ParameterTask::setObject(int index)
	{
		std::string name = SvUl::LoadStdString(IDS_OBJECTNAME_INPUT_01 + index);
		if (nullptr == m_objects[index])
		{
			m_objects[index] = std::make_unique<SvVol::LinkedValue>();
			CreateChildObject(m_objects[index].get());
			m_objects[index]->setSaveDefaultValueFlag(true);
			RegisterEmbeddedObject(m_objects[index].get(), m_startEmbeddedIdValue + index, name.c_str(), false, SvOi::SVResetItemTool);
		}

		if (nullptr == m_TypeObjects[index])
		{
			m_TypeObjects[index] = std::make_unique <SvVol::SVEnumerateValueObjectClass>();
			CreateChildObject(m_TypeObjects[index].get());
			RegisterEmbeddedObject(m_TypeObjects[index].get(), m_startEmbeddedIdType + index, (name + cTypeNamePostfix).c_str(), false, SvOi::SVResetItemOwner);
			m_TypeObjects[index]->SetEnumTypes(m_objectTypeEnumString);
			m_TypeObjects[index]->SetDefaultValue(0l, true);
		}
	}

	bool ParameterTask::checkValueAndGetDefault(const SvPb::EmbeddedValueStruct& rRequestValue, SvVol::LinkedValue& rValueObject, variant_t& rDefaultValue, SvPb::MessageContainerVector* pErrorMessage)
	{
		try
		{
			auto type = rRequestValue.type();
			switch (type)
			{
				case SvPb::InputTypeEnum::TypeDecimal:
					SvPb::ConvertProtobufToVariant(rRequestValue.value().defaultvalue(), rDefaultValue);
					if (VT_R8 != rDefaultValue.vt)
					{
						fillMessageToProtobuf(pErrorMessage, rValueObject.getObjectId(), SvStl::Tid_ValidateValue_LinkedTypeInvalid, SvStl::SourceFileParams(StdMessageParams));
						return false;
					}
					rValueObject.validateValue(rRequestValue.value());
					break;
				case SvPb::InputTypeEnum::TypeText:
					SvPb::ConvertProtobufToVariant(rRequestValue.value().defaultvalue(), rDefaultValue);
					if (VT_BSTR != rDefaultValue.vt)
					{
						fillMessageToProtobuf(pErrorMessage, rValueObject.getObjectId(), SvStl::Tid_ValidateValue_LinkedTypeInvalid, SvStl::SourceFileParams(StdMessageParams));
						return false;
					}
					rValueObject.validateValue(rRequestValue.value());
					break;
				default: //variant must be empty
					rDefaultValue.Clear();
					if (SvPb::LinkedSelectedOption::IndirectValue == rRequestValue.value().option())
					{
						SVObjectReference refObject {rRequestValue.value().indirectidstring()};
						SvStl::MessageContainerVector msgVector;
						if (false == checkObject(rRequestValue.name(), refObject.getObject(), type, &msgVector, rValueObject.getObjectId()))
						{
							if (pErrorMessage && 0 < msgVector.size())
							{
								SvPb::convertMessageToProtobuf(msgVector[0], pErrorMessage->add_messages());
							}
							return false;
						}
					}
					else
					{
						fillMessageToProtobuf(pErrorMessage, rValueObject.getObjectId(), SvStl::Tid_ValidateValue_LinkedObjectInvalid, SvStl::SourceFileParams(StdMessageParams));
						return false;
					}
					break;
			}
		}
		catch (const SvStl::MessageContainer& e)
		{
			SvStl::MessageData data = e.getMessage();
			if (data.m_AdditionalTextId && 1 == data.m_AdditionalTextList.size())
			{
				data.m_AdditionalTextList[0] = rRequestValue.name();
			}
			SvStl::MessageContainerVector messages;
			messages.emplace_back(data, rValueObject.getObjectId(), false);
			addMessagesToProtobuf(pErrorMessage, messages);
			return false;
		}
		catch (...)
		{
			SvDef::StringVector nameMessage;
			nameMessage.emplace_back(rRequestValue.name());
			fillMessageToProtobuf(pErrorMessage, rValueObject.getObjectId(), SvStl::Tid_SetParameterValuesFailed, SvStl::SourceFileParams(StdMessageParams), nameMessage);
			return false;
		}

		return true;
	}

	void ParameterTask::setEmptyOldEmbeddedIdToFreeId(google::protobuf::RepeatedPtrField< SvPb::EmbeddedValueStruct >& rEmbeddedList, long newFreePos)
	{
		for (auto iter = rEmbeddedList.begin(); rEmbeddedList.end() != iter; ++iter)
		{
			if (SvPb::NoEmbeddedId == iter->oldembeddedid())
			{
				if (newFreePos < cMaxNumberOfObjects)
				{
					iter->set_oldembeddedid(m_startEmbeddedIdValue + newFreePos);
					setObject(newFreePos);
					newFreePos++;
				}
				else
				{
					for (SvPb::EmbeddedIdEnum id = m_startEmbeddedIdValue; id < m_startEmbeddedIdValue + cMaxNumberOfObjects; id = id + 1)
					{
						if (std::none_of(rEmbeddedList.begin(), rEmbeddedList.end(), [id](auto& rObj) { return rObj.oldembeddedid() == id; }))
						{
							iter->set_oldembeddedid(id);
							int pos = id - m_startEmbeddedIdValue;
							m_objects[pos]->DisconnectInput();
							break;
						}
					}
				}
			}
		}
	}

	void ParameterTask::swapElements(google::protobuf::RepeatedPtrField< SvPb::EmbeddedValueStruct >& rValues, int pos, SvPb::MessageContainerVector& rMessageVec)
	{
		const auto& rValue = rValues[pos];
		if (SvPb::NoEmbeddedId != rValue.oldembeddedid())
		{
			int oldPos = rValue.oldembeddedid() - m_startEmbeddedIdValue;
			assert(pos <= oldPos && cMaxNumberOfObjects > oldPos);
			if (pos < oldPos && cMaxNumberOfObjects > oldPos)
			{
				//swap objectIds in errormessages
				uint32_t oldObjectId = (nullptr != m_objects[oldPos]) ? m_objects[oldPos]->getObjectId() : SvDef::InvalidObjectId;
				uint32_t newObjectId = (nullptr != m_objects[pos]) ? m_objects[pos]->getObjectId() : SvDef::InvalidObjectId;
				auto iterMessage = std::find_if(rMessageVec.mutable_messages()->begin(), rMessageVec.mutable_messages()->end(), [oldObjectId](const auto& rEntry) { return rEntry.objectid() == oldObjectId; });
				if (rMessageVec.mutable_messages()->end() != iterMessage)
				{
					iterMessage->set_objectid(newObjectId);
				}
				else
				{
					iterMessage = std::find_if(rMessageVec.mutable_messages()->begin(), rMessageVec.mutable_messages()->end(), [newObjectId](const auto& rEntry) { return rEntry.objectid() == newObjectId; });
					if (rMessageVec.mutable_messages()->end() != iterMessage)
					{
						iterMessage->set_objectid(oldObjectId);
					}
				}

				//swap in embeddedList to have same order in all lists
				swapValues(m_embeddedList, m_objects[pos].get(), m_objects[oldPos].get());
				swapValues(m_embeddedList, m_TypeObjects[pos].get(), m_TypeObjects[oldPos].get());

				std::swap(m_objects[pos], m_objects[oldPos]);
				std::swap(m_TypeObjects[pos], m_TypeObjects[oldPos]);
				for (auto iter = rValues.begin() + pos; rValues.end() != iter; ++iter)
				{
					if (iter->oldembeddedid() == m_startEmbeddedIdValue + pos)
					{	//move id, because position moved.
						iter->set_oldembeddedid(rValue.oldembeddedid());
					}
				}
			}
		}
	}

	bool ParameterTask::setElement(const SvPb::EmbeddedValueStruct& rValue, int pos, const variant_t& rDefault, SvPb::MessageContainerVector* pErrorMessage)
	{
		try
		{
			m_TypeObjects[pos]->SetValue(rValue.type());
			std::string oldName = m_objects[pos]->GetName();
			if (rValue.name() != oldName)
			{
				//check if the new name is already used. Then rename it temporary. It will be renamed to the correct name if this entry is at its turn.
				std::string tmpName = rValue.name();
				auto iterObject = std::ranges::find_if(m_embeddedList, [tmpName](const auto* pEntry) { return pEntry && pEntry->GetName() == tmpName; });
				if (m_embeddedList.end() != iterObject && nullptr != *iterObject)
				{
					std::string newName = getFirstFreeName(m_embeddedList, tmpName);
					(*iterObject)->SetObjectName(newName.c_str());
					GetInspection()->OnObjectRenamed(*(*iterObject), tmpName);
				}
				tmpName = rValue.name() + cTypeNamePostfix;
				iterObject = std::ranges::find_if(m_embeddedList, [tmpName](const auto* pEntry) { return pEntry && pEntry->GetName() == tmpName; });
				if (m_embeddedList.end() != iterObject)
				{
					std::string newName = getFirstFreeName(m_embeddedList, tmpName);
					(*iterObject)->SetObjectName(newName.c_str());
					GetInspection()->OnObjectRenamed(*(*iterObject), tmpName);
				}
			}
			m_TypeObjects[pos]->SetObjectEmbedded(m_startEmbeddedIdType + pos, this, (rValue.name() + cTypeNamePostfix).c_str());
			m_objects[pos]->SetObjectEmbedded(m_startEmbeddedIdValue + pos, this, rValue.name().c_str());
			if (rValue.name() != oldName)
			{
				GetInspection()->OnObjectRenamed(*m_objects[pos], oldName);
				GetInspection()->OnObjectRenamed(*m_TypeObjects[pos], oldName + cTypeNamePostfix);
			}
			m_objects[pos]->SetDefaultValue(rDefault);
			m_objects[pos]->setValue(rValue.value());
		}
		catch (const SvStl::MessageContainer& e)
		{
			SvStl::MessageContainerVector messages;
			messages.emplace_back(e.getMessage(), m_objects[pos]->getObjectId(), false);
			addMessagesToProtobuf(pErrorMessage, messages);
			return false;
		}
		catch (...)
		{
			SvDef::StringVector nameMessage;
			nameMessage.emplace_back(rValue.name());
			fillMessageToProtobuf(pErrorMessage, m_objects[pos]->getObjectId(), SvStl::Tid_SetParameterValuesFailed, SvStl::SourceFileParams(StdMessageParams), nameMessage);
			return false;
		}
		return true;
	}

	SV_IMPLEMENT_CLASS(InputParameterTask, SvPb::InputParameterTaskClassId);
	InputParameterTask::InputParameterTask(SVObjectClass* pOwner, int StringResourceID)
		: ParameterTask(SvPb::ExternalInputEId, SvPb::InputObjectTypeEId, cObjectTypeEnum, pOwner, StringResourceID)
	{
		m_ObjectTypeInfo.m_ObjectType = SvPb::ParameterTaskObjectType;
		m_ObjectTypeInfo.m_SubType = SvPb::ParameterInputObjectType;
	}

	InputParameterTask::~InputParameterTask()
	{
	}

	SV_IMPLEMENT_CLASS(ResultParameterTask, SvPb::ResultParameterTaskClassId);
	ResultParameterTask::ResultParameterTask(SVObjectClass* pOwner, int StringResourceID)
		: ParameterTask(SvPb::ResultObjectValueEId, SvPb::ResultObjectTypeEId, cObjectTypeEnum, pOwner, StringResourceID)
	{
		m_ObjectTypeInfo.m_ObjectType = SvPb::ParameterTaskObjectType;
		m_ObjectTypeInfo.m_SubType = SvPb::ParameterResultObjectType;
	}

	ResultParameterTask::~ResultParameterTask()
	{
	}

} //namespace SvOp
