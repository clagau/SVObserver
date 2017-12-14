#include "stdafx.h"
#include "RunReServer.h"
#include <mil.h>
#include "RunReApi\Definition.h"

int main()
{
	MIL_ID AppId(M_NULL);
	try
	{
		std::cout << "Initializing Matrox Image Library" << std::endl;
		MIL_ID AppId = MappAlloc(M_DEFAULT, M_NULL);
		if (AppId == M_NULL)
		{
			throw std::exception("MapAlloc failed");
		}
		boost::asio::io_service io_service;

		std::cout << "Starting Asio Server" << std::endl;
		RunReServer  rrServer;
		auto api = RRApi::ServerFrontEndApi::Init(io_service, &rrServer, RRApi::Default_Port);
		io_service.run();
		
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	MappFree(AppId);
	system("pause");
	return 0;
}

