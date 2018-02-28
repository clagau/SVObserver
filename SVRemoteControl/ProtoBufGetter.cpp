//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ProtoBufGetter.cpp
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#include "stdafx.h"
#include "ProtoBufGetter.h"
#include "SVProductItems.h"
#include "SVRemoteControl.h"
#include "SVValueObject.h"
#include "SVImageObject.h"
#include "WebsocketLibrary/clientservice.h"
#include "WebsocketLibrary/RunRequest.inl"
#include "SVValueObjectList.h"

CComVariant  GetComVariant(const SVRPC::Variant& var)
{
	CComVariant comvar;
	switch (var.value_case())
	{
		case SVRPC::Variant::kBoolValue:
		{
			comvar.boolVal = static_cast<BOOL>(var.bool_value());
			comvar.vt = VT_BOOL;
			return comvar;
		}
		case SVRPC::Variant::kInt32Value:
		{
			comvar.intVal = var.int32_value();
			comvar.vt = VT_I4;
			return comvar;
		}
		case SVRPC::Variant::kUint32Value:
		{
			comvar.uintVal = var.uint32_value();
			comvar.vt = VT_UI4;
			return comvar;
		}
		case SVRPC::Variant::kInt64Value:
		{
			comvar.llVal = var.int64_value();
			comvar.vt = VT_I8;
			return comvar;
		}
		case SVRPC::Variant::kUint64Value:
		{
			comvar.ullVal = var.uint64_value();
			comvar.vt = VT_UI8;
			return comvar;
		}
		case SVRPC::Variant::kStringValue:
		{
			_bstr_t bstr(var.string_value().c_str());
			comvar.bstrVal = bstr.Detach();
			comvar.vt = VT_BSTR;
			return comvar;
		}
		case SVRPC::Variant::kDoubleValue:
		{
			comvar.dblVal = var.double_value();
			comvar.vt = VT_R8;
			return comvar;
		}
		case SVRPC::Variant::kFloatValue:
		{
			comvar.fltVal = var.float_value();
			comvar.vt = VT_R4;
			return comvar;
		}
	}
	return comvar;
}

CComPtr<ISVImageObject> GetImageObjectPtr(int trigger, const std::string& name, const RRWS::CurImageId &imId, RRWS::ClientService* pClientService)
{
	CComObject<SVImageObject> *pImageObject(0);
	CComObject<SVImageObject>::CreateInstance(&pImageObject);
	CComPtr<ISVImageObject> pio(pImageObject);
	pImageObject->SetImageSok(nullptr);
	_bstr_t bname(name.c_str());
	pImageObject->put_Name(bname);
	pImageObject->put_TriggerCount(trigger);
	pImageObject->SetClientService(pClientService);
	pImageObject->SetImageId(imId);
	return pio;
}

CComPtr<ISVImageObject> GetImageObjectPtr(int trigger, const std::string& name, const RRWS::GetImageFromCurIdResponse& resp)
{
	CComObject<SVImageObject> *pImageObject(0);
	CComObject<SVImageObject>::CreateInstance(&pImageObject);
	CComPtr<ISVImageObject> pio(pImageObject);
	pImageObject->SetImageSok(nullptr);
	_bstr_t bname(name.c_str());
	pImageObject->put_Name(bname);
	pImageObject->put_TriggerCount(trigger);
	//@Todo[MEC][8.00] [10.11.2017] avoid copying ??
	if (resp.imagedata().rgb().length() > 0)
	{
		BYTE *buff = new BYTE[resp.imagedata().rgb().length()];
		memcpy(buff, resp.imagedata().rgb().c_str(), resp.imagedata().rgb().length());
		boost::shared_array<BYTE> b(buff);
		pImageObject->SetLen((ULONG)resp.imagedata().rgb().length());
		pImageObject->SetDIB(b);
	}

	else
	{
		pImageObject->SetLen(0);
	}


	return pio;

}
CComPtr<ISVValueObject> GetValueObjectPtr(int trigger, const std::string& name, const SVRPC::Variant& var)
{
	CComObject<SVValueObject>* pValueObject(0);
	CComObject<SVValueObject>::CreateInstance(&pValueObject);
	CComPtr<ISVValueObject> pvo(pValueObject);
	_bstr_t bname(name.c_str());
	pValueObject->put_Name(bname);
	pValueObject->put_Status(0);
	pValueObject->put_TriggerCount(trigger);
	pValueObject->Add(GetComVariant(var));
	return pvo;
}


CComPtr<ISVProductItems> GetProductPtr(RRWS::ClientService& rClientService, const RRWS::Product &rResp)
{
	CComObject<SVProductItems> *pProd = 0;
	CComObject<SVProductItems>::CreateInstance(&pProd);
	CComPtr<ISVProductItems> ppi(pProd);
	bool bpImageName = rResp.images_size() == rResp.imagenames_size();
	bool bpValueName = rResp.values_size() == rResp.valuenames_size();

	for (int v = 0; v < rResp.values_size(); v++)
	{
		pProd->AddValue(GetValueObjectPtr(rResp.trigger(), rResp.valuenames(v), rResp.values(v)));
	}
	for (int i = 0; i < rResp.images_size(); i++)
	{
		bool InsertAllBitmapNow(false);
		if (InsertAllBitmapNow)
		{
			RRWS::GetImageFromCurIdRequest request;
			request.mutable_id()->set_imagestore(rResp.images(i).imagestore());
			request.mutable_id()->set_imageindex(rResp.images(i).imageindex());
			request.mutable_id()->set_slotindex(rResp.images(i).slotindex());
			RRWS::GetImageFromCurIdResponse Imageresp  = 
				RRWS::runRequest(rClientService, &RRWS::ClientService::getImageFromCurId, std::move(request)).get();

			pProd->AddImage(GetImageObjectPtr(rResp.trigger(), rResp.imagenames(i), Imageresp));
		}
		else
		{
			pProd->AddImage(
				GetImageObjectPtr(rResp.trigger(), rResp.imagenames(i), rResp.images(i), &rClientService)
			);
		}

	}
	return ppi;


}

FailList GetFailList(RRWS::ClientService &rClientService , const RRWS::GetFailStatusResponse& resp)
{
	FailList list;
	int TriggerCount = resp.products_size();
	list.Create(TriggerCount, 0);
	for (int i = 0; i < TriggerCount; i++)
	{
		list[i] = GetValueObjectListPtr(resp.products(i));
		//PrintProductResponse(resp.productresponse(i));
	}
	return list;
}

CComPtr<ISVValueObjectList> GetValueObjectListPtr(const RRWS::Product& rProduct)
{
	CComObject<SVValueObjectList> *vl = 0;
	CComObject<SVValueObjectList>::CreateInstance(&vl);
	CComPtr<ISVValueObjectList> vlp(vl);
	if (rProduct.status() != RRWS::IsValid)
	{
			return vlp;
		
	}
	for (int v = 0; v < rProduct.values_size() && v < rProduct.valuenames_size(); v++)
	{
		vl->Add(GetValueObjectPtr(rProduct.trigger(), rProduct.valuenames(v), rProduct.values(v)));
	}
	return vlp;
}




