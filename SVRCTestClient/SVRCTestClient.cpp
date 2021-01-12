// SVRCTestClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "SvHttpLibrary/WebsocketClientSettings.h"
#include "SVLogLibrary/Logging.h"
#include "SVProtoBuf/SVRC.h"
#include "SVRPCLibrary/RPCClient.h"
#include "SVRPCLibrary/SimpleClient.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "WebsocketLibrary/SVRCClientService.h"
#include "WebsocketLibrary/SVRCClientServiceSettings.h"
#include "WebsocketLibrary\RunRequest.inl"


struct NotificationHandler
{
	//This compiles with a reference but not with a rhs reference 
	SvSyl::SVFuture<void>  OnNext(const SvPb::GetNotificationStreamResponse& resp)
	{
		SV_LOG_GLOBAL(info) << "Get New Notification: " <<
			resp.message_case() << " DEBUGSTRING: " << resp.DebugString() << std::endl;
		return SvSyl::SVFuture<void>::make_ready();
	}
	void OnFinish()
	{
		SV_LOG_GLOBAL(info) << "Last Notification" << std::endl;
		return;
	}
	void OnError(const SvPenv::Error& er)
	{
		std::runtime_error(std::to_string(er));
		return;
	}

};



void GetNotifications(SvWsl::SVRCClientService& client)
{
	SvPb::GetNotificationStreamRequest req;
	auto ctx = client.GetNotificationStream(std::move(req), SvRpc::Observer<SvPb::GetNotificationStreamResponse>(
		[](SvPb::GetNotificationStreamResponse&& res) -> SvSyl::SVFuture<void>
	{
		//SV_LOG_GLOBAL(info) << "Received notification " << res.id() << " " << res.type() << " " << res.message();

		SV_LOG_GLOBAL(info) << "Received notification Debug string " << res.DebugString() << std::endl;
		return SvSyl::SVFuture<void>::make_ready();
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
	for (int i = 0; i < rTreeItem.children_size(); i++)
	{
		const SvPb::TreeItem& rChildItem = rTreeItem.children(i);
		std::string selected = rChildItem.selected() ? _T("\tSelected\n") : _T("\tNot selected\n");
		rData += rSpacing + SvUl::to_ansi(rChildItem.name()) + _T("\n");
		rData += rSpacing + selected;
		rData += rSpacing + _T("\t") + rChildItem.type() + _T("\n");
		rData += rSpacing + _T("\t") + rChildItem.objectidindex() + _T("\n");
		PrintTreeItems(rChildItem, rData, rSpacing + _T("\t"));
	}
}

void PrintTreeItems(const SvPb::ConfigTreeItem& rTreeItem, std::string& rData, const std::string& rSpacing)
{
	const google::protobuf::EnumDescriptor* pTypeDescriptor = SvPb::SVObjectTypeEnum_descriptor();
	const google::protobuf::EnumDescriptor* pSubtypeDescriptor = SvPb::SVObjectSubTypeEnum_descriptor();

	for (int i = 0; i < rTreeItem.children_size(); i++)
	{
		const SvPb::ConfigTreeItem& rChildItem = rTreeItem.children(i);
		std::string state = rChildItem.isvalid() ? _T("Valid\n") : _T("Invalid\n");
		rData += rSpacing + SvUl::to_ansi(rChildItem.name()) + _T("\n");
		rData += rSpacing + std::to_string(rChildItem.objectid()) + _T("\n");
		rData += rSpacing + std::to_string(rChildItem.position()) + _T("\n");
		rData += rSpacing + pTypeDescriptor->FindValueByNumber(rChildItem.objecttype())->name() + _T("\n");
		rData += rSpacing + pSubtypeDescriptor->FindValueByNumber(rChildItem.objectsubtype())->name() + _T("\n");
		PrintTreeItems(rTreeItem.children(i), rData, rSpacing + _T("\t"));
	}
}

int main(int argc, char* argv[])
{
	SvHttp::WebsocketClientSettings ClientSettings;
	ClientSettings.Host = "192.168.10.110";
	ClientSettings.Port = SvHttp::Default_Port;
	if (argc > 1)
	{
		ClientSettings.Host = argv[1];
	}
	if (argc > 2)
	{
		ClientSettings.Port = static_cast<uint16_t> (atoi(argv[2]));
	}

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
					<< "  r  runmode" << std::endl
					<< "  c getconfig" << std::endl
					<< "  o getobjectselector" << std::endl
					<< "  i executeInspectionCmd" << std::endl
					<< "  t getconfigtree" << std::endl
					<< "  a add tool" << std::endl
					<< "  d delete tool" << std::endl
					<< "  g get config data" << std::endl;
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

					FileStream.open(_T("D:\\Temp\\Test.svz"), std::ofstream::out | std::ofstream::binary | std::ofstream::ate);
					if (FileStream.is_open())
					{
						FileStream.write(res.filedata().c_str(), res.filedata().length());
						FileStream.close();
					}
				}
			}
			else if (words[0] == "o")
			{
				SvPb::InspectionCmdRequest requestCmd;
				auto* pRequest = requestCmd.mutable_getobjectidrequest();
				pRequest->set_name("Inspections.Inspection_1");

				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::InspectionCmdRequest, SvPb::InspectionCmdResponse> clientInspectionCmd(*pRpcClient);
				auto responseCmd = clientInspectionCmd.request(std::move(requestCmd), Timeout).get();

				///GetObjectSelector
				SvPb::GetObjectSelectorItemsRequest request; 
				request.set_inspectionid(responseCmd.getobjectidresponse().objectid());
				request.set_attribute(SvPb::ObjectAttributes::viewable);
				request.set_wholearray(true);
				request.set_attributesallowed(0);
				request.add_areas(SvPb::SearchArea::globalConstantItems);
				request.add_areas(SvPb::SearchArea::ppqItems);
				request.add_areas(SvPb::SearchArea::toolsetItems);
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetObjectSelectorItemsRequest, SvPb::GetObjectSelectorItemsResponse> client(*pRpcClient);
				auto resposeTree = client.request(std::move(request), Timeout).get();
				SV_LOG_GLOBAL(info) << resposeTree.DebugString();
				if (0 != resposeTree.tree().children_size())
				{
					std::string Data;
					PrintTreeItems(resposeTree.tree(), Data, _T(""));

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
			else if (words[0] == "i")
			{
				///GetObjectID
				SvPb::InspectionCmdRequest requestCmd;
				auto* pGetObjectID = requestCmd.mutable_getobjectidrequest();
				pGetObjectID->set_name("Inspections.Inspection_1");
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::InspectionCmdRequest, SvPb::InspectionCmdResponse> client(*pRpcClient);
				auto responseCmd = client.request(std::move(requestCmd), Timeout).get();
				
				///Run Once
				requestCmd = SvPb::InspectionCmdRequest{};
				requestCmd.set_inspectionid(responseCmd.getobjectidresponse().objectid());
				SvPb::InspectionRunOnceRequest* pRunOnce = requestCmd.mutable_inspectionrunoncerequest();
				pRunOnce->set_inspectionid(responseCmd.getobjectidresponse().objectid());

				client.request(std::move(requestCmd), Timeout);

			}
			else if (words[0] == "t")
			{
				SvPb::GetConfigurationTreeRequest request;
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetConfigurationTreeRequest, SvPb::GetConfigurationTreeResponse> client(*pRpcClient);
				auto response = client.request(std::move(request), Timeout).get();

				SV_LOG_GLOBAL(info) << response.DebugString();
				if (0 != response.tree().children_size())
				{
					std::string Data;
					PrintTreeItems(response.tree(), Data, _T(""));

					std::ofstream FileStream;
					FileStream.open(_T("D:\\Temp\\ConfigTree.txt"), std::ofstream::out | std::ofstream::trunc);
					if (FileStream.is_open())
					{

						FileStream.write(Data.c_str(), Data.length());
						FileStream.close();
					}
				}
			}
			else if (words[0] == "a")
			{
				SvPb::InspectionCmdRequest requestCmd;
				auto* pGetObjectID = requestCmd.mutable_getobjectidrequest();
				pGetObjectID->set_name("Inspections.Inspection_1");
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::InspectionCmdRequest, SvPb::InspectionCmdResponse> client(*pRpcClient);
				SvPb::InspectionCmdResponse responseCmd = client.request(std::move(requestCmd), Timeout).get();
				uint32_t inspectionID = responseCmd.getobjectidresponse().objectid();

				requestCmd = SvPb::InspectionCmdRequest {};
				pGetObjectID = requestCmd.mutable_getobjectidrequest();
				pGetObjectID->set_name("Inspections.Inspection_1.Tool Set");
				responseCmd.Clear();
				responseCmd = client.request(std::move(requestCmd), Timeout).get();
				uint32_t toolSetID = responseCmd.getobjectidresponse().objectid();

				requestCmd = SvPb::InspectionCmdRequest {};
				pGetObjectID = requestCmd.mutable_getobjectidrequest();
				pGetObjectID->set_name("Inspections.Inspection_1.Tool Set.Math Tool");
				responseCmd.Clear();
				responseCmd = client.request(std::move(requestCmd), Timeout).get();

				///Create object
				requestCmd = SvPb::InspectionCmdRequest {};
				requestCmd.set_inspectionid(inspectionID);
				SvPb::CreateObjectRequest* pCreateObj = requestCmd.mutable_createobjectrequest();
				pCreateObj->set_taskobjectinsertbeforeid(responseCmd.getobjectidresponse().objectid());
				pCreateObj->set_ownerid(toolSetID);
				pCreateObj->set_classid(SvPb::WindowToolClassId);

				responseCmd.Clear();
				responseCmd = client.request(std::move(requestCmd), Timeout).get();
			}
			else if (words[0] == "d")
			{
				///GetObjectID
				SvPb::InspectionCmdRequest requestCmd;
				auto* pGetObjectID = requestCmd.mutable_getobjectidrequest();
				pGetObjectID->set_name("Inspections.Inspection_1");
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::InspectionCmdRequest, SvPb::InspectionCmdResponse> client(*pRpcClient);
				SvPb::InspectionCmdResponse responseCmd = client.request(std::move(requestCmd), Timeout).get();
				uint32_t inspectionID = responseCmd.getobjectidresponse().objectid();

				requestCmd = SvPb::InspectionCmdRequest{};
				pGetObjectID = requestCmd.mutable_getobjectidrequest();
				pGetObjectID->set_name("Inspections.Inspection_1.Tool Set.Math Tool");
				responseCmd.Clear();
				responseCmd = client.request(std::move(requestCmd), Timeout).get();

				///Delete object
				requestCmd = SvPb::InspectionCmdRequest {};
				requestCmd.set_inspectionid(inspectionID);
				SvPb::DeleteObjectRequest* pDelObj = requestCmd.mutable_deleteobjectrequest();
				pDelObj->set_objectid(responseCmd.getobjectidresponse().objectid());

				responseCmd.Clear();
				responseCmd = client.request(std::move(requestCmd), Timeout).get();

			}
			else if (words[0] == "g")
			{
				SvPb::ConfigCommandRequest requestConfigCmd;
				requestConfigCmd.mutable_configdatarequest()->GetDescriptor();
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::ConfigCommandRequest, SvPb::ConfigCommandResponse> client(*pRpcClient);
				SvPb::ConfigCommandResponse responseConfigCmd = client.request(std::move(requestConfigCmd), Timeout).get();
				SV_LOG_GLOBAL(info) << "Config File : " << responseConfigCmd.configdataresponse().configfileloaded().c_str();
				SV_LOG_GLOBAL(info) << "Last modified: " << responseConfigCmd.configdataresponse().lastmodified();
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

