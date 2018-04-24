//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ProtoBufSetter.h
/// All Rights Reserved 
//*****************************************************************************
/// Converts Com interfaces to the corresponding ProtoBuf values
//******************************************************************************
#pragma once
#include "SVRemoteControl.h"
#include "SVProtobuf/SVRC.h"

typedef CComPtr<ISVProductItems> ProductPtr;
typedef CComPtr<ISVValueObjectList> ValueListPtr;
typedef CComPtr<ISVImageObjectList> ImageListPtr;
typedef CComPtr<ISVValueObject> ValuePtr;
typedef CComPtr<ISVImageObject> ImagePtr;

void SetValue(const ValuePtr& pValuePtr, SvPb::Value* pItemValue);
void SetStringList(const CComVariant& rVariant, ::google::protobuf::RepeatedPtrField< ::std::string>* pStringList);
void SetImage(const ImagePtr& pImagePtr, SvPb::Value* pItemValue);
void SetValueList(const ValueListPtr& pValueListPtr, SvPb::SetItemsRequest* pRequest);
void SetImageList(const ImageListPtr& pImageListPtr, SvPb::SetItemsRequest* pRequest);
void SetItemsRequest(const ProductPtr& pProductPtr, SvPb::SetItemsRequest* pRequest);

