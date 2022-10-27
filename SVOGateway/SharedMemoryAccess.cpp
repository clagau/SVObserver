//******************************************************************************
/// \copyright (c) 2017,2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SharedMemoryAccess.cpp
/// All Rights Reserved
//******************************************************************************
/// Mapping from incoming RPC requests to shared memory wrapper.
//******************************************************************************

#include "stdafx.h"

//Moved to precompiled header: #include <future>
//Moved to precompiled header: #include <queue>
//Moved to precompiled header: #include <boost/log/trivial.hpp>

#pragma region Includes
#include "SharedMemoryAccess.h"
#include "SVAuthLibrary/UserDatabase.h"
#include "SVLogLibrary/Logging.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMessage/SVMessage.h"
#include "SVOGateway/WebAppVersionLoader.h"
#include "SVSharedMemoryLibrary/ShareControlSetting.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "ObjectInterfaces/ITriggerRecordR.h"
#include "ObjectInterfaces/ITriggerRecordControllerR.h"
#include "SVStatusLibrary/MessageContainer.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/MessageTextGenerator.h"
#pragma endregion Includes


namespace SvOgw
{

SharedMemoryAccess::SharedMemoryAccess(
	boost::asio::io_service& rIoService,
	const SvSml::ShareControlSettings& ControlParameter,
	const WebAppVersionLoader& rWebAppVersionLoader,
	SvRpc::RPCClient& rpcClient,
	SvAuth::UserDatabase& rUserDatabase,
	bool skipPermissionChecks
)
	: m_io_service(rIoService)
	, m_rWebAppVersionLoader(rWebAppVersionLoader)
	, m_rUserDatabase(rUserDatabase)
	, m_skipPermissionChecks(skipPermissionChecks)
	, m_pause_timer(rIoService)
	, m_overlay_controller(rIoService, rpcClient)
	, m_pShareControlInstance{std::make_unique<SvSml::ShareControl>(ControlParameter)}
	, m_SharedMemoryLock()
	, m_DisconnectCheckTimer(rIoService)
	, m_ConfigurationLockStatusTimer(rIoService)
{
	schedule_trigger_record_pause_state();
	schedule_disconnected_clients_check();
	schedule_configuration_lock_status_update();
	subscribe_to_trc();
}

SharedMemoryAccess::~SharedMemoryAccess()
{
	m_pause_timer.cancel();
	m_DisconnectCheckTimer.cancel();
	unsubscribe_from_trc();
}

void SharedMemoryAccess::GetVersion(const SvPb::GetGatewayVersionRequest& , SvRpc::Task<SvPb::GetVersionResponse> task)
{
	SvPb::GetVersionResponse Response;
	Response.set_version(SvUl::to_utf8(SvSyl::SVVersionInfo::GetShortTitleVersion()));
	task.finish(std::move(Response));
}

void SharedMemoryAccess::GetWebAppVersion(const SvPb::GetWebAppVersionRequest& , SvRpc::Task<SvPb::GetVersionResponse> task)
{
	SvPb::GetVersionResponse Response;
	Response.set_version(m_rWebAppVersionLoader.getVersion());
	task.finish(std::move(Response));
}

void SharedMemoryAccess::GetInspections(const SvPb::GetInspectionsRequest&, SvRpc::Task<SvPb::GetInspectionsResponse> task)
{
	SvPenv::Error Error;
	SvPb::GetInspectionsResponse Response;

	auto* pTrc = SvOi::getTriggerRecordControllerRInstance();
	if (nullptr != pTrc)
	{
		const auto& rInspections = pTrc->getInspections();
		for (int i = 0; i < rInspections.list_size(); ++i)
		{
			const auto& rInspection = rInspections.list(i);
			auto& entry = *Response.add_inspections();
			entry.set_id(rInspection.id());
			entry.set_name(rInspection.name());
		}
	}

	task.finish(std::move(Response));
}

void SharedMemoryAccess::GetProduct(const SvPb::GetProductRequest& rRequest, SvRpc::Task<SvPb::GetProductResponse> task)
{
	SvPenv::Error Error;
	SvPb::GetProductResponse Response;

	if (m_pShareControlInstance->GetProduct(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}
}

void SharedMemoryAccess::GetProductData(const SvPb::GetProductDataRequest& rRequest, SvRpc::Task<SvPb::GetProductDataResponse> task)
{
	auto res = std::make_shared<SvPb::GetProductDataResponse>();
	get_product_data(*res, rRequest).then(m_io_service, [task, res](SvSyl::SVFuture<void> future)
	{
		task.finish(std::move(*res));
	});
}

void SharedMemoryAccess::GetReject(const SvPb::GetRejectRequest& rRequest, SvRpc::Task<SvPb::GetRejectResponse> task)
{
	SvPenv::Error Error;
	SvPb::GetRejectResponse Response;

	if (m_pShareControlInstance->GetReject(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}
}

void SharedMemoryAccess::GetFailstatus(const SvPb::GetFailStatusRequest& rRequest, SvRpc::Task<SvPb::GetFailStatusResponse> task)
{
	SvPenv::Error Error;
	SvPb::GetFailStatusResponse Response;
	if (m_pShareControlInstance->GetFailstatus(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}

}

void SharedMemoryAccess::GetImageFromId(const SvPb::GetImageFromIdRequest& rRequest,
	SvRpc::Task<SvPb::GetImageFromIdResponse> task)
{
	SvPb::GetImageFromIdResponse Response;
	SvPenv::Error Error;

	if (m_pShareControlInstance->GetImageFromId(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}

}

void SharedMemoryAccess::GetTriggerItems(const SvPb::GetTriggerItemsRequest&, SvRpc::Task<SvPb::GetTriggerItemsResponse> task)
{
	SvPenv::Error Error;
	Error.set_errorcode(SvPenv::ErrorCode::notImplemented);
	task.error(Error);
}

void SharedMemoryAccess::QueryListName(const SvPb::QueryListNameRequest& rRequest, SvRpc::Task<SvPb::QueryListNameResponse> task)
{
	SvPb::QueryListNameResponse Response;
	SvPenv::Error Error;

	if (m_pShareControlInstance->QueryListName(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}
}

void SharedMemoryAccess::QueryListItem(const SvPb::QueryListItemRequest& rRequest, SvRpc::Task<SvPb::QueryListItemResponse> task)
{
	SvPb::QueryListItemResponse Response;
	SvPenv::Error Error;
	if (m_pShareControlInstance->QueryListItem(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}
}

void SharedMemoryAccess::StoreClientLogs(const SvPb::StoreClientLogsRequest& rRequest, SvRpc::Task<SvPb::EmptyResponse> task)
{
	auto map_to_boost_log_severity = [](const SvPb::LogSeverity& severity) -> boost::log::trivial::severity_level
	{
		switch (severity)
		{
			case SvPb::LogSeverity::fatal:
				return boost::log::trivial::fatal;
			case SvPb::LogSeverity::error:
				return boost::log::trivial::error;
			case SvPb::LogSeverity::warning:
				return boost::log::trivial::warning;
			case SvPb::LogSeverity::info:
				return boost::log::trivial::info;
			case SvPb::LogSeverity::debug:
				return boost::log::trivial::debug;
			case SvPb::LogSeverity::trace:
				return boost::log::trivial::trace;
			default:
				return boost::log::trivial::error;
		}
	};

	const auto& client = rRequest.client();
	for (const auto& entry : rRequest.logs())
	{
		const auto severity = map_to_boost_log_severity(entry.severity());
		BOOST_LOG_STREAM_WITH_PARAMS(
			::boost::log::trivial::logger::get(),
			(::boost::log::keywords::severity = severity)
		) << "[" << client << "] " << entry.message();
	}

	task.finish(SvPb::EmptyResponse());
}

void SharedMemoryAccess::SetRejectStreamPauseState(const SvPb::SetRejectStreamPauseStateRequest& rRequest, SvRpc::Task<SvPb::EmptyResponse> task)
{
	auto* pTrc = SvOi::getTriggerRecordControllerRInstance();
	if (nullptr != pTrc)
	{
		google::protobuf::uint32 inspectionId = rRequest.inspectionid();
		if (inspectionId == 0)
		{
			SV_LOG_GLOBAL(trace) << "pauseTrsOfInterest";
			pTrc->pauseTrsOfInterest(rRequest.pause());
		}
		else
		{
			const auto inspectionPos = get_inspection_pos_for_id(pTrc->getInspections(), inspectionId);
			if (inspectionPos < 0)
			{
				SV_LOG_GLOBAL(trace) << "Unknown inspection id in pauseTrsOfInterest";
				SvPenv::Error err;
				err.set_errorcode(SvPenv::ErrorCode::notFound);
				err.set_message("Unknown inspection id");
				task.error(err);
				return;
			}

			pTrc->pauseTrsOfInterest(rRequest.pause(), inspectionPos);
		}
	}

	SvPb::EmptyResponse Response;
	task.finish(std::move(Response));

	check_trigger_record_pause_state_changed();
}

void SharedMemoryAccess::GetGatewayNotificationStream(
	const SvPb::GetGatewayNotificationStreamRequest& rRequest,
	SvRpc::Observer<SvPb::GetGatewayNotificationStreamResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
{
	auto state = std::make_shared<notification_stream_t>(rRequest, observer, ctx);
	m_notification_streams.push_back(state);

	// send initial pause state to client
	send_trigger_record_pause_state_to_client(*state, m_pause_state);

	// send current configuration lock status to client
	auto lockOwnerStream = m_SharedMemoryLock.GetLockOwnerStream();
	send_configuration_lock_status(*state, lockOwnerStream);
}

void SharedMemoryAccess::GetGatewayMessageStream(
	const SvPb::GetGatewayMessageStreamRequest& rRequest,
	SvRpc::Observer<SvPb::GetGatewayMessageStreamResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
{
	auto state = std::make_shared<message_stream_t>(rRequest, observer, ctx);
	{
		std::lock_guard<std::mutex> lockGuard(m_message_streams_mutex);
		m_message_streams.push_back(state);
	}
}

void SharedMemoryAccess::GetProductStream(
	const SvPb::GetProductStreamRequest& rRequest,
	SvRpc::Observer<SvPb::GetProductStreamResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
{
	auto state = std::make_shared<product_stream_t>(rRequest, observer, ctx);
	ctx->enableThrottling();
	ctx->setMaxPendingAcks(state->interestedInRejects ? 5 : 1);
	m_ProductStreams.push_back(state);
	rebuild_trc_pos_cache(*state);
	collect_historical_triggers(*state);
	schedule_historical_triggers(state);
}

SharedMemoryAccess::product_stream_t::product_stream_t(
	const SvPb::GetProductStreamRequest& streamReq,
	SvRpc::Observer<SvPb::GetProductStreamResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
	: observer(observer)
	, ctx(ctx)
	, interestedInAllProducts(streamReq.imageids_size() > 0 || streamReq.valueids_size() > 0)
	, interestedInRejects(streamReq.rejectsonly() || streamReq.rejectimageids_size() > 0 || streamReq.rejectvalueids_size() > 0)
{
	this->req.CopyFrom(streamReq);
}

static bool is_subscribed_to(const SvPb::GetProductStreamRequest& req, const SvPb::Inspection& inspection)
{
	return req.inspectionid() == inspection.id();
}

void SharedMemoryAccess::on_new_trigger_record(const SvOi::TrInterestEventData& new_trigger)
{
	add_new_trigger_to_queue(new_trigger);

	m_io_service.post([this]()
	{
		check_queue_for_new_trigger_record();
	});
}

void SharedMemoryAccess::add_new_trigger_to_queue(const SvOi::TrInterestEventData& new_trigger)
{
	std::lock_guard<std::mutex> l(m_NewTriggerMutex);
	m_NewTriggerQueue.push_back(std::move(new_trigger));
}

void SharedMemoryAccess::pop_new_trigger_from_queue(std::vector<SvOi::TrInterestEventData>& result)
{
	std::lock_guard<std::mutex> l(m_NewTriggerMutex);
	result = m_NewTriggerQueue;
	m_NewTriggerQueue.clear();
}

void SharedMemoryAccess::check_queue_for_new_trigger_record()
{
	std::vector<SvOi::TrInterestEventData> new_triggers;
	pop_new_trigger_from_queue(new_triggers);

	if (m_ProductStreams.empty() || new_triggers.empty())
	{
		return;
	}

	clear_cancelled_product_streams();

	for (const auto& ptr : m_ProductStreams)
	{
		add_new_triggers_to_product_stream(*ptr, new_triggers);
		update_product_stream_queue(*ptr);
		flush_product_stream_queue(ptr);
	}
}

void SharedMemoryAccess::add_new_triggers_to_product_stream(product_stream_t& product_stream, const std::vector<SvOi::TrInterestEventData>& new_triggers)
{
	auto* pTrc = SvOi::getTriggerRecordControllerRInstance();
	if (nullptr != pTrc)
	{
		const auto& rInspections = pTrc->getInspections();

		for (const auto& new_trigger : new_triggers)
		{
			const auto& rInspection = rInspections.list(new_trigger.m_inspectionPos);
			if (is_subscribed_to(product_stream.req, rInspection) && ((new_trigger.m_isInterest && product_stream.interestedInRejects) || product_stream.interestedInAllProducts))
			{
				product_stream.newTriggerQueue.push_back(new_trigger);
			}
		}
	}
}

void SharedMemoryAccess::update_product_stream_queue(product_stream_t& product_stream)
{
	const auto numFreeSlots = product_stream.ctx->freeSlotsUntilThrottling();
	if (numFreeSlots == std::numeric_limits<size_t>::max())
	{
		return;
	}

	auto& queue = product_stream.newTriggerQueue;
	if (queue.empty())
	{
		return;
	}

	const auto numNewTriggers = queue.size();

	// no need to drop anything from the queue
	if (numNewTriggers < numFreeSlots)
	{
		return;
	}

	auto isRejectFn = [](const SvOi::TrInterestEventData& rEventData) -> bool {return rEventData.m_isInterest; };

	const auto numPendingRejects = std::count_if(queue.begin(), queue.end(), isRejectFn);
	auto allowedRejects = std::min<size_t>(numPendingRejects, numFreeSlots);
	auto allowedProducts = numFreeSlots - allowedRejects;

	// always keep one more than allowed to make sure we do not lose the last reject
	++allowedRejects;
	++allowedProducts;

	// walk from back to front and keep only allowed entries
	for (auto i = queue.size(); i > 0; --i)
	{
		const auto idx = i - 1;
		const auto& eventData = queue[idx];
		if (eventData.m_isInterest)
		{
			if (allowedRejects > 0)
			{
				--allowedRejects;
				continue;
			}
		}
		else
		{
			if (allowedProducts > 0)
			{
				--allowedProducts;
				continue;
			}
		}
		queue.erase(queue.begin() + idx);
	}
}

void SharedMemoryAccess::flush_product_stream_queue(std::shared_ptr<product_stream_t> ptr)
{
	auto* pTrc = SvOi::getTriggerRecordControllerRInstance();
	if (nullptr != pTrc)
	{
		const auto& rInspections = pTrc->getInspections();

		auto& queue = ptr->newTriggerQueue;
		auto numFreeSlots = ptr->ctx->freeSlotsUntilThrottling();
		while (numFreeSlots > 0 && !queue.empty())
		{
			--numFreeSlots;

			auto eventData = queue.front();
			queue.erase(queue.begin());

			const auto inspectionPos = eventData.m_inspectionPos;
			const auto isReject = eventData.m_isInterest;
			const auto trId = eventData.m_trId;

			auto tro = pTrc->createTriggerRecordObject(inspectionPos, trId);
			if (!tro)
			{
				SvDef::StringVector msgList;
				msgList.push_back(std::format(_T("{}"), inspectionPos));
				msgList.push_back(std::format(_T("{}"), trId));
				SV_LOG_GLOBAL(warning) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_Gateway_TRC_CreateTriggerRecordObjectFailed, msgList);
				SvStl::MessageManager Exception(SvStl::MsgType::Notify);
				Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Gateway_TRC_CreateTriggerRecordObjectFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
				continue;
			}

			// while historical products are still pending, new ones make one historical less
			// TODO better place for this code?
			if (!ptr->historicalTriggerQueue.empty())
			{
				ptr->historicalTriggerQueue.pop_back();
			}

			const auto& rInspection = rInspections.list(inspectionPos);

			handle_new_trigger_record(ptr, tro, inspectionPos, rInspection.id(), trId, isReject);
		}
	}
}

static bool read_image(SvPb::Image& resImg, SvOi::ITRCImagePtr imgPtr)
{
	auto hdl = imgPtr->getHandle();
	auto info = hdl->GetBitmapInfo();
	auto& buffer = hdl->GetBuffer();
	auto& out = *resImg.mutable_rgbdata();
	auto rc = SVMatroxBufferInterface::CopyBufferToFileDIB(out, info, buffer);
	// height can be negative to indicate that the image origin is the upper-left corner
	// See https://docs.microsoft.com/en-us/previous-versions/dd183376(v=vs.85)
	resImg.set_height(std::labs(info.GetHeight()));
	resImg.set_width(info.GetWidth());
	resImg.set_type("image/bmp");
	return rc == S_OK;
}

static bool read_variant(SvPb::Variant& resVar, const _variant_t& variant)
{
	return SvPb::ConvertVariantToProtobuf(variant, &resVar) == S_OK;
}

void SharedMemoryAccess::clear_duplicate_images(const product_stream_t& stream, SvPb::GetProductStreamResponse& res)
{
	if (!res.isreject())
	{
		return;
	}

	const auto& imageIds = stream.req.imageids();
	const auto& rejectImageIds = stream.req.rejectimageids();

	auto imageIdIt = imageIds.cbegin();
	for (int i = 0; imageIdIt != imageIds.cend(); ++imageIdIt, ++i)
	{
		const auto it = std::find(rejectImageIds.cbegin(), rejectImageIds.cend(), *imageIdIt);
		if (it != rejectImageIds.cend())
		{
			res.mutable_images()->at(i).Clear();
			res.mutable_overlays()->at(i).Clear();
		}
	}
}

void SharedMemoryAccess::clear_duplicate_values(const product_stream_t& stream, SvPb::GetProductStreamResponse& res)
{
	if (!res.isreject())
	{
		return;
	}

	const auto& valueIds = stream.req.valueids();
	const auto& rejectValueIds = stream.req.rejectvalueids();

	auto valueIdIt = valueIds.cbegin();
	for (int i = 0; valueIdIt != valueIds.cend(); ++valueIdIt, ++i)
	{
		const auto it = std::find(rejectValueIds.cbegin(), rejectValueIds.cend(), *valueIdIt);
		if (it != rejectValueIds.cend())
		{
			res.mutable_values()->at(i).Clear();
		}
	}
}

SvSyl::SVFuture<void> SharedMemoryAccess::handle_new_trigger_record(std::shared_ptr<product_stream_t> stream, SvOi::ITriggerRecordRPtr pTro, int inspectionPos, uint32_t inspectionId, int trId, bool is_reject)
{
	auto res = std::make_shared<SvPb::GetProductStreamResponse>();

	res->set_trigger(trId);
	res->set_isreject(is_reject);

	std::vector<SvSyl::SVFuture<void>> imageFutures;
	const auto includeoverlays = stream->req.includeoverlays();

	{ // always collect images and values for live-data stream
		collect_values(
			*res->mutable_values(),
			*pTro,
			stream->req.valueids(),
			stream->valuePositions);
		imageFutures.emplace_back(collect_images(
			*res->mutable_images(),
			*res->mutable_overlays(),
			pTro,
			stream->req.imageids(),
			stream->imagePositions,
			inspectionPos,
			inspectionId,
			includeoverlays));
	}

	// collect reject images and values
	if (is_reject)
	{
		collect_values(
			*res->mutable_rejectvalues(),
			*pTro,
			stream->req.rejectvalueids(),
			stream->rejectValuePositions);
		imageFutures.emplace_back(collect_images(
			*res->mutable_rejectimages(),
			*res->mutable_rejectoverlays(),
			pTro,
			stream->req.rejectimageids(),
			stream->rejectImagePositions,
			inspectionPos,
			inspectionId,
			includeoverlays));
	}

	auto promise = std::make_shared<SvSyl::SVPromise<void>>();
	SvSyl::SVPromise<void>::all(m_io_service, imageFutures).then(m_io_service, [this, stream, res, promise](SvSyl::SVFuture<void> future)
	{
		if (stream->ctx->isCancelled())
		{
			SvPenv::Error err;
			err.set_errorcode(SvPenv::ErrorCode::internalError);
			err.set_message("Connection lost");
			promise->set_exception(SvStl::errorToExceptionPtr(err));
			return;
		}

		// nothing to send
		if (res->images().empty() &&
			res->values().empty() &&
			res->rejectimages().empty() &&
			res->rejectvalues().empty())
		{
			promise->set_value();
			return;
		}

		// skip duplicate values/images
		clear_duplicate_images(*stream, *res);
		clear_duplicate_values(*stream, *res);

		stream->observer.onNext(std::move(*res)).then(m_io_service, [promise](SvSyl::SVFuture<void> future)
		{
			promise->set_value();
		});
	});

	return promise->get_future();
}

SvSyl::SVFuture<void> SharedMemoryAccess::get_product_data(SvPb::GetProductDataResponse& res, const SvPb::GetProductDataRequest& req)
{
	auto* pTrc = SvOi::getTriggerRecordControllerRInstance();
	const auto inspectionId = req.inspectionid();
	int inspectionPos  = (nullptr != pTrc) ? get_inspection_pos_for_id(pTrc->getInspections(), inspectionId) : -1;
	if (nullptr == pTrc || inspectionPos < 0)
	{
		// TODO;
		return SvSyl::SVPromise<void>::make_ready();
	}

	auto trId = req.trigger();
	if (trId <= 0)
	{
		trId = pTrc->getLastTrId(inspectionPos);
	}

	auto tro = pTrc->createTriggerRecordObject(inspectionPos, trId);
	if (!tro)
	{
		// TODO;
		return SvSyl::SVPromise<void>::make_ready();
	}

	std::vector<std::pair<bool, int>> imagePositions;
	std::vector<int> valuePositions;
	try
	{
		collect_image_pos(imagePositions, pTrc->getImageDefMap(inspectionPos), pTrc->getChildImageDefMap(inspectionPos), pTrc->getLinkedImageDefMap(inspectionPos), req.imageids());
		collect_value_pos(valuePositions, pTrc->getDataDefMap(inspectionPos), req.valueids());
	}
	catch (...)
	{
		std::string msgText = SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_ErrorInGetProductData);
		SV_LOG_GLOBAL(error) << msgText;
		return SvSyl::SVPromise<void>::make_ready();
	}

	res.set_trigger(trId);

	const auto includeoverlays = req.includeoverlays();
	auto future = collect_images(*res.mutable_images(), *res.mutable_overlays(), tro, req.imageids(), imagePositions, inspectionPos, inspectionId, includeoverlays);
	collect_values(*res.mutable_values(), *tro, req.valueids(), valuePositions);
	return future;
}

SvSyl::SVFuture<void> SharedMemoryAccess::collect_images(
	::google::protobuf::RepeatedPtrField<SvPb::Image>& images,
	::google::protobuf::RepeatedPtrField<SvPb::OverlayDesc>& overlays,
	SvOi::ITriggerRecordRPtr pTro,
	const ::google::protobuf::RepeatedField<uint32_t>& imageIds,
	const std::vector<std::pair<bool, int>>& imagePositions,
	int inspectionPos,
	uint32_t inspectionId,
	bool includeoverlays
)
{
	std::vector<SvSyl::SVFuture<void>> overlay_futures;

	for (auto i = 0ULL, len = imagePositions.size(); i < len; ++i)
	{
		const auto imagePosPair = imagePositions[i];
		const auto isChildImage = std::get<0>(imagePosPair);
		const auto imagePos = std::get<1>(imagePosPair);
		const auto imageId = imageIds.Get(static_cast<int>(i));

		// add a value entry to the result list and only fill it if position is valid
		auto& img = *images.Add();
		auto overlayDesc = static_cast<SvPb::OverlayDesc*>(nullptr);
		if (includeoverlays)
		{
			overlayDesc = overlays.Add();
		}

		if (imagePos < 0)
		{
			SV_LOG_GLOBAL(debug) << "Ignoring image with invalid position " << imagePos;
			continue;
		}

		SvOi::ITRCImagePtr imgPtr = nullptr;
		if (pTro)
		{
			if (isChildImage)
			{
				imgPtr = pTro->getChildImage(imagePos);
			}
			else
			{
				imgPtr = pTro->getImage(imagePos);
			}
		}
		if (!imgPtr)
		{
			SV_LOG_GLOBAL(debug) << "  > image " << imageId << " not found";
			continue;
		}

		// TODO handle image format
		//stream.req.imageformat()
		if (!read_image(img, imgPtr))
		{
			SvDef::StringVector msgList;
			msgList.push_back(std::format(_T("{}"), imageId));
			msgList.push_back(std::format(_T("{}"), inspectionPos));
			SV_LOG_GLOBAL(warning) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_Gateway_ReadImageFailed, msgList);
			SvStl::MessageManager Exception(SvStl::MsgType::Notify);
			Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Gateway_ReadImageFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
			continue;
		}

		if (includeoverlays && overlayDesc)
		{
			auto future = m_overlay_controller.getOverlays(pTro, inspectionId, imageId)
				.then<void>(m_io_service, [overlayDesc](SvSyl::SVFuture<SvPb::OverlayDesc> future)
			{
				const auto& desc = future.get();
				overlayDesc->CopyFrom(desc);
			});
			overlay_futures.emplace_back(std::move(future));
		}
	}

	return SvSyl::SVFuture<void>::all(m_io_service, overlay_futures);
}

void SharedMemoryAccess::collect_values(
	::google::protobuf::RepeatedPtrField<SvPb::Variant>& values,
	SvOi::ITriggerRecordR& tro,
	const ::google::protobuf::RepeatedField<uint32_t>& valueIds,
	const std::vector<int>& valuePositions
)
{
	for (auto i = 0ULL, len = valuePositions.size(); i < len; ++i)
	{
		// add a value entry to the result list and only fill it if position is valid
		auto& value = *values.Add();
		const auto pos = valuePositions[i];
		if (pos < 0)
		{
			continue;
		}

		auto variant = tro.getDataValue(pos);
		if (!read_variant(value, variant))
		{
			const auto valueId = valueIds.Get(static_cast<int>(i));
			SvDef::StringVector msgList;
			msgList.push_back(std::format(_T("{}"), valueId));
			SV_LOG_GLOBAL(warning) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_Gateway_ReadValueFailed, msgList);
			SvStl::MessageManager Exception(SvStl::MsgType::Notify);
			Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Gateway_ReadValueFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
	}
}

void SharedMemoryAccess::collect_historical_triggers(product_stream_t& stream)
{
	auto* pTrc = SvOi::getTriggerRecordControllerRInstance();

	int startAtTriggerCount = stream.req.startattriggercount();
	int inspectionPos = (nullptr != pTrc) ? get_inspection_pos_for_id(pTrc->getInspections(), stream.req.inspectionid()) : -1;
	if (nullptr == pTrc || startAtTriggerCount == 0 || inspectionPos < 0)
	{
		return;
	}

	if (startAtTriggerCount > 0)
	{
		int lastTrId = pTrc->getLastTrId(inspectionPos);
		if (startAtTriggerCount > lastTrId)
		{
			return;
		}

		if (stream.interestedInRejects)
		{
			int numHistoricalTrigger = lastTrId - startAtTriggerCount + 1;
			auto trList = pTrc->getTrsOfInterest(inspectionPos, numHistoricalTrigger);
			for (const auto& tr : trList)
			{
				stream.historicalTriggerQueue.emplace_back(inspectionPos, tr->getId(), false);
			}
		}
		else
		{
			for (int i = lastTrId; i >= startAtTriggerCount; --i)
			{
				stream.historicalTriggerQueue.emplace_back(inspectionPos, i, false);
			}
		}
	}
	else
	{
		int numHistoricalTrigger = std::labs(startAtTriggerCount);
		if (stream.interestedInRejects)
		{
			auto trList = pTrc->getTrsOfInterest(inspectionPos, numHistoricalTrigger);
			for (const auto& tr : trList)
			{
				stream.historicalTriggerQueue.emplace_back(inspectionPos, tr->getId(), false);
			}
		}
		else
		{
			int lastTrId = pTrc->getLastTrId(inspectionPos);
			for (int i = lastTrId; i > 0 && i >= lastTrId - numHistoricalTrigger + 1; --i)
			{
				stream.historicalTriggerQueue.emplace_back(inspectionPos, i, false);
			}
		}
	}

	if (stream.historicalTriggerQueue.empty())
	{
		return;
	}

	// update reject flag or history trigger queue entries
	auto triggersOfInterest = pTrc->getTrsOfInterest(inspectionPos, static_cast<int>(stream.historicalTriggerQueue.size()));
	for (auto& rEntry : stream.historicalTriggerQueue)
	{
		rEntry.m_isInterest = std::any_of(triggersOfInterest.cbegin(), triggersOfInterest.cend(), [&rEntry](const auto& tr) { return tr->getId() == rEntry.m_trId; });
	}
}

void SharedMemoryAccess::schedule_historical_triggers(std::shared_ptr<product_stream_t> stream)
{
	auto* pTrc = SvOi::getTriggerRecordControllerRInstance();
	if (nullptr == pTrc || stream->ctx->isCancelled())
	{
		return;
	}

	SvOi::ITriggerRecordRPtr triggerRecord = nullptr;
	SvOi::TrInterestEventData new_trigger(0, 0, false);
	while (!triggerRecord && !stream->historicalTriggerQueue.empty())
	{
		new_trigger = stream->historicalTriggerQueue.front();
		stream->historicalTriggerQueue.erase(stream->historicalTriggerQueue.begin());

		triggerRecord = pTrc->createTriggerRecordObject(new_trigger.m_inspectionPos, new_trigger.m_trId);
	}

	// There is no valid trigger record left to send. We are done.
	if (!triggerRecord)
	{
		stream->historicalTriggerQueue.clear();
		return;
	}

	auto inspectionId = stream->req.inspectionid();

	auto future = handle_new_trigger_record(stream, triggerRecord, new_trigger.m_inspectionPos, inspectionId, new_trigger.m_trId, new_trigger.m_isInterest);
	future.then(m_io_service, [this, stream](SvSyl::SVFuture<void> f)
	{
		schedule_historical_triggers(stream);
	});
}

void SharedMemoryAccess::rebuild_trc_pos_caches()
{
	if (!m_trc_ready)
	{
		return;
	}

	for (auto& streamPtr : m_ProductStreams)
	{
		rebuild_trc_pos_cache(*streamPtr);
	}
}

void SharedMemoryAccess::rebuild_trc_pos_cache(product_stream_t& stream)
{
	auto* pTrc = SvOi::getTriggerRecordControllerRInstance();
	if (nullptr == pTrc || false == m_trc_ready)
	{
		return;
	}

	try
	{
		int inspectionId = stream.req.inspectionid();
		int inspectionPos = get_inspection_pos_for_id(pTrc->getInspections(), inspectionId);
		if (inspectionPos >= 0)
		{
			const auto& rDataDefMap = pTrc->getDataDefMap(inspectionPos);
			const auto& rImageDefMap = pTrc->getImageDefMap(inspectionPos);
			const auto& rChildImageDefMap = pTrc->getChildImageDefMap(inspectionPos);
			const auto& rLinkedImageDefMap = pTrc->getLinkedImageDefMap(inspectionPos);
			collect_value_pos(stream.valuePositions, rDataDefMap, stream.req.valueids());
			collect_value_pos(stream.rejectValuePositions, rDataDefMap, stream.req.rejectvalueids());
			collect_image_pos(stream.imagePositions, rImageDefMap, rChildImageDefMap, rLinkedImageDefMap, stream.req.imageids());
			collect_image_pos(stream.rejectImagePositions, rImageDefMap, rChildImageDefMap, rLinkedImageDefMap, stream.req.rejectimageids());
		}
	}
	catch (...)
	{
		std::string msgText = SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_ErrorInRebuildTrcPosCache);
		SV_LOG_GLOBAL(error) << msgText;
		return;
	}
}

void SharedMemoryAccess::collect_value_pos(std::vector<int>& positions, const std::unordered_map<uint32_t, int>& dataDefMap, const ::google::protobuf::RepeatedField<uint32_t>& ids)
{
#if defined (TRACE_THEM_ALL) || defined (TRACE_SHARED_MEMORY_ACCESS)
	DWORD tick = ::GetTickCount();
	int notfound = 0;
#endif
	positions.clear();

	for (auto id : ids)
	{
		auto it = dataDefMap.find(id);
		if (it != dataDefMap.end())
		{
			positions.push_back(it->second);
		}
		else
		{
			positions.push_back(-1);
#if defined (TRACE_THEM_ALL) || defined (TRACE_SHARED_MEMORY_ACCESS)
			if (notfound < 3)
			{
				std::stringstream traceStream;
				traceStream << "Error collect_value_pos with id " << id << std::endl;
				OutputDebugString(traceStream.str().c_str());
				//uncomment the next line to enable error reporting to cmd window
				//SV_LOG_GLOBAL(info) << traceStream.str();
			}
			notfound++;
#endif
		}
	}
#if defined (TRACE_THEM_ALL) || defined (TRACE_SHARED_MEMORY_ACCESS)
	tick = ::GetTickCount() - tick;
	std::stringstream traceStream;
	traceStream << "Ticks needed for  collect_value_pos: " << tick << " idsSize: " << ids.size() << " dataDefMap: " <<
		dataDefMap.size() << " notfound: " << notfound << std::endl;

	OutputDebugString(traceStream.str().c_str());
	//uncomment the next line to enable error reporting to cmd window
	//SV_LOG_GLOBAL(info) << traceStream.str();
#endif
}

void SharedMemoryAccess::collect_image_pos(
	std::vector<std::pair<bool, int>>& positions,
	const std::unordered_map<uint32_t, int>& imageMap,
	const std::unordered_map<uint32_t, int>& childImageMap,
	const std::unordered_map<uint32_t, std::pair<bool,int>>& linkedImageMap,
	const ::google::protobuf::RepeatedField<uint32_t>& ids
)
{
#if defined (TRACE_THEM_ALL) || defined (TRACE_SHARED_MEMORY_ACCESS)
	DWORD tick = ::GetTickCount();
	int notfound {0};
#endif
	positions.clear();
	for (const auto& id : ids)
	{
		if (auto iter = imageMap.find(id); imageMap.end() != iter)
		{
			positions.push_back(std::make_pair(false, iter->second));
		}
		else if (auto childIter = childImageMap.find(id); childImageMap.end() != childIter)
		{
			positions.push_back(std::make_pair(true, childIter->second));
		}
		else if (auto linkedIter = linkedImageMap.find(id); linkedImageMap.end() != linkedIter)
		{
			positions.push_back(linkedIter->second);
		}
		else
		{
			positions.push_back(std::make_pair(false, -1));
#if defined (TRACE_THEM_ALL) || defined (TRACE_SHARED_MEMORY_ACCESS)
			std::stringstream traceStream;
			traceStream << "Error collect_image_pos with id " << id;
			OutputDebugString(traceStream.str().c_str());
			//uncomment the next line to enable error reporting to cmd window
			//SV_LOG_GLOBAL(error) << traceStream.str();
			notfound++;
#endif
		}
	}
#if defined (TRACE_THEM_ALL) || defined (TRACE_SHARED_MEMORY_ACCESS)
	tick = ::GetTickCount() - tick;
	std::stringstream traceStream;

	traceStream << "Ticks collect_image_pos: " << tick << " idsSize: " << ids.size() << " ImageMap: " <<
		imageMap.size() << " notfound: " << notfound << std::endl;
	OutputDebugString(traceStream.str().c_str());
	//uncomment the next line to enable error reporting to cmd window
	//SV_LOG_GLOBAL(info) << traceStream.str();
#endif

}

void SharedMemoryAccess::clear_cancelled_product_streams()
{
	for (auto it = m_ProductStreams.begin(); it != m_ProductStreams.end();)
	{
		auto& ptr = *it;
		if (ptr->ctx->isCancelled())
		{
			it = m_ProductStreams.erase(it);
			continue;
		}

		++it;
	}
}

SharedMemoryAccess::notification_stream_t::notification_stream_t(
	const SvPb::GetGatewayNotificationStreamRequest& streamReq,
	SvRpc::Observer<SvPb::GetGatewayNotificationStreamResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
	: observer(observer)
	, ctx(ctx)
{
	this->req.CopyFrom(streamReq);
}

void SharedMemoryAccess::schedule_trigger_record_pause_state()
{
	m_pause_timer.expires_from_now(boost::posix_time::milliseconds(20));
	auto waitFunctor = [this](const boost::system::error_code& rError) { return on_trigger_record_pause_state_timer(rError); };
	m_pause_timer.async_wait(waitFunctor);
}

void SharedMemoryAccess::on_trigger_record_pause_state_timer(const boost::system::error_code& error)
{
	if (error == boost::asio::error::operation_aborted)
	{
		return;
	}
	if (error)
	{
		std::stringstream ss;
		ss << error;
		SvDef::StringVector msgList;
		msgList.push_back(ss.str());
		SV_LOG_GLOBAL(warning) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_Gateway_TRC_PauseStateError, msgList);
		SvStl::MessageManager Exception(SvStl::MsgType::Notify);
		Exception.setMessage(SVMSG_SVGateway_1_GENERAL_WARNING, SvStl::Tid_Gateway_TRC_PauseStateError, msgList, SvStl::SourceFileParams(StdMessageParams));
		return;
	}

	check_trigger_record_pause_state_changed();

	schedule_trigger_record_pause_state();
}

void SharedMemoryAccess::check_trigger_record_pause_state_changed()
{
	auto* pTrc = SvOi::getTriggerRecordControllerRInstance();
	if (nullptr == pTrc || false == pTrc->isValid())
	{
		return;
	}

	int numInspections = pTrc->getInspections().list_size();
	std::vector<bool> newPauseState(numInspections, false);
	for (int i = 0; i < numInspections; ++i)
	{
		newPauseState[i] = pTrc->isPauseTrsOfInterest(i);
	}

	if (newPauseState == m_pause_state)
	{
		return;
	}

	m_pause_state = newPauseState;

	m_io_service.dispatch([this]()
	{
		on_trigger_record_pause_state_changed_impl(m_pause_state);
	});
}

void SharedMemoryAccess::on_trigger_record_pause_state_changed_impl(const std::vector<bool>& pauseState)
{
	for (auto it = m_notification_streams.begin(); it != m_notification_streams.end();)
	{
		if ((*it)->ctx->isCancelled())
		{
			it = m_notification_streams.erase(it);
			continue;
		}

		send_trigger_record_pause_state_to_client(**it, pauseState);

		++it;
	}
}

void SharedMemoryAccess::send_trigger_record_pause_state_to_client(notification_stream_t& client, const std::vector<bool>& pauseStates)
{
	auto* pTrc = SvOi::getTriggerRecordControllerRInstance();
	SvPb::GetGatewayNotificationStreamResponse res;
	if (nullptr != pTrc)
	{
		const auto& rInspections = pTrc->getInspections();

		auto& notification = *res.mutable_rejectstreampaused();
		for (int i = 0; i < static_cast<int> (pauseStates.size()); ++i)
		{
			if (pauseStates[i])
			{
				const auto& inspection = rInspections.list(i);
				notification.add_pausedinspectionids(inspection.id());
			}
		}
	}
	client.observer.onNext(std::move(res));
}

void SharedMemoryAccess::schedule_configuration_lock_status_update()
{
	m_ConfigurationLockStatusTimer.expires_from_now(boost::posix_time::milliseconds(100));
	auto waitFunctor = [this](const boost::system::error_code& rError) { return on_configuration_lock_status_timer(rError); };
	m_ConfigurationLockStatusTimer.async_wait(waitFunctor);
}

void SharedMemoryAccess::on_configuration_lock_status_timer(const boost::system::error_code& error)
{
	if (error)
	{
		SV_LOG_GLOBAL(error) << "Configuration lock status polling error: " << error.message();
		return;
	}

	send_configuration_lock_status_if_changed();
	schedule_configuration_lock_status_update();
}

void SharedMemoryAccess::send_configuration_lock_status_if_changed()
{
	if (m_SharedMemoryLock.ResetIfChanged())
	{
		m_io_service.dispatch([this]()
		{
			send_configuration_lock_status_update_to_clients();
		});
	}
}

void SharedMemoryAccess::send_configuration_lock_status_update_to_clients()
{
	auto lockOwnerStream = m_SharedMemoryLock.GetLockOwnerStream();
	for (auto it = m_notification_streams.begin(); it != m_notification_streams.end();)
	{
		if ((*it)->ctx->isCancelled())
		{
			it = m_notification_streams.erase(it);
			continue;
		}

		send_configuration_lock_status(**it, lockOwnerStream);

		++it;
	}
}

void SharedMemoryAccess::send_configuration_lock_status(notification_stream_t& stream, const std::shared_ptr<lock_acquisition_stream_t>& lockOwnerStream)
{
	SvPb::GetGatewayNotificationStreamResponse response;
	auto notification = response.mutable_configurationlockstatus();
	notification->set_status(SvPb::LockStatus::Unlocked);

	if (lockOwnerStream != nullptr)
	{
		notification->set_status(SvPb::LockStatus::Locked);
		notification->set_owner(m_SharedMemoryLock.GetLockOwner());
		notification->set_description(lockOwnerStream->request.description());
		notification->set_host(lockOwnerStream->sessionContext.host());
	}

	stream.observer.onNext(std::move(response));
}

SharedMemoryAccess::message_stream_t::message_stream_t(
	const SvPb::GetGatewayMessageStreamRequest& streamReq,
	SvRpc::Observer<SvPb::GetGatewayMessageStreamResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
	: severityList(streamReq.severitylist().begin(), streamReq.severitylist().end())
	, observer(observer)
	, ctx(ctx)
{
}

void SharedMemoryAccess::onMessageContainer(const SvStl::MessageContainer& rMessage, int type)
{
	SvPb::MessageType messageType = static_cast<SvPb::MessageType> (type);
	SvPb::MessageContainerVector messagelist;
	if (SvPb::MessageType::endMsgBox == messageType)
	{
		SvStl::MessageContainer message{ rMessage };
		message.clearWhat();
		SvPb::convertMessageToProtobuf(message, messagelist.add_messages());
	}
	else
	{
		SvPb::convertMessageToProtobuf(rMessage, messagelist.add_messages());
	}

	std::lock_guard<std::mutex> lockGuard(m_message_streams_mutex);
	for (auto it = m_message_streams.begin(); it != m_message_streams.end(); )
	{
		auto msgStreamPtr = *it;
		auto& msgStream = *msgStreamPtr;
		if (msgStream.ctx->isCancelled())
		{
			it = m_message_streams.erase(it);
			continue;
		}

		bool isSeverityInList = std::find(
			msgStream.severityList.begin(),
			msgStream.severityList.end(),
			static_cast<int> (rMessage.getSeverity())
		) != msgStream.severityList.end();
		bool allSverities = msgStream.severityList.empty();
		if (allSverities || isSeverityInList)
		{
			SvPb::GetGatewayMessageStreamResponse response;
			*response.mutable_messagelist() = messagelist;
			response.set_type(messageType);
			// TODO are we on the correct thread already? can we post from here?
			msgStream.observer.onNext(std::move(response));
		}
		++it;
	}
}

void SharedMemoryAccess::subscribe_to_trc()
{
	auto* pTrc = SvOi::getTriggerRecordControllerRInstance();
	if (nullptr != pTrc)
	{
		m_TrcReadySubscriptionRAII = pTrc->registerReadyCallback([this]()
		{
			SV_LOG_GLOBAL(debug) << "TRC is ready";
			m_trc_ready = true;
			m_io_service.post([this]()
				{
					rebuild_trc_pos_caches();
				});
		});
		m_TrcResetSubscriptionRAII = pTrc->registerResetCallback([this]()
		{
			SV_LOG_GLOBAL(debug) << "TRC was reset";
			m_trc_ready = false;
		});
		m_TrcNewInterestTrSubscriptionRAII = pTrc->registerNewInterestTrCallback([this](const std::vector<SvOi::TrInterestEventData>& rEvents)
		{
			if (!m_trc_ready)
			{
				return;
			}

			for (const auto& eventData : rEvents)
			{
				on_new_trigger_record(eventData);
			}
		});

		SV_LOG_GLOBAL(debug) << "Successfully registered to TRC callbacks";
	}
	else
	{
		SV_LOG_GLOBAL(warning) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_Gateway_TRC_NotAvailable);
		SvStl::MessageManager Exception(SvStl::MsgType::Notify);
		Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Gateway_TRC_NotAvailable, SvStl::SourceFileParams(StdMessageParams));
	}
}

void SharedMemoryAccess::unsubscribe_from_trc()
{
	m_TrcReadySubscriptionRAII.reset();
	m_TrcResetSubscriptionRAII.reset();
	m_TrcNewInterestTrSubscriptionRAII.reset();
}

int SharedMemoryAccess::get_inspection_pos_for_id(const SvPb::InspectionList& rList, uint32_t id)
{
	for (int i = 0; i < rList.list_size(); ++i)
	{
		const auto& inspection = rList.list(i);
		if (inspection.id() == id)
		{
			return i;
		}
	}
	return -1;
}

void SharedMemoryAccess::GetMyPermissions(const SvAuth::SessionContext& rSessionContext, const SvPb::GetMyPermissionsRequest&, SvRpc::Task<SvPb::GetMyPermissionsResponse> task)
{
	if (rSessionContext.username().empty())
	{
		SvPenv::Error Error;
		Error.set_errorcode(SvPenv::ErrorCode::badRequest);
		task.error(Error);
		return;
	}

	SvPb::GetMyPermissionsResponse Response;
	if (!m_rUserDatabase.getUserPermissions(rSessionContext.username(), *Response.mutable_permissions()))
	{
		SV_LOG_GLOBAL(trace) << "User not found";
		SvPenv::Error Error;
		Error.set_errorcode(SvPenv::ErrorCode::notFound);
		task.error(Error);
		return;
	}

	task.finish(std::move(Response));
}

void SharedMemoryAccess::GetGroupDetails(const SvAuth::SessionContext&, const SvPb::GetGroupDetailsRequest&, SvRpc::Task<SvPb::GetGroupDetailsResponse> task)
{
	SvPb::GetGroupDetailsResponse Response;
	std::set<std::string> groupNames;
	m_rUserDatabase.getGroupNames(groupNames);
	for (const auto& name : groupNames)
	{
		std::set<std::string> users;
		SvPb::Permissions permissions;
		if (m_rUserDatabase.getGroupDetails(name, users, permissions))
		{
			auto& entry = *Response.add_groupdetails();
			entry.set_name(name);
			entry.mutable_permissions()->MergeFrom(permissions);
			for (const auto& user : users)
			{
				entry.add_users(user);
			}
		}
	}

	task.finish(std::move(Response));
}

void SharedMemoryAccess::UpdateGroupPermissions(const SvAuth::SessionContext&, const SvPb::UpdateGroupPermissionsRequest& req, SvRpc::Task<SvPb::UpdateGroupPermissionsResponse> task)
{
	std::map<std::string, SvPb::Permissions> groupPermissions;
	for (const auto& entry : req.grouppermissions())
	{
		groupPermissions.emplace(entry.name(), entry.permissions());
	}
	m_rUserDatabase.updateGroupPermissions(groupPermissions);
	task.finish(SvPb::UpdateGroupPermissionsResponse());
}

void SharedMemoryAccess::AcquireLockStream(
	const SvAuth::SessionContext& sessionContext,
	const SvPb::LockAcquisitionStreamRequest& request,
	SvRpc::Observer<SvPb::LockAcquisitionStreamResponse> observer,
	SvRpc::ServerStreamContext::Ptr streamContext)
{
	auto streamPtr = lock_acquisition_stream_t::create(request, observer, streamContext, sessionContext);

	if (request.scope() != 1U)
	{
		SvPenv::Error error;
		error.set_errorcode(SvPenv::ErrorCode::badRequest);
		error.set_message("Scope has to be always 1 for now.");
		streamPtr->observer.error(error);
		return;
	}

	streamPtr->streamContext->registerOnCancelHandler([this, streamPtr]
	{
		if (streamPtr->isLockOwner)
		{
			release_lock(*streamPtr, SvPb::LockReleaseReason::ExplicitRelease);
		}
	});

	if (m_SharedMemoryLock.GetLockOwner() == streamPtr->sessionContext.username())
	{
		SvPenv::Error error;
		error.set_errorcode(SvPenv::ErrorCode::forbidden);
		error.set_message("Shared memory already locked by the same user.");
		streamPtr->observer.error(error);
		return;
	}

	m_io_service.dispatch([this, streamPtr] { m_SharedMemoryLock.PushBackStream(streamPtr); });
	acquire_lock(*streamPtr);
}

void SharedMemoryAccess::acquire_lock(lock_acquisition_stream_t& stream)
{
	const bool isAcquired = m_SharedMemoryLock.Acquire(
		LockState::LockedBySVOGateway, stream.sessionContext.username());

	if (isAcquired)
	{
		handle_lock_acquisition(stream);
		return;
	}

	const bool isTakeoverRequest = stream.request.requesttakeover();

	if (isTakeoverRequest)
	{
		handle_lock_takeover_request(stream);
		return;
	}

	auto lockOwnerStream = m_SharedMemoryLock.GetLockOwnerStream();

	if (lockOwnerStream == nullptr)
	{
		SvPenv::Error error;
		error.set_errorcode(SvPenv::ErrorCode::notFound);
		error.set_message("Couldn't find current lock owner");
		stream.observer.error(error);
		return;
	}

	SvPb::LockAcquisitionStreamResponse response;

	auto notification = response.mutable_lockalreadylockednotification();
	notification->set_lockid(lockOwnerStream->id);
	notification->set_description(lockOwnerStream->request.description());
	notification->set_user(m_SharedMemoryLock.GetLockOwner());
	notification->set_host(lockOwnerStream->sessionContext.host());

	stream.observer.onNext(std::move(response));
}

void SharedMemoryAccess::handle_lock_acquisition(lock_acquisition_stream_t& stream)
{
	stream.isLockOwner = true;
	notify_client_about_lock_acquisition(stream);
}

void SharedMemoryAccess::notify_client_about_lock_acquisition(const lock_acquisition_stream_t& stream)
{
	SvPb::LockAcquisitionStreamResponse response;

	auto notification = response.mutable_lockacquirednotification();
	notification->set_lockid(stream.id);
	notification->set_description(stream.request.description());
	notification->set_user(m_SharedMemoryLock.GetLockOwner());
	notification->set_host(stream.sessionContext.host());

	stream.observer.onNext(std::move(response));
}

void SharedMemoryAccess::release_lock(lock_acquisition_stream_t& stream, const SvPb::LockReleaseReason reason)
{
	stream.isLockOwner = false;
	m_SharedMemoryLock.Release();
	broadcast_release_notification(stream, reason);
}

void SharedMemoryAccess::broadcast_release_notification(
	const lock_acquisition_stream_t& stream,
	const SvPb::LockReleaseReason reason)
{
	SvPb::LockAcquisitionStreamResponse response;

	auto notification = response.mutable_lockreleasednotification();
	notification->set_lockid(stream.id);
	notification->set_reason(reason);

	for (auto& streamPtr : m_SharedMemoryLock.GetStreams())
	{
		if (!(streamPtr->streamContext->isCancelled()))
		{
			SvPb::LockAcquisitionStreamResponse responseCopy;
			responseCopy.CopyFrom(response);
			streamPtr->observer.onNext(std::move(responseCopy));
		}
	}
}

void SharedMemoryAccess::handle_lock_takeover_request(const lock_acquisition_stream_t& stream)
{
	auto lockOwnerStream = m_SharedMemoryLock.GetLockOwnerStream();
	if (lockOwnerStream == nullptr)
	{
		SvPenv::Error error;
		error.set_errorcode(SvPenv::ErrorCode::notFound);
		error.set_message("Couldn't find current lock owner");
		stream.observer.error(error);

		return;
	}

	SvPb::LockAcquisitionStreamResponse response;

	auto notification = response.mutable_locktakeovernotification();
	notification->set_takeoverid(stream.id);
	notification->set_message(stream.request.takeovermessage());
	notification->set_user(stream.sessionContext.username());
	notification->set_host(stream.sessionContext.host());

	lockOwnerStream->observer.onNext(std::move(response));
}

void SharedMemoryAccess::schedule_disconnected_clients_check()
{
	m_DisconnectCheckTimer.expires_from_now(boost::posix_time::minutes(5));
	m_DisconnectCheckTimer.async_wait(
		boost::bind(&SharedMemoryAccess::on_disconnected_clients_check, this, boost::placeholders::_1));
}

void SharedMemoryAccess::on_disconnected_clients_check(const boost::system::error_code& errorCode)
{
	if (errorCode == boost::asio::error::operation_aborted)
	{
		SV_LOG_GLOBAL(warning) << "on_disconnected_clients_check(): operation aborted!";
		return;
	}
	if (errorCode)
	{
		SV_LOG_GLOBAL(error) << "on_disconnected_clients_check(): error occured during async wait: "
			<< errorCode.message();
		return;
	}

	m_io_service.dispatch([this] { check_disconnected_clients(); });
	schedule_disconnected_clients_check();
}

void SharedMemoryAccess::check_disconnected_clients()
{
	auto& streams = m_SharedMemoryLock.GetStreams();
	for (auto it = streams.begin(); it != streams.end();)
	{
		if ((*it)->streamContext->isCancelled())
		{
			SV_LOG_GLOBAL(debug) << "client " << (*it)->id << " erased from streams vector";
			it = streams.erase(it);
			continue;
		}
		++it;
	}
}

void SharedMemoryAccess::TakeoverLock(
	const SvAuth::SessionContext&,
	const SvPb::LockTakeoverRequest& request,
	SvRpc::Task<SvPb::LockTakeoverResponse> task)
{
	auto lockOwnerStream = m_SharedMemoryLock.GetLockOwnerStream();

	if (lockOwnerStream == nullptr || lockOwnerStream->id != request.lockid())
	{
		SvPenv::Error error;
		error.set_errorcode(SvPenv::ErrorCode::notFound);
		error.set_message("Couldn't find current lock owner");
		task.error(error);

		return;
	}

	auto takeoverCandidateStream = m_SharedMemoryLock.GetStreamById(request.takeoverid());

	if (takeoverCandidateStream == nullptr)
	{
		SvPenv::Error error;
		error.set_errorcode(SvPenv::ErrorCode::notFound);
		error.set_message("Couldn't find takeover candidate!");
		task.error(error);

		return;
	}

	lockOwnerStream->isLockOwner = false;
	m_SharedMemoryLock.Takeover(takeoverCandidateStream->sessionContext.username());
	handle_lock_acquisition(*takeoverCandidateStream);
	SvPb::LockTakeoverResponse response;
	task.finish(std::move(response));
}

void SharedMemoryAccess::RejectLockTakeover(
	const SvAuth::SessionContext&,
	const SvPb::LockTakeoverRejectedRequest& request,
	SvRpc::Task<SvPb::LockTakeoverRejectedResponse> task)
{
	auto takeoverStream = m_SharedMemoryLock.GetStreamById(request.takeoverid());

	if (takeoverStream == nullptr)
	{
		SvPenv::Error error;
		error.set_errorcode(SvPenv::ErrorCode::notFound);
		error.set_message("Couldn't find takeover candidate!");
		task.error(error);

		return;
	}

	SvPb::LockAcquisitionStreamResponse notificationResponse;
	notificationResponse.mutable_locktakeoverrejectednotification();
	takeoverStream->observer.onNext(std::move(notificationResponse));

	SvPb::LockTakeoverRejectedResponse taskResponse;
	task.finish(std::move(taskResponse));
}

bool SharedMemoryAccess::CheckRequestPermissions(const SvAuth::SessionContext& rSessionContext, const SvPenv::Envelope& rEnvelope, SvRpc::Task<SvPenv::Envelope> task)
{
	if (m_skipPermissionChecks)
	{
		return false;
	}

	const auto& username = rSessionContext.username();
	if (username.empty())
	{
		SvPenv::Error Error;
		Error.set_errorcode(SvPenv::ErrorCode::badRequest);
		task.error(Error);
		return true;
	}

	// Always allow any request for sdmAdmin for now and meant as a migration phase only.
	// FIXME: this must be removed before releasing SVObserver 10.10
	if (username == "sdmAdmin")
	{
		return false;
	}

	SvPb::Permissions permissions;
	if (!m_rUserDatabase.getUserPermissions(rSessionContext.username(), permissions))
	{
		SvDef::StringVector msgList;
		msgList.push_back(std::format(_T("{}"), rEnvelope.payloadtype()));
		msgList.push_back(username);
		SV_LOG_GLOBAL(warning) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_Gateway_UserPermissionsNotConfigured, msgList);
		SvStl::MessageManager Exception(SvStl::MsgType::Notify);
		Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Gateway_UserPermissionsNotConfigured, msgList, SvStl::SourceFileParams(StdMessageParams));

		SvPenv::Error Error;
		Error.set_errorcode(SvPenv::ErrorCode::notFound);
		task.error(Error);
		return true;
	}

	if (!is_request_allowed(rEnvelope, permissions))
	{
		SvDef::StringVector msgList;
		msgList.push_back(std::format(_T("{}"), rEnvelope.payloadtype()));
		msgList.push_back(username);
		SV_LOG_GLOBAL(info) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_Gateway_RequestNotAllowed, msgList);
		SvStl::MessageManager Exception(SvStl::MsgType::Notify);
		Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Gateway_RequestNotAllowed, msgList, SvStl::SourceFileParams(StdMessageParams));

		SvPenv::Error Error;
		Error.set_errorcode(SvPenv::ErrorCode::unauthorized);
		task.error(Error);
		return true;
	}

	return false;
}

bool SharedMemoryAccess::CheckStreamPermissions(const SvAuth::SessionContext& rSessionContext, const SvPenv::Envelope& rEnvelope, SvRpc::Observer<SvPenv::Envelope> observer, SvRpc::ServerStreamContext::Ptr)
{
	if (m_skipPermissionChecks)
	{
		return false;
	}

	const auto& username = rSessionContext.username();
	if (username.empty())
	{
		SvPenv::Error Error;
		Error.set_errorcode(SvPenv::ErrorCode::badRequest);
		observer.error(Error);
		return true;
	}

	// Always allow any request for sdmAdmin for now and meant as a migration phase only.
	// FIXME: this must be removed before releasing SVObserver 10.10
	if (username == "sdmAdmin")
	{
		return false;
	}

	SvPb::Permissions permissions;
	if (!m_rUserDatabase.getUserPermissions(rSessionContext.username(), permissions))
	{
		SvDef::StringVector msgList;
		msgList.push_back(std::format(_T("{}"), rEnvelope.payloadtype()));
		msgList.push_back(username);
		SV_LOG_GLOBAL(warning) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_Gateway_UserPermissionsNotConfigured, msgList);
		SvStl::MessageManager Exception(SvStl::MsgType::Notify);
		Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Gateway_UserPermissionsNotConfigured, msgList, SvStl::SourceFileParams(StdMessageParams));

		SvPenv::Error Error;
		Error.set_errorcode(SvPenv::ErrorCode::notFound);
		observer.error(Error);
		return true;
	}

	if (!is_request_allowed(rEnvelope, permissions))
	{
		SvDef::StringVector msgList;
		msgList.push_back(std::format(_T("{}"), rEnvelope.payloadtype()));
		msgList.push_back(username);
		SV_LOG_GLOBAL(info) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_Gateway_RequestNotAllowed, msgList);
		SvStl::MessageManager Exception(SvStl::MsgType::Notify);
		Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Gateway_RequestNotAllowed, msgList, SvStl::SourceFileParams(StdMessageParams));

		SvPenv::Error Error;
		Error.set_errorcode(SvPenv::ErrorCode::unauthorized);
		observer.error(Error);
		return true;
	}

	return false;
}

bool SharedMemoryAccess::is_request_allowed(const SvPenv::Envelope& rEnvelope, const SvPb::Permissions& permissions)
{
	auto payloadType = rEnvelope.payloadtype();
	switch (payloadType)
	{
		// read version information
		case SvPb::SVRCMessages::kGetGatewayVersionRequest:
		case SvPb::SVRCMessages::kGetSVObserverVersionRequest:
		case SvPb::SVRCMessages::kGetWebAppVersionRequest:
			return true; // always allowed

		// read svobserver configuration information
		case SvPb::SVRCMessages::kQueryListNameRequest:
		case SvPb::SVRCMessages::kQueryListItemRequest:
		case SvPb::SVRCMessages::kGetProductRequest:
		case SvPb::SVRCMessages::kGetImageFromIdRequest:
		case SvPb::SVRCMessages::kGetFailStatusRequest:
		case SvPb::SVRCMessages::kGetTriggerItemsRequest:
		case SvPb::SVRCMessages::kGetRejectRequest:
		case SvPb::SVRCMessages::kGetProductStreamRequest:
		case SvPb::SVRCMessages::kGetStateRequest:
		case SvPb::SVRCMessages::kGetConfigRequest:
		case SvPb::SVRCMessages::kGetOfflineCountRequest:
		case SvPb::SVRCMessages::kGetProductFilterRequest:
		case SvPb::SVRCMessages::kGetInspectionsRequest:
		case SvPb::SVRCMessages::kGetProductDataRequest:
		case SvPb::SVRCMessages::kGetInspectionNamesRequest:
		case SvPb::SVRCMessages::kGetMonitorListPropertiesRequest:
		case SvPb::SVRCMessages::kGetMaxRejectDepthRequest:
		case SvPb::SVRCMessages::kGetConfigReportRequest:
		case SvPb::SVRCMessages::kGetDataDefinitionListRequest:
		case SvPb::SVRCMessages::kQueryMonitorListRequest:
		case SvPb::SVRCMessages::kQueryMonitorListNamesRequest:
		case SvPb::SVRCMessages::kGetObjectSelectorItemsRequest:
		case SvPb::SVRCMessages::kGetTriggerStreamRequest:
		case SvPb::SVRCMessages::kGetConfigurationTreeRequest:
		case SvPb::SVRCMessages::kGetConfigurationInfoRequest:
		case SvPb::SVRCMessages::kLockTakeoverRequest:
		case SvPb::SVRCMessages::kLockTakeoverRejectedRequest:
			return permissions.svobserver().configuration().read();

		case SvPb::SVRCMessages::kActivateMonitorListRequest:
		case SvPb::SVRCMessages::kSetProductFilterRequest:
		case SvPb::SVRCMessages::kRegisterMonitorListRequest:
		case SvPb::SVRCMessages::kSetTriggerConfigRequest:
			return permissions.svobserver().configuration().write();

		case SvPb::SVRCMessages::kLoadConfigRequest:
			return permissions.svobserver().configuration().read();

		case SvPb::SVRCMessages::kPutConfigRequest:
			return permissions.svobserver().configuration().write();

			// subscribe to notification streams
		case SvPb::SVRCMessages::kGetGatewayNotificationStreamRequest:
		case SvPb::SVRCMessages::kGetGatewayMessageStreamRequest:
		case SvPb::SVRCMessages::kGetNotificationStreamRequest:
		case SvPb::SVRCMessages::kGetMessageStreamRequest:
		case SvPb::SVRCMessages::kLockAcquisitionStreamRequest:
			return permissions.svobserver().notifications().subscribe();

		case SvPb::SVRCMessages::kGetItemsRequest:
			return permissions.svobserver().value().read();

		case SvPb::SVRCMessages::kSetItemsRequest:
			return permissions.svobserver().value().edit();

		case SvPb::SVRCMessages::kSetRejectStreamPauseStateRequest:
			// TODO what about the read state? it is part of the notification :/
			return permissions.svobserver().inspectionstate().edit();

		case SvPb::SVRCMessages::kGetDeviceModeRequest:
			// TODO there is also a mode change notification. should this be forbidden?
			return permissions.svobserver().mode().read();

		case SvPb::SVRCMessages::kRunOnceRequest: // TODO is this correct?
		case SvPb::SVRCMessages::kSetDeviceModeRequest:
			return permissions.svobserver().mode().edit();

		case SvPb::SVRCMessages::kStoreClientLogsRequest:
			return permissions.svobserver().clientlogs().store(); // TODO shall we really always allow all clients to store any logs?

		case SvPb::SVRCMessages::kGetFileRequest:
			return permissions.svobserver().file().read();

		case SvPb::SVRCMessages::kPutFileRequest:
			return permissions.svobserver().file().write();

		case SvPb::SVRCMessages::kShutdownRequest:
			return permissions.svobserver().machinestate().set();

		case SvPb::SVRCMessages::kInspectionCmdRequest:
		case SvPb::SVRCMessages::kConfigCommandRequest:
			return permissions.svobserver().command().execute();

		case SvPb::SVRCMessages::kGetMyPermissionsRequest:
			return true; // always allowed!

		case SvPb::SVRCMessages::kGetGroupDetailsRequest:
			return permissions.usermanagement().userpermissions().read();

		case SvPb::SVRCMessages::kUpdateGroupPermissionsRequest:
			return permissions.usermanagement().userpermissions().edit();

		default:
			SvDef::StringVector msgList;
			msgList.push_back(std::format(_T("{}"), payloadType));
			SV_LOG_GLOBAL(error) << SvStl::MessageTextGenerator::Instance().getText(SvStl::Tid_Gateway_RequestPermissionsNotConfigured, msgList);
			SvStl::MessageManager Exception(SvStl::MsgType::Notify);
			Exception.setMessage(SVMSG_SVGateway_2_GENERAL_INFORMATIONAL, SvStl::Tid_Gateway_RequestPermissionsNotConfigured, msgList, SvStl::SourceFileParams(StdMessageParams));
			return false;
	}
}

}// namespace SvOgw
