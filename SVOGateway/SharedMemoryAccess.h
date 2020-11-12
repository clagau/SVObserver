//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SharedMemoryAccess.h
/// All Rights Reserved
//******************************************************************************
/// Class for read-only access to SVObserver's shared memory.
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <boost/asio/io_service.hpp>
//Moved to precompiled header: #include <boost/thread.hpp>

#include "SharedMemoryAccessInterface.h"
#include "SVOverlayLibrary/OverlayController.h"
#include "SVProtobuf/SVRC.h"
#include "SVSharedMemoryLibrary/ShareControl.h"
#include "SVRPCLibrary/Task.h"
#include "ObjectInterfaces/ITriggerRecordControllerR.h"
#pragma endregion Includes

namespace SvSml
{
struct ShareControlSettings;
}
namespace SvOi
{
class ITriggerRecordR;
class ITriggerRecordControllerR;
}

namespace SvRpc
{
class RPCClient;
}

namespace SvOgw
{
class WebAppVersionLoader;

class SharedMemoryAccess : public SharedMemoryAccessInterface
{
public:
	SharedMemoryAccess(boost::asio::io_service& rIoService, const SvSml::ShareControlSettings& ControlParameter, const WebAppVersionLoader&, SvRpc::RPCClient& rpcClient);
	virtual ~SharedMemoryAccess();

public:
	void GetVersion(const SvPb::GetGatewayVersionRequest&, SvRpc::Task<SvPb::GetVersionResponse>) override;
	void GetWebAppVersion(const SvPb::GetWebAppVersionRequest&, SvRpc::Task<SvPb::GetVersionResponse>) override;
	void GetInspections(const SvPb::GetInspectionsRequest&, SvRpc::Task<SvPb::GetInspectionsResponse>) override;
	void GetProduct(const SvPb::GetProductRequest&, SvRpc::Task<SvPb::GetProductResponse>) override;
	void GetProductData(const SvPb::GetProductDataRequest&, SvRpc::Task<SvPb::GetProductDataResponse>) override;
	void GetReject(const SvPb::GetRejectRequest&, SvRpc::Task<SvPb::GetRejectResponse>) override;
	void GetFailstatus(const SvPb::GetFailStatusRequest&, SvRpc::Task<SvPb::GetFailStatusResponse>) override;
	void GetImageFromId(const SvPb::GetImageFromIdRequest&, SvRpc::Task<SvPb::GetImageFromIdResponse>) override;
	void GetTriggerItems(const SvPb::GetTriggerItemsRequest&, SvRpc::Task<SvPb::GetTriggerItemsResponse>) override;
	void QueryListName(const SvPb::QueryListNameRequest&, SvRpc::Task<SvPb::QueryListNameResponse>) override;
	void QueryListItem(const SvPb::QueryListItemRequest&, SvRpc::Task<SvPb::QueryListItemResponse>) override;
	void StoreClientLogs(const SvPb::StoreClientLogsRequest&, SvRpc::Task<SvPb::EmptyResponse>) override;
	void SetRejectStreamPauseState(const SvPb::SetRejectStreamPauseStateRequest&, SvRpc::Task<SvPb::EmptyResponse>) override;
	void GetGatewayNotificationStream(const SvPb::GetGatewayNotificationStreamRequest&, SvRpc::Observer<SvPb::GetGatewayNotificationStreamResponse>, SvRpc::ServerStreamContext::Ptr) override;
	void GetProductStream(const SvPb::GetProductStreamRequest&, SvRpc::Observer<SvPb::GetProductStreamResponse>, SvRpc::ServerStreamContext::Ptr) override;

private:
	struct product_stream_t
	{
		product_stream_t(const SvPb::GetProductStreamRequest&, SvRpc::Observer<SvPb::GetProductStreamResponse>, SvRpc::ServerStreamContext::Ptr);
		SvPb::GetProductStreamRequest req;
		SvRpc::Observer<SvPb::GetProductStreamResponse> observer;
		std::vector<SvOi::TrInterestEventData> historicalTriggerQueue;
		std::vector<SvOi::TrInterestEventData> newTriggerQueue;
		SvRpc::ServerStreamContext::Ptr ctx;
		std::vector<int> valuePositions;
		std::vector<int> rejectValuePositions;
		std::vector<std::pair<bool, int>> imagePositions;
		std::vector<std::pair<bool, int>> rejectImagePositions;
		bool interestedInAllProducts;
		bool interestedInRejects;
	};
	void on_new_trigger_record(const SvOi::TrInterestEventData&);
	void add_new_trigger_to_queue(const SvOi::TrInterestEventData&);
	void pop_new_trigger_from_queue(std::vector<SvOi::TrInterestEventData>&);
	void check_queue_for_new_trigger_record();
	void add_new_triggers_to_product_stream(product_stream_t&, const std::vector<SvOi::TrInterestEventData>&);
	void update_product_stream_queue(product_stream_t&);
	void flush_product_stream_queue(std::shared_ptr<product_stream_t>);
	void clear_cancelled_product_streams();
	void clear_duplicate_images(const product_stream_t& stream, SvPb::GetProductStreamResponse& res);
	void clear_duplicate_values(const product_stream_t& stream, SvPb::GetProductStreamResponse& res);
	SvSyl::SVFuture<void> handle_new_trigger_record(std::shared_ptr<product_stream_t>, SvOi::ITriggerRecordRPtr, int inspectionPos, uint32_t inspectionId, int trId, bool is_reject);
	SvSyl::SVFuture<void> get_product_data(SvPb::GetProductDataResponse&, const SvPb::GetProductDataRequest& rRequest);
	SvSyl::SVFuture<void> collect_images(::google::protobuf::RepeatedPtrField<SvPb::Image>&, ::google::protobuf::RepeatedPtrField<SvPb::OverlayDesc>&, SvOi::ITriggerRecordRPtr, const ::google::protobuf::RepeatedField<uint32_t>& imageIds, const std::vector<std::pair<bool, int>>& imagePositions, int inspectionPos, uint32_t inspectionId, bool includeOverlays);
	void collect_values(::google::protobuf::RepeatedPtrField<SvPb::Variant>&, SvOi::ITriggerRecordR&, const ::google::protobuf::RepeatedField<uint32_t>& valueIds, const std::vector<int>& valuePositions);
	void rebuild_trc_pos_caches();
	void rebuild_trc_pos_cache(product_stream_t&);
	void collect_historical_triggers(product_stream_t&);
	void schedule_historical_triggers(std::shared_ptr<product_stream_t>);
	void collect_value_pos(std::vector<int>&, const std::unordered_map<uint32_t, int>&, const ::google::protobuf::RepeatedField<uint32_t>& ids);
	void collect_image_pos(std::vector<std::pair<bool, int>>&, const std::unordered_map<uint32_t,int>&, const std::unordered_map<uint32_t, int>&, const ::google::protobuf::RepeatedField<uint32_t>& ids);

private:
	struct notification_stream_t
	{
		notification_stream_t(const SvPb::GetGatewayNotificationStreamRequest&, SvRpc::Observer<SvPb::GetGatewayNotificationStreamResponse>, SvRpc::ServerStreamContext::Ptr);
		SvPb::GetGatewayNotificationStreamRequest req;
		SvRpc::Observer<SvPb::GetGatewayNotificationStreamResponse> observer;
		SvRpc::ServerStreamContext::Ptr ctx;
	};
	void schedule_trigger_record_pause_state();
	void on_trigger_record_pause_state_timer(const boost::system::error_code&);
	void check_trigger_record_pause_state_changed();
	void on_trigger_record_pause_state_changed_impl(const std::vector<bool>&);
	void send_trigger_record_pause_state_to_client(notification_stream_t&, const std::vector<bool>&);

private:
	void subscribe_to_trc();
	void unsubscribe_from_trc();
	int get_inspection_pos_for_id(SvOi::ITriggerRecordControllerR&, uint32_t id);

private:
	boost::asio::io_service& m_io_service;
	const WebAppVersionLoader& m_rWebAppVersionLoader;
	std::unique_ptr<SvSml::ShareControl> m_pShareControlInstance;
	std::atomic_bool m_trc_ready {false};
	int m_TrcReadySubscriptionId;
	int m_TrcResetSubscriptionId;
	int m_TrcNewInterestTrSubscriptionId;

	std::mutex m_NewTriggerMutex;
	std::vector<SvOi::TrInterestEventData> m_NewTriggerQueue;

	std::vector<std::shared_ptr<product_stream_t>> m_ProductStreams;
	boost::asio::deadline_timer m_pause_timer;
	std::vector<bool> m_pause_state;
	std::vector<std::shared_ptr<notification_stream_t>> m_notification_streams;
	SvOv::OverlayController m_overlay_controller;
};
}// namespace SvOgw
