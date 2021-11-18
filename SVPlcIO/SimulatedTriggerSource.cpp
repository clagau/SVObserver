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
#include "SVUtilityLibrary/SVClock.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvPlc
{
constexpr unsigned int cTimerResolution = 1;
constexpr size_t cCycleParameterNumber = 8;
constexpr uint8_t cPlcInvalid = 4;
constexpr uint8_t cPlcBad = 5;
constexpr uint8_t cPlcGood = 6;
constexpr uint8_t cSingleObjectType = 1;
constexpr LPCTSTR cResultFilename = _T("Result");
constexpr LPCTSTR cResultExtension = _T(".csv");
constexpr LPCTSTR cResultHeader = _T("ObjectID; Trigger Timestamp; Result Timestamp; Results\r\n");
constexpr LPCTSTR cObjectGood = _T("ObjectGood=");
constexpr LPCTSTR cObjectBad = _T("ObjectBad=");
constexpr LPCTSTR cObjectInvalid = _T("ObjectInvalid=");


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
	uint32_t objectID {simTriggerData.m_objectID};

	while(rRun)
	{
		if(currentIndex <= simTriggerData.m_triggerPerObjectID)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(simTriggerData.m_period));
			TriggerReport triggerReport;
			triggerReport.m_triggerTimestamp = SvUl::GetTimeStamp();
			triggerReport.m_channel = simTriggerData.m_channel;
			triggerReport.m_objectID = objectID;
			triggerReport.m_objectType = cSingleObjectType;
			triggerReport.m_triggerPerObjectID = simTriggerData.m_triggerPerObjectID;
			triggerReport.m_triggerIndex = currentIndex;
			triggerReport.m_isValid = true;
			{
				std::lock_guard<std::mutex> guard {gTriggerDataMutex};
				gTriggerReport[triggerReport.m_channel] = std::move(triggerReport);
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
			objectID++;
			if(simTriggerData.m_objectNumber > 0 && objectID > simTriggerData.m_objectID + simTriggerData.m_objectNumber)
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
	HRESULT result{ S_OK };
	if(false == m_plcSimulateFile.empty())
	{
		std::ifstream cycleFile;
		std::string fileLine;
		cycleFile.open(m_plcSimulateFile.c_str(), std::ifstream::in | std::ifstream::binary);
		if(false== cycleFile.is_open())
		{
			result = ERROR_FILE_NOT_FOUND;
		}
		if (S_OK == result)
		{
			///Header
			std::getline(cycleFile, fileLine);
			while (false == cycleFile.eof())
			{
				std::getline(cycleFile, fileLine);
				if (false == fileLine.empty())
				{
					std::vector<std::string> cycleParameters;
					std::stringstream stringStream(fileLine);
					std::string value;
					while (std::getline(stringStream, value, ';'))
					{
						cycleParameters.push_back(value);
					}

					if (cCycleParameterNumber == cycleParameters.size())
					{
						SimulatedTriggerData triggerData;
						triggerData.m_name = cycleParameters[0];
						///Convert Trigger index (1 based) to trigger channel (0 based)
						uint8_t triggerIndex = static_cast<uint8_t> (std::stoul(cycleParameters[1]));
						if (0 == triggerIndex)
						{
							result = E_INVALIDARG;
							break;
						}
						triggerData.m_channel = triggerIndex - 1;
						triggerData.m_objectNumber = std::stoul(cycleParameters[2]);
						triggerData.m_objectID = std::stoul(cycleParameters[3]);
						triggerData.m_triggerPerObjectID = static_cast<uint8_t> (std::stoul(cycleParameters[4].c_str()));
						double intialDelayRatio = std::stod(cycleParameters[5]) / 100.0;
						double machineSpeed = std::stod(cycleParameters[6]);
						double objectDelayRatio = std::stod(cycleParameters[7]) / 100.0;

						if (0.0 == machineSpeed || objectDelayRatio > 1.0 || intialDelayRatio > 1.0)
						{
							result = E_INVALIDARG;
							break;
						}
						double objectPeriod = 60.0 / machineSpeed * 1000.0;
						double mainTriggerPeriod = objectPeriod * (1.0 - objectDelayRatio) ;
						triggerData.m_initialDelay = static_cast<uint32_t> (objectPeriod * intialDelayRatio);
						triggerData.m_period = static_cast<uint32_t> (mainTriggerPeriod / triggerData.m_triggerPerObjectID);
						triggerData.m_objectDelay = static_cast<uint32_t> (objectPeriod * objectDelayRatio);

						if (triggerData.m_channel < cNumberOfChannels)
						{
							m_channel[triggerData.m_channel].m_simulatedTriggerData = std::move(triggerData);
						}
					}
				}
			}
		}
		cycleFile.close();
	}
	else
	{
		result = E_INVALIDARG;
	}
	return result;
}

bool SimulatedTriggerSource::setTriggerChannel(uint8_t channel, bool active)
{
	if(channel < cNumberOfChannels)
	{
		if(active)
		{
			std::lock_guard<std::mutex> guard{ m_triggerSourceMutex };
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
			std::lock_guard<std::mutex> guard{ m_triggerSourceMutex };
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


		std::string resultFilename;
		size_t pos = m_plcSimulateFile.rfind('\\');
		if (std::string::npos != pos)
		{
			resultFilename = m_plcSimulateFile.substr(0ULL, pos + 1);
			resultFilename += cResultFilename;
			resultFilename += cResultExtension;
		}
		if (false == resultFilename.empty())
		{
			std::ofstream resultFile(resultFilename.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::ate);
			if (resultFile.is_open())
			{
				std::string fileData;
				for (size_t i=0; i < m_channel.size(); ++i)
				{
					fileData += m_channel[i].m_simulatedTriggerData.m_name;
					fileData += cObjectGood;
					fileData += std::to_string(m_ObjectsGood[i]) + _T("\r\n");
					fileData += m_channel[i].m_simulatedTriggerData.m_name;
					fileData += cObjectBad;
					fileData += std::to_string(m_ObjectsBad[i]) + _T("\r\n");
					fileData += m_channel[i].m_simulatedTriggerData.m_name;
					fileData += cObjectInvalid;
					fileData += std::to_string(m_ObjectsInvalid[i]) + _T("\r\n");
				}
				resultFile.write(fileData.c_str(), fileData.size());
				resultFile.close();
			}
		}
	}
	return result;
}

void SimulatedTriggerSource::analyzeTelegramData()
{
	if(std::any_of(gNewTrigger.cbegin(), gNewTrigger.cend(), [](const auto& rNew) { return rNew.load(); }))
	{
		checkForNewTriggers();
	}
}

void SimulatedTriggerSource::queueResult(uint8_t channel, ChannelOut1&& channelOut)
{
	/// Highest priority is invalid then bad then good (of all 14 results)
	if (channelOut.m_results.end() != std::find(channelOut.m_results.begin(), channelOut.m_results.end(), cPlcInvalid))
	{
		++m_ObjectsInvalid[channel];
	}
	else if (channelOut.m_results.end() != std::find(channelOut.m_results.begin(), channelOut.m_results.end(), cPlcBad))
	{
		++m_ObjectsBad[channel];
	}
	else if (channelOut.m_results.end() != std::find(channelOut.m_results.begin(), channelOut.m_results.end(), cPlcGood))
	{
		++m_ObjectsGood[channel];
	}
	double triggerTimestamp{0.0};
	{
		std::lock_guard<std::mutex> guard{ m_triggerSourceMutex };
		const auto iter = m_channel[channel].m_objectIDTimeMap.find(channelOut.m_objectID);
		if (m_channel[channel].m_objectIDTimeMap.end() != iter)
		{
			triggerTimestamp = iter->second;
			m_channel[channel].m_objectIDTimeMap.erase(iter);
		}
	}

	std::string resultString;
	for (auto& rResult : channelOut.m_results)
	{
		resultString += std::to_string(rResult) + ' ';
	}
	std::string fileData = SvUl::Format(_T("%4lu; %.3f; %.3f; %s\r\n"), channelOut.m_objectID, triggerTimestamp, SvUl::GetTimeStamp(), resultString.c_str());
	
	{
		std::lock_guard<std::mutex> guard{ m_triggerSourceMutex };
		///Channel has already been checked
		if (m_channel[channel].m_resultFile.is_open())
		{
			m_channel[channel].m_resultFile.write(fileData.c_str(), fileData.size());
		}
	}
}

void SimulatedTriggerSource::createTriggerReport(uint8_t channel)
{
	if(gNewTrigger[channel])
	{
		TriggerReport triggerReport;
		{
			std::lock_guard<std::mutex> guard {gTriggerDataMutex};
			triggerReport = std::move(gTriggerReport[channel]);
			gTriggerReport[channel] = {};
		}
		gNewTrigger[channel] = false;
		sendTriggerReport(triggerReport);
		{
			std::lock_guard<std::mutex> guard{ m_triggerSourceMutex };
			m_channel[channel].m_objectIDTimeMap[triggerReport.m_objectID] = triggerReport.m_triggerTimestamp;
		}
	}
}

} //namespace SvPlc
