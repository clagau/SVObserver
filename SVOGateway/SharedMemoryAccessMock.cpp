//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SharedMemoryAccessMock.cpp
/// All Rights Reserved
//******************************************************************************
/// Mock returning dummy data instead of reading from shared memory.
//******************************************************************************

#include "stdafx.h"

//Moved to precompiled header: #include <fstream>
//Moved to precompiled header: #include <future>
//Moved to precompiled header: #include <queue>

#include "SVLogLibrary/Logging.h"
#include "SharedMemoryAccessMock.h"

namespace SvOgw
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

SharedMemoryAccessMock::SharedMemoryAccessMock()
	: m_io_service(), m_io_work(m_io_service), m_io_thread(boost::bind(&boost::asio::io_service::run, &m_io_service))
{
}

SharedMemoryAccessMock::~SharedMemoryAccessMock() {}

void SharedMemoryAccessMock::GetVersion(const SvPb::GetGatewayVersionRequest& req, SvRpc::Task<SvPb::GetVersionResponse> task)
{
	SvPb::GetVersionResponse resp;
	resp.set_version("Mock Version");
	task.finish(std::move(resp));
}

void SharedMemoryAccessMock::GetInspections(const SvPb::GetInspectionsRequest&, SvRpc::Task<SvPb::GetInspectionsResponse> task)
{
	SvPb::GetInspectionsResponse resp;
	task.finish(std::move(resp));
}

void SharedMemoryAccessMock::GetProduct(const SvPb::GetProductRequest& request, SvRpc::Task<SvPb::GetProductResponse> task)
{
	SvPb::GetProductResponse resp;
	getProduct(*resp.mutable_productitem(), request.nameinresponse());
	task.finish(std::move(resp));
}



void SharedMemoryAccessMock::GetFailstatus(const SvPb::GetFailStatusRequest&, SvRpc::Task<SvPb::GetFailStatusResponse> task)
{
	SvPenv::Error err;
	err.set_errorcode(SvPenv::ErrorCode::notImplemented);
	task.error(err);
}

void SharedMemoryAccessMock::GetImageFromId(const SvPb::GetImageFromIdRequest& request,
	SvRpc::Task<SvPb::GetImageFromIdResponse> task)
{
	SvPb::GetImageFromIdResponse resp;
	this->getImageById(*resp.mutable_imagedata(), request.id());
	task.finish(std::move(resp));
}

void SharedMemoryAccessMock::GetTriggerItems(const SvPb::GetTriggerItemsRequest&, SvRpc::Task<SvPb::GetTriggerItemsResponse> task)
{
	SvPenv::Error err;
	err.set_errorcode(SvPenv::ErrorCode::notImplemented);
	task.error(err);
}

void SharedMemoryAccessMock::QueryListName(const SvPb::QueryListNameRequest&, SvRpc::Task<SvPb::QueryListNameResponse> task)
{
	SvPb::QueryListNameResponse resp;
	SvDef::StringVector stringVector;

	*(resp.add_listname()) = "Hello";
	task.finish(std::move(resp));
}

void SharedMemoryAccessMock::QueryListItem(const SvPb::QueryListItemRequest&, SvRpc::Task<SvPb::QueryListItemResponse> task)
{
	SvPenv::Error err;
	err.set_errorcode(SvPenv::ErrorCode::notImplemented);
	task.error(err);
}

void SharedMemoryAccessMock::StoreClientLogs(const SvPb::StoreClientLogsRequest& rRequest, SvRpc::Task<SvPb::EmptyResponse> task)
{
	for (const auto& entry : rRequest.logs())
	{
		SV_LOG_GLOBAL(info) << "[" << rRequest.client() << "] " << entry.message();
	}
	task.finish(SvPb::EmptyResponse());
}

void SharedMemoryAccessMock::SetRejectStreamPauseState(const SvPb::SetRejectStreamPauseStateRequest& rRequest, SvRpc::Task<SvPb::EmptyResponse> task)
{
	SvPenv::Error err;
	err.set_errorcode(SvPenv::ErrorCode::notImplemented);
	task.error(err);
}

void SharedMemoryAccessMock::GetGatewayNotificationStream(
	const SvPb::GetGatewayNotificationStreamRequest& rRequest,
	SvRpc::Observer<SvPb::GetGatewayNotificationStreamResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
{
	observer.finish();
}

void SharedMemoryAccessMock::GetProductStream(
	const SvPb::GetProductStreamRequest& req,
	SvRpc::Observer<SvPb::GetProductStreamResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
{
	observer.finish();
}

void SharedMemoryAccessMock::getProduct(SvPb::Product& product, bool name_in_response)
{
	product.set_trigger(123);
	product.add_values()->set_strval("TestValue");
	if (name_in_response)
	{
		(*product.add_valuenames()) = "TestValueName";
	}
	{
		auto image = product.add_images();
		image->set_trid(0);
		image->set_imageindex(0);
		image->set_inspectionid(0);
		(*product.add_imagenames()) = "200x200";
	}
	{
		auto image = product.add_images();
		image->set_trid(1);
		image->set_imageindex(0);
		image->set_inspectionid(0);
		(*product.add_imagenames()) = "300x300";
	}
	{
		auto image = product.add_images();
		image->set_trid(2);
		image->set_imageindex(0);
		image->set_inspectionid(0);
		(*product.add_imagenames()) = "2048x2048";
	}
}

void SharedMemoryAccessMock::getImageById(SvPb::Image& img, const SvPb::ImageId& imageId, bool rotated)
{
	switch (imageId.trid())
	{
		case 0:
			img.set_width(std::get<1>(getBitmap(200)));
			img.set_height(std::get<2>(getBitmap(200)));
			*(img.mutable_rgbdata()) = std::get<0>(getBitmap(200, rotated));
			break;
		case 1:
			img.set_width(std::get<1>(getBitmap(300)));
			img.set_height(std::get<2>(getBitmap(300)));
			*(img.mutable_rgbdata()) = std::get<0>(getBitmap(300, rotated));
			break;
		case 2:
			img.set_width(std::get<1>(getBitmap(2048)));
			img.set_height(std::get<2>(getBitmap(2048)));
			*(img.mutable_rgbdata()) = std::get<0>(getBitmap(200, rotated));
			break;
	}
}

}// namespace SvOgw
