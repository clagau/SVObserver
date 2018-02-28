//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file RunRequest.inl
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#pragma once
namespace RRWS
{
template <class TReq, class TRes>
std::future<TRes>
runRequest(ClientService& client, void (ClientService::*getter)(TReq&&, SVRPC::Task<TRes>), TReq&& req)
{
	auto promise = std::make_shared<std::promise<TRes>>();
	auto task = SVRPC::Task<TRes>(
		[promise](TRes&& res) { promise->set_value(res); },
		[promise](const SVRPC::Error& err) { promise->set_exception(SVRPC::errorToExceptionPtr(err)); });
	(client.*getter)(std::move(req), task);
	return promise->get_future();
}

template <class TReq, class TRes>
std::future<TRes> runStream(ClientService& client,
	void (ClientService::*getter)(TReq&&, SVRPC::Observer<TRes>),
	TReq&& req,
	double& volume)
{
	volume = 0;
	auto promise = std::make_shared<std::promise<TRes>>();
	auto observer = SVRPC::Observer<TRes>(
		[&volume](TRes&& res) -> std::future<void>
	{
		//Magig numbers ??
		volume += (res.ByteSize() / 1024.0 / 1024.0); 
		return std::future<void>();
	},
		[promise]() { promise->set_value({}); },
		[promise](const SVRPC::Error& err) { promise->set_exception(SVRPC::errorToExceptionPtr(err)); });
	(client.*getter)(std::move(req), observer);
	return promise->get_future();
}
}