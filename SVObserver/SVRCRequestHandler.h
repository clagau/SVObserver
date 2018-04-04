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
class SVRCCommandInterface;
class SVRCRequestHandler :public SvRpc::RequestHandler
{
public:
	SVRCRequestHandler(SVRCCommandInterface* pCommandInterface, std::shared_ptr<boost::asio::io_service::strand> pstrand);
protected:
	std::shared_ptr<boost::asio::io_service::strand> m_pstrand;
	
};

