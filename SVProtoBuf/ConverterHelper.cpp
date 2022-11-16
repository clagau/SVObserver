//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file ConverterHelper.cpp
/// All Rights Reserved
//*****************************************************************************
/// Converter helper functions between protobuf and other data types
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include <codecvt>
#include <locale>
#include "ConverterHelper.h"
#include "Definitions/GlobalConst.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVMessage/SVMessage.h"
#pragma endregion Includes

namespace SvPb
{

HRESULT ConvertVariantToProtobuf(const _variant_t& rVariant, SvPb::Variant* pPbVariant)
{
	HRESULT Result {S_OK};

	if (nullptr != pPbVariant)
	{
		if (VT_ARRAY != (rVariant.vt & VT_ARRAY))
		{
			switch (rVariant.vt)
			{
				case VT_BOOL:
					pPbVariant->set_type(VT_BOOL);
					pPbVariant->set_bval(rVariant);
					break;

				case VT_I1:
				case VT_I2:
				case VT_I4:
				case VT_INT:
					pPbVariant->set_type(rVariant.vt);
					pPbVariant->set_lval(static_cast<long> (rVariant));
					break;

				case VT_UI1:
				case VT_UI2:
				case VT_UI4:
				case VT_UINT:
					pPbVariant->set_type(rVariant.vt);
					pPbVariant->set_ulval(static_cast<unsigned long> (rVariant));
					break;

				 case VT_I8:
					 pPbVariant->set_type(VT_I8);
					 pPbVariant->set_llval(rVariant);
					 break;

				 case VT_UI8:
					 pPbVariant->set_type(VT_UI8);
					 pPbVariant->set_ullval(rVariant);
					 break;

				case VT_R4:
					pPbVariant->set_type(VT_R4);
					pPbVariant->set_fltval(rVariant);
					break;

				case VT_R8:
					pPbVariant->set_type(VT_R8);
					pPbVariant->set_dblval(rVariant);
					break;

				case VT_BSTR:
				{
					pPbVariant->set_type(VT_BSTR);
					std::string Text;
					Text.assign(_bstr_t(rVariant));
					pPbVariant->set_strval(SvUl::to_utf8(Text));
					break;
				}

				default:
					break;
			}
		}
		else
		{
			VARTYPE Type = rVariant.vt & ~VT_ARRAY;
			if (nullptr != rVariant.parray && 1 == rVariant.parray->cDims)
			{
				int count = rVariant.parray->rgsabound[0].cElements;
				//For string lists we will generate a semicolon separated string (no semicolons are allowed in the list)
				if (VT_BSTR == Type)
				{
					std::string StringArray;
					for (int i = 0; i < count; i++)
					{
						LONG Index = rVariant.parray->rgsabound[0].lLbound + i;
						_bstr_t Value;
						::SafeArrayGetElement(rVariant.parray, &Index, Value.GetAddress());
						if (!StringArray.empty())
						{
							StringArray += _T(";");
						}
						std::string Text;
						Text.assign(Value);
						StringArray += Text;
					}
					pPbVariant->set_count(count);
					pPbVariant->set_type(rVariant.vt);
					//Note SVRemoteControl.ocx has always used strval and ConvertProtobufToVariant as well so not bytesval!
					pPbVariant->set_strval(SvUl::to_utf8(StringArray));
				}
				else
				{
					std::vector<char> DataBlock;
					int BlockSize = count * rVariant.parray->cbElements;
					
					if (BlockSize > 0)
					{
						DataBlock.resize(BlockSize);
						memcpy(&DataBlock[0], rVariant.parray->pvData, BlockSize);
						pPbVariant->set_bytesval(&DataBlock[0], DataBlock.size());
					}
				
					pPbVariant->set_count(count);
					pPbVariant->set_type(rVariant.vt);
					
				}
			}
		}
	}

	return Result;
}

HRESULT ConvertProtobufToVariant(const SvPb::Variant& rPbVariant, _variant_t& rVariant, bool simpleType /*= false*/)
{
	
	HRESULT Result {S_OK};

	rVariant.vt = static_cast<VARTYPE> (rPbVariant.type());
	//@TODO[MEC][10.20][02.03.2022] use case	from message instead 
	switch (rPbVariant.type())
	{
		case VT_BOOL:
			rVariant.boolVal = (rPbVariant.bval()== 0)? VARIANT_FALSE : VARIANT_TRUE   ;
			break;

		case VT_I1:
			rVariant.cVal = static_cast<char> (rPbVariant.lval());
			break;

		case VT_I2:
			rVariant.iVal = static_cast<short> (rPbVariant.lval());
			break;

		case VT_I4:
			rVariant.lVal = rPbVariant.lval();
			break;

		case VT_INT:
			rVariant.intVal = static_cast<int> (rPbVariant.lval());
			break;

		case VT_UI1:
			rVariant.bVal = static_cast<BYTE> (rPbVariant.ulval());
			break;

		case VT_UI2:
			rVariant.uiVal = static_cast<unsigned short> (rPbVariant.ulval());
			break;

		case VT_UI4:
			rVariant.ulVal = rPbVariant.ulval();
			break;

		case VT_UINT:
			rVariant.uintVal = static_cast<unsigned int> (rPbVariant.ulval());
			break;

		case VT_I8:
			rVariant.llVal = rPbVariant.llval();
			break;

		case VT_UI8:
			rVariant.ullVal = rPbVariant.ullval();
			break;

		case VT_R4:
			rVariant.fltVal = rPbVariant.fltval();
			break;

		case VT_R8:
			rVariant.dblVal = rPbVariant.dblval();
			break;

		case VT_BSTR:
		{
			_bstr_t bstr(SvUl::to_ansi(rPbVariant.strval()).c_str());
			rVariant.bstrVal = bstr.Detach();
			break;
		}

		default:
			break;
	}


	if (VT_ARRAY == (rPbVariant.type() & VT_ARRAY))
	{
		VARTYPE Type = rPbVariant.type() & ~VT_ARRAY;
		size_t memorySize{ 0ULL };
		switch (Type)
		{
		case VT_BOOL:
			memorySize = sizeof(VARIANT::boolVal);
			break;
		case VT_I1:
			memorySize = sizeof(VARIANT::cVal);
			break;
		case VT_UI1:
			memorySize = sizeof(VARIANT::bVal);
			break;
		case VT_I2:
			memorySize = sizeof(VARIANT::iVal);
			break;
		case VT_UI2:
			memorySize = sizeof(VARIANT::uiVal);
			break;
		case VT_I4:
			memorySize = sizeof(VARIANT::lVal);
			break;
		case VT_UI4:
			memorySize = sizeof(VARIANT::ulVal);
			break;
		case VT_I8:
			memorySize = sizeof(VARIANT::llVal);
			break;
		case VT_UI8:
			memorySize = sizeof(VARIANT::ullVal);
			break;
		case VT_INT:
			memorySize = sizeof(VARIANT::intVal);
			break;
		case VT_UINT:
			memorySize = sizeof(VARIANT::uintVal);
			break;
		case VT_R4:
			memorySize = sizeof(VARIANT::fltVal);
			break;
		case VT_R8:
			memorySize = sizeof(VARIANT::dblVal);
			break;
		default:
			break;
		}

		SAFEARRAY* pSafeArray = ::SafeArrayCreateVector(Type, 0, rPbVariant.count());
		if (nullptr != pSafeArray)
		{
			if (VT_BSTR == Type)
			{
				std::string StringArray = SvUl::to_ansi(rPbVariant.strval());

				std::vector<std::string> Items;
				Items.reserve(rPbVariant.count());
				std::string Item;
				std::istringstream tokenStream(StringArray);
				while (std::getline(tokenStream, Item, ';'))
				{
					Items.push_back(Item);
				}
				LONG Index {0L};
				for (const auto& rEntry : Items)
				{
					_bstr_t Text{ rEntry.c_str() };
					Result = ::SafeArrayPutElement(pSafeArray, &Index, static_cast<void*>(Text.GetBSTR()));
					
					if (S_OK != Result)
					{
						break;
					}
					Index++;
				}
			}
			else
			{
				if(rPbVariant.bytesval().length() == rPbVariant.count() * memorySize)
				{
					void* pData{ nullptr };
					Result = ::SafeArrayAccessData(pSafeArray, &pData);
					if (S_OK == Result)
					{
						memcpy(pData, rPbVariant.bytesval().c_str(), rPbVariant.bytesval().length());
						::SafeArrayUnaccessData(pSafeArray);
					}
				}
				else
				{
					Result = E_INVALIDARG;
				}
			}
			rVariant.parray = pSafeArray;
		}
		else
		{
			Result = E_POINTER;
		}
	}

	//@TODO[gra][8.20][15.01.2019]: This should be removed when the script tester can handle the variant types
	//Simple type is required by the script tester application (SVRemoteControl.ocx)
	if(simpleType)
	{
		switch (rVariant.vt)
		{
			case VT_I1:
			case VT_I2:
			case VT_INT:
				::VariantChangeTypeEx(&rVariant, &rVariant, SvDef::LCID_USA, 0, VT_I4);
				break;
	
			case VT_UI1:
			case VT_UI2:
			case VT_UINT:
				::VariantChangeTypeEx(&rVariant, &rVariant, SvDef::LCID_USA, 0, VT_UI4);
				break;

			default:
				break;
		}
	}

	return Result;
}

SvPb::MessageContainerVector createErrorMessages(uint32_t objectId, const SvStl::SourceFileParams& rSFParams, SvStl::MessageTextEnum textId, SvDef::StringVector additionalText)
{
	SvStl::MessageContainerVector messages;
	SvStl::MessageContainer message;
	message.setMessage(SVMSG_SVO_92_GENERAL_ERROR, textId, additionalText, rSFParams, objectId);
	messages.emplace_back(message);
	return SvPb::convertMessageVectorToProtobuf(messages);
}

void ConvertStringListToProtobuf(const std::set<std::string>& rList, SvPb::Variant* pVariant)
{
	if (nullptr != pVariant)
	{
		std::string Text;
		//For protobuf we shall converts string sets to a semicolon separated string (The List should not contain any semicolons)
		for (const auto& rEntry : rList)
		{
			if (!Text.empty())
			{
				Text += _T(";");
			}
			Text += rEntry;
		}
		if (0 < rList.size() && 0 < Text.size())
		{
			//Set type to VT_ARRAY so that it is clear that a comma separated list is being sent
			pVariant->set_type(VT_BSTR | VT_ARRAY);
			pVariant->set_strval(SvUl::to_utf8(Text));
			pVariant->set_count(static_cast<int> (rList.size()));
		}
	}
}

MessageContainerVector convertMessageVectorToProtobuf(const SvStl::MessageContainerVector& rMessageVec)
{
	MessageContainerVector messageVecPB;
	for (const auto& rMessage : rMessageVec)
	{
		convertMessageToProtobuf(rMessage, messageVecPB.add_messages());
	}
	return messageVecPB;
}

void convertMessageToProtobuf(const SvStl::MessageContainer& rMessage, MessageContainer* pMessagePB)
{
	const auto& rMessageData = rMessage.getMessage();
	pMessagePB->set_messagecode(rMessageData.m_MessageCode);
	pMessagePB->set_additionaltextid(rMessageData.m_AdditionalTextId);
	for (auto text : rMessageData.m_AdditionalTextList)
	{
		pMessagePB->add_additionaltextlist(text);
	}
	pMessagePB->set_compiledate(rMessageData.m_SourceFile.m_CompileDate);
	pMessagePB->set_compiletime(rMessageData.m_SourceFile.m_CompileTime);
	pMessagePB->set_filename(rMessageData.m_SourceFile.m_FileName);
	pMessagePB->set_fileline(rMessageData.m_SourceFile.m_Line);
	pMessagePB->set_filedatetime(rMessageData.m_SourceFile.m_FileDateTime);
	pMessagePB->set_objectid(rMessage.getObjectId());
	pMessagePB->set_severity(static_cast<SvPb::MessageSeverity> (rMessage.getSeverity()));
	pMessagePB->set_messagetext(rMessage.what());
}

SvStl::MessageContainer convertProtobufToMessage(const MessageContainer& messagePB)
{
	SvStl::SourceFileParams fileParam(messagePB.compiledate().c_str(), messagePB.compiletime().c_str(), messagePB.filename().c_str(), messagePB.fileline(), messagePB.filedatetime().c_str());
	SvDef::StringVector AdditionalTextList(messagePB.additionaltextlist().size());
	std::ranges::copy(messagePB.additionaltextlist(), AdditionalTextList.begin());
	return {messagePB.messagecode(), static_cast<SvStl::MessageTextEnum>(messagePB.additionaltextid()), AdditionalTextList, fileParam, messagePB.objectid()};
}

SvStl::MessageContainerVector convertProtobufToMessageVector(const MessageContainerVector& messagesPB)
{
	SvStl::MessageContainerVector messageContainerVector(messagesPB.messages().size());
	std::ranges::transform(messagesPB.messages(), messageContainerVector.begin(), [](const auto& rEntry) { return convertProtobufToMessage(rEntry); });
	return messageContainerVector;
}


template<typename TreeItem>
void convertVectorToTree(const std::vector<typename TreeItem>& rItemVector, typename TreeItem* pTree)
{
	std::map<std::string, TreeItem*> treeNodes;

	for(const auto& rItem : rItemVector)
	{
		std::string location = rItem.location();
		TreeItem* pTreeItem = pTree;
		
		std::vector<std::string> newBranchNames;
		while(false == location.empty())
		{
			auto iter = std::as_const(treeNodes).find(location);
			if(treeNodes.end() == iter)
			{
				size_t startPos = location.rfind('.');
				if(std::string::npos != startPos)
				{
					newBranchNames.emplace_back(location.substr(startPos + 1, std::string::npos));
					location = location.substr(0, startPos);
				}
				else
				{
					newBranchNames.emplace_back(location);
					location.clear();
				}
			}
			else
			{
				pTreeItem = iter->second;
				break;
			}
		}
		std::reverse(newBranchNames.begin(), newBranchNames.end());
		
		for(const auto& rName : newBranchNames)
		{
			location = pTreeItem->location();
			if(false == location.empty())
			{
				location += '.';
			}
			location +=  rName;
			pTreeItem = pTreeItem->add_children();
			pTreeItem->set_name(rName);
			pTreeItem->set_location(location);
			treeNodes[location] = pTreeItem;
		}
		
		*pTreeItem = rItem;
	}
}

///Static library instantiations
template void convertVectorToTree<SvPb::TreeItem>(const std::vector<SvPb::TreeItem>& rItemVector, SvPb::TreeItem* pTree);
template void convertVectorToTree<SvPb::ConfigTreeItem>(const std::vector<SvPb::ConfigTreeItem>& rItemVector, SvPb::ConfigTreeItem* pTree);

variant_t getDefaultString(SvPb::LinkedValueTypeEnum type)
{
	switch (type)
	{
	case SvPb::LinkedValueTypeEnum::TypeDecimal: //decimal
		return 0.0;
	case SvPb::LinkedValueTypeEnum::TypeText:
		return "";
	default: //do nothing
		return {};
	}
}
} //namespace SvPB