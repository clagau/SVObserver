//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
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
#include "SVLogLibrary/Logging.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVOGateway/WebAppVersionLoader.h"
#include "SVSharedMemoryLibrary/ShareControlSetting.h" 
#include "SVProtoBuf/ConverterHelper.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "TriggerRecordController/ITriggerRecordR.h"
#include "TriggerRecordController/ITriggerRecordControllerR.h"
#include "SVStatusLibrary/MessageTextGenerator.h"
#pragma endregion Includes

namespace SvOgw
{

SharedMemoryAccess::SharedMemoryAccess(boost::asio::io_service& rIoService, const SvSml::ShareControlSettings& ControlParameter, const WebAppVersionLoader& rWebAppVersionLoader, SvRpc::RPCClient& rpcClient)
	: m_io_service(rIoService)
	, m_rWebAppVersionLoader(rWebAppVersionLoader)
	, m_pause_timer(rIoService)
	, m_overlay_controller(rIoService, rpcClient)
	, m_pShareControlInstance{std::make_unique<SvSml::ShareControl>(ControlParameter)}
{
	schedule_trigger_record_pause_state();
	subscribe_to_trc();
}

SharedMemoryAccess::~SharedMemoryAccess()
{
	m_pause_timer.cancel();
	unsubscribe_from_trc();
}

void SharedMemoryAccess::GetVersion(const SvPb::GetGatewayVersionRequest& rRequest, SvRpc::Task<SvPb::GetVersionResponse> task)
{
	SvPb::GetVersionResponse Response;
	Response.set_version(SvUl::to_utf8(SvSyl::SVVersionInfo::GetShortTitleVersion()));
	task.finish(std::move(Response));
}

void SharedMemoryAccess::GetWebAppVersion(const SvPb::GetWebAppVersionRequest& rRequest, SvRpc::Task<SvPb::GetVersionResponse> task)
{
	SvPb::GetVersionResponse Response;
	Response.set_version(m_rWebAppVersionLoader.getVersion());
	task.finish(std::move(Response));
}

void SharedMemoryAccess::GetInspections(const SvPb::GetInspectionsRequest& rRequest, SvRpc::Task<SvPb::GetInspectionsResponse> task)
{
	SvPenv::Error Error;
	SvPb::GetInspectionsResponse Response;

	auto& trc = SvTrc::getTriggerRecordControllerRInstance();
	const auto& inspections = trc.getInspections();
	for (int i = 0; i < inspections.list_size(); ++i)
	{
		const auto& inspection = inspections.list(i);
		auto& entry = *Response.add_inspections();
		entry.set_id(inspection.id());
		entry.set_name(inspection.name());
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
	auto& trc = SvTrc::getTriggerRecordControllerRInstance();
	google::protobuf::uint32 inspectionId = rRequest.inspectionid();
	if (inspectionId == 0)
	{
		trc.pauseTrsOfInterest(rRequest.pause());
	}
	else
	{
		const auto inspectionPos = get_inspection_pos_for_id(trc, inspectionId);
		if (inspectionPos < 0)
		{
			SvPenv::Error err;
			err.set_errorcode(SvPenv::ErrorCode::notFound);
			err.set_message("Unknown inspection id");
			task.error(err);
			return;
		}

		trc.pauseTrsOfInterest(rRequest.pause(), inspectionPos);
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
	, interestedInRejects(req.rejectsonly() || req.rejectimageids_size() > 0 || req.rejectvalueids_size() > 0)
{
	this->req.CopyFrom(streamReq);
}

static bool is_subscribed_to(const SvPb::GetProductStreamRequest& req, const SvPb::Inspection& inspection)
{
	return req.inspectionid() == inspection.id();
}

void SharedMemoryAccess::on_new_trigger_record(const SvTrc::TrInterestEventData& new_trigger)
{
	add_new_trigger_to_queue(new_trigger);

	m_io_service.post([this]()
	{
		check_queue_for_new_trigger_record();
	});
}

void SharedMemoryAccess::add_new_trigger_to_queue(const SvTrc::TrInterestEventData& new_trigger)
{
	std::lock_guard<std::mutex> l(m_NewTriggerMutex);
	m_NewTriggerQueue.push_back(std::move(new_trigger));
}

void SharedMemoryAccess::pop_new_trigger_from_queue(std::vector<SvTrc::TrInterestEventData>& result)
{
	std::lock_guard<std::mutex> l(m_NewTriggerMutex);
	result = m_NewTriggerQueue;
	m_NewTriggerQueue.clear();
}

void SharedMemoryAccess::check_queue_for_new_trigger_record()
{
	std::vector<SvTrc::TrInterestEventData> new_triggers;
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

void SharedMemoryAccess::add_new_triggers_to_product_stream(product_stream_t& product_stream, const std::vector<SvTrc::TrInterestEventData>& new_triggers)
{
	auto& trc = SvTrc::getTriggerRecordControllerRInstance();
	const auto& inspections = trc.getInspections();

	for (const auto& new_trigger : new_triggers)
	{
		const auto& inspection = inspections.list(new_trigger.m_inspectionPos);
		if (is_subscribed_to(product_stream.req, inspection) && !(!new_trigger.m_isInterest && product_stream.interestedInRejects))
		{
			product_stream.newTriggerQueue.push_back(new_trigger);
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

	auto isRejectFn = [](const SvTrc::TrInterestEventData& rEventData) -> bool {return rEventData.m_isInterest; };

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
	auto& trc = SvTrc::getTriggerRecordControllerRInstance();
	const auto& inspections = trc.getInspections();

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

		auto tro = trc.createTriggerRecordObject(inspectionPos, trId);
		if (!tro)
		{
			SV_LOG_GLOBAL(warning) << "TRC returned invalid TRO for inspectionPos=" << inspectionPos << " and trId=" << trId;
			continue;
		}

		// while historical products are still pending, new ones make one historical less
		// TODO better place for this code?
		if (!ptr->historicalTriggerQueue.empty())
		{
			ptr->historicalTriggerQueue.pop_back();
		}

		const auto& inspection = inspections.list(inspectionPos);
		
		handle_new_trigger_record(ptr, tro, inspectionPos, inspection.id(), trId, isReject);
	}
}

static bool read_image(SvPb::Image& resImg, SvTrc::IImagePtr imgPtr)
{
	auto hdl = imgPtr->getHandle();
	auto& info = hdl->GetBitmapInfo();
	auto& buffer = hdl->GetBuffer();
	auto& out = *resImg.mutable_rgbdata();
	auto rc = SVMatroxBufferInterface::CopyBufferToFileDIB(out, info, buffer);
	// height can be negative to indicate that the image origin is the upper-left corner
	// See https://docs.microsoft.com/en-us/previous-versions/dd183376(v=vs.85)
	resImg.set_height(std::labs(info.GetHeight()));
	resImg.set_width(info.GetWidth());
	return rc == S_OK;
}

static bool read_variant(SvPb::Variant& resVar, const _variant_t& variant)
{
	return SvPb::ConvertVariantToProtobuf(variant, &resVar) == S_OK;
}

SvSyl::SVFuture<void> SharedMemoryAccess::handle_new_trigger_record(std::shared_ptr<product_stream_t> stream, SvTrc::ITriggerRecordRPtr pTro, int inspectionPos, uint32_t inspectionId, int trId, bool is_reject)
{
	auto res = std::make_shared<SvPb::GetProductStreamResponse>();

	res->set_trigger(trId);
	res->set_isreject(is_reject);

	std::vector<SvSyl::SVFuture<void>> imageFutures;
	const auto includeoverlays = stream->req.includeoverlays();
	{ // collect images for live-data stream
		auto future = collect_images(
			*res->mutable_images(),
			*res->mutable_overlays(),
			pTro,
			stream->req.imageids(),
			stream->imagePositions,
			inspectionPos,
			inspectionId,
			includeoverlays);
		imageFutures.emplace_back(std::move(future));
	}
	{ // collect reject images
		auto future = collect_images(
			*res->mutable_rejectimages(),
			*res->mutable_rejectoverlays(),
			pTro,
			stream->req.rejectimageids(),
			stream->rejectImagePositions,
			inspectionPos,
			inspectionId,
			includeoverlays);
		imageFutures.emplace_back(std::move(future));
	}

	collect_values(*res->mutable_values(), *pTro, stream->req.valueids(), stream->valuePositions);
	collect_values(*res->mutable_rejectvalues(), *pTro, stream->req.rejectvalueids(), stream->rejectValuePositions);

	auto promise = std::make_shared<SvSyl::SVPromise<void>>();
	SvSyl::SVPromise<void>::all(m_io_service, imageFutures).then(m_io_service, [this, stream, res, promise](SvSyl::SVFuture<void> future)
	{
		if (stream->ctx->isCancelled())
		{
			SvPenv::Error err;
			err.set_errorcode(SvPenv::ErrorCode::internalError);
			err.set_message("Connection lost");
			promise->set_exception(SvRpc::errorToExceptionPtr(err));
			return;
		}

		stream->observer.onNext(std::move(*res)).then(m_io_service, [promise](SvSyl::SVFuture<void> future)
		{
			promise->set_value();
		});
	});

	return promise->get_future();
}

SvSyl::SVFuture<void> SharedMemoryAccess::get_product_data(SvPb::GetProductDataResponse& res, const SvPb::GetProductDataRequest& req)
{
	auto& trc = SvTrc::getTriggerRecordControllerRInstance();
	const auto inspectionId = req.inspectionid();
	const auto inspectionPos = get_inspection_pos_for_id(trc, inspectionId);
	if (inspectionPos < 0)
	{
		// TODO;
		return SvSyl::SVPromise<void>::make_ready();
	}

	auto trId = req.trigger();
	if (trId <= 0)
	{
		trId = trc.getLastTrId(inspectionPos);
	}

	auto tro = trc.createTriggerRecordObject(inspectionPos, trId);
	if (!tro)
	{
		// TODO;
		return SvSyl::SVPromise<void>::make_ready();
	}

	std::vector<std::pair<bool, int>> imagePositions;
	std::vector<int> valuePositions;
	try
	{
		collect_image_pos(imagePositions, trc.getImageDefMap(inspectionPos), trc.getChildImageDefMap(inspectionPos), req.imageids());
		collect_value_pos(valuePositions, trc.getDataDefMap(inspectionPos), req.valueids());
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
	SvTrc::ITriggerRecordRPtr pTro,
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
			continue;
		}

		SvTrc::IImagePtr imgPtr = nullptr;
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
			SV_LOG_GLOBAL(warning) << "Error reading image with id " << imageId << " for " << inspectionPos;
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
	SvTrc::ITriggerRecordR& tro,
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
			SV_LOG_GLOBAL(warning) << "Error reading value with id " << valueId;
		}
	}
}

void SharedMemoryAccess::collect_historical_triggers(product_stream_t& stream)
{
	auto& trc = SvTrc::getTriggerRecordControllerRInstance();

	int startAtTriggerCount = stream.req.startattriggercount();
	int inspectionPos = get_inspection_pos_for_id(trc, stream.req.inspectionid());
	if (startAtTriggerCount == 0 || inspectionPos < 0)
	{
		return;
	}

	if (startAtTriggerCount > 0)
	{
		int lastTrId = trc.getLastTrId(inspectionPos);
		if (startAtTriggerCount > lastTrId)
		{
			return;
		}

		if (stream.req.rejectsonly())
		{
			int numHistoricalTrigger = lastTrId - startAtTriggerCount + 1;
			auto trList = trc.getTrsOfInterest(inspectionPos, numHistoricalTrigger);
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
		if (stream.req.rejectsonly())
		{
			auto trList = trc.getTrsOfInterest(inspectionPos, numHistoricalTrigger);
			for (const auto& tr : trList)
			{
				stream.historicalTriggerQueue.emplace_back(inspectionPos, tr->getId(), false);
			}
		}
		else
		{
			int lastTrId = trc.getLastTrId(inspectionPos);
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
	auto triggersOfInterest = trc.getTrsOfInterest(inspectionPos, static_cast<int>(stream.historicalTriggerQueue.size()));
	for (auto& rEntry : stream.historicalTriggerQueue)
	{
		rEntry.m_isInterest = std::any_of(triggersOfInterest.cbegin(), triggersOfInterest.cend(), [&rEntry](const auto& tr) { return tr->getId() == rEntry.m_trId; });
	}
}

void SharedMemoryAccess::schedule_historical_triggers(std::shared_ptr<product_stream_t> stream)
{
	if (stream->historicalTriggerQueue.empty() || stream->ctx->isCancelled())
	{
		return;
	}

	const SvTrc::TrInterestEventData new_trigger = stream->historicalTriggerQueue.front();
	stream->historicalTriggerQueue.erase(stream->historicalTriggerQueue.begin());

	auto& trc = SvTrc::getTriggerRecordControllerRInstance();
	auto triggerRecord = trc.createTriggerRecordObject(new_trigger.m_inspectionPos, new_trigger.m_trId);
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
	if (!m_trc_ready)
	{
		return;
	}

	try
	{
		auto& trc = SvTrc::getTriggerRecordControllerRInstance();
		int inspectionPos = get_inspection_pos_for_id(trc, stream.req.inspectionid());
		if (inspectionPos >= 0)
		{
			collect_value_pos(stream.valuePositions, trc.getDataDefMap(inspectionPos), stream.req.valueids());
			collect_value_pos(stream.rejectValuePositions, trc.getDataDefMap(inspectionPos), stream.req.rejectvalueids());
			collect_image_pos(stream.imagePositions, trc.getImageDefMap(inspectionPos), trc.getChildImageDefMap(inspectionPos), stream.req.imageids());
			collect_image_pos(stream.rejectImagePositions, trc.getImageDefMap(inspectionPos), trc.getChildImageDefMap(inspectionPos), stream.req.rejectimageids());
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
		auto iter = imageMap.find(id);
		auto childIter = childImageMap.find(id);
		if (imageMap.end() != iter)
		{
			positions.push_back(std::make_pair(false, iter->second));
		}
		else if (childImageMap.end() != childIter)
		{
			positions.push_back(std::make_pair(true, childIter->second));
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
	m_pause_timer.async_wait(std::bind(&SharedMemoryAccess::on_trigger_record_pause_state_timer, this, std::placeholders::_1));
}

void SharedMemoryAccess::on_trigger_record_pause_state_timer(const boost::system::error_code& error)
{
	if (error == boost::asio::error::operation_aborted)
	{
		return;
	}
	if (error)
	{
		SV_LOG_GLOBAL(error) << "TRC pause state polling error: " << error;
		return;
	}

	check_trigger_record_pause_state_changed();

	schedule_trigger_record_pause_state();
}

void SharedMemoryAccess::check_trigger_record_pause_state_changed()
{
	auto& trc = SvTrc::getTriggerRecordControllerRInstance();
	if (!trc.isValid())
	{
		return;
	}

	int numInspections = trc.getInspections().list_size();
	std::vector<bool> newPauseState(numInspections, false);
	for (int i = 0; i < numInspections; ++i)
	{
		newPauseState[i] = trc.isPauseTrsOfInterest(i);
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
	auto& trc = SvTrc::getTriggerRecordControllerRInstance();
	const auto& inspections = trc.getInspections();

	SvPb::GetGatewayNotificationStreamResponse res;
	auto& notification = *res.mutable_rejectstreampaused();
	for (auto i = 0ull; i < pauseStates.size(); ++i)
	{
		if (pauseStates[i])
		{
			const auto& inspection = inspections.list(static_cast<int>(i));
			notification.add_pausedinspectionids(inspection.id());
		}
	}
	client.observer.onNext(std::move(res));
}

void SharedMemoryAccess::subscribe_to_trc()
{
	auto& trc = SvTrc::getTriggerRecordControllerRInstance();
	m_TrcReadySubscriptionId = trc.registerReadyCallback([this]()
	{
		m_trc_ready = true;
		m_io_service.post([this]()
		{
			rebuild_trc_pos_caches();
		});
	});
	m_TrcResetSubscriptionId = trc.registerResetCallback([this]()
	{
		m_trc_ready = false;
	});
	m_TrcNewInterestTrSubscriptionId = trc.registerNewInterestTrCallback([this](const std::vector<SvTrc::TrInterestEventData>& rEvents)
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
}

void SharedMemoryAccess::unsubscribe_from_trc()
{
	auto& trc = SvTrc::getTriggerRecordControllerRInstance();
	trc.unregisterReadyCallback(m_TrcReadySubscriptionId);
	trc.unregisterResetCallback(m_TrcResetSubscriptionId);
	trc.unregisterNewInterestTrCallback(m_TrcNewInterestTrSubscriptionId);
}

int SharedMemoryAccess::get_inspection_pos_for_id(SvTrc::ITriggerRecordControllerR& trc, uint32_t id)
{
	const auto& inspections = trc.getInspections();
	for (int i = 0; i < inspections.list_size(); ++i)
	{
		const auto& inspection = inspections.list(i);
		if (inspection.id() == id)
		{
			return i;
		}
	}
	return -1;
}

}// namespace SvOgw
