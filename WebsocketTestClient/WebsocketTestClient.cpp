//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RunReWebsocketClient.h
/// All Rights Reserved
//******************************************************************************
/// Interactive debugging client.
//******************************************************************************

#pragma once

#pragma region Includes
#include "stdafx.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/array.hpp>
#pragma warning(push)
#pragma warning(disable : 4702)
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "SvHttpLibrary/WebsocketClientSettings.h"
#include "SvHttpLibrary/DefaultSettings.h"
#include "SVLogLibrary/Logging.h"
#include "SVProtoBuf\SVRC.h"
#include "SVStatusLibrary/ErrorUtil.h"
#include "SVRPCLibrary/RPCClient.h"
#include "WebsocketLibrary/RunRequest.inl"
#include "WebsocketLibrary/SVRCClientService.h"
#include "WebsocketLibrary/SVRCClientServiceSettings.h"
#pragma endregion Includes

void PrintCurImage(const SvPb::ImageId& rCurrentImage)
{
	SV_LOG_GLOBAL(info) << "InspectionId: " << rCurrentImage.inspectionid() << " ImageIndex: " << rCurrentImage.imageindex()
		<< " TriggerRecord-Id: " << rCurrentImage.trid();
}
void PrintVariant(const SvPb::Variant& var)
{
	switch (var.data_case())
	{
	case SvPb::Variant::kBVal:
		SV_LOG_GLOBAL(info) << "value: " << var.bval();
		break;

	case SvPb::Variant::kLVal:
		SV_LOG_GLOBAL(info) << "value: " << var.lval();
		break;

	case SvPb::Variant::kLlVal:
		SV_LOG_GLOBAL(info) << "value: " << var.llval();
		break;

	case SvPb::Variant::kUlVal:
		SV_LOG_GLOBAL(info) << "value: " << var.ulval();
		break;

	case SvPb::Variant::kUllVal:
		SV_LOG_GLOBAL(info) << "value: " << var.ullval();
		break;

	case SvPb::Variant::kDblVal:
		SV_LOG_GLOBAL(info) << "value: " << var.dblval();
		break;

	case SvPb::Variant::kFltVal:
		SV_LOG_GLOBAL(info) << "value: " << var.fltval();
		break;

	case SvPb::Variant::kStrVal:
		SV_LOG_GLOBAL(info) << "value: " << var.strval();
		break;

	default:
		SV_LOG_GLOBAL(warning) << "Unknown variant value";
		break;
	}
}

static void GetNotifications(SvWsl::SVRCClientService& client, int wait )
{
	
	SvPb::GetNotificationStreamRequest req;
	auto ctx = client.GetNotificationStream(std::move(req), SvRpc::Observer<SvPb::GetNotificationStreamResponse>(
		[](SvPb::GetNotificationStreamResponse&& res) -> SvSyl::SVFuture<void>
		{
			//SV_LOG_GLOBAL(info) << "Received notification " << res.id() << " " << res.type() << " " << res.message();
			std::string temp = res.DebugString();
			if (res.has_event())
			{
				std::string typeName = EventType_Name(res.event().type());
				SV_LOG_GLOBAL(info) << "An event arrives: " << typeName << std::endl;
				if (res.event().eventparameters_size() > 0)
				{
					auto var = res.event().eventparameters().Get(0);
					SV_LOG_GLOBAL(info) << "with parameter: ";
					PrintVariant(var);
					SV_LOG_GLOBAL(info) << std::endl;
				}
			}			
			else
			{
				SV_LOG_GLOBAL(info) << "Received notification Debug string " << res.DebugString() << std::endl;
			}
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
	std::this_thread::sleep_for(std::chrono::seconds(wait));
	ctx.cancel();
}

static bool GetImageId(SvWsl::SVRCClientService& client, uint32_t imageWidth, SvPb::ImageId& rImageIdOut)
{
	SvPb::QueryListNameRequest mListReq;
	auto mListRes = runRequest(client, &SvWsl::SVRCClientService::QueryListName, std::move(mListReq)).get();
	for (const auto& listname : *mListRes.mutable_listname())
	{
		SvPb::GetProductRequest prodReq;
		prodReq.set_listname(listname);
		auto prodRes = runRequest(client, &SvWsl::SVRCClientService::GetProduct, std::move(prodReq)).get();
		for (const auto& imgId : prodRes.productitem().images())
		{
			SvPb::GetImageFromIdRequest imgReq;
			*imgReq.mutable_id() = imgId;
			auto imgRes = runRequest(client, &SvWsl::SVRCClientService::GetImageFromId, std::move(imgReq)).get();
			if (imgRes.imagedata().width() == imageWidth)
			{
				rImageIdOut = imgId;
				return true;
			}
		}
	}
	return false;
}
static void RunBenchmark2(SvWsl::SVRCClientService& rClient, int iterations, uint32_t imageWitdh, bool)
{
	SvPb::ImageId ImageId;
	if (!GetImageId(rClient, imageWitdh, ImageId))
	{
		SV_LOG_GLOBAL(error) << "Unable to find image for request width " << imageWitdh;
		return;
	}
	double volume = 0;
	auto start = std::chrono::steady_clock::now();

	for (int i = 0; i < iterations; ++i)
	{
		SvPb::GetImageFromIdRequest Request;
		*Request.mutable_id() = ImageId;
		auto resp = runRequest(rClient, &SvWsl::SVRCClientService::GetImageFromId, std::move(Request)).get();
		volume += resp.ByteSizeLong() / (1024.0 * 1024.0);
	}
	auto finish = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start).count();
	SV_LOG_GLOBAL(info) << "Benchmark2: iterations=" << iterations << " duration=" << duration
		<< " volume=" << volume;
}
void PrintProductResponse(const SvPb::Product& rProduct)
{
	SV_LOG_GLOBAL(info) << "Product for Trigger: " << rProduct.trigger();
	bool bpImageName = rProduct.images_size() == rProduct.imagenames_size();
	bool bpValueName = rProduct.values_size() == rProduct.valuenames_size();

	for (int i = 0; i < rProduct.images_size(); i++)
	{
		if (bpImageName)
		{
			SV_LOG_GLOBAL(info) << rProduct.imagenames(i);
		}
		PrintCurImage(rProduct.images(i));
	}
	for (int v = 0; v < rProduct.values_size(); v++)
	{
		if (bpValueName)
		{
			SV_LOG_GLOBAL(info) << rProduct.valuenames(v);
		}
		PrintVariant(rProduct.values(v));
	}
}

class Benchmark1
{
public:
	explicit Benchmark1(SvWsl::SVRCClientService& rClient) : m_rClient(rClient) {}

	enum BenchmarkType
	{
		StringBenchmark,
		ImageBenchmark,
	};

	void run(BenchmarkType benchmark_type, int num_iterations)
	{
		auto start = std::chrono::steady_clock::now();
		switch (benchmark_type)
		{
		case StringBenchmark:
			doStringBenchmark(m_rClient, num_iterations);
			break;
		case ImageBenchmark:
			doImageBenchmark(m_rClient, num_iterations);
			break;
		}
		auto finish = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start).count();
		SV_LOG_GLOBAL(info) << "Benchmark1: iterations=" << num_iterations << " duration=" << duration << "sec";
	}

private:
	void doStringBenchmark(SvWsl::SVRCClientService& rClient, int num_iterations)
	{
		for (auto i = 0; i < num_iterations; ++i)
		{
			SvPb::QueryListNameRequest request;
			auto response = runRequest(rClient, &SvWsl::SVRCClientService::QueryListName, std::move(request)).get();
		}
	}

	void doImageBenchmark(SvWsl::SVRCClientService& rClient, int num_iterations)
	{
		for (auto i = 0; i < num_iterations; ++i)
		{
			SvPb::GetImageFromIdRequest request;
			request.mutable_id()->set_trid(0);
			request.mutable_id()->set_imageindex(0);
			request.mutable_id()->set_inspectionid(0);
			auto response = runRequest(rClient, &SvWsl::SVRCClientService::GetImageFromId, std::move(request)).get();
		}
	}

private:
	SvWsl::SVRCClientService& m_rClient;
};

int main(int argc, char* argv[])
{
	SvLog::bootstrap_logging();
	SvLog::LogSettings logSettings;
	logSettings.StdoutLogEnabled = true;
	logSettings.StdoutLogLevel = "debug";
	logSettings.FileLogEnabled = false;
	logSettings.WindowsEventLogEnabled = false;
	SvLog::init_logging(logSettings);

	SvHttp::WebsocketClientSettings clientSettings;
	clientSettings.Host = "127.0.0.1";
	//	clientSettings.Host = "192.168.10.111";
	clientSettings.Port = SvHttp::Default_Port;
	if (argc > 1)
	{
		clientSettings.Host = argv[1];
	}
	if (argc > 2)
	{
		clientSettings.Port = static_cast<uint16_t> (atoi(argv[2]));
	}

	auto pRpcClient = std::make_unique<SvRpc::RPCClient>(clientSettings);
	pRpcClient->waitForConnect(boost::posix_time::seconds(6));
	SvWsl::SVRCClientServiceSetting settings;
	auto request_timeout = boost::posix_time::seconds(2);
	auto pService = std::make_unique<SvWsl::SVRCClientService>(*pRpcClient, settings);


	SV_LOG_GLOBAL(info) << "Enter a command(Ctrl-Z to stop): ";
	std::string input;
	while (std::getline(std::cin, input))
	{

		try
		{
			std::vector<std::string> words;
			boost::algorithm::split(words, input, boost::is_space(), boost::token_compress_on);
			auto wordsize = words.size();
			if (wordsize == 0)
				continue;
			if (words[0] == "q")
			{
				break;
			}
			else if (words[0] == "con")
			{
				if (wordsize > 1)
				{
					uint16_t port = SvHttp::Default_Port;
					if (wordsize > 2)
					{
						port = static_cast<uint16_t> (atoi(words[2].c_str()));
					}
					std::string ipAdress = words[1];

					pService.reset();
					pRpcClient.reset();
					pRpcClient = std::make_unique<SvRpc::RPCClient>(clientSettings);
					pRpcClient->waitForConnect(boost::posix_time::seconds(2));
					pService = std::make_unique<SvWsl::SVRCClientService>(*pRpcClient, settings);
				}
			}
			else if (words[0] == "h" || words[0] == "H")
			{
				SV_LOG_GLOBAL(info) << "commands: " << std::endl
					<< "  q  quit" << std::endl
					<< "  h  Hilfe" << std::endl
					<< "  v  (Version)" << std::endl
					<< "  n  notification" << std::endl
					<< "  m  (Monitorlist)" << std::endl
					<< "  p  name triggercount (GetProdukt)" << std::endl
					<< "  i  StoreNr  imageNr slotNr (GetImage)" << std::endl
					<< "  r  name triggercount (GetRejekt)" << std::endl
					<< "  f  name  (GetFailstatus)" << std::endl
					<< "  b1 [iterations=1000] [repeats=1] (Benchmark 1)" << std::endl
					<< "  b2 [image_width=200] [iterations=1000] [repeats=1] (Benchmark 2)" << std::endl;
				std::cout << "dis disconnect" << std::endl;
				std::cout << "con  connect [ip adress] [portnr  = " << SvHttp::Default_Port << "]\n";
				std::cout << "qli [monitorlistname] [p,r,f,a] [Image=1]  [val=1]" << std::endl;
			}
			else if (!pRpcClient || !pRpcClient->isConnected())
			{
				SV_LOG_GLOBAL(info) << "Nicht verbunde!!!" << std::endl;
			}
			else if (words[0] == "v")
			{
				try
				{

					SvPb::GetGatewayVersionRequest req;
					auto version = runRequest(*pService, &SvWsl::SVRCClientService::GetGatewayVersion, std::move(req)).get();
					SV_LOG_GLOBAL(info) << "Version: ";
					SV_LOG_GLOBAL(info) << " RunReWebsocketServer: " << version.version();
				}
				catch (const std::exception& e)
				{
					SV_LOG_GLOBAL(error) << "Unable to get version: " << e.what();
				}
				catch (...)
				{
					SV_LOG_GLOBAL(error) << "Unable to get version" << std::endl;
				}
			}
			else if (words[0] == "n")
			{
				int wait{ 10 };
				if (wordsize > 1)
				{
					int t = atoi(words[1].c_str());
					if (t > -1)
					{
						wait = t;
					}
				}

				try
				{
					GetNotifications(*pService,wait);
				}
				catch (const std::exception& e)
				{
					SV_LOG_GLOBAL(error) << "Unable to get notifications: " << e.what();
				}
			}
			else if (pRpcClient && pRpcClient->isConnected() && words[0] == "m")
			{

				auto Listnames = runRequest(*pService, &SvWsl::SVRCClientService::QueryListName, SvPb::QueryListNameRequest()).get();
				// SV_LOG_GLOBAL(info) << "QueryListNameResponse.DebugString: ";
				// SV_LOG_GLOBAL(info) << Listnames.DebugString();
				SV_LOG_GLOBAL(info) << "MonitorlistNamen: ";
				for (int i = 0; i < Listnames.listname_size(); i++)
				{
					SV_LOG_GLOBAL(info) << Listnames.listname(i);
				}
			}
			else if (words[0] == "f")
			{
				SvPb::GetFailStatusRequest FailstatusRequest;
				if (wordsize > 1)
				{
					FailstatusRequest.set_listname(words[1].c_str());
				}
				else
				{
					SV_LOG_GLOBAL(error) << "monitorname  notwendig ";
					continue;
				}
				FailstatusRequest.set_nameinresponse(true);
				auto resp = runRequest(*pService, &SvWsl::SVRCClientService::GetFailStatus, std::move(FailstatusRequest)).get();
				for (int i = 0; i < resp.products_size(); i++)
				{
					PrintProductResponse(resp.products(i));
				}
			}
			else if (words[0] == "p" || words[0] == "r")
			{
				std::string name;
				int trigger = -1;
				if (wordsize > 1)
				{
					name = words[1];
				}
				else
				{
					SV_LOG_GLOBAL(error) << "monitorname notwendig ";
					continue;
				}
				if (wordsize > 2)
				{
					int t = atoi(words[2].c_str());
					if (t > -1)
					{
						trigger = t;
					}
				}


				bool bReject = words[0] == "r";

				if (bReject)
				{
					SvPb::GetRejectRequest Request;
					Request.set_listname(name);
					Request.set_triggercount(trigger);
					Request.set_nameinresponse(true);
					Request.set_pevioustrigger(-1);

					SvPb::GetRejectResponse Response;
					Response = runRequest(*pService, &SvWsl::SVRCClientService::GetReject, std::move(Request)).get();
					PrintProductResponse(Response.productitem());
				}
				else
				{
					SvPb::GetProductRequest Request;
					Request.set_listname(name);
					Request.set_triggercount(trigger);
					Request.set_nameinresponse(true);
					Request.set_pevioustrigger(-1);

					SvPb::GetProductResponse Response;
					Response = runRequest(*pService, &SvWsl::SVRCClientService::GetProduct, std::move(Request)).get();
					PrintProductResponse(Response.productitem());
				}
			}
			else if (words[0] == "i")
			{
				if (wordsize <= 3)
				{
					SV_LOG_GLOBAL(error) << "StoreNr imageNr slotNr Eingeben";
					continue;
				}

				SvPb::GetImageFromIdRequest request;
				request.mutable_id()->set_inspectionid(atoi(words[1].c_str()));
				request.mutable_id()->set_imageindex(atoi(words[2].c_str()));
				request.mutable_id()->set_trid(atoi(words[3].c_str()));
				auto resp = runRequest(*pService, &SvWsl::SVRCClientService::GetImageFromId, std::move(request)).get();

				SV_LOG_GLOBAL(info) << "Image (Width ,Height) " << resp.imagedata().width() << "x"
					<< resp.imagedata().height();
				std::string filename = "D:\\Temp\\Images\\";
				filename += words[1] + "_" + words[2] + "_" + words[3] + "_";
				static int debug_image_count = 0;
				filename += std::to_string(debug_image_count++) + ".bmp";
				std::ofstream os(filename, std::ios_base::binary);
				os.write((char*)resp.imagedata().rgbdata().data(), resp.imagedata().rgbdata().size());
				os.close();
				// os.write((char *)iarray, sizeof(iarray));
			}
			else if (words[0] == "b1" || words[0] == "B1")
			{
				int iterations = 1000;
				int repeats = 1;
				if (wordsize >= 2)
				{
					iterations = atoi(words[1].c_str());
				}
				if (wordsize >= 3)
				{
					repeats = atoi(words[2].c_str());
				}
				for (int i = 0; i < repeats; ++i)
				{
					Benchmark1(*pService).run(Benchmark1::StringBenchmark, iterations);
				}
			}
			else if (words[0] == "b2" || words[0] == "B2")
			{
				uint32_t imgWidth = 200;
				int iterations = 1000;
				int repeats = 1;
				if (wordsize >= 2)
				{
					imgWidth = static_cast<uint32_t> (atoi(words[1].c_str()));
				}
				if (wordsize >= 3)
				{
					iterations = atoi(words[2].c_str());
				}
				if (wordsize >= 4)
				{
					repeats = atoi(words[3].c_str());
				}
				for (int i = 0; i < repeats; ++i)
				{
					RunBenchmark2(*pService, iterations, imgWidth, false);
				}
			}
			else if (words[0] == "b3" || words[0] == "B3")
			{
				uint32_t imgWidth = 200;
				int iterations = 1000;
				int repeats = 1;
				if (wordsize >= 2)
				{
					imgWidth = static_cast<uint32_t> (atoi(words[1].c_str()));
				}
				if (wordsize >= 3)
				{
					iterations = atoi(words[2].c_str());
				}
				if (wordsize >= 4)
				{
					repeats = atoi(words[3].c_str());
				}
				for (int i = 0; i < repeats; ++i)
				{
					RunBenchmark2(*pService, iterations, imgWidth, true);
				}
			}
			else  if (words[0] == "dis")
			{
				if (pService)
				{
					pService.reset();
				}
				if (pRpcClient)
				{
					pRpcClient.reset();
				}
			}

			else if (words[0] == "qli")
			{
				std::string monitorlistname;
				SvPb::ListType  t{ SvPb::ListType::all };
				bool bImage{ true };
				bool bValues{ true };
				if (wordsize >= 2)
				{
					monitorlistname = words[1].c_str();
				}
				if (wordsize >= 3)
				{
					if (words[2] == "p")
					{
						t = SvPb::ListType::productItem;
					}
					else if (words[2] == "r")
					{
						t = SvPb::ListType::rejectCondition;
					}
					else if (words[2] == "f")
					{
						t = SvPb::ListType::failStatus;
					}

				}
				if (wordsize >= 4)
				{
					int temp = atoi(words[3].c_str());
					bImage = temp > 0;
				}
				if (wordsize >= 5)
				{
					int temp = atoi(words[4].c_str());
					bValues = temp > 0;
				}
				SvPb::QueryListItemRequest  request;
				request.set_listname(monitorlistname.c_str());
				request.set_type(t);
				request.set_queryimages(bImage);
				request.set_queryvalues(bValues);

				auto resp = runRequest(*pService, &SvWsl::SVRCClientService::QueryListItem, std::move(request)).get();

				std::cout << "QueryListItemResponse .DebugString: " << std::endl;
				std::cout << resp.DebugString() << std::endl;



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
