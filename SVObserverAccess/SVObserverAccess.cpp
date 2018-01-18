//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SVObserverAccess.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Defines the exported functions for the DLL application.
//******************************************************************************
#include "stdafx.h"
#include "SVObserverAccess.h"
#include "RunReServer.h"
#include "RunReApi\Definition.h"
#include <SVSharedMemoryLibrary\SVSharedConfiguration.h>
namespace ObserverAccess
{
	SVOBSERVERACCESS_API int DefaultPort = RRApi::Default_Port;
	SVOBSERVERACCESS_API void ExecuteRunReServer(unsigned int port)
	{
		boost::asio::io_service io_service;
		RunReServer  rrServer;
		auto api = RRApi::ServerFrontEndApi::Init(io_service, &rrServer, port);
		io_service.run();
	}

	SVOBSERVERACCESS_API bool  SharedResourcesOk(bool bCheckSizeOverride)
	{
		HRESULT hr = SvSml::SVSharedConfiguration::SharedResourcesOk();
		if (hr == S_OK || hr == STG_E_INSUFFICIENTMEMORY && !bCheckSizeOverride)
		{
			return true;
		}
		std::string msg;
		if (STG_E_INSUFFICIENTMEMORY == hr)
		{
			msg = _T("Shared Resources - not enough space available.\n");
		}
		else if (STG_E_PATHNOTFOUND == hr)
		{
			msg = _T("Shared Resources - Path not found.\n");
		}
		else
		{
			msg = _T("Shared Resources - missing or inadequate.\n");
		}
		throw std::exception(msg.c_str());
		return false;
	}
}

