#pragma once

#include <chrono>
#include <codecvt>
#include <functional>
#include <locale>
#include <map>
#include <random>
#include <string>
#include <thread>

#include <boost/asio.hpp>

#include "SVLogLibrary/Logging.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVProtoBuf/SVRC.h"
#include "SVRPCLibrary/Observer.h"
#include "SVRPCLibrary/ServerStreamContext.h"
#include "SVRPCExampleServer/bitmap_image.hpp"
#include "SVRPCExampleServer/ImageCache.h"
#include "SVRPCExampleServer/SVOSharedMemory.h"

using namespace SvAuth;
using namespace SvPb;
using namespace SvRpc;

namespace SVRPCExample
{
struct SimulationSettings
{
	double Frequency = 10;
	double PercentRejects = 0.25;
	uint32_t ImageSizePx = 300;
};

class Simulation
{
public:
	Simulation(
		boost::asio::io_context& io_context,
		SimulationSettings& settings,
		SVOSharedMemory& shared_memory)
		: m_io_context(io_context)
		, m_settings(settings)
		, m_shared_memory(shared_memory)
		, m_random_device()
		, m_mt19937(m_random_device())
		, m_reject_dist(0.0, 1.0)
		, m_value_dist(0.0, 1.0)
	{
	}

	void start()
	{
		schedule_trigger_loop();
	}

public:
	void setDeviceMode(DeviceModeType v)
	{
		m_device_mode = v;
		on_device_mode_changed(v);
	}

	DeviceModeType getDeviceMode()
	{
		return m_device_mode;
	}

	std::string getConfiguration()
	{
		return "<Configuration Name=\"Dummy\"></Configuration>";
	}

	std::vector<std::string> getListNames()
	{
		std::vector<std::string> result;
		result.push_back("monitorlist1");
		return result;
	}

	std::vector<std::string> getInspectionNames()
	{
		std::vector<std::string> result;
		result.push_back("inspection1");
		result.push_back("inspection2");
		return result;
	}

	std::vector<std::string> getImageNames()
	{
		std::vector<std::string> result;
		result.push_back("inspection1.image1");
		result.push_back("inspection2.image2");
		return result;
	}

	std::vector<std::string> getValueNames()
	{
		std::vector<std::string> result;
		result.push_back("inspection1.weight");
		result.push_back("inspection1.place");
		result.push_back("inspection2.weigth");
		result.push_back("inspection2.place");

		return result;
	}

	void getCurrentProduct(Product& product, const std::string& listname, bool nameInResponse)
	{
		product.set_trigger(get_next_trigger_count());
		product.set_status(State::isValid);
		add_value_to_product(product, listname, nameInResponse);
		ImageId& imgId300px = *product.add_images();
		imgId300px.set_imageindex(m_settings.ImageSizePx);
		imgId300px.set_inspectionid(-1);
		imgId300px.set_trid(-1);
		product.add_imagenames("300px");
	}

	void getObjectSelectorItems(SvPb::TreeItem& root)
	{
		std::vector<std::string> levels = {"Root", "Level1", "Level2"};
		build_object_selector_items_response(root, levels);
	}

	void addNotificationSubscriber(Observer<GetNotificationStreamResponse> observer, std::shared_ptr<ServerStreamContext> ctx)
	{
		std::lock_guard<std::mutex> lock(m_notification_mutex);
		m_notification_subs.push_back(std::make_pair(observer, ctx));
	}

	void addRejectSubscriber(Observer<GetTriggerStreamResponse> observer, std::shared_ptr<ServerStreamContext> ctx)
	{
		std::lock_guard<std::mutex> lock(m_trigger_mutex);
		m_trigger_subs.push_back(std::make_pair(observer, ctx));
	}

private:
	void create_image_of_size(Image& img, unsigned int width, unsigned int height)
	{
		img.set_width(width);
		img.set_height(height);
		create_image_of_size(*img.mutable_rgbdata(), width, height);
	}

	void create_image_of_size(std::string& data, unsigned int width, unsigned int height)
	{
		static int iter = 0;
		int curr_iter = ++iter;
		m_image_cache.getImage(data, width, curr_iter);
	}

	void build_object_selector_items_response(TreeItem& item, std::vector<std::string> levels)
	{
		if (levels.empty())
		{
			return;
		}

		auto name = levels.front();
		levels.erase(levels.begin());

		item.set_name(name);

		for (int i = 0; i < levels.size() + 1; ++i)
		{
			auto& child = *item.add_children();
			build_object_selector_items_response(child, levels);
		}
	}

	template<class TProduct>
	void add_value_to_product(TProduct& prod, const std::string& listname, bool withValueNames = true)
	{
		if (withValueNames)
		{
			*prod.add_valuenames() = "inspection1.weight";
		}
		auto& weight = *prod.add_values();
		weight.set_type(Variant::DataCase::kFltVal);
		static int v = 0;
		weight.set_fltval(++v + 0.23f);
		if (withValueNames)
		{
			*prod.add_valuenames() = "inspection2.place";
		}
		auto& place = *prod.add_values();
		place.set_type(Variant::DataCase::kStrVal);
		place.set_strval("Munich");
	}

	uint32_t get_next_trigger_count()
	{
		return (++m_trigger_count) % 4;
	}

	void on_device_mode_changed(DeviceModeType deviceMode)
	{
		GetNotificationStreamResponse res;
		res.set_currentmode(deviceMode);
		publish_notification(res);
	}

	void publish_notification(GetNotificationStreamResponse res)
	{
		std::lock_guard<std::mutex> lock(m_notification_mutex);

		for (auto it = m_notification_subs.begin(); it != m_notification_subs.end();)
		{
			auto& observer = it->first;
			auto& ctx = it->second;

			if (ctx->isCancelled())
			{
				SV_LOG_GLOBAL(error) << "NotificationSender client disconnected. stopping notifications.";
				it = m_notification_subs.erase(it);
				continue;
			}

			GetNotificationStreamResponse resCopy;
			resCopy.CopyFrom(res);
			observer.onNext(std::move(resCopy));
			++it;
		}
	}

	void schedule_trigger_loop()
	{
		auto sleep_us = static_cast<int64_t>((1000 * 1000) / m_settings.Frequency);
		m_trigger_timer = std::make_shared<boost::asio::deadline_timer>(m_io_context);
		m_trigger_timer->expires_from_now(boost::posix_time::microseconds(sleep_us));
		m_trigger_timer->async_wait(std::bind(&Simulation::on_trigger_timer, this, std::placeholders::_1));
	}

	void on_trigger_timer(boost::system::error_code ec)
	{
		if (ec)
		{
			SV_LOG_GLOBAL(error) << "Trigger timer error: " << ec;
			return;
		}

		auto trigger = get_next_trigger_count();
		auto is_reject = m_reject_dist(m_mt19937) < m_settings.PercentRejects;

		InspectionResult inspection1;
		generate_inspection1_result(inspection1, trigger, is_reject);
		add_inspection_result_to_shared_memory(inspection1, trigger, is_reject);

		InspectionResult inspection2;
		generate_inspection2_result(inspection2, trigger, is_reject);
		add_inspection_result_to_shared_memory(inspection2, trigger, is_reject);

		publish_trigger_to_subscribers(trigger, is_reject);

		schedule_trigger_loop();
	}

	void generate_inspection1_result(InspectionResult& inspection, uint32_t trigger, bool is_reject)
	{
		inspection.set_id(SetGuidInProtoBytes(build_guid("11111111-0000-0000-0000-000000000000")));
		inspection.set_name("inspection1");
		inspection.set_trigger(trigger);
		inspection.set_isreject(is_reject);

		*inspection.add_valueids() = SetGuidInProtoBytes(build_guid("11111111-1111-0000-0000-000000000000"));
		*inspection.add_valuenames() = "inspection1.weight";
		build_double_variant(*inspection.add_values(), m_value_dist(m_mt19937) * 42);

		*inspection.add_valueids() = SetGuidInProtoBytes(build_guid("11111111-2222-0000-0000-000000000000"));
		*inspection.add_valuenames() = "inspection1.place";
		build_string_variant(*inspection.add_values(), "Munich");

		*inspection.add_imageids() = SetGuidInProtoBytes(build_guid("11111111-0000-1111-0000-000000000000"));
		*inspection.add_imagenames() = "inspection1.image1";
		create_image_of_size(*inspection.add_images(), 512, 384);
	}

	void generate_inspection2_result(InspectionResult& inspection, uint32_t trigger, bool is_reject)
	{
		inspection.set_id(SetGuidInProtoBytes(build_guid("22222222-0000-0000-0000-000000000000")));
		inspection.set_name("inspection2");
		inspection.set_trigger(trigger);
		inspection.set_isreject(is_reject);

		*inspection.add_valueids() = SetGuidInProtoBytes(build_guid("22222222-1111-0000-0000-000000000000"));
		*inspection.add_valuenames() = "inspection2.weight";
		build_double_variant(*inspection.add_values(), m_value_dist(m_mt19937) * 3.14);

		*inspection.add_valueids() = SetGuidInProtoBytes(build_guid("22222222-2222-0000-0000-000000000000"));
		*inspection.add_valuenames() = "inspection2.place";
		build_string_variant(*inspection.add_values(), "Berlin");

		*inspection.add_imageids() = SetGuidInProtoBytes(build_guid("22222222-0000-1111-0000-000000000000"));
		*inspection.add_imagenames() = "inspection2.image2";
		create_image_of_size(*inspection.add_images(), 512, 384);
	}

	GUID build_guid(const std::string& str)
	{
		std::string tmp = "{";
		tmp += str;
		tmp += "}";
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wide = converter.from_bytes(tmp);
		GUID guid;
		CLSIDFromString(wide.c_str(), &guid);
		return guid;
	}

	void build_double_variant(Variant& variant, double v)
	{
		variant.set_type(Variant::DataCase::kDblVal);
		variant.set_dblval(v);
	}

	void build_string_variant(Variant& variant, const char* str)
	{
		variant.set_type(Variant::DataCase::kStrVal);
		variant.set_strval(str);
	}

	void add_inspection_result_to_shared_memory(const InspectionResult& inspection, uint32_t trigger, bool is_reject)
	{
		InspectionResult inspectionCopy;
		inspectionCopy.CopyFrom(inspection);
		m_shared_memory.addInspectionResult(std::move(inspectionCopy));
	}

	void publish_trigger_to_subscribers(uint32_t trigger, bool is_reject)
	{
		std::lock_guard<std::mutex> lock(m_trigger_mutex);

		for (auto it = m_trigger_subs.begin(); it != m_trigger_subs.end();)
		{
			auto& observer = it->first;
			auto& ctx = it->second;

			if (ctx->isCancelled())
			{
				SV_LOG_GLOBAL(error) << "trigger sender client disconnected. stopping notifications.";
				it = m_trigger_subs.erase(it);
				continue;
			}

			GetTriggerStreamResponse res;
			res.set_trigger(trigger);
			res.set_is_reject(is_reject);
			observer.onNext(std::move(res));

			++it;
		}
	}

private:
	SimulationSettings& m_settings;
	boost::asio::io_context& m_io_context;
	SVOSharedMemory& m_shared_memory;

	DeviceModeType m_device_mode {DeviceModeType::runMode};
	uint32_t m_trigger_count = {0};
	ImageCache m_image_cache;

	std::random_device m_random_device;
	std::mt19937 m_mt19937;
	std::uniform_real_distribution<double> m_reject_dist;
	std::uniform_real_distribution<double> m_value_dist;

	std::shared_ptr<boost::asio::deadline_timer> m_trigger_timer;

	std::vector<std::pair<Observer<GetNotificationStreamResponse>, std::shared_ptr<ServerStreamContext>>> m_notification_subs;
	std::mutex m_notification_mutex; // protects m_notification_subs
	std::vector<std::pair<Observer<GetTriggerStreamResponse>, std::shared_ptr<ServerStreamContext>>> m_trigger_subs;
	std::mutex m_trigger_mutex; // protects m_trigger_subs

};
}
