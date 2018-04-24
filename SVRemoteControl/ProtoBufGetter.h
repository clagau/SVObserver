//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ProtoBufGetter.h
/// All Rights Reserved 
//*****************************************************************************
/// Converts ProtoBuf values to the corresponding Com interfaces
//******************************************************************************
#pragma once
#include "SVRemoteControl.h"
#include "SVProtobuf/SVRC.h"
#include "WebsocketLibrary/clientservice.h"

typedef CComPtr<ISVErrorObject> ErrorPtr;
typedef CComPtr<ISVErrorObjectList> ErrorListPtr;
typedef CComPtr<ISVImageObject> ImagePtr;
typedef CComPtr<ISVProductItems> ProductPtr;
typedef CComPtr<ISVValueObject> ValuePtr;
typedef CComPtr<ISVValueObjectList> ValueListPtr;
typedef CComSafeArray<VARIANT> FailList;

//typedef CComPtr<ISVImageObjectList> ImageListPtr;
typedef CComPtr<ISVDataDefObject> DataDefPtr;
typedef CComPtr<ISVDataDefObjectList> DataDefListPtr;


CComVariant  GetComVariant(const SvPb::Variant& rValue, ULONG Elements=0L);
ProductPtr GetProductPtr(SvWsl::ClientServicePointer& rClientServicePointer, const SvPb::Product& resp);
ValuePtr GetValueObjectPtr(int Count, const std::string& rName, const SvPb::Variant& rValue);
ErrorPtr GetErrorObjectPtr(LONG Status, const std::string& rName, const SvPb::Variant& rValue);
DataDefPtr GetDataDefPtr(const SvPb::DataDefinition& rDataDef);
/// Function return an ImageObject with a bitmap
ImagePtr GetImageObjectPtr(int Trigger, const std::string& rName, const std::string& rData);
/// Function return an ImageObject with a Bitmap ID
ImagePtr GetImageObjectPtr(int trigger, const std::string& name, const SvPb::CurImageId &imId, SvWsl::ClientServicePointer& rClientServicePointer);

FailList GetFailList(SvWsl::ClientServicePointer& rClientServicePointer, const SvPb::GetFailStatusResponse& resp);
ValueListPtr GetValueObjectListPtr(const SvPb::Product &productResp);
ProductPtr GetItemsPtr(const SvPb::GetItemsResponse& rResponse);
ProductPtr GetItemsPtr(const ::google::protobuf::RepeatedPtrField<::SvPb::Value>* pErrorList);
ErrorListPtr GetErrorListPtr(const ::google::protobuf::RepeatedPtrField<::SvPb::Value>* pErrorList);
DataDefListPtr GetDataDefList(const SvPb::GetDataDefinitionListResponse& rResponse);
