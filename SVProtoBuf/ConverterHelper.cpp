//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
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
#include "SVUtilityLibrary/StringHelper.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#pragma endregion Includes

namespace SvPb
{

void SetGuidInProtoBytes(std::string  *pString, const GUID& guid)
{
	if (nullptr != pString)
	{
		pString->assign(reinterpret_cast<const char*>(&guid), sizeof(GUID));
	}
}

void GetGuidFromProtoBytes(const std::string& strguid, GUID& rGuid)
{
	if (sizeof(GUID) == strguid.size())
	{
		rGuid = *(reinterpret_cast<const GUID*>(strguid.data()));
	}
	else
	{
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
		Exception.setMessage(SVMSG_SVProtoBuf_GENERAL_ERROR, SvStl::Tid_ProtBuf_ConvertToGUID_WrongSize, SvStl::SourceFileParams(StdMessageParams));
	}
}

GUID GetGuidFromProtoBytes(const std::string& strguid)
{
	GUID guid = GUID_NULL;
	GetGuidFromProtoBytes(strguid, guid);
	return guid;
}

GUID GetGuidFromString(const std::string& buf)
{
	const auto len = buf.size();
	switch (len)
	{
		case 16:
		{
			char str[39];
			snprintf(str, sizeof(str),
				"{%hhx%hhx%hhx%hhx-%hhx%hhx-%hhx%hhx-%hhx%hhx-%hhx%hhx%hhx%hhx%hhx%hhx}",
				buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7],
				buf[8], buf[9], buf[10], buf[11], buf[12], buf[13], buf[14], buf[15]);
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wcu8;
			const auto wstr = wcu8.from_bytes(str);
			GUID guid;
			CLSIDFromString(wstr.c_str(), &guid);
			return guid;
		}
		case 36:
		{
			GUID guid;
			sscanf(buf.c_str(),
				"%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
				&guid.Data1, &guid.Data2, &guid.Data3,
				&guid.Data4[0], &guid.Data4[1], &guid.Data4[2], &guid.Data4[3],
				&guid.Data4[4], &guid.Data4[5], &guid.Data4[6], &guid.Data4[7]);
			return guid;
		}
		case 38:
		{
			GUID guid;
			sscanf(buf.c_str(),
				"{%8x-%4hx-%4hx-%2hhx%2hhx-%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx}",
				&guid.Data1, &guid.Data2, &guid.Data3,
				&guid.Data4[0], &guid.Data4[1], &guid.Data4[2], &guid.Data4[3],
				&guid.Data4[4], &guid.Data4[5], &guid.Data4[6], &guid.Data4[7]);
			return guid;
		}
		default:
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_SVProtoBuf_GENERAL_ERROR, SvStl::Tid_ProtBuf_ConvertToGUID_WrongSize, SvStl::SourceFileParams(StdMessageParams));
			return GUID_NULL;
	}
}

std::string PrettyPrintGuid(const GUID& guid)
{
	wchar_t szGuidW[40] = {0};
	char szGuidA[40] = {0};
	StringFromGUID2(guid, szGuidW, 40);
	WideCharToMultiByte(CP_ACP, 0, szGuidW, -1, szGuidA, 40, NULL, NULL);
	// the indexes make sure we are not copying the curly braces, as
	// StringFromGUID2 generates string with the following format:
	// {7096126C-A089-45E2-9EA9-94B3A23CA236}
	return std::string(&szGuidA[1], &szGuidA[37]);
}

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
					pPbVariant->set_strval(Text);
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
					pPbVariant->set_bytesval(StringArray);
				}
				else
				{
					std::vector<char> DataBlock;
					int BlockSize = count * rVariant.parray->cbElements;
					DataBlock.resize(BlockSize);
					memcpy(&DataBlock[0], rVariant.parray->pvData, BlockSize);

					pPbVariant->set_count(count);
					pPbVariant->set_type(rVariant.vt);
					pPbVariant->set_bytesval(&DataBlock[0], DataBlock.size());
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
	switch (rPbVariant.type())
	{
		case VT_BOOL:
			rVariant.boolVal = rPbVariant.bval();
			break;

		case VT_I1:
			rVariant.bVal = static_cast<byte> (rPbVariant.lval());
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
			rVariant.cVal = static_cast<char> (rPbVariant.ulval());
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
			_bstr_t bstr(rPbVariant.strval().c_str());
			rVariant.bstrVal = bstr.Detach();
			break;
		}

		default:
			break;
	}


	if (VT_ARRAY == (rPbVariant.type() & VT_ARRAY))
	{
		VARTYPE Type = rPbVariant.type() & ~VT_ARRAY;
		SAFEARRAY* pSafeArray = ::SafeArrayCreateVector(Type, 0, rPbVariant.count());
		if (nullptr != pSafeArray)
		{
			if (VT_BSTR == Type)
			{
				const std::string& rStringArray = SvUl::to_ansi(rPbVariant.strval());

				std::vector<std::string> Items;
				Items.reserve(rPbVariant.count());
				std::string Item;
				std::istringstream tokenStream(rStringArray);
				while (std::getline(tokenStream, Item, ';'))
				{
					Items.push_back(Item);
				}
				LONG Index {0L};
				for (const auto& rEntry : Items)
				{
					_bstr_t Text {rEntry.c_str()};
					Result = ::SafeArrayPutElement(pSafeArray, &Index, static_cast<void*> (Text.Detach()));
					if (S_OK != Result)
					{
						break;
					}
					Index++;
				}
			}
			else
			{
				void* pData {nullptr};
				Result = ::SafeArrayAccessData(pSafeArray, &pData);
				if (S_OK == Result)
				{
					memcpy(pData, rPbVariant.bytesval().c_str(), rPbVariant.bytesval().length());
					::SafeArrayUnaccessData(pSafeArray);
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


void ConvertStringListToProtobuf(const SvDef::StringSet& rList, SvPb::Variant* pVariant)
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

void convertVectorToTree(const std::vector<SvPb::TreeItem>& rItemVector, SvPb::TreeItem* pTree)
{
	for(const auto& rItem : rItemVector)
	{
		std::string location = rItem.location();
		SvPb::TreeItem* pTreeItem = pTree;

		size_t startPos{0LL};
		size_t endPos = location.find(_T('.'), startPos);
		std::string branchName = location.substr(startPos, endPos-startPos);
		while(!branchName.empty())
		{
			bool bFound{false};
			for(int i=0; i < pTreeItem->children_size(); i++)
			{
				if(pTreeItem->children(i).name() == branchName)
				{
					bFound = true;
					pTreeItem = pTreeItem->mutable_children(i);
					break;
				}
			}
			if(!bFound)
			{
				pTreeItem = pTreeItem->add_children();
				pTreeItem->set_name(branchName);
				//If it is the leaf then set th item
				if(endPos == std::string::npos)
				{
					*pTreeItem = rItem;
				}
				else
				{
					pTreeItem->set_location(location.substr(0, endPos));
				}
			}
			if(std::string::npos != endPos)
			{
				startPos = endPos + 1;
				endPos = location.find(_T('.'), startPos);
				branchName = location.substr(startPos, (std::string::npos == endPos) ? endPos : endPos - startPos);
			}
			else
			{
				branchName.clear();
			}
		}
	}
}
} //namespace SvPB