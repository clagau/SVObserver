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
#include "SVProtobuf/RunRe.h"
#include "WebsocketLibrary/clientservice.h"


CComVariant  GetComVariant(const SvPb::Variant& var);
CComPtr<ISVProductItems> GetProductPtr(SvWsl::ClientServicePointer& rClientServicePointer, const SvPb::Product& resp);
CComPtr<ISVValueObject> GetValueObjectPtr(int trigger, const std::string& name, const SvPb::Variant& var);
/// Function return an ImageObject with a bitmap
CComPtr<ISVImageObject> GetImageObjectPtr(int trigger, const std::string& name, const SvPb::GetImageFromCurIdResponse& resp);
/// Function return an ImageObject with a Bitmap ID
CComPtr<ISVImageObject> GetImageObjectPtr(int trigger, const std::string& name, const SvPb::CurImageId &imId, SvWsl::ClientServicePointer& rClientServicePointer);

typedef CComSafeArray<VARIANT> FailList;
FailList GetFailList(SvWsl::ClientServicePointer& rClientServicePointer, const SvPb::GetFailStatusResponse& resp);
CComPtr<ISVValueObjectList> GetValueObjectListPtr(const SvPb::Product &productResp);

