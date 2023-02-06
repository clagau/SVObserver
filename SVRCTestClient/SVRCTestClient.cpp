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
			resp.type() << " DEBUGSTRING: " << resp.DebugString() << std::endl;
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
		rData += rSpacing + SvUl::to_ansi(rChildItem.name()) + _T("\n");
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
DWORD GetObjectId(const std::string& dottedName, SvRpc::RPCClient& rClient)
{
	//No Inspection id necessary!!
	auto Timeout = boost::posix_time::seconds(60);
	SvPb::InspectionCmdRequest requestCmd;
	auto* pGetObjectID_Req = requestCmd.mutable_getobjectidrequest();
	pGetObjectID_Req->set_name(dottedName.c_str());
	SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::InspectionCmdRequest, SvPb::InspectionCmdResponse> client(rClient);
	auto responseCmd = client.request(std::move(requestCmd), Timeout).get();
	return responseCmd.getobjectidresponse().objectid();

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

	NotificationHandler notificationHandler;
	auto nextNotificationFunction = [&notificationHandler](const SvPb::GetNotificationStreamResponse& rResponse) { return notificationHandler.OnNext(rResponse); };
	auto finishNotificationFunction = [&notificationHandler]() { return notificationHandler.OnFinish(); };
	auto errorNotificationFunction = [&notificationHandler](const SvPenv::Error& error) { return notificationHandler.OnError(error); };

	SvRpc::Observer<SvPb::GetNotificationStreamResponse> NotificationObserver(nextNotificationFunction, finishNotificationFunction, errorNotificationFunction);
	SvRpc::ClientStreamContext csx(nullptr);


	std::unique_ptr< SvRpc::RPCClient> pRpcClient;
	try
	{
		pRpcClient = std::make_unique<SvRpc::RPCClient>(ClientSettings);
		pRpcClient->waitForConnect(boost::posix_time::seconds(6));
	}

	catch (std::exception& e)
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
					<< "  cc clipboard copy tool" << std::endl
					<< "  cv clipboard paste tool" << std::endl
					<< "  cx clipboard cut tool" << std::endl
					<< "  g get config data" << std::endl
					<< " gci  get configuration info" << std::endl
					<< " s  software trigger" << std::endl
					<< " set_v  {value} {n, o, i, t }  inspection command set embedded value" << std::endl
					<< " set_l  {value} {n, o, i, t }  inspection command set embedded linked value" << std::endl
					<< " get   {BAR ,BA, AR }  inspection command get embedded value" << std::endl
					<< " p {x, y, w, h} {value} Adjust Tool Position and Size" << std::endl;
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

				csx = streamClient.stream(std::move(getNotificationStreamRequest), NotificationObserver);

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
				requestCmd = SvPb::InspectionCmdRequest {};
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
				pGetObjectID->set_name("Inspections.Inspection_1.Tool Set");	//"Inspections.Inspection_1.Tool Set.LoopTool"
				responseCmd.Clear();
				responseCmd = client.request(std::move(requestCmd), Timeout).get();
				uint32_t parentID = responseCmd.getobjectidresponse().objectid();

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
				pCreateObj->set_ownerid(parentID);
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

				requestCmd = SvPb::InspectionCmdRequest {};
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
			else if (words[0] == "cc")
			{
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::InspectionCmdRequest, SvPb::InspectionCmdResponse> clientInspection(*pRpcClient);
				SvPb::InspectionCmdRequest requestCmd;
				auto* pGetObjectID = requestCmd.mutable_getobjectidrequest();
				pGetObjectID->set_name("Inspections.Inspection_1.Tool Set.Math Tool");
				SvPb::InspectionCmdResponse responseCmd = clientInspection.request(std::move(requestCmd), Timeout).get();

				SvPb::ConfigCommandRequest requestConfigCmd;
				auto* pClipboard = requestConfigCmd.mutable_clipboardrequest();
				pClipboard->set_action(SvPb::ClipboardActionEnum::Copy);
				pClipboard->set_objectid(responseCmd.getobjectidresponse().objectid());
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::ConfigCommandRequest, SvPb::ConfigCommandResponse> client(*pRpcClient);
				client.request(std::move(requestConfigCmd), Timeout);
			}
			else if (words[0] == "cv")
			{
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::InspectionCmdRequest, SvPb::InspectionCmdResponse> clientInspection(*pRpcClient);
				SvPb::InspectionCmdRequest requestCmd;
				auto* pGetObjectID = requestCmd.mutable_getobjectidrequest();
				pGetObjectID->set_name("Inspections.Inspection_1.Tool Set.Archive Tool");
				SvPb::InspectionCmdResponse responseCmd = clientInspection.request(std::move(requestCmd), Timeout).get();

				SvPb::ConfigCommandRequest requestConfigCmd;
				auto* pClipboard = requestConfigCmd.mutable_clipboardrequest();
				pClipboard->set_action(SvPb::ClipboardActionEnum::Paste);
				pClipboard->set_objectid(responseCmd.getobjectidresponse().objectid());
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::ConfigCommandRequest, SvPb::ConfigCommandResponse> client(*pRpcClient);
				client.request(std::move(requestConfigCmd), Timeout);
			}
			else if (words[0] == "cx")
			{
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::InspectionCmdRequest, SvPb::InspectionCmdResponse> clientInspection(*pRpcClient);
				SvPb::InspectionCmdRequest requestCmd;
				auto* pGetObjectID = requestCmd.mutable_getobjectidrequest();
				pGetObjectID->set_name("Inspections.Inspection_1.Tool Set.Math Tool");
				SvPb::InspectionCmdResponse responseCmd = clientInspection.request(std::move(requestCmd), Timeout).get();

				SvPb::ConfigCommandRequest requestConfigCmd;
				auto* pClipboard = requestConfigCmd.mutable_clipboardrequest();
				pClipboard->set_action(SvPb::ClipboardActionEnum::Cut);
				pClipboard->set_objectid(responseCmd.getobjectidresponse().objectid());
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::ConfigCommandRequest, SvPb::ConfigCommandResponse> client(*pRpcClient);
				client.request(std::move(requestConfigCmd), Timeout);
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
			else if (words[0] == "gci")
			{
				SvPb::GetConfigurationInfoRequest requestConfigInfo;

				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::GetConfigurationInfoRequest, SvPb::GetConfigurationInfoResponse> client(*pRpcClient);
				SvPb::GetConfigurationInfoResponse response = client.request(std::move(requestConfigInfo), Timeout).get();
				SV_LOG_GLOBAL(info) << "Config File : " << response.filename().c_str();
				SV_LOG_GLOBAL(info) << "Last modified: " << response.lastmodified();
				SV_LOG_GLOBAL(info) << "Loaded Since: " << response.loadedsince();
				SV_LOG_GLOBAL(info) << "hash: " << response.hash();
			}
			else if (words[0] == "s")
			{
				SvPb::SoftwareTriggerRequest requestSoftwareTrigger;
				requestSoftwareTrigger.set_inspectionname("Inspection_1");
				requestSoftwareTrigger.set_period(0L);
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::SoftwareTriggerRequest, SvPb::StandardResponse> client(*pRpcClient);
				auto response = client.request(std::move(requestSoftwareTrigger), Timeout).get();
			}
			else if (words[0] == "cn")
			{

				csx.cancel();

			}
			else if (words[0] == "set_v")
			{

				///set embedded values
					///GetObjectID
				DWORD OIDArchiveTool = GetObjectId("Inspections.Inspection_1.Tool Set.Archive Tool", *pRpcClient);
				DWORD OIDArchiveToolMaxImageCount = GetObjectId("Inspections.Inspection_1.Tool Set.Archive Tool.Archive Max Images Count", *pRpcClient);
				DWORD ObjectIdInspection = GetObjectId("Inspections.Inspection_1", *pRpcClient);
				//DWORD ObjectIdAnalyzer = GetObjectId("Inspections.Inspection_1.Tool Set.Window Tool.Blob Analyzer", *pRpcClient);
				//DWORD ObjectIdMaxNumberofBlobs = GetObjectId("Inspections.Inspection_1.Tool Set.Window Tool.Blob Analyzer.Max Number of Blobs", *pRpcClient);

				SvPb::InspectionCmdRequest requestCmd = SvPb::InspectionCmdRequest {};

				///Run SetEmbeddedValuesRequest
				requestCmd = SvPb::InspectionCmdRequest {};
				requestCmd.set_inspectionid(ObjectIdInspection);
				SvPb::SetEmbeddedValuesRequest* pEmbeddevalueRequest = requestCmd.mutable_setembeddedvaluesrequest();
				//pEmbeddevalueRequest->set_objectid(ObjectIdAnalyzer);
				pEmbeddevalueRequest->set_objectid(OIDArchiveTool);

				SvPb::SetValueObjectValues* pSetValueObjectValues = pEmbeddevalueRequest->add_list();
				pSetValueObjectValues->set_ismodified(true);
				pSetValueObjectValues->set_isdefaultmodified(false);
				SvPb::ValueObjectValues* pvalueOpbectvalues = pSetValueObjectValues->mutable_values();
				//pvalueOpbectvalues->set_objectid(ObjectIdMaxNumberofBlobs);
				pvalueOpbectvalues->set_objectid(OIDArchiveToolMaxImageCount);
				SvPb::StandardValue* pVvalue = pvalueOpbectvalues->mutable_value();
				SvPb::Variant* pVariant = pVvalue->mutable_value();
				int val = 24;
				if (words.size() > 1)
				{
					val = std::stoi(words[1]);
				}
				SvPb::ResetType rt = SvPb::ResetType::RT_FromObject;
				if (words.size() > 2)
				{
					if (words[2] == "n")
						rt = SvPb::ResetType::RT_None;
					else if (words[2] == "o")
						rt = SvPb::ResetType::RT_FromObject;
					else if (words[2] == "i")
						rt = SvPb::ResetType::RT_IP;
					else if (words[2] == "t")
						rt = SvPb::ResetType::RT_Tool;
				}
				pEmbeddevalueRequest->set_resettype(rt);

				//pVariant->set_lval(val);
				//pVariant->set_type(VT_I4);
				pVariant->set_ulval(val);
				pVariant->set_type(VT_UI4);
				SvPb::Variant* pDVariant = pVvalue->mutable_defaultvalue();
				//pDVariant->set_lval(24);
				//pDVariant->set_type(VT_I4);
				pDVariant->set_ulval(88);
				pDVariant->set_type(VT_UI4);

				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::InspectionCmdRequest, SvPb::InspectionCmdResponse> client(*pRpcClient);
				SvPb::InspectionCmdResponse cmdresp = client.request(std::move(requestCmd), Timeout).get();
				HRESULT hres = cmdresp.hresult();
				SV_LOG_GLOBAL(info) << cmdresp.DebugString() << std::endl;
				SV_LOG_GLOBAL(info) << "hres  : " << hres << std::endl;
			}
			else if (words[0] == "set_l")
			{

				DWORD ObjectIdInspection = GetObjectId("Inspections.Inspection_1", *pRpcClient);
				// DWORD ObjectIdTool = GetObjectId("Inspections.Inspection_1.Tool Set.Window Tool", *pRpcClient);
				//	DWORD ObjectIdAnalyzer = GetObjectId("Inspections.Inspection_1.Tool Set.Window Tool.Blob Analyzer", *pRpcClient);
				//	DWORD ObjectIdMaxNumberofBlobs = GetObjectId("Inspections.Inspection_1.Tool Set.Window Tool.Blob Analyzer.Max Number of Blobs", *pRpcClient);
				DWORD ObjectIdBlobAnalyzerResultAreaRange = GetObjectId("Inspections.Inspection_1.Tool Set.Window Tool.Blob Analyzer.Result Area.Range", *pRpcClient);
				// DWORD ObjectIdBlobAnalyzerResultAreaRangeFailHigh = GetObjectId("Inspections.Inspection_1.Tool Set.Window Tool.Blob Analyzer.Result Area.Range.Fail High", *pRpcClient);
				DWORD ObjectIdBlobAnalyzerResultAreaRangeWarnHigh = GetObjectId("Inspections.Inspection_1.Tool Set.Window Tool.Blob Analyzer.Result Area.Range.Warn High", *pRpcClient);


				///Run SetEmbeddedValuesRequest

				SvPb::InspectionCmdRequest requestCmd = SvPb::InspectionCmdRequest {};
				requestCmd.set_inspectionid(ObjectIdInspection);
				SvPb::SetEmbeddedValuesRequest* pEmbeddevalueRequest = requestCmd.mutable_setembeddedvaluesrequest();
				pEmbeddevalueRequest->set_objectid(ObjectIdBlobAnalyzerResultAreaRange);
				//pEmbeddevalueRequest->set_objectid(ObjectIdTool);

				SvPb::SetValueObjectValues* pSetValueObjectValues = pEmbeddevalueRequest->add_list();
				pSetValueObjectValues->set_ismodified(true);
				pSetValueObjectValues->set_isdefaultmodified(false);
				SvPb::ValueObjectValues* pvalueOpbectvalues = pSetValueObjectValues->mutable_values();
				pvalueOpbectvalues->set_objectid(ObjectIdBlobAnalyzerResultAreaRangeWarnHigh);
				SvPb::LinkedValue* pLvalue = pvalueOpbectvalues->mutable_linkedvalue();


				int val = 500;
				if (words.size() > 1)
				{
					val = std::stoi(words[1]);
				}
				SvPb::ResetType rt = SvPb::ResetType::RT_FromObject;
				if (words.size() > 2)
				{
					if (words[2] == "n")
						rt = SvPb::ResetType::RT_None;
					else if (words[2] == "o")
						rt = SvPb::ResetType::RT_FromObject;
					else if (words[2] == "i")
						rt = SvPb::ResetType::RT_IP;
					else if (words[2] == "t")
						rt = SvPb::ResetType::RT_Tool;
				}
				pEmbeddevalueRequest->set_resettype(rt);
				SvPb::Variant* pdirectVariant = pLvalue->mutable_directvalue();
				pdirectVariant->set_dblval(val);
				pdirectVariant->set_type(VT_R8);

				SvPb::Variant* pVariant = pLvalue->mutable_value();
				pVariant->set_dblval(val);
				pVariant->set_type(VT_R8);
				SvPb::Variant* pDVariant = pLvalue->mutable_defaultvalue();
				pDVariant->set_dblval(24);
				pDVariant->set_type(VT_R8);
				pLvalue->set_option(SvPb::DirectValue);
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::InspectionCmdRequest, SvPb::InspectionCmdResponse> client(*pRpcClient);
				SvPb::InspectionCmdResponse cmdresp = client.request(std::move(requestCmd), Timeout).get();
				HRESULT hres = cmdresp.hresult();
				SV_LOG_GLOBAL(info) << cmdresp.DebugString() << std::endl;
				SV_LOG_GLOBAL(info) << "hres  : " << hres << std::endl;
			}

			else if (words[0] == "get")
			{

				DWORD ObjectIdInspection = GetObjectId("Inspections.Inspection_1", *pRpcClient);
				//	DWORD ObjectIdTool = GetObjectId("Inspections.Inspection_1.Tool Set.Window Tool", *pRpcClient);
				DWORD ObjectIdAnalyzer = GetObjectId("Inspections.Inspection_1.Tool Set.Window Tool.Blob Analyzer", *pRpcClient);
				//	DWORD ObjectIdMaxNumberofBlobs = GetObjectId("Inspections.Inspection_1.Tool Set.Window Tool.Blob Analyzer.Max Number of Blobs", *pRpcClient);
				DWORD ObjectIdBlobAnalyzerResultAreaRange = GetObjectId("Inspections.Inspection_1.Tool Set.Window Tool.Blob Analyzer.Result Area.Range", *pRpcClient);
				DWORD OIDArchiveTool = GetObjectId("Inspections.Inspection_1.Tool Set.Archive Tool", *pRpcClient);

				///Run GetEmbeddedValuesRequest 
				SvPb::InspectionCmdRequest requestCmd = SvPb::InspectionCmdRequest {};
				requestCmd.set_inspectionid(ObjectIdInspection);
				SvPb::GetEmbeddedValuesRequest* pEmbeddevalueRequest = requestCmd.mutable_getembeddedvaluesrequest();

				if (words.size() > 1 && words[1] == "BAR")
				{
					pEmbeddevalueRequest->set_objectid(ObjectIdBlobAnalyzerResultAreaRange);
				}
				else if (words.size() > 1 && words[1] == "BA")
				{
					pEmbeddevalueRequest->set_objectid(ObjectIdAnalyzer);
				}
				else
				{
					pEmbeddevalueRequest->set_objectid(OIDArchiveTool);
				}

				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::InspectionCmdRequest, SvPb::InspectionCmdResponse> client(*pRpcClient);
				SvPb::InspectionCmdResponse cmdresp = client.request(std::move(requestCmd), Timeout).get();
				HRESULT hres = cmdresp.hresult();

				if (cmdresp.has_getembeddedvaluesresponse())
				{
					SV_LOG_GLOBAL(info) << "GetEmbedetvalueResponse   : " << cmdresp.getembeddedvaluesresponse().DebugString() << std::endl;

				}

				SV_LOG_GLOBAL(info) << "hres  : " << hres << std::endl;
			}
			else if (words[0] == "p")
			{
				SvPb::InspectionCmdRequest requestCmd;
				auto* pGetObjectID = requestCmd.mutable_getobjectidrequest();
				pGetObjectID->set_name("Inspections.Inspection_1");
				SvRpc::SimpleClient<SvPb::SVRCMessages, SvPb::InspectionCmdRequest, SvPb::InspectionCmdResponse> client(*pRpcClient);
				SvPb::InspectionCmdResponse responseCmd = client.request(std::move(requestCmd), Timeout).get();
				uint32_t inspectionID = responseCmd.getobjectidresponse().objectid();

				requestCmd = SvPb::InspectionCmdRequest {};
				pGetObjectID = requestCmd.mutable_getobjectidrequest();
				pGetObjectID->set_name("Inspections.Inspection_1.Tool Set.Window Tool");
				responseCmd.Clear();
				responseCmd = client.request(std::move(requestCmd), Timeout).get();
				uint32_t windowToolID = responseCmd.getobjectidresponse().objectid();

				requestCmd = SvPb::InspectionCmdRequest {};
				pGetObjectID = requestCmd.mutable_getobjectidrequest();
				auto* pInfo = pGetObjectID->mutable_info();
				pInfo->set_ownerid(windowToolID);
				auto* pInfoStruct = pInfo->mutable_infostruct();
				pInfoStruct->set_objecttype(SvPb::SVToolSizeAdjustTaskType);
				responseCmd.Clear();
				responseCmd = client.request(std::move(requestCmd), Timeout).get();
				uint32_t adjustTaskID = responseCmd.getobjectidresponse().objectid();

				
				SvPb::SVExtentPropertyEnum extentProperty {SvPb::SVExtentPropertyEnum::SVExtentPropertyPositionPointX};
				SvPb::EmbeddedIdEnum embeddedID {SvPb::EmbeddedIdEnum::NoEmbeddedId};
				if (words.size() > 1)
				{
					if (words[1] == "x")
					{
						extentProperty = SvPb::SVExtentPropertyEnum::SVExtentPropertyPositionPointX;
						embeddedID = SvPb::EmbeddedIdEnum::ToolSizeAdjustSizePositionXModeEId;
					}
					else if (words[1] == "y")
					{
						extentProperty = SvPb::SVExtentPropertyEnum::SVExtentPropertyPositionPointY;
						embeddedID = SvPb::EmbeddedIdEnum::ToolSizeAdjustSizePositionYModeEId;
					}
					else if (words[1] == "w")
					{
						extentProperty = SvPb::SVExtentPropertyEnum::SVExtentPropertyWidth;
						embeddedID = SvPb::EmbeddedIdEnum::ToolSizeAdjustSizeWidthModeEId;
					}
					else if (words[1] == "h")
					{
						extentProperty = SvPb::SVExtentPropertyEnum::SVExtentPropertyHeight;
						embeddedID = SvPb::EmbeddedIdEnum::ToolSizeAdjustSizeHeightModeEId;
					}
				}

				int value {10};
				if (words.size() > 2)
				{
					value = std::stoi(words[2]);
				}

				requestCmd = SvPb::InspectionCmdRequest {};
				requestCmd.set_inspectionid(inspectionID);
				SvPb::GetEmbeddedValuesRequest* pEmbeddevalueRequest = requestCmd.mutable_getembeddedvaluesrequest();
				pEmbeddevalueRequest->set_objectid(adjustTaskID);
				responseCmd.Clear();
				responseCmd = client.request(std::move(requestCmd), Timeout).get();
				long mode {-1};
				if (responseCmd.has_getembeddedvaluesresponse())
				{
					for (int i = 0; i < responseCmd.getembeddedvaluesresponse().list_size(); ++i)
					{
						if (embeddedID == responseCmd.getembeddedvaluesresponse().list(i).embeddedid())
						{
							mode = responseCmd.getembeddedvaluesresponse().list(i).value().value().lval();
							break;
						}
					}
				}
				//Mode 0 is fixed value every other mode value is not editable!
				if (0 == mode)
				{
					requestCmd = SvPb::InspectionCmdRequest {};
					requestCmd.set_inspectionid(inspectionID);
					SvPb::SetExtentParameterRequest* pRequest = requestCmd.mutable_setextentparameterrequest();
					pRequest->set_objectid(windowToolID);
					auto* pParameter = pRequest->mutable_extentlist()->mutable_extentlist()->Add();
					pParameter->set_type(extentProperty);
					pParameter->set_value(value);
					pParameter->set_filteredoutflag(false);
					pParameter->set_issetbyreset(false);

					responseCmd = client.request(std::move(requestCmd), Timeout).get();
					if (S_OK == responseCmd.hresult())
					{
						printf("successful\r\n");
					}
					else
					{
						printf("failed setting value\r\n");
					}
				}
				else
				{
					printf("failed not editable\r\n");
				}
			}
			else
			{
				SV_LOG_GLOBAL(info) << "Unknown Command" << std::endl;
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

