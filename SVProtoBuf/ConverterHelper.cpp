//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file ConverterHelper.cpp
/// All Rights Reserved
//*****************************************************************************
/// Converter helper functions between protobuf and other data types
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
#include "ConverterHelper.h"
#include "Definitions/ObjectDefines.h"
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
}

GUID GetGuidFromProtoBytes(const std::string& strguid)
{
	GUID guid = GUID_NULL;
	GetGuidFromProtoBytes(strguid, guid);
	return guid;
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
					pPbVariant->set_type(VT_I4);
					pPbVariant->set_lval(static_cast<long> (rVariant));
					break;

				case VT_UI1:
				case VT_UI2:
				case VT_UI4:
				case VT_UINT:
					pPbVariant->set_type(VT_UI4);
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

HRESULT ConvertProtobufToVariant(const SvPb::Variant& rPbVariant, _variant_t& rVariant)
{
	HRESULT Result {S_OK};

	rVariant.vt = static_cast<VARTYPE> (rPbVariant.type());
	switch (rPbVariant.type())
	{
		case VT_BOOL:
			rVariant.boolVal = rPbVariant.bval();
			break;

		case VT_I4:
			rVariant.lVal = rPbVariant.lval();
			break;

		case VT_UI4:
			rVariant.ulVal = rPbVariant.ulval();
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
				const std::string& rStringArray = rPbVariant.strval();

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
			pVariant->set_strval(Text);
			pVariant->set_count(static_cast<int> (rList.size()));
		}
	}
}

UINT PbObjectAttributes2Attributes(const SvPb::ObjectAttributes& rAttributes)
{
	UINT Result{SvDef::SV_NO_ATTRIBUTES};

	switch(rAttributes)
	{
		case SvPb::ObjectAttributes::noAttributes:
			Result = SvDef::SV_NO_ATTRIBUTES;
			break;
		case SvPb::ObjectAttributes::viewable:
			Result = SvDef::SV_VIEWABLE;
			break;
		case SvPb::ObjectAttributes::publishable:
			Result = SvDef::SV_PUBLISHABLE;
			break;
		case SvPb::ObjectAttributes::archivable:
			Result = SvDef::SV_ARCHIVABLE;
			break;
		case SvPb::ObjectAttributes::selectableForEquation:
			Result = SvDef::SV_SELECTABLE_FOR_EQUATION;
			break;
		case SvPb::ObjectAttributes::embedable:
			Result = SvDef::SV_EMBEDABLE;
			break;
		case SvPb::ObjectAttributes::selectableForStatistics:
			Result = SvDef::SV_SELECTABLE_FOR_STATISTICS;
			break;
		case SvPb::ObjectAttributes::archivableImage:
			Result = SvDef::SV_ARCHIVABLE_IMAGE;
			break;
		case SvPb::ObjectAttributes::printable:
			Result = SvDef::SV_PRINTABLE;
			break;
		case SvPb::ObjectAttributes::remotelySetable:
			Result = SvDef::SV_REMOTELY_SETABLE;
			break;
		case SvPb::ObjectAttributes::setableOnline:
			Result = SvDef::SV_SETABLE_ONLINE;
			break;
		case SvPb::ObjectAttributes::extentObject:
			Result = SvDef::SV_EXTENT_OBJECT;
			break;
		case SvPb::ObjectAttributes::publishResultImage:
			Result = SvDef::SV_PUBLISH_RESULT_IMAGE;
			break;
		case SvPb::ObjectAttributes::dataDefinitionValue:
			Result = SvDef::SV_DD_VALUE;
			break;
		case SvPb::ObjectAttributes::dataDefinitionImage:
			Result = SvDef::SV_DD_IMAGE;
			break;
		default:
			break;
	}
	return Result;
}

} //namespace SvPB