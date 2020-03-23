//******************************************************************************
/// \copyright (c) 2017,2019 by Seidenader Maschinenbau GmbH
/// \file OverlayController.cpp
/// All Rights Reserved
//******************************************************************************


#pragma region Includes
#include "stdafx.h"
#include "OverlayController.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVProtoBuf/SVRC.h"
#include "SVRPCLibrary/ErrorUtil.h"
#include "SVRPCLibrary/SimpleClient.h"
#include "TriggerRecordController/ITriggerRecordR.h"
#include "FillHelper.h"
#pragma endregion Includes

namespace SvOv
{

OverlayController::OverlayController(boost::asio::io_context& io_context, SvRpc::RPCClient& rpcClient)
	: m_io_context(io_context)
	, m_rpc_client(rpcClient)
	, m_inspectionCmd_client(rpcClient)
{
}

SvSyl::SVFuture<SvPb::OverlayDesc> OverlayController::getOverlays(std::shared_ptr<SvTrc::ITriggerRecordR> pTr, GUID inspectionId, GUID imageId)
{
	if (nullptr != pTr && pTr->isObjectUpToTime())
	{
		auto promise = std::make_shared<SvSyl::SVPromise<SvPb::OverlayDesc>>();
		getOverlayStruct(pTr, inspectionId, imageId).then<void>(m_io_context, [this, pTr, promise](SvSyl::SVFuture<SvPb::OverlayDesc> future)
		{
			try
			{
				if (nullptr != pTr && pTr->isObjectUpToTime())
				{
					auto& desc = future.get();
					fillOverlay(desc, *pTr.get());
					promise->set_value(desc);
				}
				else
				{
					SvPenv::Error err;
					err.set_errorcode(SvPenv::ErrorCode::unknown);
					err.set_message("TriggerRecord is not valid anymore");
					promise->set_exception(SvRpc::errorToExceptionPtr(err));
				}
			}
			catch (const std::exception& rEx)
			{
				promise->set_exception(std::make_exception_ptr(rEx));
			}
		});

		return promise->get_future();
	}
	else
	{
		return SvSyl::SVFuture<SvPb::OverlayDesc>::make_ready(SvPb::OverlayDesc());
	}
}

SvSyl::SVFuture<SvPb::OverlayDesc> OverlayController::restructOverlayDesc(GUID inspectionId, GUID imageId)
{
	auto promise = std::make_shared<SvSyl::SVPromise<SvPb::OverlayDesc>>();
	SvRpc::Task<SvPb::InspectionCmdResponse> task(
		[promise](SvPb::InspectionCmdResponse&& responseCmd)
	{
		if (responseCmd.has_getoverlaystructresponse())
		{
			promise->set_value(responseCmd.getoverlaystructresponse().overlays());
			return;
		}
		SvPenv::Error err;
		err.set_errorcode(SvPenv::ErrorCode::unknown);
		err.set_message("Message GetOverlayStructRequest do not return a valid response");
		promise->set_exception(SvRpc::errorToExceptionPtr(err));
	},
		[promise](const SvPenv::Error& err)
	{
		promise->set_exception(SvRpc::errorToExceptionPtr(err));
	});

	SvPb::InspectionCmdRequest requestCmd;
	SvPb::SetGuidInProtoBytes(requestCmd.mutable_inspectionid(), inspectionId);
	auto* pRequest = requestCmd.mutable_getoverlaystructrequest();
	SvPb::SetGuidInProtoBytes(pRequest->mutable_imageid(), imageId);
	m_inspectionCmd_client.request(std::move(requestCmd), task, m_timeout);
	return promise->get_future();
}

SvSyl::SVFuture<SvPb::OverlayDesc> OverlayController::getOverlayStruct(std::shared_ptr<SvTrc::ITriggerRecordR> pTr, GUID inspectionId, GUID imageId)
{
	{	//lock map
		std::lock_guard<std::mutex> lockMap(m_overlayMapMutex);
		OverlayDefStruct& rOverlayDesc = m_overlayMap[imageId];
		if (rOverlayDesc.m_resetId == pTr->getResetId())
		{
			return SvSyl::SVFuture<SvPb::OverlayDesc>::make_ready(rOverlayDesc.m_overlayDesc);
		}
	}
	
	auto promise = std::make_shared<SvSyl::SVPromise<SvPb::OverlayDesc>>();
	int resetId = pTr->getResetId();
	restructOverlayDesc(inspectionId, imageId).then<void>(m_io_context,
		[this, resetId, imageId, promise](SvSyl::SVFuture<SvPb::OverlayDesc> future)
	{
		try
		{
			const auto& desc = future.get();
			std::lock_guard<std::mutex> lockMap(m_overlayMapMutex);
			m_overlayMap[imageId] = OverlayDefStruct(resetId, desc);
			promise->set_value(desc);
		}
		catch (const std::exception& rEx)
		{
			promise->set_exception(std::make_exception_ptr(rEx));
		}
	});

	return promise->get_future();
}
}// namespace SvOv
