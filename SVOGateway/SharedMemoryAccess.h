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
#pragma endregion Includes

namespace SvSml
{
struct ShareControlSettings;
}
namespace SvTrc
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
	struct new_trigger_t
	{
		new_trigger_t(int inspectionPos, int trId, bool is_reject);
		int inspectionPos;
		int trId;
		bool isReject;
	};
	struct product_stream_t
	{
		product_stream_t(const SvPb::GetProductStreamRequest&, SvRpc::Observer<SvPb::GetProductStreamResponse>, SvRpc::ServerStreamContext::Ptr);
		SvPb::GetProductStreamRequest req;
		SvRpc::Observer<SvPb::GetProductStreamResponse> observer;
		SvRpc::ServerStreamContext::Ptr ctx;
		std::vector<int> valuePositions;
		std::vector<int> imagePositions;
	};
	void on_new_trigger_record(int inspectionPos, int trId, bool is_reject);
	void add_new_trigger_to_queue(std::shared_ptr<new_trigger_t>);
	std::shared_ptr<new_trigger_t> pop_new_trigger_from_queue(bool is_reject);
	void check_queue_for_new_trigger_record(bool is_reject);
	void handle_new_trigger_record(std::shared_ptr<product_stream_t>, SvTrc::ITriggerRecordRPtr, int inspectionPos, uint32_t inspectionId, int trId, bool is_reject);
	SvSyl::SVFuture<void> get_product_data(SvPb::GetProductDataResponse&, const SvPb::GetProductDataRequest& rRequest);
	SvSyl::SVFuture<void> collect_images(::google::protobuf::RepeatedPtrField<SvPb::Image>&, ::google::protobuf::RepeatedPtrField<SvPb::OverlayDesc>&, SvTrc::ITriggerRecordRPtr, const ::google::protobuf::RepeatedField<uint32_t>& imageIds, const std::vector<int>& imagePositions, int inspectionPos, uint32_t inspectionId, bool includeOverlays);
	void collect_values(::google::protobuf::RepeatedPtrField<SvPb::Variant>&, SvTrc::ITriggerRecordR&, const ::google::protobuf::RepeatedField<uint32_t>& valueIds, const std::vector<int>& valuePositions);
	void rebuild_trc_pos_caches();
	void rebuild_trc_pos_cache(product_stream_t&);
	void collect_value_pos(std::vector<int>&, const std::unordered_map<uint32_t, int>&, const ::google::protobuf::RepeatedField<uint32_t>& ids);
	void collect_image_pos(std::vector<int>&, const std::unordered_map<uint32_t,int>&, const ::google::protobuf::RepeatedField<uint32_t>& ids);

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
	void on_trigger_record_pause_state_changed_impl(bool paused);
	void send_trigger_record_pause_state_to_client(notification_stream_t&, bool paused);

private:
	void subscribe_to_trc();
	void unsubscribe_from_trc();
	int get_inspection_pos_for_id(SvTrc::ITriggerRecordControllerR&, uint32_t id);

private:
	boost::asio::io_service& m_io_service;
	const WebAppVersionLoader& m_rWebAppVersionLoader;
	std::unique_ptr<SvSml::ShareControl> m_pShareControlInstance;
	std::atomic_bool m_trc_ready {false};
	int m_TrcReadySubscriptionId;
	int m_TrcResetSubscriptionId;
	int m_TrcNewTrSubscriptionId;
	int m_TrcNewInterestTrSubscriptionId;

	std::mutex m_NewTriggerMutex;
	std::vector<std::shared_ptr<new_trigger_t>> m_NewTriggerQueue;
	std::vector<std::shared_ptr<new_trigger_t>> m_NewTriggerInterestQueue;

	std::vector<std::shared_ptr<product_stream_t>> m_ProductStreams;
	boost::asio::deadline_timer m_pause_timer;
	std::atomic<bool> m_pause_state {false};
	std::vector<std::shared_ptr<notification_stream_t>> m_notification_streams;
	SvOv::OverlayController m_overlay_controller;
};
}// namespace SvOgw
