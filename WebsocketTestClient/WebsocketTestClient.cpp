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
#pragma warning (push,2)
#include "SVPROTOBUF/RunRE.pb.h"
#pragma warning (pop)
#include "SVHttpLibrary/WebsocketClientFactory.h"
#include "SVRPCLibrary/ErrorUtil.h"
#include "SVRPCLibrary/RPCClient.h"
#include "WebsocketLibrary/RunRequest.inl"
#include "WebsocketLibrary/Definition.h"


namespace RRWS
{
void PrintCurImage(const CurImageId& curIm)
{
	BOOST_LOG_TRIVIAL(info) << "StoreIndex: " << curIm.imagestore() << " ImageIndex: " << curIm.imageindex()
		<< " SlotIndex: " << curIm.slotindex();
}
void PrintVariant(const ::SVRPC::Variant& var)
{
	switch (var.value_case())
	{
		case SVRPC::Variant::kBoolValue:
			BOOST_LOG_TRIVIAL(info) << "value: " << var.bool_value();
			break;

		case SVRPC::Variant::kInt32Value:
			BOOST_LOG_TRIVIAL(info) << "value: " << var.int32_value();
			break;

		case SVRPC::Variant::kInt64Value:
			BOOST_LOG_TRIVIAL(info) << "value: " << var.int64_value();
			break;

		case SVRPC::Variant::kUint32Value:
			BOOST_LOG_TRIVIAL(info) << "value: " << var.uint32_value();
			break;

		case SVRPC::Variant::kUint64Value:
			BOOST_LOG_TRIVIAL(info) << "value: " << var.uint64_value();
			break;

		case SVRPC::Variant::kDoubleValue:
			BOOST_LOG_TRIVIAL(info) << "value: " << var.double_value();
			break;

		case SVRPC::Variant::kFloatValue:
			BOOST_LOG_TRIVIAL(info) << "value: " << var.float_value();
			break;

		case SVRPC::Variant::kStringValue:
			BOOST_LOG_TRIVIAL(info) << "value: " << var.string_value();
			break;

		default:
			BOOST_LOG_TRIVIAL(warning) << "Unknown variant value";
			break;
	}
}


static bool GetImageId(ClientService& client, int imageWidth, CurImageId& imgIdOut)
{
	QueryListNameRequest mListReq;
	auto mListRes = runRequest(client, &ClientService::queryListName, std::move(mListReq)).get();
	for (const auto& listname : *mListRes.mutable_listname())
	{
		GetProductRequest prodReq;
		prodReq.set_name(listname);
		auto prodRes = runRequest(client, &ClientService::getProduct, std::move(prodReq)).get();
		for (const auto& imgId : prodRes.product().images())
		{
			GetImageFromCurIdRequest imgReq;
			*imgReq.mutable_id() = imgId;
			auto imgRes = runRequest(client, &ClientService::getImageFromCurId, std::move(imgReq)).get();
			if (imgRes.imagedata().w() == imageWidth)
			{
				imgIdOut = imgId;
				return true;
			}
		}
	}
	return false;
}
static void RunBenchmark2(ClientService& client, int iterations, int imageWitdh, bool use_streaming)
{
	CurImageId imgId;
	if (!GetImageId(client, imageWitdh, imgId))
	{
		BOOST_LOG_TRIVIAL(error) << "Unable to find image for request width " << imageWitdh;
		return;
	}
	double volume = 0;
	auto start = std::chrono::steady_clock::now();
	if (use_streaming)
	{
		GetImageStreamFromCurIdRequest request;
		request.set_count(iterations);
		*request.mutable_id() = imgId;
		auto response = runStream(client, &ClientService::getImageStreamFromCurId, std::move(request), volume).get();
	}
	else
	{
		for (int i = 0; i < iterations; ++i)
		{
			GetImageFromCurIdRequest request;
			*request.mutable_id() = imgId;
			auto resp = runRequest(client, &ClientService::getImageFromCurId, std::move(request)).get();
			volume += resp.ByteSize() / (1024.0 * 1024.0);
		}
	}
	auto finish = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start).count();
	BOOST_LOG_TRIVIAL(info) << "Benchmark2: iterations=" << iterations << " duration=" << duration
		<< " volume=" << volume;
}
void PrintProductResponse(const Product& product)
{
	BOOST_LOG_TRIVIAL(info) << "Product for Trigger: " << product.trigger();
	bool bpImageName = product.images_size() == product.imagenames_size();
	bool bpValueName = product.values_size() == product.valuenames_size();

	for (int i = 0; i < product.images_size(); i++)
	{
		if (bpImageName)
		{
			BOOST_LOG_TRIVIAL(info) << product.imagenames(i);
		}
		PrintCurImage(product.images(i));
	}
	for (int v = 0; v < product.values_size(); v++)
	{
		if (bpValueName)
		{
			BOOST_LOG_TRIVIAL(info) << product.valuenames(v);
		}
		PrintVariant(product.values(v));
	}
}

class Benchmark1
{
public:
	Benchmark1(ClientService& client) : m_client(client) {}

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
				doStringBenchmark(m_client, num_iterations);
				break;
			case ImageBenchmark:
				doImageBenchmark(m_client, num_iterations);
				break;
			case ImageStreamBenchmark:
				doImageStreamBenchmark(m_client, num_iterations);
				break;
		}
		auto finish = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start).count();
		BOOST_LOG_TRIVIAL(info) << "Benchmark1: iterations=" << num_iterations << " duration=" << duration << "sec";
	}

private:
	void doStringBenchmark(ClientService& client, int num_iterations)
	{
		for (auto i = 0; i < num_iterations; ++i)
		{
			QueryListNameRequest request;
			auto response = runRequest(client, &ClientService::queryListName, std::move(request)).get();
		}
	}

	void doImageBenchmark(ClientService& client, int num_iterations)
	{
		for (auto i = 0; i < num_iterations; ++i)
		{
			GetImageFromCurIdRequest request;
			request.mutable_id()->set_slotindex(0); // TODO: slotindex should be product-index
			request.mutable_id()->set_imageindex(0);
			request.mutable_id()->set_imagestore(0); // TODO: what is this? MonitorList?
			auto response = runRequest(client, &ClientService::getImageFromCurId, std::move(request)).get();
		}
	}

	void doImageStreamBenchmark(ClientService& client, int num_iterations)
	{
		GetImageStreamFromCurIdRequest request;
		request.set_count(num_iterations);
		request.mutable_id()->set_slotindex(0); // TODO: slotindex should be product-index
		request.mutable_id()->set_imageindex(0);
		request.mutable_id()->set_imagestore(0); // TODO: what is this? MonitorList?
		double volume = 0;
		auto response = runStream(client, &ClientService::getImageStreamFromCurId, std::move(request), volume).get();
	}

private:
	ClientService& m_client;
};
}
int main(int argc, char* argv[])
{
	using namespace RRWS;
	
	LogSettings logSettings;
	logSettings.log_level = "debug";
	logSettings.log_to_stdout_enabled = true;
	logSettings.windows_event_log_enabled = false;
	init_logging(logSettings);

	std::string host("127.0.0.1");
	std::string stPort("8080");
	uint16_t port(RRWS::Default_Port);
	if (argc > 1)
		host = argv[1];
	if (argc > 2)
	{
		stPort = argv[2];
		port = atoi(stPort.c_str());
	}
	//SVHTTP::WebsocketClientFactory wsClientFactory(host, port);
	SVRPC::RPCClient rpcClient(host,port);
	//rpcClient.waitForConnect();

	auto request_timeout = boost::posix_time::seconds(2);
	ClientService service(rpcClient, request_timeout);


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
					GetVersionRequest req;
					req.set_trigger_timeout(true);
					auto version = runRequest(service, &ClientService::getVersion, std::move(req)).get();
					BOOST_LOG_TRIVIAL(info) << "Version: ";
					BOOST_LOG_TRIVIAL(info) << " RunReWebsocketServer: " << version.version();
				}
				catch (const std::exception& e)
				{
					BOOST_LOG_TRIVIAL(error) << "Unable to get version: " << e.what();
				}
			}
			else if ( rpcClient.isConnected() && words[0] == "m" )
			{

				auto Listnames = runRequest(service, &ClientService::queryListName, QueryListNameRequest()).get();
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
				GetFailStatusRequest FailstatusRequest;
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
				auto resp = runRequest(service, &ClientService::getFailStatus, std::move(FailstatusRequest)).get();
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

				GetProductRequest ProductRequest;
				ProductRequest.set_name(name);
				ProductRequest.set_trigger(trigger);
				ProductRequest.set_nameinresponse(true);
				ProductRequest.set_breject(bReject);
				ProductRequest.set_pevioustrigger(-1);

				GetProductResponse resp;
				resp = runRequest(service, &ClientService::getProduct, std::move(ProductRequest)).get();
				PrintProductResponse(resp.product());


			}
			else if (words[0] == "i")
			{
				if (wordsize <= 3)
				{
					BOOST_LOG_TRIVIAL(error) << "StoreNr imageNr slotNr Eingeben";
					continue;
				}

				GetImageFromCurIdRequest request;
				request.mutable_id()->set_imagestore(atoi(words[1].c_str()));
				request.mutable_id()->set_imageindex(atoi(words[2].c_str()));
				request.mutable_id()->set_slotindex(atoi(words[3].c_str()));
				auto resp = runRequest(service, &ClientService::getImageFromCurId, std::move(request)).get();

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
					Benchmark1(service).run(Benchmark1::StringBenchmark, iterations);
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
					RunBenchmark2(service, iterations, imgWidth, false);
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
					RunBenchmark2(service, iterations, imgWidth, true);
				}
			}
			else  if (words[0] == "dis") 
			{
				rpcClient.Disconnect();
			}
			else if (words[0] == "con")
			{
				if (wordsize > 1)
				{
					uint16_t port = Default_Port;
					if (wordsize > 2)
					{
						port = atoi(words[2].c_str());
					}
					std::string ipAdress = words[1];
					rpcClient.Disconnect();
					rpcClient.Connect(ipAdress, port);
					//rpcClient.waitForConnect();
				}
			}
			else if (words[0] == "qli")
			{
				std::string monitorlistname;
				ListType  t{ All };
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
						t = ProductItem;
					}
					else if (words[2] == "r")
					{
						t = RejectCondition;
					}
					else if (words[2] == "f")
					{
						t = FailStatus;
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
				QueryListItemRequest  request;
				request.set_name(monitorlistname.c_str());
				request.set_type(t);
				request.set_queryimages(bImage);
				request.set_queryvalues(bValues) ;

				auto resp = runRequest(service, &ClientService::queryListItem, std::move(request)).get();
				

				std::cout << "QueryListItemResponse .DebugString: " << std::endl;
				std::cout << resp.DebugString() << std::endl;



			}
			else if (words[0] == "h" || words[0] == "H")
			{
				BOOST_LOG_TRIVIAL(info) << "commands: " << std::endl 
					<< "  q  quit" << std::endl 
					<< "  h  Hilfe" << std::endl 
					<< "  v  (Version)" << std::endl 
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


	rpcClient.stop();
	system("pause");
	return 0;
}
