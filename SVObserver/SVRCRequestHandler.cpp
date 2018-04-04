#include "stdafx.h"
#include "SVRCRequestHandler.h"
#include "SVRCCommandInterface.h"
#include "SVProtoBuf\SVRC.h"
#include <boost\asio\strand.hpp>

SVRCRequestHandler::SVRCRequestHandler(SVRCCommandInterface* pCommandInterface, std::shared_ptr<boost::asio::io_service::strand> pstrand)
{
	
	m_pstrand = pstrand;
	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetDeviceModeRequest,
		SvPb::GetDeviceModeRequest,
		SvPb::GetDeviceModeResponse>(
		[pCommandInterface, this](SvPb::GetDeviceModeRequest&& req, SvRpc::Task<SvPb::GetDeviceModeResponse> task)
	{
		//pCommandInterface->GetDeviceMode(req, task);
		this->m_pstrand->post([req, task, pCommandInterface]() { pCommandInterface->GetDeviceMode(req, task); });
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kSetDeviceModeRequest,
		SvPb::SetDeviceModeRequest,
		SvPb::SetDeviceModeResponse>(
		[pCommandInterface,this](SvPb::SetDeviceModeRequest&& req, SvRpc::Task<SvPb::SetDeviceModeResponse> task)
	{
		//pCommandInterface->SetDeviceMode(req, task);
		this->m_pstrand->post([req, task, pCommandInterface]() { pCommandInterface->SetDeviceMode(req, task); });
		
	});
}



