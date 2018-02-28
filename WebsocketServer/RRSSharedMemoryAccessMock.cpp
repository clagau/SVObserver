//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file RRSSharedMemoryAccessMock.cpp
/// All Rights Reserved
//******************************************************************************
/// Mock returning dummy data instead of reading from shared memory.
//******************************************************************************

#include "stdafx.h"

#include "RRSSharedMemoryAccessInterface.h"
#include "RRSSharedMemoryAccessMock.h"

#include <fstream>
#include <future>
#include <queue>

namespace RRWS
{

std::string loadBitmap(const char* filename)
{
	std::string buffer;
	std::ifstream is(filename, std::ios::in | std::ios::binary);
	if (is)
	{
		is.seekg(0, std::ios::end);
		auto numBytes = is.tellg();
		buffer.resize(numBytes);
		is.seekg(0, std::ios::beg);
		is.read(&buffer[0], numBytes);
	}
	else
	{
		std::cout << "Error could not read the mock bitmap: " << filename << std::endl;
	}
	return buffer;
}

typedef std::tuple<std::string, unsigned int, unsigned int> BitmapWithMetadata;

const BitmapWithMetadata& getBitmap(unsigned int width, bool rotated = false)
{
	static BitmapWithMetadata testimage200x200 = {loadBitmap("testbild200x200.bmp"), 200, 200};
	static BitmapWithMetadata testimage300x300 = {loadBitmap("testbild300x300.bmp"), 300, 300};
	static BitmapWithMetadata testimage2048x2048 = {loadBitmap("testbild2048x2048.bmp"), 2048, 2048};

	static BitmapWithMetadata testimage200x200_rot = {loadBitmap("testbild200x200_rot.bmp"), 200, 200};
	static BitmapWithMetadata testimage300x300_rot = {loadBitmap("testbild300x300_rot.bmp"), 300, 300};
	static BitmapWithMetadata testimage2048x2048_rot = {loadBitmap("testbild2048x2048_rot.bmp"), 2048, 2048};

	switch (width)
	{
		case 200:
			return rotated ? testimage200x200_rot : testimage200x200;
		case 300:
			return rotated ? testimage300x300_rot : testimage300x300;
		case 2048:
			return rotated ? testimage2048x2048_rot : testimage2048x2048;
		default:
			throw std::runtime_error("unsupported test image size");
	}
}

RRSSharedMemoryAccessMock::RRSSharedMemoryAccessMock()
	: m_io_service(), m_io_work(m_io_service), m_io_thread(boost::bind(&boost::asio::io_service::run, &m_io_service))
{
}

RRSSharedMemoryAccessMock::~RRSSharedMemoryAccessMock() {}

void RRSSharedMemoryAccessMock::GetVersion(const GetVersionRequest& req, SVRPC::Task<GetVersionResponse> task)
{
	// trigger timeout by not calling task.finish()
	if (req.trigger_timeout())
	{
		return;
	}

	GetVersionResponse resp;
	resp.set_version("Mock Version");
	task.finish(std::move(resp));
}

void RRSSharedMemoryAccessMock::GetProduct(const GetProductRequest& request, SVRPC::Task<GetProductResponse> task)
{
	GetProductResponse resp;
	getProduct(*resp.mutable_product(), request.nameinresponse());
	task.finish(std::move(resp));
}



void RRSSharedMemoryAccessMock::GetFailstatus(const GetFailStatusRequest&, SVRPC::Task<GetFailStatusResponse> task)
{
	SVRPC::Error err;
	err.set_error_code(SVRPC::ErrorCode::NotImplemented);
	task.error(err);
}

void RRSSharedMemoryAccessMock::GetImageFromCurId(const GetImageFromCurIdRequest& request,
	SVRPC::Task<GetImageFromCurIdResponse> task)
{
	GetImageFromCurIdResponse resp;
	this->getImageById(*resp.mutable_imagedata(), request.id());
	task.finish(std::move(resp));
}

void RRSSharedMemoryAccessMock::GetImageStreamFromCurId(const GetImageStreamFromCurIdRequest& req,
	SVRPC::Observer<GetImageStreamFromCurIdResponse> observer)
{
	m_io_service.post(
		std::bind(&RRSSharedMemoryAccessMock::getImageStreamFromCurIdStep, this, req.count(), req.id(), observer));
}

void RRSSharedMemoryAccessMock::GetItems(const GetItemsRequest&, SVRPC::Task<GetItemsResponse> task)
{
	SVRPC::Error err;
	err.set_error_code(SVRPC::ErrorCode::NotImplemented);
	task.error(err);
}

void RRSSharedMemoryAccessMock::QueryListName(const QueryListNameRequest&, SVRPC::Task<QueryListNameResponse> task)
{
	QueryListNameResponse resp;
	SvDef::StringVector stringVector;

	*(resp.add_listname()) = "Hello";
	task.finish(std::move(resp));
}

void RRSSharedMemoryAccessMock::QueryListItem(const QueryListItemRequest&, SVRPC::Task<QueryListItemResponse> task)
{
	SVRPC::Error err;
	err.set_error_code(SVRPC::ErrorCode::NotImplemented);
	task.error(err);
}

void RRSSharedMemoryAccessMock::getProduct(Product& product, bool name_in_response)
{
	product.set_trigger(123);
	product.add_values()->set_string_value("TestValue");
	if (name_in_response)
	{
		(*product.add_valuenames()) = "TestValueName";
	}
	{
		auto image = product.add_images();
		image->set_slotindex(0);
		image->set_imageindex(0);
		image->set_imagestore(0);
		(*product.add_imagenames()) = "200x200";
	}
	{
		auto image = product.add_images();
		image->set_slotindex(1);
		image->set_imageindex(0);
		image->set_imagestore(0);
		(*product.add_imagenames()) = "300x300";
	}
	{
		auto image = product.add_images();
		image->set_slotindex(2);
		image->set_imageindex(0);
		image->set_imagestore(0);
		(*product.add_imagenames()) = "2048x2048";
	}
}

void RRSSharedMemoryAccessMock::getImageById(RRWS::Image& img, const RRWS::CurImageId& imageId, bool rotated)
{
	switch (imageId.slotindex())
	{
		case 0:
			img.set_w(std::get<1>(getBitmap(200)));
			img.set_h(std::get<2>(getBitmap(200)));
			*(img.mutable_rgb()) = std::get<0>(getBitmap(200, rotated));
			break;
		case 1:
			img.set_w(std::get<1>(getBitmap(300)));
			img.set_h(std::get<2>(getBitmap(300)));
			*(img.mutable_rgb()) = std::get<0>(getBitmap(300, rotated));
			break;
		case 2:
			img.set_w(std::get<1>(getBitmap(2048)));
			img.set_h(std::get<2>(getBitmap(2048)));
			*(img.mutable_rgb()) = std::get<0>(getBitmap(200, rotated));
			break;
	}
}

void RRSSharedMemoryAccessMock::getImageStreamFromCurIdStep(int iterations,
	const CurImageId& id,
	SVRPC::Observer<GetImageStreamFromCurIdResponse> observer)
{
	static const auto MAX_PENDING_RESPONSES = 20;
	std::queue<std::future<void>> futures;
	for (int i = 0; i < iterations; ++i)
	{
		bool even = i % 2 == 0;
		// will be true in the very first iteration only
		while (futures.size() > MAX_PENDING_RESPONSES)
		{
			futures.front().wait();
			futures.pop();
		}

		GetImageStreamFromCurIdResponse resp;
		this->getImageById(*resp.mutable_imagedata(), id, even);

		auto future = observer.onNext(std::move(resp));
		futures.emplace(std::move(future));
	}

	observer.finish();
}
}
