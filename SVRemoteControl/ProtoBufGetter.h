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
#pragma warning( push )
#pragma warning( disable : 4800 ) 
#include "SVProtobuf/RunRe.pb.h"
#pragma warning( pop )
#include "WebsocketLibrary/clientservice.h"

CComVariant  GetComVariant(const SVRPC::Variant& var);
CComPtr<ISVProductItems> GetProductPtr(RRWS::ClientService& rClientService, const RRWS::Product& resp);
CComPtr<ISVValueObject> GetValueObjectPtr(int trigger, const std::string& name, const SVRPC::Variant& var);
/// Function return an ImageObject with a bitmap
CComPtr<ISVImageObject> GetImageObjectPtr(int trigger, const std::string& name, const RRWS::GetImageFromCurIdResponse& resp);
/// Function return an ImageObject with a Bitmap ID
CComPtr<ISVImageObject> GetImageObjectPtr(int trigger, const std::string& name, const RRWS::CurImageId &imId, RRWS::ClientService* pClientService);

typedef CComSafeArray<VARIANT> FailList;
FailList GetFailList(RRWS::ClientService& clientService, const RRWS::GetFailStatusResponse& resp);
CComPtr<ISVValueObjectList> GetValueObjectListPtr(const RRWS::Product &productResp);

//FailList GetFailList(RRApi::ClientFrontEndApi &FrontEndApi, const RRApi::GetFailStatusResponse& resp);