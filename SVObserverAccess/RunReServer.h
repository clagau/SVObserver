//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file RunReServer.h
/// All Rights Reserved 
//*****************************************************************************
/// the runrejectServer class implements the runreject function. 
/// This function access the data in the shared memory 
//******************************************************************************
#pragma once

#include "RunReApi\ServerFrontEndApi.h"
#pragma warning( push )
#pragma warning( disable : 4800 ) 
#include "RunReApi\RunReApi.pb.h"
#pragma warning( pop )

#include "ShareControl.h"
#include "LastResponseData.h"

using namespace RRApi;
//!class implements  the runreject function. Function access the data in the shared memory 
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
	//! Fills pResp struct from pProduct, adds input name to pResp if nameInResponse is true.
	void SetProductResponse(bool nameInResponse, const SvSml::MLProduct* pProduct, GetProductResponse* pResp);
	//! Fills pFailstatus struct from rFailStatResp, adds input name to pFailstatus depend from GetFailStatusRequest
	void GetFailstatus(SvSml::vecpProd* pFailstatus, const GetFailStatusRequest&, GetFailStatusResponse& rFailStatResp) ;
	//! 
	void GetProduct(SvSml::MLProduct* pProduct, const GetProductRequest&, GetProductResponse&);
	//!
	void GetProductOrReject(bool bProduct, const GetProductRequest& request, GetProductResponse& resp);
private:
	std::unique_ptr<ShareControl> m_pShareControl;
	std::unique_ptr<LastResponseData> m_pLastResponseData;

};
