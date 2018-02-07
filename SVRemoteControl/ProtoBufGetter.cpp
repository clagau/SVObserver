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
#include "RunReApi\ClientFrontEndApi.h"
#include "SVValueObjectList.h"

CComVariant  GetComVariant(const ::RRApi::Variant& var)
{
	CComVariant comvar;

	if (var.has_bool_value())
	{
		comvar.boolVal = static_cast<BOOL>(var.bool_value());
		comvar.vt = VT_BOOL;
		return comvar;
	}
	else if (var.has_int32_value())
	{
		comvar.intVal = var.int32_value();
		comvar.vt = VT_I4;
		return comvar;
	}
	else if (var.has_int64_value())
	{
		comvar.llVal = var.int64_value();
		comvar.vt = VT_I8;
		return comvar;

	}
	else if (var.has_uint32_value())
	{
		comvar.uintVal = var.uint32_value();
		comvar.vt = VT_UI4;
		return comvar;
	}
	else if (var.has_uint64_value())
	{
		comvar.ullVal = var.uint64_value();
		comvar.vt = VT_UI8;
		return comvar;
	}
	else if (var.has_string_value())
	{

		_bstr_t bstr(var.string_value().c_str());
		comvar.bstrVal = bstr.Detach();
		comvar.vt = VT_BSTR;
		return comvar;
	}
	else if (var.has_double_value())
	{
		comvar.dblVal = var.double_value();
		comvar.vt = VT_R8;
		return comvar;

	}
	else if (var.has_float_value())
	{
		comvar.fltVal = var.float_value();
		comvar.vt = VT_R4;
		return comvar;
	}
	return comvar;
}

CComPtr<ISVImageObject> GetImageObjectPtr(int trigger, const std::string& name, const RRApi::CurImageId &imId, RRApi::ClientFrontEndApi* pFrontEndApi)
{
	CComObject<SVImageObject> *pImageObject(0);
	CComObject<SVImageObject>::CreateInstance(&pImageObject);
	CComPtr<ISVImageObject> pio(pImageObject);
	pImageObject->SetImageSok(nullptr);
	_bstr_t bname(name.c_str());
	pImageObject->put_Name(bname);
	pImageObject->put_TriggerCount(trigger);
	pImageObject->SetClientFrontEndApi(pFrontEndApi);
	pImageObject->SetImageId(imId);
	return pio;
}
CComPtr<ISVImageObject> GetImageObjectPtr(int trigger, const std::string& name, const RRApi::GetImageFromCurIdResponse& resp)
{
	CComObject<SVImageObject> *pImageObject(0);
	CComObject<SVImageObject>::CreateInstance(&pImageObject);
	CComPtr<ISVImageObject> pio(pImageObject);
	pImageObject->SetImageSok(nullptr);
	_bstr_t bname(name.c_str());
	pImageObject->put_Name(bname);
	pImageObject->put_TriggerCount(trigger);
	//@Todo[MEC][8.00] [10.11.2017] avoid copying 
	if (resp.has_status() && resp.status() == RRApi::IsValid)
	{
		if (resp.imagedata().has_rgb())
		{
			BYTE *buff = new BYTE[resp.imagedata().rgb().length()];
			memcpy(buff, resp.imagedata().rgb().c_str(), resp.imagedata().rgb().length());
			boost::shared_array<BYTE> b(buff);
			pImageObject->SetLen((ULONG)resp.imagedata().rgb().length());
			pImageObject->SetDIB(b);
		}
	}
	else
	{
		pImageObject->SetLen(0);
	}


	return pio;

}
CComPtr<ISVValueObject> GetValueObjectPtr(int trigger, const std::string& name, const ::RRApi::Variant& var)
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


CComPtr<ISVProductItems> GetProductPtr(RRApi::ClientFrontEndApi &FrontEndApi, const RRApi::GetProductResponse& resp)
{
	CComObject<SVProductItems> *pProd = 0;
	CComObject<SVProductItems>::CreateInstance(&pProd);
	CComPtr<ISVProductItems> ppi(pProd);
	bool bpImageName = resp.images_size() == resp.imagenames_size();
	bool bpValueName = resp.values_size() == resp.valuenames_size();

	for (int v = 0; v < resp.values_size(); v++)
	{
		pProd->AddValue(GetValueObjectPtr(resp.trigger(), resp.valuenames(v), resp.values(v)));
	}
	for (int i = 0; i < resp.images_size(); i++)
	{
		bool InsertAllBitmapNow(false);
		if (InsertAllBitmapNow)
		{
			RRApi::GetImageFromCurIdRequest request;
			request.mutable_id()->set_imagestore(resp.images(i).imagestore());
			request.mutable_id()->set_imageindex(resp.images(i).imageindex());
			request.mutable_id()->set_slotindex(resp.images(i).slotindex());
			RRApi::GetImageFromCurIdResponse Imageresp = FrontEndApi.GetImageFromCurId(request).get();
			pProd->AddImage(GetImageObjectPtr(resp.trigger(), resp.imagenames(i), Imageresp));
		}
		else
		{
			pProd->AddImage(
				GetImageObjectPtr(resp.trigger(), resp.imagenames(i), resp.images(i), &FrontEndApi)
			);
		}

	}
	return ppi;


}

FailList GetFailList(RRApi::ClientFrontEndApi &FrontEndApi, const RRApi::GetFailStatusResponse& resp)
{
	FailList list;
	int TriggerCount = resp.productresponse_size();
	list.Create(TriggerCount, 0);
	for (int i = 0; i < TriggerCount; i++)
	{
		list[i] = GetValueObjectListPtr(resp.productresponse(i));
		//PrintProductResponse(resp.productresponse(i));
	}
	return list;
}

CComPtr<ISVValueObjectList> GetValueObjectListPtr(const RRApi::GetProductResponse& productResp)
{
	CComObject<SVValueObjectList> *vl = 0;
	CComObject<SVValueObjectList>::CreateInstance(&vl);
	CComPtr<ISVValueObjectList> vlp(vl);
	if (productResp.has_status())
	{
		if (productResp.status() != RRApi::IsValid)
		{
			return vlp;
		}
	}
	for (int v = 0; v < productResp.values_size() && v < productResp.valuenames_size(); v++)
	{
		vl->Add(GetValueObjectPtr(productResp.trigger(), productResp.valuenames(v), productResp.values(v)));
	}
	return vlp;
}




