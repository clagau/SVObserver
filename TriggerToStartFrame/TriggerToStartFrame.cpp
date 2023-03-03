//****************************************************************************
/// \copyright COPYRIGHT (c) 2020,2020 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file TriggerToStartFrame.cpp
/// \brief Console main file for TriggerToStartFrame application to test
/// Trigger signal to image start frame packet or between different trigger signals
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "Digitizer.h"
#include "SVIOTriggerLoadLibraryClass.h"
#include "Triggering/SVDigitizerLoadLibraryClass.h"
#include "Triggering/TriggerData.h"
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
constexpr LPCTSTR cEtherCatIoDll = "EtherCatIO.dll";
constexpr LPCTSTR cLptIoDll = "SVLptIO.dll";
constexpr LPCTSTR cMatroxGigeDll = "SVMatroxGige.dll";

constexpr LPCTSTR cHelpOption1 = "-h";
constexpr LPCTSTR cHelpOption2 = "-?";
constexpr LPCTSTR cTriggerOption = "-t:";
constexpr LPCTSTR cModeOption = "-m:";
constexpr LPCTSTR cFileOption = "-f:";
constexpr LPCTSTR cNegativeTimeOption = "-n:";
constexpr LPCTSTR cPositiveTimeOption = "-p:";

constexpr std::array<LPCTSTR, cNumberOfChannels> cCameraIpAdresses =
{
	"192.168.101.2", 
	"192.168.102.2", 
	"192.168.103.2", 
	"192.168.104.2"
};

enum DllMode
{
	PlcIOStartFrame,
	PlcIOLptIO,
	EthercatLptIO,
	Count,
};

std::mutex g_masterQueueMutex;
std::mutex g_slaveQueueMutex;
std::queue<double> g_masterTimestampQueue;
std::queue<double> g_slaveTimestampQueue;
std::atomic_bool g_masterStarted {false};
SVAcquisitionBuffer g_Acquisition;
//Time window default +5ms -1.7ms
double g_timeWindowPositive {5.0};
double g_timeWindowNegative {-1.7};
uint8_t g_triggerChannel {0};
std::ofstream g_logFile;


double GetTimeStamp()
{
	return static_cast<double> (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()) / cNanoToMilli;
}

void TriggerCorrelate()
{
	static unsigned long slaveCount {0UL};
	static unsigned long prevCount {0UL};
	static unsigned long masterCount {0UL};

	while (false == g_masterStarted)
	{
		Sleep(5);
	}

	while (true == g_masterStarted)
	{
		double slaveTimestamp {0.0};
		double masterTimestamp {0.0};

		{
			std::lock_guard<std::mutex> guard {g_slaveQueueMutex};
			if (false == g_slaveTimestampQueue.empty())
			{
				slaveTimestamp = g_slaveTimestampQueue.front();
				g_slaveTimestampQueue.pop();
				++slaveCount;
			}
		}
		if (true == g_logFile.is_open() && slaveCount % 10 == 0 && slaveCount != prevCount)
		{
			std::cout << "SlaveCount=" << slaveCount << '\n';
			prevCount = slaveCount;
		}

		while (0.0 < slaveTimestamp)
		{
			{
				std::lock_guard<std::mutex> guard {g_masterQueueMutex};
				if (false == g_masterTimestampQueue.empty())
				{
					masterTimestamp = g_masterTimestampQueue.front();
					g_masterTimestampQueue.pop();
					++masterCount;
				}
			}
			if (0.0 < masterTimestamp)
			{
				std::string line;
				double diff = slaveTimestamp - masterTimestamp;
				if (diff > g_timeWindowNegative && diff < g_timeWindowPositive)
				{
					line = std::format("{}; {}; {}; {}; {}; {}\n", masterCount, slaveCount, masterTimestamp, slaveTimestamp, diff, 1);
					slaveTimestamp = 0.0;
				}
				else if (diff > g_timeWindowPositive)
				{
					line = std::format("{}; {}; {}; {}; {}; {}\n", masterCount, slaveCount, masterTimestamp, slaveTimestamp, diff, 0);
					slaveTimestamp = 0.0;
				}
				else
				{
					line = std::format("{}; {}; {}; {}; {}; {}\n", masterCount, slaveCount, masterTimestamp, slaveTimestamp, diff, -1);
					slaveTimestamp = 0.0;
				}
				if (false == line.empty())
				{
					if (true == g_logFile.is_open())
					{
						g_logFile.write(line.c_str(), line.size());
					}
					else
					{
						std::cout << line.c_str();
					}
				}
			}
			if (false == g_masterStarted)
			{
				break;
			}
			Sleep(0);
		}
		Sleep(0);
	}
}

void OnImage(CameraInfo&& cameraInfo)
{
	std::lock_guard<std::mutex> guard {g_slaveQueueMutex};
	g_slaveTimestampQueue.push(cameraInfo.m_startFrameTimestamp);
}

void OnSlaveTrigger(const SvTrig::TriggerData& rTriggerData)
{
	double slaveTimestamp = (0.0 != rTriggerData.m_triggerTimestamp) ? rTriggerData.m_triggerTimestamp : GetTimeStamp();

	std::lock_guard<std::mutex> guard {g_slaveQueueMutex};
	g_slaveTimestampQueue.push(slaveTimestamp);
}

void OnMasterTrigger(const SvTrig::TriggerData& rTriggerData)
{
	double masterTimestamp = (0.0 != rTriggerData.m_triggerTimestamp) ? rTriggerData.m_triggerTimestamp : GetTimeStamp();

	{
		std::lock_guard<std::mutex> guard {g_masterQueueMutex};
		g_masterTimestampQueue.push(masterTimestamp);
	}

	if (false == g_masterStarted)
	{
		{
			std::lock_guard<std::mutex> guard {g_slaveQueueMutex};
			g_slaveTimestampQueue = {};
		}
		g_masterStarted = true;
	}
}

HRESULT PlcTriggerToStartFrame()
{
	SVIOTriggerLoadLibraryClass PlcIoDll;
	SvTrig::SVDigitizerLoadLibraryClass DigitizerDll;

	HRESULT result = PlcIoDll.Open(cPlcIoDll);
	if (S_OK == result)
	{
		std::cout << std::format("Loaded {}\n", cPlcIoDll);

		result = DigitizerDll.Open(cMatroxGigeDll);
		if (S_OK == result)
		{
			std::cout << std::format("Loaded {}\n", cMatroxGigeDll);
			std::cout << "Note that using camera start frame,\nthe camera should be configured using a camera file prior to starting this application\n";
			int triggerIndex {g_triggerChannel + 1};

			result = PlcIoDll.Register(triggerIndex, &OnMasterTrigger);
			if (S_OK == result)
			{
				std::cout << std::format("PLC Trigger {} callback registered\n", triggerIndex);
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
				std::cout << std::format("Camera {} callback registered\n", triggerIndex);
				result = PlcIoDll.Start(triggerIndex);
				if (S_OK == result)
				{
					std::cout << std::format("PLC Trigger {} started\n", triggerIndex);
				}

				result = DigitizerDll.CreateBuffers(cameraHandle);
				if (S_OK == result)
				{
					std::cout << std::format("Camera Buffers created for camera {}\n", cameraHandle);
				}

				result = DigitizerDll.Start(cameraHandle);
				if (S_OK == result)
				{
					std::cout << std::format("Camera {} started\n", cameraHandle);
				}

				_variant_t moduleReadyState {true};
				if (S_OK == result)
				{
					PlcIoDll.SetParameterValue(cModuleReadyIndex, moduleReadyState);

					std::cout << "Module Ready sent\nTo exit press x\n";
					std::string header {"TriggerCount; ImageCount; TriggerTimestamp; StartFrame; TriggerToStartFrame; Match\n"};
					if (true == g_logFile.is_open())
					{
						g_logFile.write(header.c_str(), header.size());
					}
					else
					{
						std::cout << header.c_str();
					}

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
				std::cout << "Module Ready reset\n";
				PlcIoDll.Stop(triggerIndex);
				DigitizerDll.Stop(cameraHandle);
				PlcIoDll.Unregister(triggerIndex);
				PlcIoDll.Close();
				DigitizerDll.UnregisterBufferInterface(cameraHandle);
				DigitizerDll.Close();
				std::cout << std::format("Trigger {} stopped\n", triggerIndex);
			}
		}
	}
	return result;
}

HRESULT Trigger1DllToTrigger2Dll(LPCSTR DllName1, LPCSTR DllName2)
{
	SVIOTriggerLoadLibraryClass TriggerDll1;
	SVIOTriggerLoadLibraryClass TriggerDll2;

	HRESULT result = TriggerDll1.Open(DllName1);
	if (S_OK == result)
	{
		std::cout << std::format("Loaded {}\n", DllName1);
		result = TriggerDll2.Open(DllName2);
		if (S_OK == result)
		{
			std::cout << std::format("Loaded {}\n", DllName2);
			int triggerIndex {g_triggerChannel + 1};

			if (std::string(cLptIoDll) == DllName1)
			{
				_variant_t value {cRabbitBoardType};
				TriggerDll1.SetParameterValue(cBoardTypeIndex, value);
				value = cFallingEdge;
				TriggerDll1.TrigSetParameterValue(triggerIndex, cSignalEdgeIndex, value);
			}
			else if (std::string(cLptIoDll) == DllName2)
			{
				_variant_t value {cRabbitBoardType};
				TriggerDll2.SetParameterValue(cBoardTypeIndex, value);
				value = cFallingEdge;
				TriggerDll2.TrigSetParameterValue(triggerIndex, cSignalEdgeIndex, value);
			}

			result = TriggerDll1.Register(triggerIndex, &OnMasterTrigger);
			if (S_OK == result)
			{
				std::cout << std::format("{} Trigger {} callback registered\n", DllName1,  triggerIndex);
			}
			result = TriggerDll2.Register(triggerIndex, &OnSlaveTrigger);
			if (S_OK == result)
			{
				std::cout << std::format("{} Trigger {} callback registered\n", DllName2,  triggerIndex);

				result = TriggerDll1.Start(triggerIndex);
				if (S_OK == result)
				{
					std::cout << std::format("{} Trigger {} started\n", DllName1, triggerIndex);
				}
				result = TriggerDll2.Start(triggerIndex);
				if (S_OK == result)
				{
					std::cout << std::format("{} Trigger {} started\n", DllName2, triggerIndex);
				}

				_variant_t moduleReadyState {true};
				if (S_OK == result)
				{
					TriggerDll1.SetParameterValue(cModuleReadyIndex, moduleReadyState);
					TriggerDll2.SetParameterValue(cModuleReadyIndex, moduleReadyState);

					std::cout << "Module Ready sent\nTo exit press x\n";
					std::string header {std::format("TriggerCount {}; TriggerCount {}; Timestamp {}; TimeStamp {}; TimestampDiff; Match\n", DllName1, DllName2, DllName1, DllName2)};
					if (true == g_logFile.is_open())
					{
						g_logFile.write(header.c_str(), header.size());
					}
					else
					{
						std::cout << header.c_str();
					}

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
				TriggerDll1.SetParameterValue(cModuleReadyIndex, moduleReadyState);
				TriggerDll2.SetParameterValue(cModuleReadyIndex, moduleReadyState);
				std::cout << "Module Ready reset\n";
				TriggerDll1.Stop(triggerIndex);
				TriggerDll2.Stop(triggerIndex);
				TriggerDll1.Unregister(triggerIndex);
				TriggerDll2.Unregister(triggerIndex);
				TriggerDll1.Close();
				TriggerDll2.Close();
				std::cout << std::format("Trigger {} stopped\n", triggerIndex);
			}
		}
	}
	return result;
}

int main(int argc, const char* args[])
{
	std::vector<std::string> cmdOptions;
	cmdOptions.resize(argc - 1);
	DllMode dllCompareType {DllMode::PlcIOStartFrame};

	for (int i = 1; i < argc; ++i)
	{
		cmdOptions[i-1] = args[i];
		std::transform(cmdOptions[i-1].begin(), cmdOptions[i-1].end(), cmdOptions[i-1].begin(), [](unsigned char c) { return static_cast<char> (std::tolower(c)); });
	}

	for (const auto& rOption : cmdOptions)
	{
		if (rOption == cHelpOption1 || rOption == cHelpOption2)
		{
			std::cout << "Cmd Options\n";
			std::cout << "-h or -? displays all the available options\n";
			std::cout << "-t:<triggenNumber> where trigger number (1-4) is which trigger is to be used default is trigger=1\n";
			std::cout << "-m:<mode> where \n\tmode 0=SVPlcIO.dll vs SVMatroxGige.dll (start frame) is default\n";
			std::cout << "\tmode 1=SVPlcIO.dll vs SVLptIO.dll\n";
			std::cout << "\tmode 2=SVLptIO.dll vs EtherCatIO.dll\n";
			std::cout << "-f:<filename> where filename is the name of the file (can be with path) to write the data to\n";
			std::cout << std::format("-n:<negativeTime> where negativeTime is the pre tigger time window in ms default {:.2f} ms\n", g_timeWindowNegative);
			std::cout << std::format("-p:<positiveTime> where positiveTime is the post tigger time window in ms default {:.2f} ms\n", g_timeWindowPositive);
			return S_OK;
		}
		else if (std::string::npos != rOption.find(cTriggerOption))
		{
			std::string option = rOption.substr(std::strlen(cTriggerOption));
			g_triggerChannel = static_cast<uint8_t> (std::stoi(option.c_str()));
			if (0 == g_triggerChannel || g_triggerChannel > cNumberOfChannels)
			{
				std::cout << "Valid values for trigger number are 1-4\n";
				return E_FAIL;
			}
			//Trigger channol is zero based
			--g_triggerChannel;
		}
		else if (std::string::npos != rOption.find(cModeOption))
		{
			std::string option = rOption.substr(std::strlen(cModeOption));
			int modeOption = std::stoi(option.c_str());

			if (modeOption < 0 || modeOption >= static_cast<int> (DllMode::Count))
			{
				std::cout << "Valid values for mode are 0-2\n";
				return E_FAIL;
			}
			dllCompareType = static_cast<DllMode> (modeOption);
		}
		else if (std::string::npos != rOption.find(cFileOption))
		{
			std::string option = rOption.substr(std::strlen(cFileOption));
			g_logFile.open(option.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
			if (false == g_logFile.is_open())
			{
				std::cout << std::format("Could not open file {}\n", option.c_str());
				return E_FAIL;
			}
		}
		else if (std::string::npos != rOption.find(cNegativeTimeOption))
		{
			std::string option = rOption.substr(std::strlen(cNegativeTimeOption));
			g_timeWindowNegative = std::stof(option.c_str());
		}
		else if (std::string::npos != rOption.find(cPositiveTimeOption))
		{
			std::string option = rOption.substr(std::strlen(cPositiveTimeOption));
			g_timeWindowPositive = std::stof(option.c_str());
		}
	}

	auto correlateThread = std::thread(&TriggerCorrelate);

	::SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	::SetThreadPriority(correlateThread.native_handle(), THREAD_PRIORITY_HIGHEST);

	HRESULT result {S_OK};

	switch (dllCompareType)
	{
		case DllMode::PlcIOStartFrame:
		{
			result = PlcTriggerToStartFrame();
			break;
		}

		case DllMode::PlcIOLptIO:
		{
			result = Trigger1DllToTrigger2Dll(cPlcIoDll, cLptIoDll);
			break;
		}

		case DllMode::EthercatLptIO:
		{
			result = Trigger1DllToTrigger2Dll(cLptIoDll, cEtherCatIoDll);
			break;
		}

		default:
		{
			break;
		}
	}

	if (false == g_masterStarted)
	{
		g_masterStarted = true;
		Sleep(10);
	}

	g_masterStarted = false;
	if (correlateThread.joinable())
	{
		correlateThread.join();

		{
			std::lock_guard<std::mutex> guard {g_masterQueueMutex};
			if (false == g_masterTimestampQueue.empty())
			{
				std::cout << std::format("Master Queue count = {}\n", g_masterTimestampQueue.size());
			}
		}

		{
			std::lock_guard<std::mutex> guard {g_slaveQueueMutex};
			if (false == g_slaveTimestampQueue.empty())
			{
				std::cout << std::format("Slave Queue count = {}\n", g_slaveTimestampQueue.size());
			}
		}
	}

	if (S_OK != result)
	{
		std::cout << std::format("Error = {:#x}\n", static_cast<unsigned int> (result));
	}

	return result;
}
