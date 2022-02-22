//*****************************************************************************
/// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file TestNonIO.cpp
/// \brief Console main file for TestNonIO app
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "Digitizer.h"
#include "SVDigitizerLoadLibraryClass.h"
#include "SVIOTriggerLoadLibraryClass.h"
#pragma endregion Includes

constexpr uint8_t cNumberOfChannels = 4;
constexpr int cSignalEdgeIndex = 0;
constexpr int cModuleReadyIndex = 3;
constexpr int cBoardTypeIndex = 102;
constexpr int cSVGigeParameterIPAddress = 104;
constexpr long cRabbitBoardType = 20L;
constexpr long cFallingEdge = -1L;
constexpr double cNanoToMilli = 1000000.0;

constexpr LPCTSTR cPlcIoDll = "SVPlcIO.dll";
constexpr LPCTSTR cLptIoDll = "SVLptIO.dll";
constexpr LPCTSTR cMatroxGigeDll = "SVMatroxGige.dll";

constexpr std::array<LPCTSTR, cNumberOfChannels> cCameraIpAdresses =
{
	"192.168.101.2", 
	"192.168.102.2", 
	"192.168.103.2", 
	"192.168.104.2"
};

std::mutex g_triggerQueueMutex;
std::mutex g_cameraTriggerQueueMutex;
std::queue<double> g_triggerQueue;
std::queue<double> g_cameraTriggerQueue;
std::atomic_bool g_triggerStarted {false};
SVAcquisitionBuffer g_Acquisition;
//Timespan default +-5ms
double g_timespanPlus {10.0};
double g_timespanMinus {-1.7};
uint8_t g_triggerChannel {0};


double GetTimeStamp()
{
	return static_cast<double> (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()) / cNanoToMilli;
}

void TriggerCorrelate()
{
	static unsigned long imageCount {0UL};
	static unsigned long triggerCount {0UL};

	while (false == g_triggerStarted)
	{
		Sleep(1);
	}

	while (true == g_triggerStarted)
	{
		double cameraTriggerTimestamp {0.0};
		double triggerTimestamp {0.0};

		{
			std::lock_guard<std::mutex> guard {g_cameraTriggerQueueMutex};
			if (false == g_cameraTriggerQueue.empty())
			{
				cameraTriggerTimestamp = g_cameraTriggerQueue.front();
				g_cameraTriggerQueue.pop();
				++imageCount;
			}
		}

		while (0.0 < cameraTriggerTimestamp)
		{
			{
				std::lock_guard<std::mutex> guard {g_triggerQueueMutex};
				if (false == g_triggerQueue.empty())
				{
					triggerTimestamp = g_triggerQueue.front();
					g_triggerQueue.pop();
					++triggerCount;
				}
			}
			if (0.0 < triggerTimestamp)
			{
				double diff = cameraTriggerTimestamp - triggerTimestamp;
				if (diff > g_timespanMinus && diff < g_timespanPlus)
				{
					printf("%lu; %lu; %f; %f; %f; %d\n", triggerCount, imageCount, triggerTimestamp, cameraTriggerTimestamp, diff, true);
					cameraTriggerTimestamp = 0.0;
				}
				else if (diff > g_timespanPlus)
				{
					printf("%lu; %lu; %f; %f; %f; %d\n", triggerCount, imageCount, triggerTimestamp, cameraTriggerTimestamp, diff, false);
				}
				else if (diff < g_timespanMinus)
				{
					cameraTriggerTimestamp = 0.0;
					std::lock_guard<std::mutex> guard {g_triggerQueueMutex};
				}
			}
			if (false == g_triggerStarted)
			{
				break;
			}
		}
		Sleep(1);
	}
}

void OnImage(CameraInfo&& cameraInfo)
{
	std::lock_guard<std::mutex> guard {g_cameraTriggerQueueMutex};
	g_cameraTriggerQueue.push(cameraInfo.m_startFrameTimestamp);
}

void OnLptIO(const TriggerData& rTriggerData)
{
	double lptTimestamp = (VT_EMPTY != rTriggerData[TriggerDataEnum::TimeStamp].vt) ? static_cast<double> (rTriggerData[TriggerDataEnum::TimeStamp]) : GetTimeStamp();

	std::lock_guard<std::mutex> guard {g_cameraTriggerQueueMutex};
	g_cameraTriggerQueue.push(lptTimestamp);
}

void OnPlcTrigger(const TriggerData& rTriggerData)
{
	double triggerTimestamp = (VT_EMPTY != rTriggerData[TriggerDataEnum::TimeStamp].vt) ? static_cast<double> (rTriggerData[TriggerDataEnum::TimeStamp]) : GetTimeStamp();

	{
		std::lock_guard<std::mutex> guard {g_triggerQueueMutex};
		g_triggerQueue.push(triggerTimestamp);
	}

	if (false == g_triggerStarted)
	{
		{
			std::lock_guard<std::mutex> guard {g_cameraTriggerQueueMutex};
			g_cameraTriggerQueue = {};
		}
		g_triggerStarted = true;
	}
}

HRESULT PlcTriggerToStartFrame()
{
	SVIOTriggerLoadLibraryClass PlcIoDll;
	SVDigitizerLoadLibraryClass DigitizerDll;

	HRESULT result = PlcIoDll.Open(cPlcIoDll);
	if (S_OK == result)
	{
		printf("Loaded %s\n", cPlcIoDll);

		result = DigitizerDll.Open(cMatroxGigeDll);
		if (S_OK == result)
		{
			printf("Loaded %s\n", cMatroxGigeDll);
			printf("Note that using camera start frame,\nthe camera should be configured using a camera file prior to starting this application\n");
			int triggerIndex {g_triggerChannel + 1};

			result = PlcIoDll.Register(triggerIndex, &OnPlcTrigger);
			if (S_OK == result)
			{
				printf("PLC Trigger %d callback registered\n", triggerIndex);
			}
			unsigned long cameraCount = DigitizerDll.GetCount();

			unsigned long cameraHandle {0UL};

			for(unsigned long i=0; i < cameraCount; ++i)
			{
				unsigned long hCamera = DigitizerDll.GetHandle(i);
				_variant_t ipAddress = DigitizerDll.ParameterGetValue(hCamera, cSVGigeParameterIPAddress);
				if (VT_BSTR == ipAddress.vt)
				{
					std::string cameraIP {_bstr_t(ipAddress.bstrVal)};
					int j {0};
					for (; j < cNumberOfChannels; ++j)
					{
						if (cCameraIpAdresses[j] == cameraIP)
						{
							break;
						}
					}
					//Is it the desired trigger channel
					if (j == g_triggerChannel)
					{
						cameraHandle = hCamera;
					}
				}
			}
			if(0UL == cameraHandle)
			{
				return E_FAIL;
			}
			g_Acquisition.m_pCallBack = &OnImage;

			result = DigitizerDll.RegisterBufferInterface(cameraHandle, dynamic_cast<SVAcquisitionBufferInterface*> (&g_Acquisition));
			if (S_OK == result)
			{
				printf("Camera %d callback registered\n", triggerIndex);
				result = PlcIoDll.Start(triggerIndex);
				if (S_OK == result)
				{
					printf("PLC Trigger %d started\n", triggerIndex);
				}

				result = DigitizerDll.CreateBuffers(cameraHandle);
				if (S_OK == result)
				{
					printf("Camera Buffers created for camera %lu\n", cameraHandle);
				}

				result = DigitizerDll.Start(cameraHandle);
				if (S_OK == result)
				{
					printf("Camera %lu started\n", cameraHandle);
				}

				_variant_t moduleReadyState {true};
				if (S_OK == result)
				{
					PlcIoDll.SetParameterValue(cModuleReadyIndex, moduleReadyState);

					printf("Module Ready sent\nTo exit press x\n");
					printf("TriggerCount; ImageCount; TriggerTimestamp; StartFrame; TriggerToStartFrame; Match\n");

					bool exit {false};
					while (false == exit)
					{
						Sleep(1000);
						if (_kbhit())
						{
							exit = ('x' == _getch());
						}
					}
				}
				moduleReadyState = false;
				PlcIoDll.SetParameterValue(cModuleReadyIndex, moduleReadyState);
				printf("Module Ready reset\n");
				PlcIoDll.Stop(triggerIndex);
				DigitizerDll.Stop(cameraHandle);
				PlcIoDll.Unregister(triggerIndex);
				PlcIoDll.Close();
				DigitizerDll.UnregisterBufferInterface(cameraHandle);
				DigitizerDll.Close();
				printf("Trigger %d stopped\n", triggerIndex);
			}
		}
	}
	return result;
}

HRESULT PlcTriggerToCameraTrigger()
{
	SVIOTriggerLoadLibraryClass PlcIoDll;
	SVIOTriggerLoadLibraryClass LptIODll;

	HRESULT result = PlcIoDll.Open(cPlcIoDll);
	if (S_OK == result)
	{
		printf("Loaded %s\n", cPlcIoDll);

		result = LptIODll.Open(cLptIoDll);
		if (S_OK == result)
		{
			printf("Loaded %s\n", cLptIoDll);
			int triggerIndex {g_triggerChannel + 1};

			_variant_t value {cRabbitBoardType};
			LptIODll.SetParameterValue(cBoardTypeIndex, value);
			value = cFallingEdge;
			LptIODll.TrigSetParameterValue(triggerIndex, cSignalEdgeIndex, value);

			result = PlcIoDll.Register(triggerIndex, &OnPlcTrigger);
			if (S_OK == result)
			{
				printf("PLC Trigger %d callback registered\n", triggerIndex);
			}
			result = LptIODll.Register(triggerIndex, &OnLptIO);
			if (S_OK == result)
			{
				printf("LPT Trigger %d callback registered\n", triggerIndex);

				result = PlcIoDll.Start(triggerIndex);
				if (S_OK == result)
				{
					printf("PLC Trigger %d started\n", triggerIndex);
				}
				result = LptIODll.Start(triggerIndex);
				if (S_OK == result)
				{
					printf("LPT Trigger %d started\n", triggerIndex);
				}

				_variant_t moduleReadyState {true};
				if (S_OK == result)
				{
					PlcIoDll.SetParameterValue(cModuleReadyIndex, moduleReadyState);
					LptIODll.SetParameterValue(cModuleReadyIndex, moduleReadyState);

					printf("Module Ready sent\nTo exit press x\n");
					printf("TriggerCount; CameraTriggerCount; TriggerTimestamp; CameraTriggerTimeStamp; TriggerToCameraTrigger; Match\n");

					bool exit {false};
					while (false == exit)
					{
						Sleep(1000);
						if (_kbhit())
						{
							exit = ('x' == _getch());
						}
					}
				}
				moduleReadyState = false;
				PlcIoDll.SetParameterValue(cModuleReadyIndex, moduleReadyState);
				LptIODll.SetParameterValue(cModuleReadyIndex, moduleReadyState);
				printf("Module Ready reset\n");
				PlcIoDll.Stop(triggerIndex);
				LptIODll.Stop(triggerIndex);
				PlcIoDll.Unregister(triggerIndex);
				LptIODll.Unregister(triggerIndex);
				PlcIoDll.Close();
				LptIODll.Close();
				printf("Trigger %d stopped\n", triggerIndex);
			}
		}
	}
	return result;
}

int main(int argc, char* args[])
{
	bool isLptIODll {false};
	if (argc >= 2)
	{
		g_triggerChannel = static_cast<uint8_t> (std::stoi(args[1]) - 1);
		if(g_triggerChannel >= cNumberOfChannels)
		{
			g_triggerChannel = 0;
		}
	}
	if (argc >= 3)
	{
		isLptIODll = std::stoi(args[2]) > 0 ? true : false;
	}
	auto correlateThread = std::thread(&TriggerCorrelate);

	::SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	HRESULT result {S_OK};

	if (isLptIODll)
	{
		result = PlcTriggerToCameraTrigger();
	}
	else
	{
		result = PlcTriggerToStartFrame();
	}

	if (false == g_triggerStarted)
	{
		g_triggerStarted = true;
		Sleep(10);
	}

	g_triggerStarted = false;
	if (correlateThread.joinable())
	{
		correlateThread.join();

		{
			std::lock_guard<std::mutex> guard {g_triggerQueueMutex};
			if (false == g_triggerQueue.empty())
			{
				printf("TriggerQueue count = %llu\n", g_triggerQueue.size());
			}
		}

		{
			std::lock_guard<std::mutex> guard {g_cameraTriggerQueueMutex};
			if (false == g_cameraTriggerQueue.empty())
			{
				printf("CameraQueue count = %zu\n", g_cameraTriggerQueue.size());
			}
		}
	}

	if (S_OK != result)
	{
		printf("Error = 0%X08\n", static_cast<unsigned int> (result));
	}

	return result;
}
