//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file RunRequest.inl
/// All Rights Reserved 
//*****************************************************************************
/// Util for client to to start an reequest
//******************************************************************************
#pragma once
namespace SvWsl
{
template <class TClientService ,class TReq, class TRes>
std::future<TRes> runRequest(TClientService& client, void (TClientService::*getter)(TReq&&, SvRpc::Task<TRes>), TReq&& req)
{
	auto promise = std::make_shared<std::promise<TRes>>();
	auto task = SvRpc::Task<TRes>(
		[promise](TRes&& res) { promise->set_value(res); },
		[promise](const SvPenv::Error& err) { promise->set_exception(SvStl::errorToExceptionPtr(err)); });
	(client.*getter)(std::move(req), task);
	return promise->get_future();
}

template <class TClientService, class TReq, class TRes>
std::future<TRes> runStream(TClientService& client,
	void (TClientService::*getter)(TReq&&, SvRpc::Observer<TRes>),
	TReq&& req,
	double& volume)
{
	volume = 0;
	auto promise = std::make_shared<std::promise<TRes>>();
	auto observer = SvRpc::Observer<TRes>(
		[&volume](TRes&& res) -> SvSyl::SVFuture<void>
	{
		//Magig numbers ??
		volume += (res.ByteSize() / 1024.0 / 1024.0); 
		return SvSyl::SVFuture<void>::make_ready();
	},
		[promise]() { promise->set_value({}); },
		[promise](const SvPenv::Error& err) { promise->set_exception(SvUl::errorToExceptionPtr(err)); });
	(client.*getter)(std::move(req), observer);
	return promise->get_future();
}
}