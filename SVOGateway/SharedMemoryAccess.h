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
#include "SVRPCLibrary/RPCClient.h"
#include "SVRPCLibrary/Task.h"
#pragma endregion Includes

namespace SvSml
{
struct ShareControlSettings;
}
namespace SvTrc
{
class ITriggerRecordR;
class TriggerRecordController;
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
		SvRpc::ServerStreamContext::Ptr ctx;
		std::vector<int> valuePositions;
		std::vector<int> imagePositions;
	};
	void on_new_trigger_record(int inspectionPos, int trId, bool is_reject);
	void handle_new_trigger_record(std::shared_ptr<product_stream_t>, SvTrc::ITriggerRecordRPtr, int inspectionPos, GUID inspectionId, int trId, bool is_reject);
	SvSyl::SVFuture<void> collect_images(SvPb::GetProductStreamResponse&, SvTrc::ITriggerRecordRPtr, const ::google::protobuf::RepeatedPtrField<std::string>& imageGuids, const std::vector<int>& imagePositions, int inspectionPos, GUID inspectionId, bool includeOverlays);
	void collect_values(SvPb::GetProductStreamResponse&, SvTrc::ITriggerRecordR&, const ::google::protobuf::RepeatedPtrField<std::string>& valueGuids, const std::vector<int>& valuePositions);
	void rebuild_trc_pos_caches();
	void rebuild_trc_pos_cache(product_stream_t&);
	void collect_value_pos(std::vector<int>&, const SvPb::DataDefinitionList&, const ::google::protobuf::RepeatedPtrField<std::string>& guids);
	void collect_image_pos(std::vector<int>&, const SvPb::ImageList&, const ::google::protobuf::RepeatedPtrField<std::string>& guids);

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
	int get_inspection_pos_for_guid(SvTrc::TriggerRecordController&, const std::string& guid);

private:
	boost::asio::io_service& m_io_service;
	const WebAppVersionLoader& m_rWebAppVersionLoader;
	std::unique_ptr<SvSml::ShareControl> m_pShareControlInstance;
	std::atomic_bool m_trc_ready {false};
	int m_TrcReadySubscriptionId;
	int m_TrcResetSubscriptionId;
	int m_TrcNewTrSubscriptionId;
	int m_TrcNewInterestTrSubscriptionId;
	std::vector<std::shared_ptr<product_stream_t>> m_ProductStreams;
	boost::asio::deadline_timer m_pause_timer;
	std::atomic<bool> m_pause_state {false};
	std::vector<std::shared_ptr<notification_stream_t>> m_notification_streams;
	SvOv::OverlayController m_overlay_controller;
};
}// namespace SvOgw
