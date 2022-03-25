//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ProtoBufSetter.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Converts Com interfaces to the corresponding ProtoBuf values
//******************************************************************************
#include "stdafx.h"
#include "ProtoBufSetter.h"
#include "SVProductItems.h"
#include "SVRemoteControl.h"
#include "SVValueObject.h"
#include "SVImageObject.h"
#include "SVErrorObject.h"
#include "SVValueObjectList.h"
#include "SVUtilityLibrary/StringHelper.h"

void SetValue(const ValuePtr& pValuePtr, SvPb::Value* pItemValue)
{
	if(nullptr == pValuePtr || nullptr == pItemValue)
	{
		return;
	}

	_bstr_t bName;
	pValuePtr->get_Name(bName.GetAddress());
	pItemValue->set_name(SvUl::to_utf8(bName));
	long Status{0L};
	pValuePtr->get_Status(&Status);
	pItemValue->set_status(Status);
	long Count {0L};
	pValuePtr->get_Count(&Count);
	std::string ValueData;
	//The value list can only contain variant of type VT_BSTR
	for (long i = 0; i < Count; ++i)
	{
		CComVariant Value;
		CComVariant Index = i;

		pValuePtr->get_Item(Index, &Value);
		if(VT_BSTR == Value.vt)
		{
			//Separate the list of strings with semicolon
			ValueData += (0 == i) ? _T("") :  _T(";");
			ValueData += SvUl::to_utf8(_bstr_t(Value.bstrVal));
		}
	}
	 
	VARTYPE Type = VT_BSTR;
	Type |= (Count > 1) ? VT_ARRAY : VT_EMPTY ;
#ifdef  SAFEARRAY_FORMAT_IN_SETITEM		
	VARTYPE Type = VT_BSTR | VT_ARRAY;
#endif	
	pItemValue->mutable_item()->set_type(Type);
	pItemValue->mutable_item()->set_strval(ValueData);
	pItemValue->mutable_item()->set_count(Count);
}

void SetStringList(const CComVariant& rVariant, ::google::protobuf::RepeatedPtrField< ::std::string>* pStringList)
{
	if (VT_BSTR == rVariant.vt)
	{
		if(nullptr != pStringList)
		{
			std::string* pName = pStringList->Add();
			*pName = SvUl::to_utf8(_bstr_t(rVariant.bstrVal, false));
		}
	}
	else if ((VT_ARRAY | VT_BSTR) == rVariant.vt)
	{
		SAFEARRAY * pSafeArray = rVariant.parray;
		LONG LowerBound {0};
		LONG UpperBound {0};
		::SafeArrayGetLBound(pSafeArray, 1, &LowerBound);
		::SafeArrayGetUBound(pSafeArray, 1, &UpperBound);
		BSTR* pData;
		::SafeArrayAccessData(pSafeArray, reinterpret_cast<void**> (&pData));
		for (LONG i = LowerBound; i <= UpperBound; ++i)
		{
			if (nullptr != pStringList)
			{
				_bstr_t bName(pData[i], false);
				std::string* pName = pStringList->Add();
				*pName = SvUl::to_utf8(bName);
				bName.Detach();
			}
		}
		::SafeArrayUnaccessData(pSafeArray);
	}
	else
	{
		throw CAtlException(OLE_E_CANTCONVERT);
	}
}

void SetImage(const ImagePtr& pImagePtr, SvPb::Value* pItemValue)
{
	if (nullptr == pImagePtr || nullptr == pItemValue)
	{
		return;
	}

	_bstr_t bName;
	pImagePtr->get_Name(bName.GetAddress());
	pItemValue->set_name(SvUl::to_utf8(bName));
	long Status {0L};
	pImagePtr->get_Status(&Status);
	pItemValue->set_status(Status);
	long TriggerCount {0L};
	pImagePtr->get_TriggerCount(&TriggerCount);
	pItemValue->set_count(TriggerCount);

	CComVariant TempImage;
	pImagePtr->GetImage(false, 1.0, BMP, &TempImage);
	LONG ImageSize = 0;
	::SafeArrayGetUBound(TempImage.parray, 1, &ImageSize);

	if (0 != ImageSize)
	{
		//Safearray size is Upper bound - lower bound + 1 so we need to increment
		ImageSize++;
		std::vector<char> ImageData;
		ImageData.resize(ImageSize);
		BYTE* pData{nullptr};
		::SafeArrayAccessData(TempImage.parray, reinterpret_cast<void**>(&pData));
		memcpy(&ImageData[0], pData, ImageSize);
		::SafeArrayUnaccessData(TempImage.parray);

		pItemValue->mutable_item()->set_type(VT_UI1 | VT_ARRAY);
		pItemValue->mutable_item()->set_bytesval(&ImageData[0], ImageSize);
	}
}

void SetValueList(const ValueListPtr& pValueListPtr, SvPb::SetItemsRequest* pRequest)
{
	if (nullptr == pValueListPtr ||  nullptr == pRequest)
	{
		return;
	}

	long Count;
	pValueListPtr->get_Count(&Count);
	for (long i=0; i < Count; ++i)
	{
		CComVariant Index = i;
		CComVariant Value;
		pValueListPtr->get_Item(Index, &Value);
		ValuePtr pValuePtr(static_cast<ISVValueObject*> (Value.pdispVal));
		if(nullptr != pValuePtr)
		{
			SvPb::Value* pItemValue = pRequest->add_valuelist();
			SetValue(pValuePtr, pItemValue);
		}
	}
}

void SetImageList(const ImageListPtr& pImageListPtr, SvPb::SetItemsRequest* pRequest)
{
	if (nullptr == pImageListPtr || nullptr == pRequest)
	{
		return;
	}

	long Count;
	pImageListPtr->get_Count(&Count);
	for (long i = 0; i < Count; ++i)
	{
		CComVariant Index = i;
		CComVariant Value;
		pImageListPtr->get_Item(Index, &Value);
		ImagePtr pImagePtr(static_cast<ISVImageObject*> (Value.pdispVal));
		if (nullptr != pImagePtr)
		{
			SvPb::Value* pItemValue = pRequest->add_imagelist();
			SetImage(pImagePtr, pItemValue);
		}
	}
}

void SetItemsRequest(const ProductPtr& pProductPtr, SvPb::SetItemsRequest* pRequest)
{
	if(nullptr == pProductPtr || nullptr == pRequest)
	{
		return;
	}

	ISVValueObjectList* pValueList(nullptr);
	pProductPtr->get_Values(&pValueList);
	ValueListPtr pValuePtr(pValueList);

	SetValueList(pValuePtr, pRequest);
	if (nullptr != pValueList)
	{
		pValueList->Release();
	}

	ISVImageObjectList* pImageList(nullptr);
	pProductPtr->get_Images(&pImageList);
	ImageListPtr pImagePtr(pImageList);

	SetImageList(pImagePtr, pRequest);
	if (nullptr != pImageList)
	{
		pImageList->Release();
	}
}
