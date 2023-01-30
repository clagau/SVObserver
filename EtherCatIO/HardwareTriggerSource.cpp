//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file HardwareTriggerSource.cpp
/// This is the class used for triggers set with the data from the Hilscher card
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "HardwareTriggerSource.h"
#include "PowerLinkConnection.h"
#include "Triggering/TriggerData.h"
#pragma endregion Includes

constexpr int cTimerConversion = 10000;

namespace SvEcat
{

#if defined (ETHERCAT_TIMER)

void CycleTimer(std::atomic_bool& rTimerOn, std::function<void(double)> dispatcher)
{
	HANDLE timerHandle = ::CreateWaitableTimerEx(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
	if (nullptr != timerHandle)
	{
		LARGE_INTEGER pauseTime;
		pauseTime.QuadPart = -static_cast<int64_t>(100 * cTimerConversion);
		while (rTimerOn)
		{
			if (0 == ::SetWaitableTimer(timerHandle, &pauseTime, 0, nullptr, nullptr, FALSE))
			{
				::CancelWaitableTimer(timerHandle);
				::CloseHandle(timerHandle);
				timerHandle = nullptr;
				rTimerOn = false;
			}

			if (nullptr != timerHandle)
			{
				if (WaitForSingleObject(timerHandle, INFINITE) == WAIT_OBJECT_0)
				{
					if (rTimerOn && nullptr != dispatcher)
					{
						dispatcher(0.0);
					}
				}
			}
		}
		::CancelWaitableTimer(timerHandle);
		::CloseHandle(timerHandle);
	}
}
#endif

HardwareTriggerSource::HardwareTriggerSource(std::function<void(const SvTrig::TriggerData&)> pTriggerDataCallback, const std::string& rAdditionalData) : TriggerSource(pTriggerDataCallback)
, m_additionalData {rAdditionalData}
{
	::OutputDebugString("Triggers are received from PLC via CifX card.\n");
}

HardwareTriggerSource::~HardwareTriggerSource()
{
#if defined (ETHERCAT_TIMER)
	m_timerOn = false;
	if (m_timerThread.joinable())
	{
		m_timerThread.join();
	}
#endif
	m_cifXCard.closeCifX();
}

HRESULT HardwareTriggerSource::initialize()
{
	HRESULT result{E_FAIL};

	if(m_initialized == false)
	{
		m_cifXCard.setHandleForTelegramReceptionEvent(g_hSignalEvent);
		result = m_cifXCard.OpenAndInitializeCifX();
		if (S_OK != result)
		{
			::OutputDebugString("Could not open and initialize PLC connection!\n");
			return result;
		}
#if defined (ETHERCAT_TIMER)
		m_timerOn = true;
		std::function<void(double)> dispatcher = [this](double timestamp) { writeOutputs(timestamp); };
		m_timerThread = std::thread(&CycleTimer, std::ref(m_timerOn), dispatcher);
#endif
	}

	return result;
}

uint32_t HardwareTriggerSource::getInputs() const
{
	uint32_t result {0UL};
	::memcpy(&result, &m_previousInputData[0], sizeof(result));
	return result;
}

void HardwareTriggerSource::setOutput(uint8_t outputNr, bool state)
{
	constexpr uint8_t cBitsPerModule = 8;
	uint8_t outputIndex = outputNr / cBitsPerModule;
	uint8_t outputBit = outputNr % cBitsPerModule;
	uint8_t outputMask = state ? (1 << outputBit) : ~(1 << outputBit);
	if(outputIndex < m_outputData.size())
	{
		std::lock_guard<std::mutex> guard {m_triggerSourceMutex};
		if (state)
		{
			m_outputData[outputIndex] |= outputMask;
		}
		else
		{
			m_outputData[outputIndex] &= outputMask;
		}
	}
}

#if defined (ETHERCAT_TIMER)
void HardwareTriggerSource::writeOutputs(double)
{
	m_outputData[0] = (0 == m_outputData[0]) ? 2 : 0;
}
#endif


void HardwareTriggerSource::analyzeTelegramData()
{
	while (m_cifXCard.popInputDataQueue())
	{
		const InputData& rInputData = m_cifXCard.getCurrentInputData();

		std::array<uint8_t, cEtherCatDataSize> outputData {0,0,0,0,0,0,0,0};
		{
			std::lock_guard<std::mutex> guard {m_triggerSourceMutex};
			outputData = m_outputData;
		}

		m_cifXCard.sendOperationData(outputData);

		//If new trigger data check for new triggers
		if (0 != memcmp(&m_previousInputData[0], &rInputData.m_dynamicData[0], cEtherCatDataSize))
		{
			checkForNewTriggers();
			memcpy(&m_previousInputData[0], &rInputData.m_dynamicData[0], cEtherCatDataSize);
		}
	}
}

void HardwareTriggerSource::createTriggerData(uint8_t channel)
{
	uint8_t inputMask = (1U << channel);
	const InputData& rInputData = m_cifXCard.getCurrentInputData();
	bool newTrigger = (rInputData.m_dynamicData[0] & inputMask) != 0 && (m_previousInputData[0] & inputMask) == 0;
	if (newTrigger)
	{
		SvTrig::TriggerData triggerData;
		triggerData.m_channel = channel;
		triggerData.m_triggerTimestamp = rInputData.m_notificationTime;
		sendTriggerData(triggerData);
	}
}
} //namespace SvEcat
