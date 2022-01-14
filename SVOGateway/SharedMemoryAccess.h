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

#include "SVOverlayLibrary/OverlayController.h"
#include "SVProtoBuf/SVRC.h"
#include "SVProtoBuf/SVAuth.h"
#include "SVSharedMemoryLibrary/ShareControl.h"
#include "SVRPCLibrary/ServerStreamContext.h"
#include "SVRPCLibrary/Task.h"
#include "SVSharedMemoryLibrary/SharedMemoryLock.h"
#include "ObjectInterfaces/ITriggerRecordControllerR.h"
#pragma endregion Includes

namespace SvAuth
{
class SessionContext;
class UserDatabase;
}
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

namespace SvStl
{
	class MessageContainer;
}

namespace SvOgw
{
class WebAppVersionLoader;

class SharedMemoryAccess
{
public:
	SharedMemoryAccess(
		boost::asio::io_service& rIoService,
		const SvSml::ShareControlSettings& ControlParameter,
		const WebAppVersionLoader&,
		SvRpc::RPCClient& rpcClient,
		SvAuth::UserDatabase& userDatabase,
		bool skipPermissionChecks
	);
	~SharedMemoryAccess();

public:
	void onMessageContainer(const SvStl::MessageContainer& rMessage, int messageType);

public:
	void GetVersion(const SvPb::GetGatewayVersionRequest&, SvRpc::Task<SvPb::GetVersionResponse>);
	void GetWebAppVersion(const SvPb::GetWebAppVersionRequest&, SvRpc::Task<SvPb::GetVersionResponse>);
	void GetInspections(const SvPb::GetInspectionsRequest&, SvRpc::Task<SvPb::GetInspectionsResponse>);
	void GetProduct(const SvPb::GetProductRequest&, SvRpc::Task<SvPb::GetProductResponse>);
	void GetProductData(const SvPb::GetProductDataRequest&, SvRpc::Task<SvPb::GetProductDataResponse>);
	void GetReject(const SvPb::GetRejectRequest&, SvRpc::Task<SvPb::GetRejectResponse>);
	void GetFailstatus(const SvPb::GetFailStatusRequest&, SvRpc::Task<SvPb::GetFailStatusResponse>);
	void GetImageFromId(const SvPb::GetImageFromIdRequest&, SvRpc::Task<SvPb::GetImageFromIdResponse>);
	void GetTriggerItems(const SvPb::GetTriggerItemsRequest&, SvRpc::Task<SvPb::GetTriggerItemsResponse>);
	void QueryListName(const SvPb::QueryListNameRequest&, SvRpc::Task<SvPb::QueryListNameResponse>);
	void QueryListItem(const SvPb::QueryListItemRequest&, SvRpc::Task<SvPb::QueryListItemResponse>);
	void StoreClientLogs(const SvPb::StoreClientLogsRequest&, SvRpc::Task<SvPb::EmptyResponse>);
	void SetRejectStreamPauseState(const SvPb::SetRejectStreamPauseStateRequest&, SvRpc::Task<SvPb::EmptyResponse>);
	void GetGatewayNotificationStream(const SvPb::GetGatewayNotificationStreamRequest&, SvRpc::Observer<SvPb::GetGatewayNotificationStreamResponse>, SvRpc::ServerStreamContext::Ptr);
	void GetGatewayMessageStream(const SvPb::GetGatewayMessageStreamRequest&, SvRpc::Observer<SvPb::GetGatewayMessageStreamResponse>, SvRpc::ServerStreamContext::Ptr);
	void GetProductStream(const SvPb::GetProductStreamRequest&, SvRpc::Observer<SvPb::GetProductStreamResponse>, SvRpc::ServerStreamContext::Ptr);

	void GetMyPermissions(const SvAuth::SessionContext&, const SvPb::GetMyPermissionsRequest& req, SvRpc::Task<SvPb::GetMyPermissionsResponse> task);
	void GetGroupDetails(const SvAuth::SessionContext&, const SvPb::GetGroupDetailsRequest&, SvRpc::Task<SvPb::GetGroupDetailsResponse>);
	void UpdateGroupPermissions(const SvAuth::SessionContext&, const SvPb::UpdateGroupPermissionsRequest&, SvRpc::Task<SvPb::UpdateGroupPermissionsResponse>);

	void AcquireLockStream(const SvAuth::SessionContext&, const SvPb::LockAcquisitionStreamRequest&, SvRpc::Observer<SvPb::LockAcquisitionStreamResponse>, SvRpc::ServerStreamContext::Ptr);
	void TakeoverLock(const SvAuth::SessionContext&, const SvPb::LockTakeoverRequest&, SvRpc::Task<SvPb::LockTakeoverResponse>);

	bool CheckRequestPermissions(const SvAuth::SessionContext&, const SvPenv::Envelope&, SvRpc::Task<SvPenv::Envelope>);
	bool CheckStreamPermissions(const SvAuth::SessionContext&, const SvPenv::Envelope&, SvRpc::Observer<SvPenv::Envelope>, SvRpc::ServerStreamContext::Ptr);

	void unsubscribe_from_trc();

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
	struct message_stream_t
	{
		message_stream_t(const SvPb::GetGatewayMessageStreamRequest&, SvRpc::Observer<SvPb::GetGatewayMessageStreamResponse>, SvRpc::ServerStreamContext::Ptr);
		std::vector<int> severityList;
		SvRpc::Observer<SvPb::GetGatewayMessageStreamResponse> observer;
		SvRpc::ServerStreamContext::Ptr ctx;
	};

private:
	struct lock_acquisition_stream_t
	{
		lock_acquisition_stream_t(
			const SvPb::LockAcquisitionStreamRequest&,
			const SvRpc::Observer<SvPb::LockAcquisitionStreamResponse>&,
			const SvRpc::ServerStreamContext::Ptr&,
			const SvAuth::SessionContext&);

		static std::shared_ptr<lock_acquisition_stream_t> create(
			const SvPb::LockAcquisitionStreamRequest&,
			const SvRpc::Observer<SvPb::LockAcquisitionStreamResponse>&,
			const SvRpc::ServerStreamContext::Ptr&,
			const SvAuth::SessionContext&);

		static std::uint32_t ID_COUNTER;

		std::uint32_t id;
		SvPb::LockAcquisitionStreamRequest request;
		SvRpc::Observer<SvPb::LockAcquisitionStreamResponse> observer;
		SvRpc::ServerStreamContext::Ptr streamContext;
		SvAuth::SessionContext sessionContext;
		bool isLockOwner;
	};

	void acquire_lock(lock_acquisition_stream_t&);
	void handle_lock_acquisition(lock_acquisition_stream_t&);
	void notify_client_about_lock_acquisition(const lock_acquisition_stream_t&);
	void schedule_auto_release_lock(lock_acquisition_stream_t&, const std::uint64_t);
	void auto_release_lock(lock_acquisition_stream_t&);
	void release_lock(lock_acquisition_stream_t&, const SvPb::LockReleaseReason);
	void broadcast_release_notification(const lock_acquisition_stream_t& stream, SvPb::LockReleaseReason);
	void handle_lock_takeover_request(lock_acquisition_stream_t&);
	void notify_owner_about_lock_takeover(const lock_acquisition_stream_t&);
	void schedule_cancel_for_lock_takeover(lock_acquisition_stream_t&, const std::uint64_t);
	void cancel_lock_takeover(lock_acquisition_stream_t&);
	void schedule_disconnected_clients_check();
	void on_disconnected_clients_check(const boost::system::error_code&);
	void check_disconnected_clients();

private:
	void subscribe_to_trc();
	int get_inspection_pos_for_id(const SvPb::InspectionList& rList, uint32_t id);

private:
	bool is_request_allowed(const SvPenv::Envelope& rEnvelope, const SvPb::Permissions& permissions);

private:
	boost::asio::io_service& m_io_service;
	const WebAppVersionLoader& m_rWebAppVersionLoader;
	SvAuth::UserDatabase& m_rUserDatabase;
	bool m_skipPermissionChecks;
	std::unique_ptr<SvSml::ShareControl> m_pShareControlInstance;
	std::atomic_bool m_trc_ready {false};
	SvOi::RAIIPtr m_TrcReadySubscriptionRAII;
	SvOi::RAIIPtr m_TrcResetSubscriptionRAII;
	SvOi::RAIIPtr m_TrcNewInterestTrSubscriptionRAII;

	SharedMemoryLock m_SharedMemoryLock;
	std::string m_LockOwner;
	boost::asio::deadline_timer m_AutoReleaseTimer;
	boost::asio::deadline_timer m_LockTakeoverCancelTimer;
	boost::asio::deadline_timer m_DisconnectCheckTimer;
	std::vector<std::shared_ptr<lock_acquisition_stream_t>> m_LockAcquisitionStreams;

	std::mutex m_NewTriggerMutex;
	std::vector<SvOi::TrInterestEventData> m_NewTriggerQueue;

	std::vector<std::shared_ptr<product_stream_t>> m_ProductStreams;
	boost::asio::deadline_timer m_pause_timer;
	std::vector<bool> m_pause_state;
	std::vector<std::shared_ptr<notification_stream_t>> m_notification_streams;
	std::mutex m_message_streams_mutex;
	std::vector<std::shared_ptr<message_stream_t>> m_message_streams;
	SvOv::OverlayController m_overlay_controller;
};
}// namespace SvOgw
