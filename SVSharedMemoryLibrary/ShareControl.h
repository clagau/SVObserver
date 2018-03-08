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
	bool QueryListName(const RRWS::QueryListNameRequest& req, RRWS::QueryListNameResponse& resp, SVRPC::Error& err);
	bool  QueryListItem(const RRWS::QueryListItemRequest& req, RRWS::QueryListItemResponse& resp, SVRPC::Error& err);
	bool GetProductOrReject(const RRWS::GetProductRequest& request, RRWS::GetProductResponse& resp, SVRPC::Error& err);
	bool GetFailstatus(const RRWS::GetFailStatusRequest& request, RRWS::GetFailStatusResponse& resp, SVRPC::Error& err);
	bool  GetImageFromCurId(const  RRWS::GetImageFromCurIdRequest& req, RRWS::GetImageFromCurIdResponse& resp, SVRPC::Error& err);
private:
	bool GetProduct(SvSml::MLProduct* pProduct, bool nameInResponse, RRWS::Product* pProdMsg, SVRPC::Error& err);
	bool SetProductResponse(bool nameInResponse, const SvSml::MLProduct* pProduct, RRWS::Product& rProduct, SVRPC::Error& err);
	bool GetFailstatus(SvSml::vecpProd* pFailstatus, const RRWS::GetFailStatusRequest& request, RRWS::GetFailStatusResponse& resp, SVRPC::Error& err);

private:
	SvSml::SharedMemReader   m_MemReader;
	std::unique_ptr<LastResponseData> m_pLastResponseData;
};
}

