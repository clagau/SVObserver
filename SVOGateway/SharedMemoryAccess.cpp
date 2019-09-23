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
#include "TriggerRecordController/TriggerRecord.h"
#include "TriggerRecordController/TriggerRecordController.h"
#pragma endregion Includes

namespace SvOgw
{

SharedMemoryAccess::SharedMemoryAccess(boost::asio::io_service& rIoService, const SvSml::ShareControlSettings& ControlParameter, const WebAppVersionLoader& rWebAppVersionLoader, SvRpc::RPCClient& rpcClient)
	: m_io_service(rIoService)
	, m_rWebAppVersionLoader(rWebAppVersionLoader)
	, m_pause_timer(rIoService)
	, m_overlay_controller(rIoService, rpcClient)
{
	m_pShareControlInstance = std::make_unique<SvSml::ShareControl>(ControlParameter);
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
	Response.set_version(SvUl::to_utf8(SvSyl::SVVersionInfo::GetVersion()));
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

	auto& trc = SvTrc::getTriggerRecordControllerInstance();
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
	auto& trc = SvTrc::getTriggerRecordControllerInstance();
	trc.pauseTrsOfInterest(rRequest.pause());
	check_trigger_record_pause_state_changed();

	SvPb::EmptyResponse Response;
	task.finish(std::move(Response));
}

void SharedMemoryAccess::GetGatewayNotificationStream(
	const SvPb::GetGatewayNotificationStreamRequest& rRequest,
	SvRpc::Observer<SvPb::GetGatewayNotificationStreamResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
{
	auto state = std::make_shared<notification_stream_t>(rRequest, observer, ctx);
	m_notification_streams.push_back(state);

	// send initial pause state to client
	auto& trc = SvTrc::getTriggerRecordControllerInstance();
	send_trigger_record_pause_state_to_client(*state, trc.isPauseTrsOfInterest());
}

void SharedMemoryAccess::GetProductStream(
	const SvPb::GetProductStreamRequest& rRequest,
	SvRpc::Observer<SvPb::GetProductStreamResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
{
	// TODO handle startAtTriggerCount
	// TODO mutex protection for stream lists
	auto state = std::make_shared<product_stream_t>(rRequest, observer, ctx);
	ctx->enableThrottling();
	ctx->setMaxPendingAcks(rRequest.rejectsonly() ? 5 : 0);
	m_ProductStreams.push_back(state);
	rebuild_trc_pos_cache(*state);
}

SharedMemoryAccess::new_trigger_t::new_trigger_t(int inspectionPos, int trId, bool isReject)
	: inspectionPos(inspectionPos)
	, trId(trId)
	, isReject(isReject)
{
}

SharedMemoryAccess::product_stream_t::product_stream_t(
	const SvPb::GetProductStreamRequest& streamReq,
	SvRpc::Observer<SvPb::GetProductStreamResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
	: observer(observer)
	, ctx(ctx)
{
	this->req.CopyFrom(streamReq);
}

static bool is_guid_equal(const GUID& guid1, const GUID& guid2)
{
	return IsEqualGUID(guid1, guid2) != 0;
}

static bool is_guid_equal(const std::string& guid1, const std::string& guid2)
{
	return is_guid_equal(SvPb::GetGuidFromString(guid1), SvPb::GetGuidFromString(guid2));
}

static bool is_guid_equal(const GUID& guid1, const std::string& guid2)
{
	return is_guid_equal(guid1, SvPb::GetGuidFromString(guid2));
}

static bool is_guid_equal(const std::string& guid1, const GUID& guid2)
{
	return is_guid_equal(SvPb::GetGuidFromString(guid1), guid2);
}

static bool is_subscribed_to(const SvPb::GetProductStreamRequest& req, const SvPb::Inspection& inspection)
{
	return is_guid_equal(req.inspectionid(), inspection.id());
}

void SharedMemoryAccess::on_new_trigger_record(int inspectionPos, int trId, bool is_reject)
{
	add_new_trigger_to_queue(std::make_shared<new_trigger_t>(inspectionPos, trId, is_reject));

	m_io_service.post([this, is_reject]()
	{
		check_queue_for_new_trigger_record(is_reject);
	});
}

void SharedMemoryAccess::add_new_trigger_to_queue(std::shared_ptr<new_trigger_t> new_trigger)
{
	std::lock_guard<std::mutex> l(m_NewTriggerMutex);
	auto& queue = new_trigger->isReject ? m_NewTriggerInterestQueue : m_NewTriggerQueue;
	queue.push_back(new_trigger);
}

std::shared_ptr<SharedMemoryAccess::new_trigger_t> SharedMemoryAccess::pop_new_trigger_from_queue(bool is_reject)
{
	std::lock_guard<std::mutex> l(m_NewTriggerMutex);

	auto& queue = is_reject ? m_NewTriggerInterestQueue : m_NewTriggerQueue;
	if (queue.empty())
	{
		return nullptr;
	}

	// TODO allow processing more than one entry of the reject queue!
	// TODO print warning when more than one entry in reject queue? this means we are dropping rejects here

	auto new_trigger = queue.back();
	queue.clear();

	return new_trigger;
}

void SharedMemoryAccess::check_queue_for_new_trigger_record(bool is_reject)
{
	auto new_trigger_ptr = pop_new_trigger_from_queue(is_reject);
	if (!new_trigger_ptr || m_ProductStreams.empty())
	{
		return;
	}

	const auto& new_trigger = *new_trigger_ptr;

	auto& trc = SvTrc::getTriggerRecordControllerInstance();
	auto tro = trc.createTriggerRecordObject(new_trigger.inspectionPos, new_trigger.trId);
	if (!tro)
	{
		SV_LOG_GLOBAL(warning) << "TRC returned invalid TRO for inspectionPos=" << new_trigger.inspectionPos << " and trId=" << new_trigger.trId;
		return;
	}

	const auto& inspections = trc.getInspections();
	const auto& inspection = inspections.list(new_trigger.inspectionPos);
	const auto inspectionId = SvPb::GetGuidFromString(inspection.id());

	for (auto it = m_ProductStreams.begin(); it != m_ProductStreams.end();)
	{
		auto& ptr = *it;
		if (ptr->ctx->isCancelled())
		{
			it = m_ProductStreams.erase(it);
			continue;
		}

		if (is_subscribed_to(ptr->req, inspection) && is_reject == ptr->req.rejectsonly())
		{
			handle_new_trigger_record(ptr, tro, new_trigger.inspectionPos, inspectionId, new_trigger.trId, is_reject);
		}

		++it;
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

void SharedMemoryAccess::handle_new_trigger_record(std::shared_ptr<product_stream_t> stream, SvTrc::ITriggerRecordRPtr pTro, int inspectionPos, GUID inspectionId, int trId, bool is_reject)
{
	auto res = std::make_shared<SvPb::GetProductStreamResponse>();

	res->set_trigger(trId);
	res->set_isreject(is_reject);

	const auto includeoverlays = stream->req.includeoverlays();
	auto future = collect_images(*res->mutable_images(), *res->mutable_overlays(), pTro, stream->req.imageids(), stream->imagePositions, inspectionPos, inspectionId, includeoverlays);
	collect_values(*res->mutable_values(), *pTro, stream->req.valueids(), stream->valuePositions);

	future.then(m_io_service, [stream, res](SvSyl::SVFuture<void> future)
	{
		if (!stream->ctx->isCancelled())
		{
			stream->observer.onNext(std::move(*res));
		}
	});
}

SvSyl::SVFuture<void> SharedMemoryAccess::get_product_data(SvPb::GetProductDataResponse& res, const SvPb::GetProductDataRequest& req)
{
	auto& trc = SvTrc::getTriggerRecordControllerInstance();
	const auto& inspectionIdStr = req.inspectionid();
	const auto inspectionId = SvPb::GetGuidFromString(inspectionIdStr);
	const auto inspectionPos = get_inspection_pos_for_guid(trc, inspectionIdStr);
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

	std::vector<int> imagePositions;
	collect_image_pos(imagePositions, trc.getImageDefList(inspectionPos), req.imageids());

	std::vector<int> valuePositions;
	collect_value_pos(valuePositions, trc.getDataDefList(inspectionPos), req.valueids());

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
	const ::google::protobuf::RepeatedPtrField<std::string>& imageGuids,
	const std::vector<int>& imagePositions,
	int inspectionPos,
	GUID inspectionId,
	bool includeoverlays
)
{
	std::vector<SvSyl::SVFuture<void>> overlay_futures;

	for (auto i = 0ULL, len = imagePositions.size(); i < len; ++i)
	{
		const auto imagePos = imagePositions[i];
		const auto imageGiud = imageGuids.Get(static_cast<int>(i));
		const auto guid = SvPb::GetGuidFromString(imageGiud);

		auto& img = *images.Add();
		auto overlayDesc = static_cast<SvPb::OverlayDesc*>(nullptr);
		if (includeoverlays)
		{
			overlayDesc = overlays.Add();
		}

		SvTrc::IImagePtr imgPtr = (nullptr != pTro) ? pTro->getImage(imagePos) : nullptr;
		if (!imgPtr)
		{
			SV_LOG_GLOBAL(debug) << "  > image " << SvPb::PrettyPrintGuid(guid) << " not found";
			continue;
		}

		// TODO handle image format
		//stream.req.imageformat()
		if (!read_image(img, imgPtr))
		{
			SV_LOG_GLOBAL(warning) << "Error reading image with guid " << SvPb::PrettyPrintGuid(guid) << " for " << inspectionPos;
			continue;
		}

		if (includeoverlays && overlayDesc)
		{
			auto future = m_overlay_controller.getOverlays(pTro, inspectionId, guid)
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
	const ::google::protobuf::RepeatedPtrField<std::string>& valueGuids,
	const std::vector<int>& valuePositions
)
{
	for (auto i = 0ULL, len = valuePositions.size(); i < len; ++i)
	{
		auto& value = *values.Add();
		auto variant = tro.getDataValue(valuePositions[i]);
		if (!read_variant(value, variant))
		{
			const auto valueGuid = valueGuids.Get(static_cast<int>(i));
			const auto guid = SvPb::GetGuidFromString(valueGuid);
			const auto prettyGuid = SvPb::PrettyPrintGuid(guid);
			SV_LOG_GLOBAL(warning) << "Error reading value with guid " << prettyGuid;
		}
	}
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

	auto& trc = SvTrc::getTriggerRecordControllerInstance();
	int inspectionPos = get_inspection_pos_for_guid(trc, stream.req.inspectionid());
	if (inspectionPos >= 0)
	{
		collect_value_pos(stream.valuePositions, trc.getDataDefList(inspectionPos), stream.req.valueids());
		collect_image_pos(stream.imagePositions, trc.getImageDefList(inspectionPos), stream.req.imageids());
	}
}

void SharedMemoryAccess::collect_value_pos(std::vector<int>& positions, const SvPb::DataDefinitionList& dataDefList, const ::google::protobuf::RepeatedPtrField<std::string>& guids)
{
	positions.clear();
	for (const auto& guid : guids)
	{
		const auto pos = SvTrc::findGuidPos(dataDefList.list(), guid);
		positions.push_back(pos);
	}
}

void SharedMemoryAccess::collect_image_pos(std::vector<int>& positions, const SvPb::ImageList& imageList, const ::google::protobuf::RepeatedPtrField<std::string>& guids)
{
	positions.clear();
	for (const auto& guid : guids)
	{
		const auto pos = SvTrc::findGuidPos(imageList.list(), guid);
		positions.push_back(pos);
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
	auto& trc = SvTrc::getTriggerRecordControllerInstance();
	if (!trc.isValid())
	{
		return;
	}

	const auto newState = trc.isPauseTrsOfInterest();
	const auto prevState = m_pause_state.exchange(newState);
	if (prevState == newState)
	{
		return;
	}

	m_io_service.dispatch([this, newState]()
	{
		on_trigger_record_pause_state_changed_impl(newState);
	});
}

void SharedMemoryAccess::on_trigger_record_pause_state_changed_impl(bool paused)
{
	for (auto it = m_notification_streams.begin(); it != m_notification_streams.end();)
	{
		if ((*it)->ctx->isCancelled())
		{
			it = m_notification_streams.erase(it);
			continue;
		}

		send_trigger_record_pause_state_to_client(**it, paused);

		++it;
	}
}

void SharedMemoryAccess::send_trigger_record_pause_state_to_client(notification_stream_t& client, bool paused)
{
	SvPb::GetGatewayNotificationStreamResponse res;
	auto& notification = *res.mutable_rejectstreampaused();
	notification.set_paused(paused);
	client.observer.onNext(std::move(res));
}

void SharedMemoryAccess::subscribe_to_trc()
{
	auto& trc = SvTrc::getTriggerRecordControllerInstance();
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
	m_TrcNewTrSubscriptionId = trc.registerNewTrCallback([this](SvTrc::TrEventData eventData)
	{
		if (!m_trc_ready)
		{
			return;
		}

		on_new_trigger_record(eventData.m_inspectionPos, eventData.m_trId, false);
	});
	m_TrcNewInterestTrSubscriptionId = trc.registerNewInterestTrCallback([this](std::vector<SvTrc::TrEventData> events)
	{
		if (!m_trc_ready)
		{
			return;
		}

		for (const auto& eventData : events)
		{
			on_new_trigger_record(eventData.m_inspectionPos, eventData.m_trId, true);
		}
	});
}

void SharedMemoryAccess::unsubscribe_from_trc()
{
	auto& trc = SvTrc::getTriggerRecordControllerInstance();
	trc.unregisterReadyCallback(m_TrcReadySubscriptionId);
	trc.unregisterResetCallback(m_TrcResetSubscriptionId);
	trc.unregisterNewTrCallback(m_TrcNewTrSubscriptionId);
	trc.unregisterNewInterestTrCallback(m_TrcNewInterestTrSubscriptionId);
}

int SharedMemoryAccess::get_inspection_pos_for_guid(SvTrc::TriggerRecordController& trc, const std::string& guid)
{
	const auto& inspections = trc.getInspections();
	for (int i = 0; i < inspections.list_size(); ++i)
	{
		const auto& inspection = inspections.list(i);
		if (guid.compare(inspection.id()) == 0)
		{
			return i;
		}
	}
	return -1;
}

}// namespace SvOgw
