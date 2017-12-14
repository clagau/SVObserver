//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file RunReServer.h
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#pragma once

#include "RunReApi\ServerFrontEndApi.h"
#include "RunReApi\format.pb.h"
#include "ShareControl.h"
#include "LastResponseData.h"

using namespace RRApi;
class RunReServer : public RRApi::IRRServer
{
public:
	RunReServer();
	~RunReServer();
public:
	void GetVersion(const GetVersionRequest&, GetVersionResponse&);
	void GetProduct(const GetProductRequest&, GetProductResponse&);
	void GetReject(const GetProductRequest&, GetProductResponse&);
	void GetFailstatus(const GetFailStatusRequest&, GetFailStatusResponse&);
	void GetImageFromCurId(const GetImageFromCurIdRequest&, GetImageFromCurIdResponse&);
	void GetItems(const GetItemsRequest&, GetItemsResponse&);
	void QueryListName(const QueryListNameRequest&, QueryListNameResponse&);
	void QueryListItem(const QueryListItemRequest&, QueryListItemResponse&);
private:
	void SetProductResponse(bool nameInResponse, const SvSml::MLProduct* pProduct, GetProductResponse* pResp);
	void GetFailstatus(SvSml::Failstatus*, const GetFailStatusRequest&, GetFailStatusResponse&);
	void GetProduct(SvSml::MLProduct* pProduct, const GetProductRequest&, GetProductResponse&);
	void GetProductOrReject(bool bProduct, const GetProductRequest& request, GetProductResponse& resp);
private:
	std::unique_ptr<CShareControl> m_pShareControl;
	std::unique_ptr<CLastResponseData> m_pLastResponseData;

};
