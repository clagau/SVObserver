//******************************************************************************
/// \copyright (c) 2017,2019 by Seidenader Maschinenbau GmbH
/// \file OverlayController.h
/// All Rights Reserved
//******************************************************************************

#pragma once

#include "SVProtoBuf/SVRC.h"
#include "SVProtoBuf/Overlay.h"
#include "SVRPCLibrary/RPCClient.h"
#include "SVRPCLibrary/SimpleClient.h"
#include "SVSystemLibrary/SVFuture.h"

namespace SvTrc
{
class ITriggerRecordR;
};

namespace SvOv
{
class OverlayController
{
public:
	OverlayController(boost::asio::io_context& io_context, SvRpc::RPCClient& rpcClient);

	/// Return the overlay depending of the image and filled with the live-data from the triggerRecord.
	/// \param pTr [in] TriggerRecord.
	/// \param inspectionId [in] Guid of the inspection.
	/// \param imageId [in] Guid of the image.
	/// \returns SvSyl::SVFuture<SvPb::OverlayDesc>
	SvSyl::SVFuture<SvPb::OverlayDesc> getOverlays(std::shared_ptr<SvTrc::ITriggerRecordR> pTr, GUID inspectionId, GUID imageId);

public:
	boost::asio::io_context& m_io_context;
	SvRpc::RPCClient& m_rpc_client;
	boost::posix_time::seconds m_timeout {2};
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::ExecuteInspectionCmdRequest, SvPb::ExecuteInspectionCmdResponse> m_inspectionCmd_client;

private:
	/// Request the new structure of the overlay from the SVObserver.
	/// \param inspectionId [in] Guid of the inspection.
	/// \param imageId [in] Guid of the image.
	/// \returns SvSyl::SVFuture<SvPb::OverlayDesc>
	SvSyl::SVFuture<SvPb::OverlayDesc> restructOverlayDesc(GUID inspectionId, GUID imageId);

	/// Return the overlay structure.
	/// \param pTr [in] TriggerRecord.
	/// \param inspectionId [in] Guid of the inspection.
	/// \param imageId [in] Guid of the image.
	/// \returns SvSyl::SVFuture<SvPb::OverlayDesc>
	SvSyl::SVFuture<SvPb::OverlayDesc> getOverlayStruct(std::shared_ptr<SvTrc::ITriggerRecordR> pTr, GUID inspectionId, GUID imageId);

	struct OverlayDefStruct 
	{
		OverlayDefStruct() = default;
		OverlayDefStruct(int resetId, SvPb::OverlayDesc desc) : m_resetId(resetId), m_overlayDesc(desc) {};
		int m_resetId = -1;
		SvPb::OverlayDesc m_overlayDesc;
	};

	struct GUIDHash
	{
		std::size_t operator()(const GUID& k) const
		{
			return k.Data1;
		}
	};
	std::unordered_map <GUID, OverlayDefStruct, GUIDHash> m_overlayMap;
	std::mutex m_overlayMapMutex;
};

} // namespace SvRpc
