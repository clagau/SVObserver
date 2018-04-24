// SVRCTestClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "SVHttpLibrary/WebsocketClient.h"
#include "SVProtobuf/SVRC.h"
#include "SVRPCLibrary/RPCClient.h"
#include "SVRPCLibrary/SimpleClient.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/thread.hpp>
#include "WebsocketLibrary\Definition.h"

int main(int argc, char* argv[])
{
	std::string host("127.0.0.1");
	std::string stPort("9000");
	uint16_t port(SvWsl::Default_SecondPort);
	if (argc > 1)
		host = argv[1];
	if (argc > 2)
	{
		stPort = argv[2];
		port = atoi(stPort.c_str());
	}
	
	std::unique_ptr< SvRpc::RPCClient> pRpcClient;
	try
    {
		pRpcClient = std::make_unique<SvRpc::RPCClient>(host, port);
		pRpcClient->waitForConnect(6000);
	}
   
    catch (std::exception&  e)
    {
		BOOST_LOG_TRIVIAL(error) << e.what();
	}

	BOOST_LOG_TRIVIAL(info) << "Enter a command(Ctrl-Z to stop): ";
	std::string input;
	while (std::getline(std::cin, input))
	{

		try
		{
			auto Timeout = boost::posix_time::seconds(5);
			std::vector<std::string> words;
			boost::algorithm::split(words, input, boost::is_space(), boost::token_compress_on);
			auto wordsize = words.size();
			if (wordsize == 0)
				continue;
			if (words[0] == "q")
			{
				break;
			}
			else if (words[0] == "m")
			{
				///GetMode
				SvPb::GetDeviceModeRequest request;
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetDeviceModeRequest, SvPb::GetDeviceModeResponse> client(*pRpcClient);
				auto res = client.request(std::move(request), Timeout).get();
				BOOST_LOG_TRIVIAL(info) << res.DebugString();
			}
			else if (words[0] == "c")
			{
				///GetConfig
				SvPb::GetConfigRequest request;
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetConfigRequest, SvPb::GetConfigResponse> client(*pRpcClient);
				auto res = client.request(std::move(request), Timeout).get();
				BOOST_LOG_TRIVIAL(info) << res.DebugString();
				if (S_OK == res.hresult())
				{
					std::ofstream FileStream;

					FileStream.open(_T("D:\\Temp\\Test.pac"), std::ofstream::out | std::ofstream::binary | std::ofstream::ate);
					if (FileStream.is_open())
					{
						FileStream.write(res.filedata().c_str(), res.filedata().length());
						FileStream.close();
					}
				}
			}
			else if (words[0] == "e" || words[0] == "r")
			{
			
				SvPb::SetDeviceModeRequest request;
				if(words[0] == "e")
				{
					request.set_mode(SvPb::EditMode);
				}
				else
				{
					request.set_mode(SvPb::RunMode);
				}

				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::SetDeviceModeRequest, SvPb::StandardResponse> client(*pRpcClient);
				auto res = client.request(std::move(request), Timeout).get();
				BOOST_LOG_TRIVIAL(info) << res.DebugString();
			}
		}

		catch (std::exception& e)
		{
			BOOST_LOG_TRIVIAL(error) << e.what();
		}
	}


	pRpcClient->stop();
	system("pause");
	return 0;
			
}

