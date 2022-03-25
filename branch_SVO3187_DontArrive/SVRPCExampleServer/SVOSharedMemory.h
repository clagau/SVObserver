#pragma once

#include <map>
#include <mutex>
#include <string>
#include <sstream>
#include <vector>

#include "SVProtoBuf/SVRC-Enum.h"
#include "SVProtoBuf/SVRC.h"

using namespace SvPb;

namespace SVRPCExample
{
class SVOSharedMemory
{
public:
	SVOSharedMemory()
	{
	}

public:
	void addInspectionResult(SvPb::InspectionResult&& inspection)
	{
		std::lock_guard<std::mutex> lock(m_inspections_mutex);

		m_inspections.emplace_back(std::move(inspection));
		// TODO fix for multiple inspections!
		while (m_inspections.size() > 20)
		{
			m_inspections.erase(m_inspections.begin());
		}
	}

	bool getInspectionResultForTrigger(uint32_t trigger, const std::string& inspectionId, SvPb::InspectionResult& inspection)
	{
		std::lock_guard<std::mutex> lock(m_inspections_mutex);

		for (const auto& entry : m_inspections)
		{
			if (entry.trigger() == trigger && entry.id() == inspectionId)
			{
				inspection.CopyFrom(entry);
				return true;
			}
		}
		return false;
	}

private:
	std::vector<SvPb::InspectionResult> m_inspections;
	std::mutex m_inspections_mutex; // protects m_inspections
};
}
