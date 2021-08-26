//*****************************************************************************
/// \copyright (c) 2020,2020 by Seidenader Maschinenbau GmbH
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

	constexpr long cMaxNumberOfObjects = 50;
	constexpr const char* cObjectTypeEnum = _T("Decimal=0,Text=1,Table=2,GrayImage=3,ColorImage=4,Image=5");
	constexpr const char* cTypeNamePostfix = _T(" Type");

	SvPb::MessageContainerVector createMessage(uint32_t objectId, SvStl::MessageTextEnum textId, SvDef::StringVector additionalText = {})
	{
		SvStl::MessageContainerVector messages;
		SvStl::MessageContainer message;
		message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, textId, additionalText, SvStl::SourceFileParams(StdMessageParams), 0, objectId);
		messages.emplace_back(message);
		return SvPb::convertMessageVectorToProtobuf(messages);
	}

	void checkNameForUnique(uint32_t objectId, SvPb::SetAndSortEmbeddedValueRequest request, SvPb::InspectionCmdResponse& rCmd)
	{
		auto* pResponse = rCmd.mutable_setandsortembeddedvalueresponse(); 
		for (auto iter = request.embeddedlist().begin(); request.embeddedlist().end() != iter; ++iter)
		{
			//check name
			std::string name = iter->name();
			SvDef::StringVector nameMessage;
			nameMessage.emplace_back(name);
			if (1 < std::count_if(request.embeddedlist().begin(), request.embeddedlist().end(), [name](const auto& rEntry) { return rEntry.name() == name; }))
			{ //duplicate
				pResponse->mutable_errormessages()->CopyFrom(createMessage(objectId, SvStl::Tid_DuplicateParameterName, nameMessage));
				pResponse->set_errorrow(static_cast<int>(std::distance(request.embeddedlist().begin(), iter)));
				rCmd.set_hresult(E_FAIL);
				return;
			}
			//linked name
			name = iter->name() + SvDef::cLinkName;
			if (0 < std::count_if(request.embeddedlist().begin(), request.embeddedlist().end(), [name](const auto& rEntry) { return rEntry.name() == name; }))
			{ //duplicate
				pResponse->mutable_errormessages()->CopyFrom(createMessage(objectId, SvStl::Tid_DuplicateParameterNameWithLinked, nameMessage));
				pResponse->set_errorrow(static_cast<int>(std::distance(request.embeddedlist().begin(), iter)));
				rCmd.set_hresult(E_FAIL);
				return;
			}
			//Type name
			name = iter->name() + cTypeNamePostfix;
			if (0 < std::count_if(request.embeddedlist().begin(), request.embeddedlist().end(), [name](const auto& rEntry) { return rEntry.name() == name; }))
			{ //duplicate
				pResponse->mutable_errormessages()->CopyFrom(createMessage(objectId, SvStl::Tid_DuplicateParameterNameWithType, nameMessage));
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
#pragma endregion Declarations

	ParameterTask::ParameterTask(SvPb::EmbeddedIdEnum startEmbeddedIdValue, SvPb::EmbeddedIdEnum startEmbeddedIdType,
		SVObjectClass* pOwner, int StringResourceID)
		: SVTaskObjectClass(pOwner, StringResourceID)
		, m_startEmbeddedIdValue(startEmbeddedIdValue)
		, m_startEmbeddedIdType(startEmbeddedIdType)
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
		//@TODO[MZA][10.20][02.07.2021] more comments needed and method should be split in more methods
		SvPb::InspectionCmdResponse cmd;
		auto* pResponse = cmd.mutable_setandsortembeddedvalueresponse();
		pResponse->set_errorrow(-1);
		
		if (request.embeddedlist_size() > cMaxNumberOfObjects)
		{
			pResponse->mutable_errormessages()->CopyFrom(createMessage(getObjectId(), SvStl::Tid_TooManyVariables));
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
		std::vector<variant_t> defaults;
		defaults.resize(request.embeddedlist_size());
		for (int i = 0; request.embeddedlist_size() > i; ++i)
		{
			try
			{
				const auto& rValues = request.embeddedlist(i);
				auto type = rValues.type();
				switch (type)
				{
				case SvPb::InputTypeEnum::TypeDecimal:
					SvPb::ConvertProtobufToVariant(rValues.value().defaultvalue(), defaults[i]);
					if (VT_R8 != defaults[i].vt)
					{
						pResponse->mutable_errormessages()->CopyFrom(createMessage(getObjectId(), SvStl::Tid_ValidateValue_LinkedTypeInvalid));
						cmd.set_hresult(E_FAIL);
						return cmd;
					}
					m_objects[i]->validateValue(rValues.value());
					break;
				case SvPb::InputTypeEnum::TypeText:
					SvPb::ConvertProtobufToVariant(rValues.value().defaultvalue(), defaults[i]);
					if (VT_BSTR != defaults[i].vt)
					{
						pResponse->mutable_errormessages()->CopyFrom(createMessage(getObjectId(), SvStl::Tid_ValidateValue_LinkedTypeInvalid));
						cmd.set_hresult(E_FAIL);
						return cmd;
					}
					m_objects[i]->validateValue(rValues.value());
					break;
				default: //variant must be empty
					defaults[i].Clear();
					if (SvPb::LinkedSelectedType::IndirectValue == rValues.value().type())
					{
						SVObjectReference refObject{ rValues.value().indirectidstring() };
						SvStl::MessageContainerVector msgVector;
						if (false == checkObject(rValues.name(), refObject.getObject(), type, &msgVector))
						{
							pResponse->mutable_errormessages()->CopyFrom(SvPb::convertMessageVectorToProtobuf(msgVector));
							cmd.set_hresult(E_FAIL);
							return cmd;
						}
					}
					else
					{
						assert(false);
						pResponse->mutable_errormessages()->CopyFrom(createMessage(getObjectId(), SvStl::Tid_ValidateValue_LinkedObjectInvalid));
						cmd.set_hresult(E_FAIL);
						return cmd;
					}
					break;
				}
			}
			catch (const SvStl::MessageContainer& e)
			{
				SvStl::MessageData data = e.getMessage();
				if (data.m_AdditionalTextId && 1 == data.m_AdditionalTextList.size() )
				{
					data.m_AdditionalTextList[0] = request.embeddedlist(i).name();
				}
				SvStl::MessageContainerVector messages;
				messages.emplace_back(data, getObjectId(), false);
				pResponse->mutable_errormessages()->CopyFrom(SvPb::convertMessageVectorToProtobuf(messages));
				pResponse->set_errorrow(i);
				cmd.set_hresult(E_FAIL);
				return cmd;
			}
			catch (...)
			{
				SvDef::StringVector nameMessage;
				nameMessage.emplace_back(request.embeddedlist(i).name());
				pResponse->mutable_errormessages()->CopyFrom(createMessage(getObjectId(), SvStl::Tid_SetParameterValuesFailed, nameMessage));
				pResponse->set_errorrow(i);
				cmd.set_hresult(E_FAIL);
				return cmd;
			}
		}
		
		long newFreePos = cMaxNumberOfObjects;
		m_NumberOfObjects.GetValue(newFreePos);

		//Set empty-"oldembeddedid" to the free position
		for (auto iter = request.mutable_embeddedlist()->begin(); request.mutable_embeddedlist()->end() != iter; ++iter)
		{
			if (SvPb::NoEmbeddedId == iter->oldembeddedid())
			{
				if (newFreePos < cMaxNumberOfObjects)
				{
					iter->set_oldembeddedid(m_startEmbeddedIdValue + newFreePos);
					newFreePos++;
				}
				else
				{
					for (SvPb::EmbeddedIdEnum id = m_startEmbeddedIdValue; id < m_startEmbeddedIdValue + cMaxNumberOfObjects; id = id + 1)
					{
						if (std::none_of(request.embeddedlist().begin(), request.embeddedlist().end(), [id](auto& rObj) { return rObj.oldembeddedid() == id; }))
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

		//Sort values in embedded names and set values and rename.
		for (int i = 0; request.embeddedlist_size() > i; ++i)
		{
			const auto& rValues = request.embeddedlist(i);
			if (SvPb::NoEmbeddedId != rValues.oldembeddedid())
			{
				int oldPos = rValues.oldembeddedid() - m_startEmbeddedIdValue;
				assert(i <= oldPos && cMaxNumberOfObjects > oldPos);
				if (i < oldPos && cMaxNumberOfObjects > oldPos)
				{
					std::swap(m_objects[i], m_objects[oldPos]);
					std::swap(m_TypeObjects[i], m_TypeObjects[oldPos]);
					for (auto iter = request.mutable_embeddedlist()->begin() + i; request.mutable_embeddedlist()->end() != iter; ++iter)
					{
						if (iter->oldembeddedid() == m_startEmbeddedIdValue + i)
						{	//move id, because position moved.
							iter->set_oldembeddedid(rValues.oldembeddedid());
						}
					}
				}
			}
			std::string oldName = m_objects[i]->GetName();
			m_TypeObjects[i]->SetValue(rValues.type());
			m_TypeObjects[i]->SetObjectEmbedded(m_startEmbeddedIdType + i, this, (rValues.name() + cTypeNamePostfix).c_str());
			m_objects[i]->SetObjectEmbedded(m_startEmbeddedIdValue + i, this, rValues.name().c_str());
			m_objects[i]->setDefaultValue(defaults[i]);
			m_objects[i]->setValue(rValues.value());
			assert (m_objects[i]->resetAllObjects());
			if (rValues.name() != oldName)
			{
				GetInspection()->OnObjectRenamed(*m_objects[i], oldName);
				GetInspection()->OnObjectRenamed(*m_TypeObjects[i], oldName + cTypeNamePostfix);
			}
		}

		for (int i = request.embeddedlist_size(); m_objects.size() > i; ++i)
		{
			m_objects[i]->SetObjectEmbedded(m_startEmbeddedIdValue + i, this, m_objects[i]->GetObjectName());
			m_TypeObjects[i]->SetObjectEmbedded(m_startEmbeddedIdType + i, this, m_TypeObjects[i]->GetObjectName());
		}

		m_NumberOfObjects.SetValue(request.embeddedlist_size());
		registerParameter();
		//The next method is needed, because if an object will be deleted, but used in the ResultView, the ResultList has to rebuild, otherwise it can crash.
		GetInspectionInterface()->SetDefaultInputs();

		return cmd;
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
		for (int i = 0; i < cMaxNumberOfObjects; ++i)
		{
			std::string name = SvUl::LoadStdString(IDS_OBJECTNAME_INPUT_01 + i);
			m_objects[i] = std::make_unique<SvVol::LinkedValue>();
			m_objects[i]->setSaveDefaultValueFlag(true);
			m_TypeObjects[i] = std::make_unique <SvVol::SVEnumerateValueObjectClass>();
			RegisterEmbeddedObject(m_objects[i].get(), m_startEmbeddedIdValue + i, name.c_str(), false, SvOi::SVResetItemTool);
			RegisterEmbeddedObject(m_TypeObjects[i].get(), m_startEmbeddedIdType + i, (name + cTypeNamePostfix).c_str(), false, SvOi::SVResetItemOwner);
			m_TypeObjects[i]->SetEnumTypes(cObjectTypeEnum);
			m_TypeObjects[i]->SetDefaultValue(0l, true);
		}

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
			m_objects[i]->setDefaultValue(0.0);
			m_objects[i]->setDirectValue(m_objects[i]->getDefaultValue());
			m_objects[i]->SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
			m_objects[i]->DisconnectInput();
			m_TypeObjects[i]->SetObjectAttributesAllowed(SvPb::noAttributes, SvOi::SetAttributeType::OverwriteAttribute);
		}
	}

	bool ParameterTask::checkObject(const std::string& name, const SvOi::IObjectClass* pObject, SvPb::InputTypeEnum type, SvStl::MessageContainerVector* pErrorMessages)
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
				msgList.emplace_back(name);
				SvStl::MessageManager Msg(SvStl::MsgType::Log);
				Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidInputTable, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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
				msgList.emplace_back(name);
				SvStl::MessageManager Msg(SvStl::MsgType::Log);
				Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidInputImage, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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
					msgList.emplace_back(name);
					SvStl::MessageManager Msg(SvStl::MsgType::Log);
					Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidInputGrayImage, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					if (nullptr != pErrorMessages)
					{
						pErrorMessages->push_back(Msg.getMessageContainer());
					}
					bRet = false;
				}
				else if (SvPb::InputTypeEnum::TypeColorImage == type && pImage->GetObjectSubType() != SvPb::SVImageColorType)
				{
					SvDef::StringVector msgList;
					msgList.emplace_back(name);
					SvStl::MessageManager Msg(SvStl::MsgType::Log);
					Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidInputColorImage, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
					if (nullptr != pErrorMessages)
					{
						pErrorMessages->push_back(Msg.getMessageContainer());
					}
					bRet = false;
				}
			}
			break;
		}
		default:
		{
			SvDef::StringVector msgList;
			msgList.emplace_back(name);
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidInputType, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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
					msgList.emplace_back(pObject->GetName());
					Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidInputValue, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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
			{
				bRet &= checkObject(pObject->GetName(), pObject, typeEnum, pErrorMessages);
				break;
			}
			default:
			{
				SvDef::StringVector msgList;
				msgList.emplace_back(std::to_string(i));
				SvStl::MessageManager Msg(SvStl::MsgType::Log);
				Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_InvalidInputType, msgList, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
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

	SV_IMPLEMENT_CLASS(InputParameterTask, SvPb::InputParameterTaskClassId);
	InputParameterTask::InputParameterTask(SVObjectClass* pOwner, int StringResourceID)
		: ParameterTask(SvPb::ExternalInputEId, SvPb::InputObjectTypeEId, pOwner, StringResourceID)
	{
		m_ObjectTypeInfo.m_ObjectType = SvPb::ParameterTaskObjectType;
		m_ObjectTypeInfo.m_SubType = SvPb::ParameterInputObjectType;
	}

	InputParameterTask::~InputParameterTask()
	{
	}

	SV_IMPLEMENT_CLASS(ResultParameterTask, SvPb::ResultParameterTaskClassId);
	ResultParameterTask::ResultParameterTask(SVObjectClass* pOwner, int StringResourceID)
		: ParameterTask(SvPb::ResultObjectValueEId, SvPb::ResultObjectTypeEId, pOwner, StringResourceID)
	{
		m_ObjectTypeInfo.m_ObjectType = SvPb::ParameterTaskObjectType;
		m_ObjectTypeInfo.m_SubType = SvPb::ParameterResultObjectType;
	}

	ResultParameterTask::~ResultParameterTask()
	{
	}

} //namespace SvOp
