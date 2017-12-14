//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ProtoBufGetter.h
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#pragma once
#include "SVRemoteControl.h"
#include <atlcomcli.h>
#include "RunReApi\format.pb.h"
#include "RunReApi\ClientFrontEndApi.h"

CComVariant  GetComVariant(const ::RRApi::Variant& var);
CComPtr<ISVProductItems> GetProductPtr(RRApi::ClientFrontEndApi &FrontEndApi, const RRApi::GetProductResponse& resp);
CComPtr<ISVValueObject> GetValueObjectPtr(int trigger, const std::string& name, const ::RRApi::Variant& var);
/// Function return an ImageObject with a bitmap
CComPtr<ISVImageObject> GetImageObjectPtr(int trigger, const std::string& name, const RRApi::GetImageFromCurIdResponse& resp);
/// Function return an ImageObject with a Bitmap ID
CComPtr<ISVImageObject> GetImageObjectPtr(int trigger, const std::string& name, const RRApi::CurImageId &imId, RRApi::ClientFrontEndApi* pFrontEndApi);

typedef CComSafeArray<VARIANT> FailList;
FailList GetFailList(RRApi::ClientFrontEndApi &FrontEndApi, const RRApi::GetFailStatusResponse& resp);
CComPtr<ISVValueObjectList> GetValueObjectListPtr(const RRApi::GetProductResponse& productResp);

//FailList GetFailList(RRApi::ClientFrontEndApi &FrontEndApi, const RRApi::GetFailStatusResponse& resp);