// RunReAsioClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <fstream>
#include <memory>
#include "RunReApi/Definition.h"
#include "RunReApi/ClientFrontEndApi.h"

#define NUM_IO_THREADS 2

void PrintCurImage(const ::RRApi::CurImageId& curIm)
{
	if (curIm.has_imagestore())
	{
		std::cout << "StoreIndex: " << curIm.imagestore();
	}
	if (curIm.has_imageindex())
	{
		std::cout << " ImageIndex: " << curIm.imageindex();
	}
	if (curIm.has_slotindex())
	{
		std::cout << " SlotIndex: " << curIm.slotindex() << std::endl;
	}
	else
	{
		std::cout << std::endl;
	}

}
void PrintVariant(const ::RRApi::Variant& var)
{
	
	if (var.has_bool_value())
	{
		std::cout << "value: " << var.bool_value() << std::endl;
	}
	if (var.has_int32_value())
	{
		std::cout << "value: " << var.int32_value() << std::endl;
	}
	if (var.has_int64_value())
	{
		std::cout << "value: " << var.int64_value() << std::endl;
	}
	if (var.has_uint32_value())
	{
		std::cout << "value: " << var.uint32_value() << std::endl;
	}
	if (var.has_uint64_value())
	{
		std::cout << "value: " << var.uint64_value() << std::endl;
	}
	if (var.has_string_value())
	{
		std::cout << "value: " << var.string_value() << std::endl;
	}
	if (var.has_double_value())
	{
		std::cout << "value: " << var.double_value() << std::endl;
	}
	if (var.has_float_value())
	{
		std::cout << "value: " << var.float_value() << std::endl;
	}

}
static void RunBenchmark1(RRApi::ClientFrontEndApi* api, int iterations)
{
	auto start = std::chrono::steady_clock::now();
	for (int i = 0; i < iterations; ++i)
	{
		RRApi::QueryListNameRequest request;
		RRApi::QueryListNameResponse Listnames = api->QueryListName(request).get();
	}
	auto finish = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start).count();
	std::cout << "Benchmark1: iterations=" << iterations << " duration=" << duration << "sec" << std::endl;
}
static bool GetImageId(RRApi::ClientFrontEndApi* api, int imageWidth, RRApi::CurImageId& imgIdOut)
{
	RRApi::QueryListNameRequest mListReq;
	auto mListRes = api->QueryListName(mListReq).get();
	for (const auto& listname : *mListRes.mutable_listname())
	{
		RRApi::GetProductRequest prodReq;
		prodReq.set_name(listname);
		auto prodRes = api->GetProduct(prodReq).get();
		for (const auto& imgId : *prodRes.mutable_images())
		{
			RRApi::GetImageFromCurIdRequest imgReq;
			*imgReq.mutable_id() = imgId;
			auto imgRes = api->GetImageFromCurId(imgReq).get();
			if (imgRes.imagedata().w() == imageWidth)
			{
				imgIdOut = imgId;
				return true;
			}
		}
	}
	return false;
}
static void RunBenchmark2(RRApi::ClientFrontEndApi* api, int iterations, int imageWitdh)
{
	long ClientId = api->GetClientId();
	RRApi::CurImageId imgId;
	if (!GetImageId(api, imageWitdh, imgId))
	{
		std::cerr << "Unable to find image for request width " << imageWitdh << std::endl;
		return;
	}
	auto start = std::chrono::steady_clock::now();
	for (int i = 0; i < iterations; ++i)
	{
		RRApi::GetImageFromCurIdRequest request;
		*request.mutable_id() = imgId;
		RRApi::GetImageFromCurIdResponse resp = api->GetImageFromCurId(request).get();
	}
	auto finish = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start).count();
	std::cout << "Benchmark2: iterations=" << iterations << " duration=" << duration << "sec" << std::endl;
}

void PrintProductResponse(const RRApi::GetProductResponse& resp)
{
	if (resp.has_status())
	{
		std::cout << "Get Product status: " << RRApi::State_Name(resp.status()) << std::endl;
		if (resp.status() != RRApi::IsValid)
		{
			return;
		}
	}
	std::cout << "Product: " << "for Trigger: " << resp.trigger() << std::endl;
	bool bpImageName = resp.images_size() == resp.imagenames_size();
	bool bpValueName = resp.values_size() == resp.valuenames_size();

	for (int i = 0; i < resp.images_size(); i++)
	{
		if (bpImageName)
		{
			std::cout << resp.imagenames(i) << std::endl;
		}
		PrintCurImage(resp.images(i));
	}
	for (int v = 0; v < resp.values_size(); v++)
	{
		if (bpValueName)
		{
			std::cout << resp.valuenames(v) << std::endl;
		}
		PrintVariant(resp.values(v));
	}
}


int main(int argc, char *argv[])
{
	int count = 0;
	try
	{
		//boost::thread_group threads;
		std::vector<std::shared_ptr<std::thread>> Threads;
		boost::asio::io_service io_service;
		boost::asio::io_service::work work(io_service);
		for (std::size_t i = 0; i < RRApi::Default_NrOfIoThreads; ++i)
		{
			Threads.push_back(std::make_unique<std::thread>(boost::bind(&boost::asio::io_service::run, &io_service)));
		}
		
		std::string ipAdress("127.0.0.1");
		if (argc > 1)
			ipAdress = argv[1];
		auto api = RRApi::ClientFrontEndApi::Init(io_service);
		api->Connect(RRApi::Default_Port, ipAdress);

		std::cout << "Enter a command(Ctrl-Z to stop): " << std::endl;
		std::string input;
		while (std::getline(std::cin, input))
		{
			std::vector<std::string> words;
			boost::algorithm::split(words, input, boost::is_space(), boost::token_compress_on);
			int wordsize = static_cast<int>( words.size());
			if (wordsize == 0)
				continue;
			if (words[0] == "v")
			{
				auto version = api->GetVersion(RRApi::GetVersionRequest()).get();
				std::cout << "Version: " << std::endl;
				std::cout << " RuReAsioServer: " << version.version() << std::endl;
			}
			else if (words[0] == "m")
			{
				RRApi::QueryListNameResponse Listnames =
					api->QueryListName(RRApi::QueryListNameRequest()).get();
				//std::cout << "QueryListNameResponse.DebugString: " << std::endl;
				//std::cout << Listnames.DebugString() << std::endl;
				std::cout << "MonitorlistNamen: " << std::endl;
				for (int i = 0; i < Listnames.listname_size(); i++)
				{
					std::cout << Listnames.listname(i) << std::endl;
				}

			}
			else if (words[0] == "f")
			{
				RRApi::GetFailStatusRequest FailstatusRequest;
				if (wordsize > 1)
				{
					FailstatusRequest.set_name(words[1].c_str());
				}
				else
				{
					std::cout << "Monitorname  notwendig " << std::endl;
					continue;
				}
				FailstatusRequest.set_nameinresponse(true);
				RRApi::GetFailStatusResponse resp;
				resp = api->GetFailStatus(FailstatusRequest).get();
				if (resp.has_status())
				{
					std::cout << "Get Failstatus  status: " << RRApi::State_Name(resp.status()) << std::endl;
					if (resp.status() != RRApi::IsValid)
					{
						continue;
					}
				}
				for (int i = 0; i < resp.productresponse_size(); i++)
				{
					PrintProductResponse(resp.productresponse(i));

				}

			}
			else if (words[0] == "p" || words[0] == "r")
			{
				RRApi::GetProductRequest ProductRequest;
				if (wordsize > 1)
				{
					ProductRequest.set_name(words[1].c_str());
				}
				else
				{
					std::cout << "monitorname notwendig " << std::endl;
					continue;
				}
				if (wordsize > 2)
				{
					int t = atoi(words[2].c_str());
					if (t > -1)
					{
						ProductRequest.set_trigger(t);
					}
				}
				ProductRequest.set_nameinresponse(true);
				RRApi::GetProductResponse resp;
				if (words[0] == "p")
				{
					resp = api->GetProduct(ProductRequest).get();
				}
				else
				{
					resp = api->GetReject(ProductRequest).get();
				}
				//std::cout << "GetProductResponse.DebugString: " << std::endl;
				//std::cout << resp.DebugString() << std::endl;

				PrintProductResponse(resp);

			}
			else if (words[0] == "i")
			{
				if (wordsize <= 3)
				{
					std::cout << "StoreNr  imageNr slotNr Eingeben" << std::endl;
					continue;
				}

				RRApi::GetImageFromCurIdRequest request;
				request.mutable_id()->set_imagestore(atoi(words[1].c_str()));
				request.mutable_id()->set_imageindex(atoi(words[2].c_str()));
				request.mutable_id()->set_slotindex(atoi(words[3].c_str()));
				RRApi::GetImageFromCurIdResponse resp = api->GetImageFromCurId(request).get();

				if (resp.has_status())
				{
					std::cout << "Get Image  status: " << RRApi::State_Name(resp.status()) << std::endl;
					if (resp.status() != RRApi::IsValid)
					{
						continue;
					}
					std::cout << "Image (Width ,Height) " << resp.imagedata().w() << "x" << resp.imagedata().h() << std::endl;
					std::string  filename = "D:\\Temp\\Images\\";
					filename += words[1] + "_" + words[2] + "_" + words[3] + "_";
					filename += std::to_string(count++) + ".bmp";
					std::ofstream  os(filename, std::ios_base::binary);
					os.write((char *)resp.imagedata().rgb().data(), resp.imagedata().rgb().size());
					os.close();
					//os.write((char *)iarray, sizeof(iarray));

				}
			}
			else if (words[0] == "b1" )
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
					RunBenchmark1(api.get(), iterations);
				}
			}
			else if (words[0] == "b2" )
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
					RunBenchmark2(api.get(), iterations, imgWidth);
				}
			}
			else  if (words[0] == "d")
			{
				api->DisConnect();
			}
			else if (words[0] == "c")
			{
				if (wordsize > 1)
				{
					uint16_t port = RRApi::Default_Port;
					if (wordsize > 2)
						port = atoi(words[2].c_str());
					std::string ipAdress = words[1];
					api->Connect(port, ipAdress);
				}
			}
			else if (words[0] == "qli")
			{
				std::string monitorlistname;
				RRApi::ListType  t{ RRApi::All };
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
						t = RRApi::ProductItem;
					}
					else if (words[2] == "r")
					{
						t = RRApi::RejectCondition;
					}
					else if (words[2] == "f")
					{
						t = RRApi::FailStatus;
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
				RRApi::QueryListItemRequest  request;
				request.set_name(monitorlistname.c_str());
				request.set_type(t);
				request.set_queryimages(bImage);
				request.set_queryvalues(bValues) ;

				RRApi::QueryListItemResponse resp = api->QueryListItem(request).get();
				
				std::cout << "QueryListItemResponse .DebugString: " << std::endl;
				std::cout << resp.DebugString() << std::endl;



			}
			else if (words[0] == "h" )
			{
				std::cout << "commands: " << std::endl;
				std::cout << "v (Version)" << std::endl;
				std::cout << "h (Hilfe)" << std::endl;
				std::cout << "m (Monitorlist)" << std::endl;
				std::cout << "p  name triggercount (GetProdukt)" << std::endl;
				std::cout << "i  StoreNr  imageNr slotNr (GetImage)" << std::endl;
				std::cout << "r  name triggercount (GetRejekt)" << std::endl;
				std::cout << "f  name  (GetFailstatus)" << std::endl;
				std::cout << "b1 [iterations=1000] [repeats=1] (Benchmark 1)" << std::endl;
				std::cout << "b2 [image_width=200] [iterations=1000] [repeats=1] (Benchmark 2)" << std::endl;
				std::cout << "d disconnect" << std::endl;
				std::cout << "c  connect [ip adress] [portnr  = 8080] " << std::endl;
				std::cout << "qli [monitorlistname] [p,r,f,a] [Image=1]  [val=1]" << std::endl;
			}
		}

		io_service.stop();
		for (auto &t :Threads  )
		{
			t->join();
		}
		//for (std::size_t i = 0; i < NUM_IO_THREADS; ++i)
		//	Treads[i]->join();
		//threads.join_all();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	system("pause");
	return 0;
}

