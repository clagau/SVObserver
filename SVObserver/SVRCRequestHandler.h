//*****************************************************************************
/// \copyright (c) 2018,2018 by Seidenader Maschinenbau GmbH
/// \file SVRCRequestHandler.h
/// All Rights Reserved 
//*****************************************************************************
/// PLEASE 
/// ENTER 
/// A DESCRIPTION
//******************************************************************************
#pragma once
#include "SVRPCLibrary/RequestHandler.h"
#include <boost\asio\strand.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>
#include "SVRCCommand.h"

class SVRCCommandInterface;
class SVRCRequestHandler :public SvRpc::RequestHandler
{
public:
	SVRCRequestHandler(SVRCCommand* pCommand);
	~SVRCRequestHandler();
private:
	boost::asio::io_service m_io_service;
	boost::asio::io_service::work m_io_work;
	boost::thread m_io_thread;

	
};

