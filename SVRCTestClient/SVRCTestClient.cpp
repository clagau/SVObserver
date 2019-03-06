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
#include <boost/thread.hpp>
#include "WebsocketLibrary/SVRCClientService.h"
#include "WebsocketLibrary\RunRequest.inl"
#include "SVLogLibrary/Logging.h"
#include "SVUtilityLibrary/StringHelper.h"


struct NotificationHandler
{
	//This compiles with a reference but not with a rhs reference 
	std::future<void>  OnNext(SvPb::GetNotificationStreamResponse& resp)
	{

		SV_LOG_GLOBAL(info) << "Get New Notification: " <<
			resp.message_case() << " DEBUGSTRING: " << resp.DebugString() << std::endl;
		return std::future<void>();
	}
	void OnFinish()
	{
		SV_LOG_GLOBAL(info) << "Last Notification" << std::endl;
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
		//SV_LOG_GLOBAL(info) << "Received notification " << res.id() << " " << res.type() << " " << res.message();

		SV_LOG_GLOBAL(info) << "Received notification Debug string " << res.DebugString() << std::endl;
		return std::future<void>();
	},
		[]()
	{
		SV_LOG_GLOBAL(info) << "Finished receiving notifications";
	},
		[](const SvPenv::Error& err)
	{
		SV_LOG_GLOBAL(info) << "Error while receiving notifications: " << err.message();
	}));
	std::this_thread::sleep_for(std::chrono::seconds(300));
	ctx.cancel();
}

void PrintTreeItems(const SvPb::TreeItem& rTreeItem, std::string& rData, const std::string& rSpacing)
{
	for(int i=0; i < rTreeItem.children_size(); i++)
	{
		rData += rSpacing + SvUl::to_ansi(rTreeItem.children(i).name()) + (rTreeItem.children(i).selected() ? _T("*\n") :  _T("\n"));
		PrintTreeItems(rTreeItem.children(i), rData, rSpacing + _T("\t"));
	}
}


int main(int argc, char* argv[])
{
	SvHttp::WebsocketClientSettings ClientSettings;
	ClientSettings.Host = "192.168.10.110";
	ClientSettings.Port = SvHttp::Default_Port;
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
		SV_LOG_GLOBAL(error) << e.what();
	}
	SvWsl::SVRCClientServiceSetting settings;

	

	auto pService = std::make_unique<SvWsl::SVRCClientService>(*pRpcClient, settings);

	SV_LOG_GLOBAL(info) << "Enter a command(Ctrl-Z to stop): ";
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
				SV_LOG_GLOBAL(info) << "commands: " << std::endl
					<< "  q  quit" << std::endl
					<< "  h  Hilfe" << std::endl
					<< "  m  (GetMode)" << std::endl
					<< "  n  notification" << std::endl
					<< "  e  (editmode)" << std::endl
					<< "  r  rummode" << std::endl
					<< "  c getconfig" << std::endl
					<< "  o getobjectselector" << std::endl;
			}
			else if (words[0] == "m")
			{
				///GetMode
				SvPb::GetDeviceModeRequest request;
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetDeviceModeRequest, SvPb::GetDeviceModeResponse> client(*pRpcClient);
				auto res = client.request(std::move(request), Timeout).get();
				SV_LOG_GLOBAL(info) << res.mode() << " DEBUGSTRING: " << res.DebugString();
			}
			else if (words[0] == "c")
			{
				///GetConfig
				SvPb::GetConfigRequest request;
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetConfigRequest, SvPb::GetConfigResponse> client(*pRpcClient);
				auto res = client.request(std::move(request), Timeout).get();
				SV_LOG_GLOBAL(info) << res.DebugString();
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
			else if (words[0] == "o")
			{
				///GetObjectSelector
				SvPb::GetObjectSelectorItemsRequest request;
				request.set_inspectionid(_T("Inspection_1"));
				request.set_attribute(SvPb::ObjectAttributes::viewable);
				request.set_wholearray(true);
				request.set_filter(SvPb::SelectorFilter::attributesAllowed);
				request.add_types(SvPb::ObjectSelectorType::globalConstantItems);
				request.add_types(SvPb::ObjectSelectorType::ppqItems);
				request.add_types(SvPb::ObjectSelectorType::toolsetItems);
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetObjectSelectorItemsRequest, SvPb::GetObjectSelectorItemsResponse> client(*pRpcClient);
				auto res = client.request(std::move(request), Timeout).get();
				SV_LOG_GLOBAL(info) << res.DebugString();
				if (0 != res.tree().children_size())
				{
					std::string Data;
					PrintTreeItems(res.tree(), Data, _T(""));

					std::ofstream FileStream;
					FileStream.open(_T("D:\\Temp\\ObjectSelector.txt"), std::ofstream::out | std::ofstream::trunc);
					if (FileStream.is_open())
					{

						FileStream.write(Data.c_str(), Data.length());
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
				SV_LOG_GLOBAL(info) << res.DebugString();
			}
			else if (words[0] == "gn")
			{
				try
				{
					GetNotifications(*pService);
				}
				catch (const std::exception& e)
				{
					SV_LOG_GLOBAL(error) << "Unable to get notifications: " << e.what();
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
			SV_LOG_GLOBAL(error) << e.what();
		}
	}


	pRpcClient->stop();
	system("pause");
	return 0;

}

