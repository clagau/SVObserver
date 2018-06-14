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
#include "WebsocketLibrary/SVRCClientService.h"
#include "WebsocketLibrary/Logging.h"
#include "WebsocketLibrary\Definition.h"
#include "WebsocketLibrary\RunRequest.inl"



struct NotificationHandler
{
	//This compiles with a reference but not with a rhs reference 
	std::future<void>  OnNext(SvPb::GetNotificationStreamResponse& resp)
	{

		BOOST_LOG_TRIVIAL(info) << "Get New Notification: " <<
			resp.message_case() << " DEBUGSTRING: " << resp.DebugString() << std::endl;
		return std::future<void>();
	}
	void OnFinish()
	{
		BOOST_LOG_TRIVIAL(info) << "Last Notification" << std::endl;
		return;
	}
	void OnError(const SvPenv::Error& er)
	{
		SvRpc::errorToException(er);
		return;
	}

};



static void GetNotifications(SvWsl::SVRCClientService& client)
{
	SvPb::GetNotificationStreamRequest req;
	auto ctx = client.GetNotificationStream(std::move(req), SvRpc::Observer<SvPb::GetNotificationStreamResponse>(
		[](SvPb::GetNotificationStreamResponse&& res) -> std::future<void>
	{
		//BOOST_LOG_TRIVIAL(info) << "Received notification " << res.id() << " " << res.type() << " " << res.message();

		BOOST_LOG_TRIVIAL(info) << "Received notification Debug string " << res.DebugString() << std::endl;
		return std::future<void>();
	},
		[]()
	{
		BOOST_LOG_TRIVIAL(info) << "Finished receiving notifications";
	},
		[](const SvPenv::Error& err)
	{
		BOOST_LOG_TRIVIAL(info) << "Error while receiving notifications: " << err.message();
	}));
	std::this_thread::sleep_for(std::chrono::seconds(300));
	ctx.cancel();
}



int main(int argc, char* argv[])
{
	SvHttp::WebsocketClientSettings ClientSettings;
	ClientSettings.Host = "127.0.0.1";
	ClientSettings.Port = SvWsl::Default_SecondPort;
	if (argc > 1)
		ClientSettings.Host = argv[1];
	if (argc > 2)
		ClientSettings.Port = atoi(argv[2]);

	NotificationHandler Handler;

	SvRpc::Observer<SvPb::GetNotificationStreamResponse> NotifikationObserver(boost::bind(&NotificationHandler::OnNext, &Handler, _1),
		boost::bind(&NotificationHandler::OnFinish, &Handler),
		boost::bind(&NotificationHandler::OnError, &Handler, _1));
	SvRpc::ClientStreamContext csx(nullptr);


	std::unique_ptr< SvRpc::RPCClient> pRpcClient;
	try
	{
		pRpcClient = std::make_unique<SvRpc::RPCClient>(ClientSettings);
		pRpcClient->waitForConnect(boost::posix_time::seconds(6));
	}

	catch (std::exception&  e)
	{
		BOOST_LOG_TRIVIAL(error) << e.what();
	}
	auto pService = std::make_unique<SvWsl::SVRCClientService>(*pRpcClient);

	BOOST_LOG_TRIVIAL(info) << "Enter a command(Ctrl-Z to stop): ";
	std::string input;
	while (std::getline(std::cin, input))
	{

		try
		{
			auto Timeout = boost::posix_time::seconds(60);
			std::vector<std::string> words;
			boost::algorithm::split(words, input, boost::is_space(), boost::token_compress_on);
			auto wordsize = words.size();
			if (wordsize == 0)
				continue;
			if (words[0] == "q")
			{
				break;
			}
			else if (words[0] == "h" || words[0] == "H")
			{
				BOOST_LOG_TRIVIAL(info) << "commands: " << std::endl
					<< "  q  quit" << std::endl
					<< "  h  Hilfe" << std::endl
					<< "  m  (GetMode)" << std::endl
					<< "  n  notification" << std::endl
					<< "  e  (editmode)" << std::endl
					<< "  r  rummode" << std::endl
					<< "  c getconfig" << std::endl;
			}
			else if (words[0] == "m")
			{
				///GetMode
				SvPb::GetDeviceModeRequest request;
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetDeviceModeRequest, SvPb::GetDeviceModeResponse> client(*pRpcClient);
				auto res = client.request(std::move(request), Timeout).get();
				BOOST_LOG_TRIVIAL(info) << res.mode() << " DEBUGSTRING: " << res.DebugString();
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
				if (words[0] == "e")
				{
					request.set_mode(SvPb::DeviceModeType::editMode);
				}
				else
				{
					request.set_mode(SvPb::DeviceModeType::runMode);
				}

				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::SetDeviceModeRequest, SvPb::StandardResponse> client(*pRpcClient);
				auto res = client.request(std::move(request), Timeout).get();
				BOOST_LOG_TRIVIAL(info) << res.DebugString();
			}
			else if (words[0] == "gn")
			{
				try
				{
					GetNotifications(*pService);
				}
				catch (const std::exception& e)
				{
					BOOST_LOG_TRIVIAL(error) << "Unable to get notifications: " << e.what();
				}
			}

			/// Notification
			else if (words[0] == "n" || words[0] == "N")
			{

				SvRpc::SimpleClient
					<SvPb::SVRCMessages, SvPb::GetNotificationStreamRequest, SvPb::GetNotificationStreamResponse>
					streamClient(*pRpcClient);

				SvPb::GetNotificationStreamRequest getNotificationStreamRequest;

				csx = streamClient.stream(std::move(getNotificationStreamRequest), NotifikationObserver);
	
			}
			else if (words[0] == "cn" )
			{

				csx.cancel();
				
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

