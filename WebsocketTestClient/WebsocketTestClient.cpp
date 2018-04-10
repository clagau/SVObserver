//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RunReWebsocketClient.h
/// All Rights Reserved
//******************************************************************************
/// Interactive debugging client.
//******************************************************************************

#pragma once

#include "stdafx.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/thread.hpp>

#include "WebsocketLibrary/ClientService.h"
#include "WebsocketLibrary/Logging.h"
#include "SVProtoBuf\SVRC.h"
#include "SvHttpLibrary/WebsocketClientFactory.h"
#include "SVRPCLibrary/ErrorUtil.h"
#include "SVRPCLibrary/RPCClient.h"
#include "WebsocketLibrary/RunRequest.inl"
#include "WebsocketLibrary/Definition.h"


void PrintCurImage(const SvPb::CurImageId& rCurrentImage)
{
	BOOST_LOG_TRIVIAL(info) << "StoreIndex: " << rCurrentImage.imagestore() << " ImageIndex: " << rCurrentImage.imageindex()
		<< " SlotIndex: " << rCurrentImage.slotindex();
}
void PrintVariant(const SvPb::Variant& var)
{
	switch (var.value_case())
	{
		case SvPb::Variant::kBoolValue:
			BOOST_LOG_TRIVIAL(info) << "value: " << var.bool_value();
			break;

		case SvPb::Variant::kInt32Value:
			BOOST_LOG_TRIVIAL(info) << "value: " << var.int32_value();
			break;

		case SvPb::Variant::kInt64Value:
			BOOST_LOG_TRIVIAL(info) << "value: " << var.int64_value();
			break;

		case SvPb::Variant::kUint32Value:
			BOOST_LOG_TRIVIAL(info) << "value: " << var.uint32_value();
			break;

		case SvPb::Variant::kUint64Value:
			BOOST_LOG_TRIVIAL(info) << "value: " << var.uint64_value();
			break;

		case SvPb::Variant::kDoubleValue:
			BOOST_LOG_TRIVIAL(info) << "value: " << var.double_value();
			break;

		case SvPb::Variant::kFloatValue:
			BOOST_LOG_TRIVIAL(info) << "value: " << var.float_value();
			break;

		case SvPb::Variant::kStringValue:
			BOOST_LOG_TRIVIAL(info) << "value: " << var.string_value();
			break;

		default:
			BOOST_LOG_TRIVIAL(warning) << "Unknown variant value";
			break;
	}
}

static void GetNotifications(SvWsl::ClientService& client)
{
	SvPb::GetNotificationStreamRequest req;
	auto ctx = client.getNotificationStream(std::move(req), SvRpc::Observer<SvPb::GetNotificationStreamResponse>(
		[](SvPb::GetNotificationStreamResponse&& res) -> std::future<void>
	{
		BOOST_LOG_TRIVIAL(info) << "Received notification " << res.id() << " " << res.type() << " " << res.message();
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
	std::this_thread::sleep_for(std::chrono::seconds(2));
	ctx.cancel();
}

static bool GetImageId(SvWsl::ClientService& client, int imageWidth, SvPb::CurImageId& rImageIdOut)
{
	SvPb::QueryListNameRequest mListReq;
	auto mListRes = runRequest(client, &SvWsl::ClientService::queryListName, std::move(mListReq)).get();
	for (const auto& listname : *mListRes.mutable_listname())
	{
		SvPb::GetProductRequest prodReq;
		prodReq.set_name(listname);
		auto prodRes = runRequest(client, &SvWsl::ClientService::getProduct, std::move(prodReq)).get();
		for (const auto& imgId : prodRes.product().images())
		{
			SvPb::GetImageFromCurIdRequest imgReq;
			*imgReq.mutable_id() = imgId;
			auto imgRes = runRequest(client, &SvWsl::ClientService::getImageFromCurId, std::move(imgReq)).get();
			if (imgRes.imagedata().w() == imageWidth)
			{
				rImageIdOut = imgId;
				return true;
			}
		}
	}
	return false;
}
static void RunBenchmark2(SvWsl::ClientService& rClient, int iterations, int imageWitdh, bool use_streaming)
{
	SvPb::CurImageId ImageId;
	if (!GetImageId(rClient, imageWitdh, ImageId))
	{
		BOOST_LOG_TRIVIAL(error) << "Unable to find image for request width " << imageWitdh;
		return;
	}
	double volume = 0;
	auto start = std::chrono::steady_clock::now();
	if (use_streaming)
	{
		SvPb::GetImageStreamFromCurIdRequest request;
		request.set_count(iterations);
		*request.mutable_id() = ImageId;
		auto response = runStream(rClient, &SvWsl::ClientService::getImageStreamFromCurId, std::move(request), volume).get();
	}
	else
	{
		for (int i = 0; i < iterations; ++i)
		{
			SvPb::GetImageFromCurIdRequest Request;
			*Request.mutable_id() = ImageId;
			auto resp = runRequest(rClient, &SvWsl::ClientService::getImageFromCurId, std::move(Request)).get();
			volume += resp.ByteSize() / (1024.0 * 1024.0);
		}
	}
	auto finish = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start).count();
	BOOST_LOG_TRIVIAL(info) << "Benchmark2: iterations=" << iterations << " duration=" << duration
		<< " volume=" << volume;
}
void PrintProductResponse(const SvPb::Product& rProduct)
{
	BOOST_LOG_TRIVIAL(info) << "Product for Trigger: " << rProduct.trigger();
	bool bpImageName = rProduct.images_size() == rProduct.imagenames_size();
	bool bpValueName = rProduct.values_size() == rProduct.valuenames_size();

	for (int i = 0; i < rProduct.images_size(); i++)
	{
		if (bpImageName)
		{
			BOOST_LOG_TRIVIAL(info) << rProduct.imagenames(i);
		}
		PrintCurImage(rProduct.images(i));
	}
	for (int v = 0; v < rProduct.values_size(); v++)
	{
		if (bpValueName)
		{
			BOOST_LOG_TRIVIAL(info) << rProduct.valuenames(v);
		}
		PrintVariant(rProduct.values(v));
	}
}

class Benchmark1
{
public:
	Benchmark1(SvWsl::ClientService& rClient) : m_rClient(rClient) {}

	enum BenchmarkType
	{
		StringBenchmark,
		ImageBenchmark,
		ImageStreamBenchmark,
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
			case ImageStreamBenchmark:
				doImageStreamBenchmark(m_rClient, num_iterations);
				break;
		}
		auto finish = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start).count();
		BOOST_LOG_TRIVIAL(info) << "Benchmark1: iterations=" << num_iterations << " duration=" << duration << "sec";
	}

private:
	void doStringBenchmark(SvWsl::ClientService& rClient, int num_iterations)
	{
		for (auto i = 0; i < num_iterations; ++i)
		{
			SvPb::QueryListNameRequest request;
			auto response = runRequest(rClient, &SvWsl::ClientService::queryListName, std::move(request)).get();
		}
	}

	void doImageBenchmark(SvWsl::ClientService& rClient, int num_iterations)
	{
		for (auto i = 0; i < num_iterations; ++i)
		{
			SvPb::GetImageFromCurIdRequest request;
			request.mutable_id()->set_slotindex(0); // TODO: slotindex should be product-index
			request.mutable_id()->set_imageindex(0);
			request.mutable_id()->set_imagestore(0); // TODO: what is this? MonitorList?
			auto response = runRequest(rClient, &SvWsl::ClientService::getImageFromCurId, std::move(request)).get();
		}
	}

	void doImageStreamBenchmark(SvWsl::ClientService& rClient, int num_iterations)
	{
		SvPb::GetImageStreamFromCurIdRequest request;
		request.set_count(num_iterations);
		request.mutable_id()->set_slotindex(0); // TODO: slotindex should be product-index
		request.mutable_id()->set_imageindex(0);
		request.mutable_id()->set_imagestore(0); // TODO: what is this? MonitorList?
		double volume = 0;
		auto response = runStream(rClient, &SvWsl::ClientService::getImageStreamFromCurId, std::move(request), volume).get();
	}

private:
	SvWsl::ClientService& m_rClient;
};

int main(int argc, char* argv[])
{
	
	SvWsl::LogSettings logSettings;
	logSettings.log_level = "debug";
	logSettings.log_to_stdout_enabled = true;
	logSettings.windows_event_log_enabled = false;
	init_logging(logSettings);

	std::string host("127.0.0.1");
	std::string stPort("8080");
	uint16_t port(SvWsl::Default_Port);
	if (argc > 1)
		host = argv[1];
	if (argc > 2)
	{
		stPort = argv[2];
		port = atoi(stPort.c_str());
	}
	
	auto pRpcClient = std::make_unique<SvRpc::RPCClient>(host,port);
	pRpcClient->waitForConnect(6000);

	auto request_timeout = boost::posix_time::seconds(2);
	auto pService = std::make_unique<SvWsl::ClientService>(*pRpcClient);


	BOOST_LOG_TRIVIAL(info) << "Enter a command(Ctrl-Z to stop): ";
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

			
			else if (words[0] == "v")
			{
				try
				{

					SvPb::GetVersionRequest req;
					req.set_trigger_timeout(true);
					auto version = runRequest(*pService, &SvWsl::ClientService::getVersion, std::move(req)).get();
					BOOST_LOG_TRIVIAL(info) << "Version: ";
					BOOST_LOG_TRIVIAL(info) << " RunReWebsocketServer: " << version.version();
				}
				catch (const std::exception& e)
				{
					BOOST_LOG_TRIVIAL(error) << "Unable to get version: " << e.what();
				}
			}
			else if (words[0] == "n")
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
			else if (pRpcClient && pRpcClient->isConnected() && words[0] == "m" )
			{

				auto Listnames = runRequest(*pService, &SvWsl::ClientService::queryListName, SvPb::QueryListNameRequest()).get();
				// BOOST_LOG_TRIVIAL(info) << "QueryListNameResponse.DebugString: ";
				// BOOST_LOG_TRIVIAL(info) << Listnames.DebugString();
				BOOST_LOG_TRIVIAL(info) << "MonitorlistNamen: ";
				for (int i = 0; i < Listnames.listname_size(); i++)
				{
					BOOST_LOG_TRIVIAL(info) << Listnames.listname(i);
				}
			}
			else if (words[0] == "f")
			{
				SvPb::GetFailStatusRequest FailstatusRequest;
				if (wordsize > 1)
				{
					FailstatusRequest.set_name(words[1].c_str());
				}
				else
				{
					BOOST_LOG_TRIVIAL(error) << "monitorname  notwendig ";
					continue;
				}
				FailstatusRequest.set_nameinresponse(true);
				auto resp = runRequest(*pService, &SvWsl::ClientService::getFailStatus, std::move(FailstatusRequest)).get();
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
					BOOST_LOG_TRIVIAL(error) << "monitorname notwendig ";
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

				SvPb::GetProductRequest ProductRequest;
				ProductRequest.set_name(name);
				ProductRequest.set_trigger(trigger);
				ProductRequest.set_nameinresponse(true);
				ProductRequest.set_breject(bReject);
				ProductRequest.set_pevioustrigger(-1);

				SvPb::GetProductResponse resp;
				resp = runRequest(*pService, &SvWsl::ClientService::getProduct, std::move(ProductRequest)).get();
				PrintProductResponse(resp.product());


			}
			else if (words[0] == "i")
			{
				if (wordsize <= 3)
				{
					BOOST_LOG_TRIVIAL(error) << "StoreNr imageNr slotNr Eingeben";
					continue;
				}

				SvPb::GetImageFromCurIdRequest request;
				request.mutable_id()->set_imagestore(atoi(words[1].c_str()));
				request.mutable_id()->set_imageindex(atoi(words[2].c_str()));
				request.mutable_id()->set_slotindex(atoi(words[3].c_str()));
				auto resp = runRequest(*pService, &SvWsl::ClientService::getImageFromCurId, std::move(request)).get();

				BOOST_LOG_TRIVIAL(info) << "Image (Width ,Height) " << resp.imagedata().w() << "x"
					<< resp.imagedata().h();
				std::string filename = "D:\\Temp\\Images\\";
				filename += words[1] + "_" + words[2] + "_" + words[3] + "_";
				static int debug_image_count = 0;
				filename += std::to_string(debug_image_count++) + ".bmp";
				std::ofstream os(filename, std::ios_base::binary);
				os.write((char*)resp.imagedata().rgb().data(), resp.imagedata().rgb().size());
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
				int imgWidth = 200;
				int iterations = 1000;
				int repeats = 1;
				if (wordsize >= 2)
				{
					imgWidth = atoi(words[1].c_str());
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
				int imgWidth = 200;
				int iterations = 1000;
				int repeats = 1;
				if (wordsize >= 2)
				{
					imgWidth = atoi(words[1].c_str());
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
			else if (words[0] == "con")
			{
				if (wordsize > 1)
				{
					uint16_t port = SvWsl::Default_Port;
					if (wordsize > 2)
					{
						port = atoi(words[2].c_str());
					}
					std::string ipAdress = words[1];
			
					pService.reset();
					pRpcClient.reset();
					pRpcClient = std::make_unique<SvRpc::RPCClient>(host, port);
					pRpcClient->waitForConnect(2000);
					pService = std::make_unique<SvWsl::ClientService>(*pRpcClient);
				}
			}
			else if (words[0] == "qli")
			{
				std::string monitorlistname;
				SvPb::ListType  t{SvPb::All };
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
						t = SvPb::ProductItem;
					}
					else if (words[2] == "r")
					{
						t = SvPb::RejectCondition;
					}
					else if (words[2] == "f")
					{
						t = SvPb::FailStatus;
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
				request.set_name(monitorlistname.c_str());
				request.set_type(t);
				request.set_queryimages(bImage);
				request.set_queryvalues(bValues) ;

				auto resp = runRequest(*pService, &SvWsl::ClientService::queryListItem, std::move(request)).get();

				std::cout << "QueryListItemResponse .DebugString: " << std::endl;
				std::cout << resp.DebugString() << std::endl;



			}
			else if (words[0] == "h" || words[0] == "H")
			{
				BOOST_LOG_TRIVIAL(info) << "commands: " << std::endl 
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
				std::cout << "con  connect [ip adress] [portnr  = 8080] " << std::endl;
				std::cout << "qli [monitorlistname] [p,r,f,a] [Image=1]  [val=1]" << std::endl;
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
