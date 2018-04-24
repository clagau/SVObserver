//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVRCRequestHandler.h
/// All Rights Reserved 
//*****************************************************************************
/// Request handler for all the SVRC commands processed by SVObserver
//******************************************************************************
#pragma once
#pragma region Includes
//Moved to precompiled header: #include <boost/asio/io_service.hpp>
//Moved to precompiled header: #include <thread>
#include "SVRPCLibrary/RequestHandler.h"
#include "SVRCCommand.h"
#pragma endregion Includes

class SVRCRequestHandler :public SvRpc::RequestHandler
{
public:
	SVRCRequestHandler(SVRCCommand* pCommand);
	~SVRCRequestHandler();
private:
	boost::asio::io_service m_IoRunService;
	boost::asio::io_service::work m_IoWork;
	std::thread m_IoThread;
};

