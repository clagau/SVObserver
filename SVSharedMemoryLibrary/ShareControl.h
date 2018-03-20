//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file ShareControl.h
/// All Rights Reserved 
//*****************************************************************************
/// ShareControl implements the connection to the shared memory 
//******************************************************************************
#pragma once

#include "SharedMemReader.h"
#include "LastResponseData.h"
#include "SVProtoBuf/RunRe.h"

namespace SvSml
{
//! ShareControl implements the connection to the shared memory 

class ShareControl
{
public:
	ShareControl();
	~ShareControl();
	bool EventHandler(DWORD event);
	bool QueryListName(const SvPb::QueryListNameRequest& req, SvPb::QueryListNameResponse& resp, SvPenv::Error& err);
	bool  QueryListItem(const SvPb::QueryListItemRequest& req, SvPb::QueryListItemResponse& resp, SvPenv::Error& err);
	bool GetProductOrReject(const SvPb::GetProductRequest& request, SvPb::GetProductResponse& resp, SvPenv::Error& err);
	bool GetFailstatus(const SvPb::GetFailStatusRequest& request, SvPb::GetFailStatusResponse& resp, SvPenv::Error& err);
	bool  GetImageFromCurId(const  SvPb::GetImageFromCurIdRequest& req, SvPb::GetImageFromCurIdResponse& resp, SvPenv::Error& err);
private:
	bool GetProduct(SvSml::MLProduct* pProduct, bool nameInResponse, SvPb::Product* pProdMsg, SvPenv::Error& err);
	bool SetProductResponse(bool nameInResponse, const SvSml::MLProduct* pProduct, SvPb::Product& rProduct, SvPenv::Error& err);
	bool GetFailstatus(SvSml::vecpProd* pFailstatus, const SvPb::GetFailStatusRequest& request, SvPb::GetFailStatusResponse& resp, SvPenv::Error& err);

private:
	SvSml::SharedMemReader   m_MemReader;
	std::unique_ptr<LastResponseData> m_pLastResponseData;
};
}

