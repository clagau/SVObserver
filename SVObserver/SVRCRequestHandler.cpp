#include "stdafx.h"
#include "SVRCRequestHandler.h"
#include "SVRCCommand.h"
#include "SVProtoBuf\SVRC.h"
#include <functional>


SVRCRequestHandler::~SVRCRequestHandler()
{ 
	m_io_service.stop();
	if (m_io_thread.joinable())
	{
		m_io_thread.join();
	}
}


SVRCRequestHandler::SVRCRequestHandler(SVRCCommand* pCommand)//, std::shared_ptr<boost::asio::io_service::strand> pstrand)
	: m_io_service(),
	m_io_work(m_io_service),
	m_io_thread(boost::bind(&boost::asio::io_service::run, &m_io_service))
{
	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kGetDeviceModeRequest,
		SvPb::GetDeviceModeRequest,
		SvPb::GetDeviceModeResponse>(
		[pCommand, this](SvPb::GetDeviceModeRequest&& req, SvRpc::Task<SvPb::GetDeviceModeResponse> task)
	{
		m_io_service.post([=]() { pCommand->GetDeviceMode(req, task); });
	});

	registerRequestHandler<
		SvPb::SVRCMessages,
		SvPb::SVRCMessages::kSetDeviceModeRequest,
		SvPb::SetDeviceModeRequest,
		SvPb::SetDeviceModeResponse>(
		[pCommand,this](SvPb::SetDeviceModeRequest&& req, SvRpc::Task<SvPb::SetDeviceModeResponse> task)
	{
		m_io_service.post([=]() { pCommand->SetDeviceMode(req, task); }
		);

	});
}



