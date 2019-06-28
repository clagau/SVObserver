//******************************************************************************
/// \copyright (c) 2017,2018 by Seidenader Maschinenbau GmbH
/// \file SharedMemoryAccess.cpp
/// All Rights Reserved
//******************************************************************************
/// Mapping from incoming RPC requests to shared memory wrapper.
//******************************************************************************

#include "stdafx.h"

//Moved to precompiled header: #include <future>
//Moved to precompiled header: #include <queue>
//Moved to precompiled header: #include <boost/log/trivial.hpp>

#pragma region Includes
#include "SharedMemoryAccess.h"
#include "SVLogLibrary/Logging.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVSharedMemoryLibrary/ShareControlSetting.h" 
#include "SVProtoBuf/ConverterHelper.h"
#include "SVSystemLibrary/SVVersionInfo.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "TriggerRecordController/TriggerRecordController.h"
#pragma endregion Includes

namespace SvOgw
{

SharedMemoryAccess::SharedMemoryAccess(boost::asio::io_service& rIoService, const SvSml::ShareControlSettings& ControlParameter)
	: m_io_service(rIoService)
{
	m_pShareControlInstance = std::make_unique<SvSml::ShareControl>(ControlParameter);
	auto& trc = SvTrc::getTriggerRecordControllerInstance();
	m_TrcSubscriptionId = trc.registerNewTrCallback([this](SvTrc::TrEventData eventData)
	{
		m_io_service.post([this, eventData]()
		{
			on_new_trigger_record(eventData.m_inspectionPos, eventData.m_trId);
		});
	});
}

SharedMemoryAccess::~SharedMemoryAccess()
{
	auto& trc = SvTrc::getTriggerRecordControllerInstance();
	trc.unregisterNewTrCallback(m_TrcSubscriptionId);
}

void SharedMemoryAccess::GetVersion(const SvPb::GetGatewayVersionRequest& rRequest, SvRpc::Task<SvPb::GetVersionResponse> task)
{
	SvPb::GetVersionResponse Response;
	Response.set_version(SvUl::to_utf8(SvSyl::SVVersionInfo::GetVersion()));
	task.finish(std::move(Response));
}

void SharedMemoryAccess::GetInspections(const SvPb::GetInspectionsRequest& rRequest, SvRpc::Task<SvPb::GetInspectionsResponse> task)
{
	SvPenv::Error Error;
	SvPb::GetInspectionsResponse Response;

	auto& trc = SvTrc::getTriggerRecordControllerInstance();
	const auto& inspections = trc.getInspections();
	for (int i = 0; i < inspections.list_size(); ++i)
	{
		const auto& inspection = inspections.list(i);
		auto& entry = *Response.add_inspections();
		entry.set_id(inspection.id());
		entry.set_name(inspection.name());
	}

	task.finish(std::move(Response));
}

void SharedMemoryAccess::GetProduct(const SvPb::GetProductRequest& rRequest, SvRpc::Task<SvPb::GetProductResponse> task)
{
	SvPenv::Error Error;
	SvPb::GetProductResponse Response;

	if (m_pShareControlInstance->GetProduct(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}
}

void SharedMemoryAccess::GetReject(const SvPb::GetRejectRequest& rRequest, SvRpc::Task<SvPb::GetRejectResponse> task)
{
	SvPenv::Error Error;
	SvPb::GetRejectResponse Response;

	if (m_pShareControlInstance->GetReject(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}
}

void SharedMemoryAccess::GetFailstatus(const SvPb::GetFailStatusRequest& rRequest, SvRpc::Task<SvPb::GetFailStatusResponse> task)
{
	SvPenv::Error Error;
	SvPb::GetFailStatusResponse Response;
	if (m_pShareControlInstance->GetFailstatus(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}

}

void SharedMemoryAccess::GetImageFromId(const SvPb::GetImageFromIdRequest& rRequest,
	SvRpc::Task<SvPb::GetImageFromIdResponse> task)
{
	SvPb::GetImageFromIdResponse Response;
	SvPenv::Error Error;

	if (m_pShareControlInstance->GetImageFromId(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}

}

void SharedMemoryAccess::GetTriggerItems(const SvPb::GetTriggerItemsRequest&, SvRpc::Task<SvPb::GetTriggerItemsResponse> task)
{
	SvPenv::Error Error;
	Error.set_errorcode(SvPenv::ErrorCode::notImplemented);
	task.error(Error);
}

void SharedMemoryAccess::QueryListName(const SvPb::QueryListNameRequest& rRequest, SvRpc::Task<SvPb::QueryListNameResponse> task)
{
	SvPb::QueryListNameResponse Response;
	SvPenv::Error Error;

	if (m_pShareControlInstance->QueryListName(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}
}
void SharedMemoryAccess::QueryListItem(const SvPb::QueryListItemRequest& rRequest, SvRpc::Task<SvPb::QueryListItemResponse> task)
{
	SvPb::QueryListItemResponse Response;
	SvPenv::Error Error;
	if (m_pShareControlInstance->QueryListItem(rRequest, Response, Error))
	{
		task.finish(std::move(Response));
	}
	else
	{
		task.error(Error);
	}
}
void SharedMemoryAccess::StoreClientLogs(const SvPb::StoreClientLogsRequest& rRequest, SvRpc::Task<SvPb::EmptyResponse> task)
{
	auto map_to_boost_log_severity = [](const SvPb::LogSeverity& severity) -> boost::log::trivial::severity_level
	{
		switch (severity)
		{
			case SvPb::LogSeverity::fatal:
				return boost::log::trivial::fatal;
			case SvPb::LogSeverity::error:
				return boost::log::trivial::error;
			case SvPb::LogSeverity::warning:
				return boost::log::trivial::warning;
			case SvPb::LogSeverity::info:
				return boost::log::trivial::info;
			case SvPb::LogSeverity::debug:
				return boost::log::trivial::debug;
			case SvPb::LogSeverity::trace:
				return boost::log::trivial::trace;
			default:
				return boost::log::trivial::error;
		}
	};

	const auto& client = rRequest.client();
	for (const auto& entry : rRequest.logs())
	{
		const auto severity = map_to_boost_log_severity(entry.severity());
		BOOST_LOG_STREAM_WITH_PARAMS(
			::boost::log::trivial::logger::get(),
			(::boost::log::keywords::severity = severity)
		) << "[" << client << "] " << entry.message();
	}

	task.finish(SvPb::EmptyResponse());
}

void SharedMemoryAccess::GetProductStream(const SvPb::GetProductStreamRequest& rRequest,
	SvRpc::Observer<SvPb::GetProductStreamResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
{
	// TODO handle startAtTriggerCount
	auto state = std::make_shared<product_stream_t>(rRequest, observer, ctx);
	m_ProductStreams.push_back(state);
}

SharedMemoryAccess::product_stream_t::product_stream_t(
	const SvPb::GetProductStreamRequest& streamReq,
	SvRpc::Observer<SvPb::GetProductStreamResponse> observer,
	SvRpc::ServerStreamContext::Ptr ctx)
	: observer(observer)
	, ctx(ctx)
{
	this->req.CopyFrom(streamReq);
}

static bool is_subscribed_to(const SvPb::GetProductStreamRequest& req, const SvPb::Inspection& inspection)
{
	const auto guid1 = SvPb::GetGuidFromString(req.inspectionid());
	const auto guid2 = SvPb::GetGuidFromString(inspection.id());
	const auto guidsAreEqual = IsEqualGUID(guid1, guid2) != 0;
	return guidsAreEqual;
}

void SharedMemoryAccess::on_new_trigger_record(int inspectionPos, int trId)
{
	if (m_ProductStreams.empty())
	{
		return;
	}

	auto& trc = SvTrc::getTriggerRecordControllerInstance();
	auto tro = trc.createTriggerRecordObject(inspectionPos, trId);
	const auto& defList = trc.getDataDefList(inspectionPos);
	const auto& inspections = trc.getInspections();
	const auto& inspection = inspections.list(inspectionPos);

	for (auto it = m_ProductStreams.begin(); it != m_ProductStreams.end();)
	{
		auto& ptr = *it;
		if (ptr->ctx->isCancelled())
		{
			it = m_ProductStreams.erase(it);
			continue;
		}

		if (is_subscribed_to(ptr->req, inspection))
		{
			handle_new_trigger_record(*ptr, *tro, inspectionPos, trId);
		}

		++it;
	}
}

static bool read_image(SvPb::Image& resImg, SvTrc::IImagePtr imgPtr)
{
	auto hdl = imgPtr->getHandle();
	auto& info = hdl->GetBitmapInfo();
	auto& buffer = hdl->GetBuffer();
	auto& out = *resImg.mutable_rgbdata();
	auto rc = SVMatroxBufferInterface::CopyBufferToFileDIB(out, info, buffer);
	return rc == S_OK;
}

static bool read_variant(SvPb::Variant& resVar, const _variant_t& variant)
{
	return SvPb::ConvertVariantToProtobuf(variant, &resVar) == S_OK;
}

void SharedMemoryAccess::handle_new_trigger_record(product_stream_t& stream, SvTrc::ITriggerRecordR& tro, int inspectionPos, int trId)
{
	SvPb::GetProductStreamResponse res;

	if (stream.req.rejectsonly())
	{
		// TODO handle rejects only
		//res.set_isreject();
	}

	// TODO read fail status values
	//res.add_failstatusvalues()

	// TODO inform client about current trigger
	//res.set_trigger();

	for (const auto& imgId : stream.req.imageids())
	{
		auto& img = *res.add_images();
		auto guid = SvPb::GetGuidFromString(imgId);
		auto imgPtr = tro.getImage(guid);
		if (!imgPtr)
		{
			SV_LOG_GLOBAL(debug) << "  > image " << SvPb::PrettyPrintGuid(guid) << " not found";
			continue;
		}

		// TODO handle image format
		//stream.req.imageformat()
		if (!read_image(img, imgPtr))
		{
			SV_LOG_GLOBAL(warning) << "Error reading image with guid " << SvPb::PrettyPrintGuid(guid) << " for " << inspectionPos;
		}
	}

	for (const auto& valueId : stream.req.valueids())
	{
		auto& value = *res.add_values();
		auto guid = SvPb::GetGuidFromString(valueId);
		auto variant = tro.getDataValue(guid);
		if (!read_variant(value, variant))
		{
			SV_LOG_GLOBAL(warning) << "Error reading value with guid " << SvPb::PrettyPrintGuid(guid) << " for " << inspectionPos;
		}
	}

	stream.observer.onNext(std::move(res));
}

}// namespace SvOgw
