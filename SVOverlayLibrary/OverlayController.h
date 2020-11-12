//******************************************************************************
/// \copyright (c) 2017,2019 by Seidenader Maschinenbau GmbH
/// \file OverlayController.h
/// All Rights Reserved
//******************************************************************************

#pragma once

#include "SVProtoBuf/SVRC.h"
#include "SVProtoBuf/Overlay.h"
#include "SVRPCLibrary/SimpleClient.h"
#include "SVSystemLibrary/SVFuture.h"

namespace SvRpc
{
class RPCClient;
}

namespace SvOi
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
	/// \param inspectionId [in] Id of the inspection.
	/// \param imageId [in] Id of the image.
	/// \returns SvSyl::SVFuture<SvPb::OverlayDesc>
	SvSyl::SVFuture<SvPb::OverlayDesc> getOverlays(std::shared_ptr<SvOi::ITriggerRecordR> pTr, uint32_t inspectionId, uint32_t imageId);

public:
	boost::asio::io_context& m_io_context;
	SvRpc::RPCClient& m_rpc_client;
	boost::posix_time::seconds m_timeout {2};
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::InspectionCmdRequest, SvPb::InspectionCmdResponse> m_inspectionCmd_client;

private:
	/// Request the new structure of the overlay from the SVObserver.
	/// \param inspectionId [in] id of the inspection.
	/// \param imageId [in] id of the image.
	/// \returns SvSyl::SVFuture<SvPb::OverlayDesc>
	SvSyl::SVFuture<SvPb::OverlayDesc> restructOverlayDesc(uint32_t inspectionId, uint32_t imageId);

	/// Return the overlay structure.
	/// \param pTr [in] TriggerRecord.
	/// \param inspectionId [in] id of the inspection.
	/// \param imageId [in] id of the image.
	/// \returns SvSyl::SVFuture<SvPb::OverlayDesc>
	SvSyl::SVFuture<SvPb::OverlayDesc> getOverlayStruct(std::shared_ptr<SvOi::ITriggerRecordR> pTr, uint32_t inspectionId, uint32_t imageId);

	struct OverlayDefStruct 
	{
		OverlayDefStruct() = default;
		OverlayDefStruct(int resetId, SvPb::OverlayDesc desc) : m_resetId(resetId), m_overlayDesc(desc) {};
		int m_resetId = -1;
		SvPb::OverlayDesc m_overlayDesc;
	};

	std::unordered_map <uint32_t, OverlayDefStruct> m_overlayMap;
	std::mutex m_overlayMapMutex;
};

} // namespace SvRpc
