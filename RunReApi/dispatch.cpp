//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file dispatch.cpp
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#include "stdafx.h"
#pragma warning( push )
#pragma warning( disable : 4800 ) 
#include "RunReApi.pb.h"
#pragma warning( pop )

#include "Helper.h"
#include "ServerFrontEndApi.h"
namespace RRApi
{
	void dispatch(const MessageEnvelope& reqEnvelope, MessageEnvelope& resEnvelope, IRRServer*  server)
	{
		auto type = reqEnvelope.type();
		switch (type)
		{
		case MessageType::GetVersion:
		{
			GetVersionRequest req;
			Helper::unwrap(reqEnvelope, req);
			GetVersionResponse res;
			server->GetVersion(req, res);
			Helper::wrap(resEnvelope, reqEnvelope.type(), reqEnvelope.id(), res);
			break;
		}
		case MessageType::GetProduct:
		{
			GetProductRequest req;
			Helper::unwrap(reqEnvelope, req);
			GetProductResponse res;
			server->GetProduct(req, res);
			Helper::wrap(resEnvelope, reqEnvelope.type(), reqEnvelope.id(), res);
			break;
		}
		case MessageType::GetReject:
		{
			GetProductRequest req;
			Helper::unwrap(reqEnvelope, req);
			GetProductResponse res;
			server->GetReject(req, res);
			Helper::wrap(resEnvelope, reqEnvelope.type(), reqEnvelope.id(), res);
			break;
		}
		case MessageType::GetFailStatus:
		{
			GetFailStatusRequest req;
			Helper::unwrap(reqEnvelope, req);
			GetFailStatusResponse res;
			server->GetFailstatus(req, res);
			Helper::wrap(resEnvelope, reqEnvelope.type(), reqEnvelope.id(), res);
			break;
		}
		case MessageType::GetImageFromCurId:
		{
			GetImageFromCurIdRequest req;
			Helper::unwrap(reqEnvelope, req);
			GetImageFromCurIdResponse res;
			server->GetImageFromCurId(req, res);
			Helper::wrap(resEnvelope, reqEnvelope.type(), reqEnvelope.id(), res);
			break;
		}
		case MessageType::GetItems:
		{
			GetItemsRequest req;
			Helper::unwrap(reqEnvelope, req);
			GetItemsResponse res;
			server->GetItems(req, res);
			Helper::wrap(resEnvelope, reqEnvelope.type(), reqEnvelope.id(), res);
			break;
		}
		case MessageType::QueryListName:
		{
			QueryListNameRequest req;
			Helper::unwrap(reqEnvelope, req);
			QueryListNameResponse res;
			server->QueryListName(req, res);
			Helper::wrap(resEnvelope, reqEnvelope.type(), reqEnvelope.id(), res);
			break;
		}
		case MessageType::QueryListItem:
		{
			QueryListItemRequest req;
			Helper::unwrap(reqEnvelope, req);
			QueryListItemResponse res;
			server->QueryListItem(req, res);
			Helper::wrap(resEnvelope, reqEnvelope.type(), reqEnvelope.id(), res);
			break;
		}
		}
	}
}