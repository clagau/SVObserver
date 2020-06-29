//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file SimulatedTriggerSource.cpp
/// This is the class used to simulate triggers without the Hilscher card
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "InspectionState.h"
#include "PowerLinkConnection.h"
#include "SimulatedTriggerSource.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvPlc
{
constexpr unsigned int cTimerResolution = 1;
constexpr size_t cCycleParameterNumber = 8;
constexpr LPCTSTR cResultFilename = _T("Result");
constexpr LPCTSTR cResultExtension = _T(".csv");
constexpr LPCTSTR cResultHeader = _T("ObjectID; Trigger Timestamp; Result Timestamp; Results\r\n");


std::mutex gTriggerDataMutex;
std::array<TriggerReport, cNumberOfChannels> gTriggerReport;
std::array<std::atomic_bool, cNumberOfChannels> gNewTrigger {false, false, false, false};

void ChannelTimer(std::atomic_bool& rRun, SimulatedTriggerData simTriggerData)
{
	if(0 != simTriggerData.m_initialDelay)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(simTriggerData.m_initialDelay));
	}
	uint8_t currentIndex {1};
	uint32_t currentObjectID {simTriggerData.m_objectID};

	while(rRun)
	{
		if(currentIndex <= simTriggerData.m_triggerPerObjectID)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(simTriggerData.m_period));
			TriggerReport triggerReport;
			triggerReport.m_triggerTimestamp = SvTl::GetTimeStamp();
			triggerReport.m_channel = simTriggerData.m_channel;
			triggerReport.m_triggerPerObjectID = simTriggerData.m_triggerPerObjectID;
			triggerReport.m_currentObjectID = currentObjectID;
			triggerReport.m_triggerIndex = currentIndex;
			triggerReport.m_isValid = true;
			{
				std::lock_guard<std::mutex> guard {gTriggerDataMutex};
				std::swap(gTriggerReport[triggerReport.m_channel], triggerReport);
			}
			gNewTrigger[simTriggerData.m_channel] = true;
			currentIndex++;
			::SetEvent(g_hSignalEvent);
		}

		if(currentIndex > simTriggerData.m_triggerPerObjectID)
		{
			if(0 != simTriggerData.m_objectDelay)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(simTriggerData.m_objectDelay));
			}
			currentIndex = 1;
			currentObjectID++;
			if(simTriggerData.m_objectNumber > 0 && currentObjectID >= simTriggerData.m_objectID + simTriggerData.m_objectNumber)
			{
				rRun = false;
			}
		}
	}
}

SimulatedTriggerSource::SimulatedTriggerSource(std::function<void(const TriggerReport&)> pReportTrigger, const std::string& rSimulateFile) : TriggerSource(pReportTrigger)
	,m_plcSimulateFile{rSimulateFile}
{

}

HRESULT SimulatedTriggerSource::initialize()
{
	if(false == m_plcSimulateFile.empty())
	{
		std::ifstream cycleFile;
		std::string fileLine;
		cycleFile.open(m_plcSimulateFile.c_str(), std::ifstream::in | std::ifstream::binary);
		if(false== cycleFile.is_open())
		{
			return ERROR_FILE_NOT_FOUND;
		}
		///Header
		std::getline(cycleFile, fileLine);
		while (false == cycleFile.eof())
		{
			std::getline(cycleFile, fileLine);
			if(false == fileLine.empty())
			{
				std::vector<std::string> cycleParameters;
				std::stringstream stringStream(fileLine);
				std::string value;
				while (std::getline(stringStream, value, ';'))
				{
					cycleParameters.push_back(value);
				}

				if(cCycleParameterNumber == cycleParameters.size())
				{
					SimulatedTriggerData triggerData;
					triggerData.m_name = cycleParameters[0];
					///Covert Trigger index (1 based) to trigger channel (0 based)
					triggerData.m_channel = static_cast<uint8_t> (std::stoul(cycleParameters[1].c_str()) - 1);
					triggerData.m_objectNumber = std::stoul(cycleParameters[2].c_str());
					triggerData.m_objectID = std::stoul(cycleParameters[3].c_str());
					triggerData.m_triggerPerObjectID = static_cast<uint8_t> (std::stoul(cycleParameters[4].c_str()));
					triggerData.m_initialDelay = std::stoul(cycleParameters[5].c_str());
					triggerData.m_period = std::stoul(cycleParameters[6].c_str());
					triggerData.m_objectDelay = std::stoul(cycleParameters[7].c_str());

					if(triggerData.m_channel < cNumberOfChannels)
					{
						m_channel[triggerData.m_channel].m_simulatedTriggerData = triggerData;
					}
				}
			}
		}
		cycleFile.close();
	}
	return S_OK;
}

bool SimulatedTriggerSource::setTriggerChannel(uint8_t channel, bool active)
{
	if(channel < cNumberOfChannels)
	{
		if(active)
		{
			//Restart trigger channel
			if(m_channel[channel].m_runThread == false)
			{
				///Write result file in same directory as cycles file
				std::string resultFilename;
				size_t pos = m_plcSimulateFile.rfind('\\');
				if(std::string::npos != pos)
				{
					resultFilename = m_plcSimulateFile.substr(0ULL, pos + 1);
					resultFilename += cResultFilename;
					resultFilename += m_channel[channel].m_simulatedTriggerData.m_name;
					resultFilename += cResultExtension;
				}
				if(false == resultFilename.empty())
				{
					m_channel[channel].m_resultFile.open(resultFilename.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::ate);
					if(m_channel[channel].m_resultFile.is_open())
					{
						std::string fileHeader {cResultHeader};
						m_channel[channel].m_resultFile.write(fileHeader.c_str(), fileHeader.size());
					}
				}
				m_channel[channel].m_runThread = true;
				m_channel[channel].m_timerThread = std::thread(&ChannelTimer, std::ref(m_channel[channel].m_runThread), m_channel[channel].m_simulatedTriggerData);
			}
		}
		else
		{
			if (m_channel[channel].m_timerThread.joinable())
			{
				m_channel[channel].m_runThread = false;
				m_channel[channel].m_timerThread.join();
				if (m_channel[channel].m_resultFile.is_open())
				{
					m_channel[channel].m_resultFile.close();
				}
			}
		}
	}

	bool result = __super::setTriggerChannel(channel, active);
	if(true == result)
	{
		::timeBeginPeriod(cTimerResolution);
	}
	else
	{
		::timeEndPeriod(cTimerResolution);
	}
	return result;
}

bool SimulatedTriggerSource::analyzeTelegramData()
{
	if(std::any_of(gNewTrigger.cbegin(), gNewTrigger.cend(), [](const auto& rNew) { return rNew.load(); }))
	{
		return checkForNewTriggers();
	}

	return false;
}

void SimulatedTriggerSource::queueResult(uint8_t channel, ChannelOut&& channelOut)
{
	///Channel has already been checked
	if (m_channel[channel].m_resultFile.is_open())
	{
		double triggerTimestamp{0.0};
		const auto iter = m_channel[channel].m_objectIDTimeMap.find(channelOut.m_currentObjectID);
		if(m_channel[channel].m_objectIDTimeMap.end() != iter)
		{
			triggerTimestamp = iter->second;
			m_channel[channel].m_objectIDTimeMap.erase(iter);
		}

		std::string resultString;
		for (auto& rResult : channelOut.m_results)
		{
			resultString += std::to_string(rResult) + ' ';
		}
		std::string fileData = SvUl::Format(_T("%4lu; %.3f; %.3f; %s\r\n"), channelOut.m_currentObjectID, triggerTimestamp, SvTl::GetTimeStamp(), resultString.c_str());
		m_channel[channel].m_resultFile.write(fileData.c_str(), fileData.size());
	}
}

void SimulatedTriggerSource::createTriggerReport(uint8_t channel)
{
	if(gNewTrigger[channel])
	{
		TriggerReport triggerReport;
		{
			std::lock_guard<std::mutex> guard {gTriggerDataMutex};
			std::swap(triggerReport, gTriggerReport[channel]);
		}
		sendTriggerReport(triggerReport);
		m_channel[channel].m_objectIDTimeMap[triggerReport.m_currentObjectID] = triggerReport.m_triggerTimestamp;
	}
}

} //namespace SvPlc
