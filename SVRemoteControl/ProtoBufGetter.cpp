//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ProtoBufGetter.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Converts ProtoBuf values to the corresponding Com interfaces
//******************************************************************************
#include "stdafx.h"
#include "ProtoBufGetter.h"
#include "SVProductItems.h"
#include "SVRemoteControl.h"
#include "SVValueObject.h"
#include "SVImageObject.h"
#include "SVErrorObject.h"
#include "SVErrorObjectList.h"
#include "SVDataDefObject.h"
#include "SVDataDefObjectList.h"
#include "WebsocketLibrary/RunRequest.inl"
#include "SVValueObjectList.h"
#include "SVProtoBuf/ConverterHelper.h"

CComVariant  GetComVariant(const SvPb::Variant& rPbVariant, ULONG Elements /*= 0L*/)
{
	CComVariant Result;

	_variant_t Variant;
	SvPb::ConvertProtobufToVariant(rPbVariant, Elements, Variant);
	Result = Variant;

	return Result;
}

ValuePtr GetValueObjectPtr(int Count, const std::string& rName, const SvPb::Variant& rValue)
{
	CComObject<SVValueObject>* pValueObject{nullptr};
	CComObject<SVValueObject>::CreateInstance(&pValueObject);
	ValuePtr pvo(pValueObject);

	_bstr_t bname(rName.c_str());
	bool isArray = VT_ARRAY == (rValue.type() & VT_ARRAY);
	pValueObject->put_Name(bname);
	pValueObject->put_Status(0);
	//If not array then Count is trigger count
	pValueObject->put_TriggerCount(isArray ? 0 : Count);
	//If array then Count is array size count
	pValueObject->Add(GetComVariant(rValue, isArray ? Count : 0));
	return pvo;
}

ErrorPtr GetErrorObjectPtr(LONG Status, const std::string& rName, const SvPb::Variant& rValue)
{
	CComObject<SVErrorObject>* pErrorObject{nullptr};
	CComObject<SVErrorObject>::CreateInstance(&pErrorObject);
	ErrorPtr pErrorObjectPtr(pErrorObject);

	_bstr_t bName(rName.c_str());
	pErrorObject->put_Name(bName);
	pErrorObject->put_Status(Status);
	return pErrorObjectPtr;
}

DataDefPtr GetDataDefPtr(const SvPb::DataDefinition& rDataDef)
{
	CComObject<SVDataDefObject>* pDataDef{nullptr};
	CComObject<SVDataDefObject>::CreateInstance(&pDataDef);
	DataDefPtr pDataDefPtr(pDataDef);

	_bstr_t bName(rDataDef.name().c_str());
	pDataDefPtr->put_Name(bName);
	pDataDefPtr->put_Writable(rDataDef.writable());
	pDataDefPtr->put_Published(rDataDef.published());
	_bstr_t bType(rDataDef.type().c_str());
	pDataDefPtr->put_DataType(bType);
	for(int i=0; i < rDataDef.additionalinfo_size(); i++)
	{
		CComVariant Value;
		Value.vt = VT_BSTR;
		_bstr_t bTemp(rDataDef.additionalinfo(i).c_str());
		Value.bstrVal = bTemp;
		pDataDefPtr->Add(Value);
	}
	
	return pDataDefPtr;
}

ImagePtr GetImageObjectPtr(int trigger, const std::string& name, const SvPb::CurImageId &imId, SvWsl::ClientServicePointer& rClientServicePointer)
{
	CComObject<SVImageObject> *pImageObject{nullptr};
	CComObject<SVImageObject>::CreateInstance(&pImageObject);
	ImagePtr pio(pImageObject);
	_bstr_t bname(name.c_str());
	pImageObject->put_Name(bname);
	pImageObject->put_TriggerCount(trigger);
	pImageObject->SetClientService(rClientServicePointer);
	pImageObject->SetImageId(imId);
	return pio;
}

ImagePtr GetImageObjectPtr(int Trigger, const std::string& rName, const std::string& rData)
{
	CComObject<SVImageObject> *pImageObject{nullptr};
	CComObject<SVImageObject>::CreateInstance(&pImageObject);
	ImagePtr pImageObjectPtr(pImageObject);

	_bstr_t bName(rName.c_str());
	pImageObject->put_Name(bName);
	pImageObject->put_TriggerCount(Trigger);
	//@Todo[MEC][8.00] [10.11.2017] avoid copying ??
	if (rData.length() > 0)
	{
		BYTE* pBuffer = new BYTE[rData.length()];
		memcpy(pBuffer, rData.c_str(), rData.length());
		boost::shared_array<BYTE> b(pBuffer);
		pImageObject->SetLen(static_cast<ULONG> (rData.length()));
		pImageObject->SetDIB(b);
	}

	else
	{
		pImageObject->SetLen(0);
	}

	return pImageObjectPtr;
}

ProductPtr GetProductPtr(SvWsl::ClientServicePointer& rClientServicePointer, const SvPb::Product &rResp)
{
	CComObject<SVProductItems> *pProdItems = 0;
	CComObject<SVProductItems>::CreateInstance(&pProdItems);
	ProductPtr pProdItemsPtr(pProdItems);

	bool bpImageName = rResp.images_size() == rResp.imagenames_size();
	bool bpValueName = rResp.values_size() == rResp.valuenames_size();

	for (int v = 0; v < rResp.values_size(); v++)
	{
		pProdItems->AddValue(GetValueObjectPtr(rResp.trigger(), rResp.valuenames(v), rResp.values(v)));
	}
	for (int i = 0; i < rResp.images_size(); i++)
	{
		bool InsertAllBitmapNow(false);
		if (InsertAllBitmapNow)
		{
			SvPb::GetImageFromCurIdRequest request;
			request.mutable_id()->set_imagestore(rResp.images(i).imagestore());
			request.mutable_id()->set_imageindex(rResp.images(i).imageindex());
			request.mutable_id()->set_slotindex(rResp.images(i).slotindex());
			SvPb::GetImageFromCurIdResponse Imageresp = SvWsl::runRequest(*rClientServicePointer, &SvWsl::ClientService::getImageFromCurId, std::move(request)).get();

			pProdItems->AddImage(GetImageObjectPtr(rResp.trigger(), rResp.imagenames(i), Imageresp.imagedata().rgb()));
		}
		else
		{
			pProdItems->AddImage(GetImageObjectPtr(rResp.trigger(), rResp.imagenames(i), rResp.images(i), rClientServicePointer));
		}

	}
	return pProdItemsPtr;


}

FailList GetFailList(SvWsl::ClientServicePointer& rClientServicePointer, const SvPb::GetFailStatusResponse& resp)
{
	FailList list;
	int TriggerCount = resp.products_size();
	list.Create(TriggerCount, 0);
	for (int i = 0; i < TriggerCount; i++)
	{
		list[i] = GetValueObjectListPtr(resp.products(i));
	}

	return list;
}

ValueListPtr GetValueObjectListPtr(const SvPb::Product& rProduct)
{
	CComObject<SVValueObjectList>* pValueList{nullptr};
	CComObject<SVValueObjectList>::CreateInstance(&pValueList);
	ValueListPtr pValueListPtr(pValueList);

	if (rProduct.status() != SvPb::IsValid)
	{
			return pValueListPtr;
		
	}
	for (int v = 0; v < rProduct.values_size() && v < rProduct.valuenames_size(); v++)
	{
		pValueList->Add(GetValueObjectPtr(rProduct.trigger(), rProduct.valuenames(v), rProduct.values(v)));
	}
	return pValueListPtr;
}


ProductPtr GetItemsPtr(const SvPb::GetItemsResponse& rResponse)
{
	CComObject<SVProductItems>* pProdItems{nullptr};
	CComObject<SVProductItems>::CreateInstance(&pProdItems);
	ProductPtr pProdItemsPtr(pProdItems);

	for (int i=0; i < rResponse.valuelist_size(); i++)
	{
		const SvPb::Value& rValue = rResponse.valuelist(i);
		pProdItems->AddValue(GetValueObjectPtr(rValue.count(),  rValue.name(), rValue.item()));
	}

	for (int i = 0; i < rResponse.imagelist_size(); i++)
	{
		const SvPb::Value& rValue = rResponse.imagelist(i);
		if((VT_UI1 | VT_ARRAY) == rValue.item().type())
		{
			pProdItems->AddImage( GetImageObjectPtr(rValue.count(), rValue.name(), rValue.item().bytesval()));
		}
	}

	for (int i = 0; i < rResponse.errorlist_size(); i++)
	{
		const SvPb::Value& rValue = rResponse.errorlist(i);
		pProdItems->AddError(GetErrorObjectPtr(rValue.status(), rValue.name(), rValue.item()));
	}
	return pProdItemsPtr;
}

ProductPtr GetItemsPtr(const ::google::protobuf::RepeatedPtrField<::SvPb::Value>* pErrorList)
{
	CComObject<SVProductItems>* pProdItems {nullptr};
	CComObject<SVProductItems>::CreateInstance(&pProdItems);
	ProductPtr pProdItemsPtr(pProdItems);

	if(nullptr != pErrorList)
	{
		for (int i = 0; i < pErrorList->size(); i++)
		{
			const SvPb::Value& rValue = pErrorList->Get(i);
			pProdItems->AddError(GetErrorObjectPtr(rValue.status(), rValue.name(), rValue.item()));
		}
	}
	return pProdItemsPtr;
}

ErrorListPtr GetErrorListPtr(const ::google::protobuf::RepeatedPtrField<::SvPb::Value>* pErrorProtoList)
{
	CComObject<SVErrorObjectList>* pErrorList{nullptr};
	CComObject<SVErrorObjectList>::CreateInstance(&pErrorList);
	ErrorListPtr pErrorListPtr(pErrorList);

	if (nullptr != pErrorProtoList)
	{
		for (int i = 0; i < pErrorProtoList->size(); i++)
		{
			const SvPb::Value& rValue = pErrorProtoList->Get(i);
			pErrorListPtr->Add(GetErrorObjectPtr(rValue.status(), rValue.name(), rValue.item()));
		}
	}
	return pErrorListPtr;
}

DataDefListPtr GetDataDefList(const SvPb::GetDataDefinitionListResponse& rResponse)
{
	CComObject<SVDataDefObjectList>* pDataDefList {nullptr};
	CComObject<SVDataDefObjectList>::CreateInstance(&pDataDefList);
	DataDefListPtr pDataDefListPtr(pDataDefList);

	for (int i = 0; i < rResponse.list_size(); i++)
	{
		pDataDefListPtr->Add(GetDataDefPtr(rResponse.list(i)));
	}

	return pDataDefListPtr;
}