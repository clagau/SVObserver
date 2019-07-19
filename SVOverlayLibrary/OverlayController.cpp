//******************************************************************************
/// \copyright (c) 2017,2019 by Seidenader Maschinenbau GmbH
/// \file OverlayController.cpp
/// All Rights Reserved
//******************************************************************************

#include "stdafx.h"

#pragma region Includes
#include "OverlayController.h"
#include <wtypes.h> // variant
#include "SVProtoBuf/ConverterHelper.h"
#include "SVProtoBuf/SVRC.h"
#include "SVRPCLibrary/ErrorUtil.h"
#include "SVRPCLibrary/SimpleClient.h"
#pragma endregion Includes

namespace SvOv
{

OverlayController::OverlayController(boost::asio::io_context& io_context, SvRpc::RPCClient& rpcClient)
	: m_io_context(io_context)
	, m_rpc_client(rpcClient)
	, m_version_client(rpcClient)
{
}

static uint32_t rgba_to_uint32(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return
		(static_cast<uint32_t>(r) * 256 * 256 * 256) +
		(static_cast<uint32_t>(g) * 256 * 256) +
		(static_cast<uint32_t>(b) * 256) +
		(static_cast<uint32_t>(a));
}

static void set_int_value(SvPb::ValueObject& obj, int v)
{
	auto& val = *obj.mutable_value();
	val.set_type(VT_I4);
	val.set_lval(v);
}

static void generate_dummy_overlay_data(SvPb::OverlayDesc& desc)
{
	auto& overlay1 = *desc.add_overlays();
	{
		overlay1.set_name("Rectangle Overlay");
		overlay1.set_color(rgba_to_uint32(0xf0, 0x50, 0x50, 0xff));
		// rectangular bounding box
		auto& bb = *overlay1.mutable_boundingbox();
		auto& bb_shape = *bb.mutable_rect();
		set_int_value(*bb_shape.mutable_x(), 100);
		set_int_value(*bb_shape.mutable_y(), 200);
		set_int_value(*bb_shape.mutable_w(), 50);
		set_int_value(*bb_shape.mutable_h(), 80);
		// rectangular shape
		auto& s1 = *overlay1.add_shapes();
		auto& s1r = *s1.mutable_rect();
		s1r.set_x(0);
		s1r.set_y(0);
		s1r.set_w(1);
		s1r.set_h(1);
	}

	auto& overlay2 = *desc.add_overlays();
	{
		overlay2.set_name("Arrow Overlay");
		overlay2.set_color(rgba_to_uint32(0xf0, 0x50, 0x50, 0xff));
		// rectangular bounding box
		auto& bb = *overlay2.mutable_boundingbox();
		auto& bb_shape = *bb.mutable_rect();
		set_int_value(*bb_shape.mutable_x(), 300);
		set_int_value(*bb_shape.mutable_y(), 300);
		set_int_value(*bb_shape.mutable_w(), 100);
		set_int_value(*bb_shape.mutable_h(), 50);
		// rectangular shape
		auto& s1 = *overlay2.add_shapes();
		auto& s1r = *s1.mutable_rect();
		s1r.set_x(0);
		s1r.set_y(0);
		s1r.set_w(1);
		s1r.set_h(1);
		// add horizontal arrow in the middle
		auto& s2 = *overlay2.add_shapes();
		auto& s2a = *s2.mutable_arrow();
		s2a.set_x1(0.0f);
		s2a.set_y1(0.5f);
		s2a.set_x2(1.0f);
		s2a.set_y2(0.5f);
		s2a.set_arrowheight(0.1f);
	}

	auto& overlay3 = *desc.add_overlays();
	{
		overlay3.set_name("Arrow Overlay Left");
		overlay3.set_color(rgba_to_uint32(0x00, 0xff, 0xff, 0xff));
		// rectangular bounding box
		auto& bb = *overlay3.mutable_boundingbox();
		auto& bb_shape = *bb.mutable_rect();
		set_int_value(*bb_shape.mutable_x(), 300);
		set_int_value(*bb_shape.mutable_y(), 500);
		set_int_value(*bb_shape.mutable_w(), 100);
		set_int_value(*bb_shape.mutable_h(), 50);
		// rectangular shape
		auto& s1 = *overlay3.add_shapes();
		auto& s1r = *s1.mutable_rect();
		s1r.set_x(0);
		s1r.set_y(0);
		s1r.set_w(1);
		s1r.set_h(1);
		// add horizontal arrow in the middle
		auto& s2 = *overlay3.add_shapes();
		auto& s2a = *s2.mutable_arrow();
		s2a.set_x1(1.0f);
		s2a.set_y1(0.5f);
		s2a.set_x2(0.0f);
		s2a.set_y2(0.5f);
		s2a.set_arrowheight(0.1f);
	}

	auto& overlay4 = *desc.add_overlays();
	{
		overlay4.set_name("Arrow Overlay Up");
		overlay4.set_color(rgba_to_uint32(0xff, 0x00, 0x50, 0xff));
		// rectangular bounding box
		auto& bb = *overlay4.mutable_boundingbox();
		auto& bb_shape = *bb.mutable_rect();
		set_int_value(*bb_shape.mutable_x(), 450);
		set_int_value(*bb_shape.mutable_y(), 500);
		set_int_value(*bb_shape.mutable_w(), 50);
		set_int_value(*bb_shape.mutable_h(), 100);
		// rectangular shape
		auto& s1 = *overlay4.add_shapes();
		auto& s1r = *s1.mutable_rect();
		s1r.set_x(0);
		s1r.set_y(0);
		s1r.set_w(1);
		s1r.set_h(1);
		// add horizontal arrow in the middle
		auto& s2 = *overlay4.add_shapes();
		auto& s2a = *s2.mutable_arrow();
		s2a.set_x1(0.5f);
		s2a.set_y1(1.0f);
		s2a.set_x2(0.5f);
		s2a.set_y2(0.0f);
		s2a.set_arrowheight(0.1f);
	}

	auto& overlay5 = *desc.add_overlays();
	{
		overlay5.set_name("Arrow Overlay Down");
		overlay5.set_color(rgba_to_uint32(0xff, 0x00, 0x50, 0xff));
		// rectangular bounding box
		auto& bb = *overlay5.mutable_boundingbox();
		auto& bb_shape = *bb.mutable_rect();
		set_int_value(*bb_shape.mutable_x(), 450);
		set_int_value(*bb_shape.mutable_y(), 200);
		set_int_value(*bb_shape.mutable_w(), 50);
		set_int_value(*bb_shape.mutable_h(), 100);
		// rectangular shape
		auto& s1 = *overlay5.add_shapes();
		auto& s1r = *s1.mutable_rect();
		s1r.set_x(0);
		s1r.set_y(0);
		s1r.set_w(1);
		s1r.set_h(1);
		// add horizontal arrow in the middle
		auto& s2 = *overlay5.add_shapes();
		auto& s2a = *s2.mutable_arrow();
		s2a.set_x1(0.5f);
		s2a.set_y1(0.0f);
		s2a.set_x2(0.5f);
		s2a.set_y2(1.0f);
		s2a.set_arrowheight(0.1f);
	}

	auto& overlay6 = *desc.add_overlays();
	{
		overlay6.set_name("Rectangle Overlay");
		overlay6.set_color(rgba_to_uint32(0xf0, 0x50, 0x50, 0xff));
		// rectangular bounding box
		auto& bb = *overlay6.mutable_boundingbox();
		auto& bb_shape = *bb.mutable_rect();
		set_int_value(*bb_shape.mutable_x(), 600);
		set_int_value(*bb_shape.mutable_y(), 600);
		set_int_value(*bb_shape.mutable_w(), 80);
		set_int_value(*bb_shape.mutable_h(), 50);
		// rectangular shape
		auto& s1 = *overlay6.add_shapes();
		auto& s1r = *s1.mutable_rect();
		s1r.set_x(0);
		s1r.set_y(0);
		s1r.set_w(1);
		s1r.set_h(1);
	}
}

SvSyl::SVFuture<SvPb::OverlayDesc> OverlayController::getOverlays(GUID inspectionId, GUID imageId)
{
	auto promise = std::make_shared<SvSyl::SVPromise<SvPb::OverlayDesc>>();

	SvRpc::Task<SvPb::GetVersionResponse> task(
		[promise](SvPb::GetVersionResponse&& res)
	{
		SvPb::OverlayDesc desc;
		generate_dummy_overlay_data(desc);
		promise->set_value(std::move(desc));
	},
		[promise](const SvPenv::Error& err)
	{
		promise->set_exception(SvRpc::errorToExceptionPtr(err));
	});

	m_version_client.request(SvPb::GetSVObserverVersionRequest(), task, m_timeout);

	return promise->get_future();
}

}// namespace SvOv
