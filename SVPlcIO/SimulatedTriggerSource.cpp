//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file SimulatedTriggerSource.cpp
/// This is the class used to simulate triggers without the Hilscher card
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "InspectionState.h"
#include "PowerLinkConnection.h"
#include "SimulatedTriggerSource.h"
#include "FilesystemUtilities/FilepathUtilities.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvPlc
{
constexpr size_t cCycleParameterNumber = 9;
constexpr uint8_t cPlcInvalid = 4;
constexpr uint8_t cPlcBad = 5;
constexpr uint8_t cPlcGood = 6;
constexpr uint8_t cSingleObjectType = 1;
constexpr uint32_t cNormalizePeriod = 500;
constexpr int cTimerConversion = 10;
constexpr double cSecondsPerMinute = 60.0;
constexpr double cConversionMicrosec = 1000000.0;
constexpr LPCTSTR cResultFilename = _T("Result");
constexpr LPCTSTR cResultExtension = _T(".csv");
constexpr LPCTSTR cResultHeader = _T("ObjectID; Results; Acquisition File\r\n");
constexpr LPCTSTR cObjectGood = _T("ObjectGood=");
constexpr LPCTSTR cObjectBad = _T("ObjectBad=");
constexpr LPCTSTR cObjectInvalid = _T("ObjectInvalid=");


std::mutex gTriggerDataMutex;
std::array<TriggerReport, cNumberOfChannels> gTriggerReport;
std::array<std::atomic_bool, cNumberOfChannels> gNewTrigger {false, false, false, false};

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
		cycleFile.open(m_plcSimulateFile.c_str(), std::ifstream::in | std::ifstream::binary);
		if(false== cycleFile.is_open())
		{
			result = ERROR_FILE_NOT_FOUND;
		}
		if (S_OK == result)
		{
			std::vector<std::vector<std::string>> cycleParamList;
			cycleParamList.reserve(cNumberOfChannels);
			///Header
			std::string fileLine;
			std::getline(cycleFile, fileLine);
			while (false == cycleFile.eof())
			{
				std::getline(cycleFile, fileLine);
				if (true == fileLine.empty() || fileLine.starts_with(_T("//")))
				{
					continue;
				}
				std::vector<std::string> cycleParam;
				cycleParam.resize(cCycleParameterNumber);
				std::stringstream stringStream(fileLine);
				std::string value;
				size_t parameterNr {0};
				while (std::getline(stringStream, value, ';'))
				{
					if (parameterNr >= cCycleParameterNumber)
					{
						break;
					}
					cycleParam[parameterNr] = value;
					++parameterNr;
				}

				cycleParamList.emplace_back(std::move(cycleParam));
			}
			cycleFile.close();

			result = initChannel(cycleParamList);
		}
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
			if(nullptr == m_channel[channel].m_timerInfo.m_callbackFunction)
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
				m_channel[channel].m_timerInfo.m_name = m_channel[channel].m_simulatedTriggerData.m_name;
				const uint32_t& rPeriod = m_channel[channel].m_simulatedTriggerData.m_period;
				//Normalize the period to the next 500µs
				m_channel[channel].m_timerInfo.m_period = rPeriod + cNormalizePeriod - rPeriod % cNormalizePeriod;
				m_channel[channel].m_timerInfo.m_initialDelay = m_channel[channel].m_simulatedTriggerData.m_initialDelay;
				m_channel[channel].m_timerInfo.m_callbackFunction = [this](const std::string& rName, double timestamp) {  dispatchTrigger(rName, timestamp); };
				SvUl::PeriodicTimer::SetTimer(m_channel[channel].m_timerInfo);
			}
		}
		else
		{
			std::lock_guard<std::mutex> guard{ m_triggerSourceMutex };
			if (nullptr != m_channel[channel].m_timerInfo.m_callbackFunction)
			{
				SvUl::PeriodicTimer::CloseTimer(m_channel[channel].m_timerInfo.m_name.c_str());
				if (m_channel[channel].m_resultFile.is_open())
				{
					m_channel[channel].m_resultFile.close();
				}
			}
		}
	}

	bool result = __super::setTriggerChannel(channel, active);
	if(false == result)
	{
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
				resultFile.write(fileData.c_str(), static_cast<int64_t> (fileData.size()));
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
	std::string  acuisitionFile;
	{
		std::lock_guard<std::mutex> guard{ m_triggerSourceMutex };
		const auto iter = m_channel[channel].m_objectIDFileMap.find(channelOut.m_objectID);
		if (m_channel[channel].m_objectIDFileMap.end() != iter)
		{
			acuisitionFile = iter->second;
			m_channel[channel].m_objectIDFileMap.erase(iter);
		}
	}

	std::string resultString;
	for (auto& rResult : channelOut.m_results)
	{
		resultString += std::to_string(rResult) + ' ';
	}
	std::string fileData = std::format(_T("{};{};{}\r\n"), channelOut.m_objectID, resultString.c_str(), acuisitionFile.c_str());
	
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
		if(triggerReport.m_triggerIndex == triggerReport.m_triggerPerObjectID)
		{
			std::lock_guard<std::mutex> guard{ m_triggerSourceMutex };
			m_channel[channel].m_objectIDFileMap[triggerReport.m_objectID] = triggerReport.m_acquisitionFile;
		}
	}
}

HRESULT SimulatedTriggerSource::initChannel(const std::vector<std::vector<std::string>>& rCycleParamList)
{
	if (0ULL == rCycleParamList.size())
	{
		return E_INVALIDARG;
	}

	for (const auto rCycleParam : rCycleParamList)
	{
		SimulatedTriggerData triggerData;
		triggerData.m_name = rCycleParam[FileParamColumnPos::triggerName];
		///Convert Trigger index (1 based) to trigger channel (0 based)
		uint8_t triggerIndex = static_cast<uint8_t> (std::stoul(rCycleParam[FileParamColumnPos::triggerIndex]));
		if (0 == triggerIndex)
		{
			return E_INVALIDARG;
		}
		triggerData.m_channel = triggerIndex - 1;
		triggerData.m_objectNumber = std::stol(rCycleParam[FileParamColumnPos::objectNumber]);
		triggerData.m_objectID = std::stoul(rCycleParam[FileParamColumnPos::objectID]);
		triggerData.m_triggerPerObjectID = static_cast<uint8_t> (std::stoul(rCycleParam[FileParamColumnPos::triggerPerObjectID].c_str()));
		double intialDelayRatio = std::stod(rCycleParam[FileParamColumnPos::initialDelay]) / 100.0;
		double machineSpeed = std::stod(rCycleParam[FileParamColumnPos::machineSpeed]);
		double objectDelayRatio = std::stod(rCycleParam[FileParamColumnPos::objectDelay]) / 100.0;

		if (0.0 == machineSpeed || objectDelayRatio > 1.0 || intialDelayRatio > 1.0)
		{
			return E_INVALIDARG;
		}
		double objectPeriod = cSecondsPerMinute / machineSpeed * cConversionMicrosec;
		double mainTriggerPeriod = objectPeriod * (1.0 - objectDelayRatio);
		triggerData.m_initialDelay = static_cast<uint32_t> (objectPeriod * intialDelayRatio);
		triggerData.m_period = static_cast<uint32_t> (mainTriggerPeriod / triggerData.m_triggerPerObjectID);
		triggerData.m_objectDelay = static_cast<uint32_t> (objectPeriod * objectDelayRatio);

		std::string validationFolder = SvUl::Trim(rCycleParam[FileParamColumnPos::validationFolder].c_str());

		if (false == validationFolder.empty())
		{
			triggerData.m_LoadImageList = SvFs::getFileList(validationFolder.c_str(), ImageFileFormat::bmp, false);
		}

		if (triggerData.m_channel < cNumberOfChannels)
		{
			m_channel[triggerData.m_channel].m_simulatedTriggerData = std::move(triggerData);
			m_channel[triggerData.m_channel].m_intialize = true;
		}
	}
	return S_OK;
}

void SimulatedTriggerSource::dispatchTrigger(const std::string& rName, double timestamp)
{
	struct CurrentTriggerData
	{
		uint32_t m_imageIndex {0UL};
		uint8_t m_triggerIndex {1};
		uint32_t m_objectID {0};
	};
	static std::array<CurrentTriggerData, cNumberOfChannels> currentTriggerData;
	int triggerChannel {-1};

	for (int i = 0; i < static_cast<int> (m_channel.size()); ++i)
	{
		if (rName == m_channel[i].m_simulatedTriggerData.m_name)
		{
			triggerChannel = i;
			break;
		}
	}
	if (triggerChannel < 0 || triggerChannel >= m_channel.size())
	{
		return;
	}
	SimulatedTriggerData& rSimTriggerData = m_channel[triggerChannel].m_simulatedTriggerData;

	if (m_channel[triggerChannel].m_intialize)
	{
		currentTriggerData[triggerChannel].m_imageIndex = 0UL;
		currentTriggerData[triggerChannel].m_triggerIndex = 1;
		currentTriggerData[triggerChannel].m_objectID = rSimTriggerData.m_objectID;
		m_channel[triggerChannel].m_intialize = false;
	}

	std::string acquisitionFile;
	if (currentTriggerData[triggerChannel].m_imageIndex < static_cast<uint32_t> (rSimTriggerData.m_LoadImageList.size()))
	{
		acquisitionFile = rSimTriggerData.m_LoadImageList[currentTriggerData[triggerChannel].m_imageIndex];
	}
	if (false == acquisitionFile.empty())
	{
		size_t pos2 = acquisitionFile.find_last_of('.');
		size_t pos1 = acquisitionFile.find_last_of('_', pos2);
		uint8_t triggerIndex {0};
		if (std::string::npos != pos1 && std::string::npos != pos2)
		{
			triggerIndex = static_cast<uint8_t> (std::stoul(acquisitionFile.substr(pos1 + 1, pos2 - pos1 - 1)));
			pos2 = pos1 - 1;
		}
		pos1 = acquisitionFile.find_last_of('_', pos2);
		if (std::string::npos != pos1 && std::string::npos != pos2 && 0 != triggerIndex)
		{
			currentTriggerData[triggerChannel].m_objectID = std::stoul(acquisitionFile.substr(pos1 + 1, pos2 - pos1));
			currentTriggerData[triggerChannel].m_triggerIndex = triggerIndex;
		}
	}

	if (false == m_channel[triggerChannel].m_timerInfo.m_pause)
	{
		TriggerReport triggerReport;
		triggerReport.m_triggerTimestamp = timestamp;
		triggerReport.m_channel = rSimTriggerData.m_channel;
		triggerReport.m_objectID = currentTriggerData[triggerChannel].m_objectID;
		triggerReport.m_objectType = cSingleObjectType;
		triggerReport.m_triggerPerObjectID = rSimTriggerData.m_triggerPerObjectID;
		triggerReport.m_triggerIndex = currentTriggerData[triggerChannel].m_triggerIndex;
		triggerReport.m_isValid = true;
		triggerReport.m_acquisitionFile = acquisitionFile;
		{
			std::lock_guard<std::mutex> guard {gTriggerDataMutex};
			gTriggerReport[triggerReport.m_channel] = std::move(triggerReport);
		}
		gNewTrigger[rSimTriggerData.m_channel] = true;
		++currentTriggerData[triggerChannel].m_triggerIndex;
		++currentTriggerData[triggerChannel].m_imageIndex;
		::SetEvent(g_hSignalEvent);
	}

	if (currentTriggerData[triggerChannel].m_triggerIndex > rSimTriggerData.m_triggerPerObjectID)
	{
		if (0 != rSimTriggerData.m_objectDelay)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(rSimTriggerData.m_objectDelay));
		}
		currentTriggerData[triggerChannel].m_triggerIndex = 1;
		currentTriggerData[triggerChannel].m_objectID++;
		if (rSimTriggerData.m_objectNumber > 0 && currentTriggerData[triggerChannel].m_objectID >= rSimTriggerData.m_objectID + rSimTriggerData.m_objectNumber)
		{
			m_channel[triggerChannel].m_timerInfo.m_pause = true;
		}
		if (0 < rSimTriggerData.m_LoadImageList.size() && currentTriggerData[triggerChannel].m_imageIndex >= rSimTriggerData.m_LoadImageList.size())
		{
			m_channel[triggerChannel].m_timerInfo.m_pause = true;
		}
	}
}

} //namespace SvPlc
