//*****************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// \file TesterFunction.cpp
/// All Rights Reserved 
//*****************************************************************************
/// Function used for the test.
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TesterFunction.h"
#include "LogClass.h"
#include "TrcTesterConfiguration.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary\SVMatroxBuffer.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVUtilityLibrary\StringHelper.h"
#include "TriggerRecordController\ITriggerRecordControllerR.h"
#include "TriggerRecordController\ITriggerRecordControllerRW.h"
#include "TriggerRecordController\ITriggerRecordR.h"
#include "TriggerRecordController\ITriggerRecordRW.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HANDLE g_resetEvent = nullptr;
HANDLE g_readyEvent = nullptr;
HANDLE g_newTrEvent = nullptr;
constexpr int triggerIdOffset = 100'000;
LPTSTR strTestWithMoreThreads = _T("testWithMoreThreads");

using namespace std::chrono_literals;
#pragma endregion Declarations

#pragma region Local Function
HANDLE createEvent(LPCTSTR eventName)
{
	PSECURITY_DESCRIPTOR psd = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	InitializeSecurityDescriptor(psd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(psd, TRUE, NULL, FALSE);
	SECURITY_ATTRIBUTES sa = {0};
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = psd;
	sa.bInheritHandle = FALSE;

	return ::CreateEvent(&sa, false, false, eventName);
}

bool checkImages(const TrcTesterConfiguration::InspectionsDef& rIPData, SvTrc::ITriggerRecordRPtr tr2R, LogClass& rLogClass, int writerRunId, int testDataId, int ipId, int runId, int triggerCount)
{
	bool retValue = true;
	for (int imagePos = 0; imagePos < rIPData.m_imageFilesList.size(); imagePos++)
	{
		const auto imageId = rIPData.m_imageFilesList[imagePos][writerRunId%rIPData.m_imageFilesList[imagePos].size()];
		auto imageHandle = tr2R->getImage(imagePos);
		if (nullptr == imageHandle || imageHandle->isEmpty())
		{
			if (tr2R->isObjectUpToTime())
			{
				std::string errorStr = SvUl::Format(_T("Reader Tests (%d): getImage(%d:%d) return nullptr by run %d!"), testDataId, ipId, imagePos, runId);
				rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
				retValue = false;
				continue;
			}
			else
			{
				std::string errorStr = SvUl::Format(_T("Reader Tests (%d): getImage(%d:%d) Tr is not longer up to time by run %d!"), testDataId, ipId, imagePos, runId);
				rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::WARN, __LINE__, strTestWithMoreThreads);
				return retValue;
			}
		}
		else
		{
			if (!areImageEqual(imageHandle->getHandle()->GetBuffer().GetIdentifier(), imageId))
			{
				std::string errorStr = SvUl::Format(_T("Reader Tests (%d): ImageCopy(%d:%d) failed. TriggerCount = %d. Images are not equal by run %d!"), testDataId, ipId, imagePos, triggerCount, runId);
				rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
				retValue = false;
				continue;
			}
		}
	}
	//check special image
	{
		const auto imageId = rIPData.m_imageFilesList[0][0];
		auto imageHandle = tr2R->getImage(static_cast<int>(rIPData.m_imageFilesList.size()));
		if (nullptr == imageHandle || imageHandle->isEmpty())
		{
			if (tr2R->isObjectUpToTime())
			{
				std::string errorStr = SvUl::Format(_T("Reader Tests (%d): getImage(%d:last) (TriggerCount = %d) return nullptr by run %d. Is TR upToTime: %d!"), testDataId, ipId, triggerCount, runId, tr2R->isObjectUpToTime());
				rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
				retValue = false;
			}
			else
			{
				std::string errorStr = SvUl::Format(_T("Reader Tests (%d): getImage(%d:last) Tr is not longer up to time by run %d!"), testDataId, ipId, runId);
				rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::WARN, __LINE__, strTestWithMoreThreads);
				return retValue;
			}
		}
		else
		{
			if (!areImageEqual(imageHandle->getHandle()->GetBuffer().GetIdentifier(), imageId))
			{
				std::string errorStr = SvUl::Format(_T("Reader Tests (%d): ImageCopy(%d:last) failed. TriggerCount = %d. Images are not equal by run %d!"), testDataId, ipId, triggerCount, runId);
				rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
				retValue = false;
			}
		}
	}
	return retValue;
}

void getInterestPausePoints(int numberOfRecords, int numberOfRuns, int divValue, int& startPos, int& stopPos)
{
	startPos = numberOfRecords * divValue;
	stopPos = std::min<int>(startPos + 20, numberOfRuns - 10);
	
	//fix that ever stopped because value the same.
	if (startPos == stopPos)
	{
		startPos -= 10;
	}
}

std::vector<int> getInterestList(int numberOfRecords, int numberOfRuns, int divValue)
{
	std::vector<int> retVec;
	int startPos, stopPos;
	getInterestPausePoints(numberOfRecords, numberOfRuns, divValue, startPos, stopPos);
	for (int i = 0; i < numberOfRuns; i++)
	{
		if (0 == i%divValue && (i < startPos || i >= stopPos))
		{
			retVec.push_back(i);
		}
	}
	return retVec;
}

void OnResetTRC()
{
	::SetEvent(g_resetEvent);
}

void OnReadyTRC()
{
	::SetEvent(g_readyEvent);
}

std::vector<SvTrc::TrEventData> g_newTrInfoVec;
std::mutex g_newTrInfoVecMutex;
void OnNewTr(SvTrc::TrEventData data)
{
	std::lock_guard<std::mutex> lock(g_newTrInfoVecMutex);
	g_newTrInfoVec.emplace_back(data);
	::SetEvent(g_newTrEvent);
}

std::mutex g_newInterestTrIpMutex;
std::map<int,std::vector<int>> g_newInterestTrMap;
void OnNewInterestTr(const std::vector<SvTrc::TrEventData>& rDataVec)
{
	std::lock_guard<std::mutex> lock(g_newInterestTrIpMutex);
	for (const auto& rData : rDataVec)
	{
		if (0 <= rData.m_inspectionPos)
		{
			g_newInterestTrMap[rData.m_inspectionPos].emplace_back(rData.m_trId);
		}
	}
}
#pragma endregion Local Function 


#pragma region Global Function
bool areImageEqual(MIL_ID image1, MIL_ID image2)
{
	MIL_ID tmpImage = MbufClone(image1, M_DEFAULT, M_DEFAULT, M_DEFAULT, M_DEFAULT, M_DEFAULT, M_DEFAULT, M_NULL);
	MimArith(image1, image2, tmpImage, M_EQUAL);
	MIL_ID contextID = MimAlloc(M_DEFAULT_HOST, M_STATISTICS_CONTEXT, M_DEFAULT, M_NULL);
	MIL_ID resultID = MimAllocResult(M_DEFAULT_HOST, M_DEFAULT, M_STATISTICS_RESULT, M_NULL);
	MimControl(contextID, M_STAT_MIN, M_ENABLE);
	MimStatCalculate(contextID, tmpImage, resultID, M_DEFAULT);
	MIL_INT minValue = 0;
	MimGetResult(resultID, M_STAT_MIN + M_TYPE_MIL_INT, &minValue);
	MimFree(resultID);
	MimFree(contextID);
	MbufFree(tmpImage);
	return (255 == minValue);
}

SVMatroxBufferCreateStruct specifyBufferFromImage(MIL_ID imageId)
{
	SVMatroxBufferCreateStruct bufferStruct;
	bufferStruct.m_lSizeX = static_cast<long>(MbufInquire(imageId, M_SIZE_X, M_NULL));
	bufferStruct.m_lSizeY = static_cast<long>(MbufInquire(imageId, M_SIZE_Y, M_NULL));
	bufferStruct.m_eAttribute = SVBufAttImageProcDib;
	bufferStruct.m_eType = SV8BitUnsigned;
	return bufferStruct;
}

bool setInspections(std::vector < std::pair <int, int> > numbersOfRecords, SvTrc::ITriggerRecordControllerRW& rTrController, LogClass& rLogClass, LPCSTR testAreaStr)
{
	SvPb::InspectionList inspList;
	for (std::pair<int, int> numberOfRecords : numbersOfRecords)
	{
		auto* pInspStruct = inspList.add_list();
		if (nullptr == pInspStruct)
		{
			rLogClass.Log(_T("createTR2WriteAndRead: Could not create inspection!"), LogLevel::Error, LogType::FAIL, __LINE__, testAreaStr);
			return false;
		}
		pInspStruct->set_numberofrecords(numberOfRecords.first);
		pInspStruct->set_numberrecordsofinterest(numberOfRecords.second);
	}
	return rTrController.setInspections(std::move(inspList));
}

bool writerTest(LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData, int sleepBetweenTrigger)
{
	bool retValue = true;
	GUID guid = GUID_NULL;
	auto& rTrController = SvTrc::getTriggerRecordControllerRWInstance();

	for (int testDataId = 0; testDataId < rTestData.size(); testDataId++)
	{
		const auto& rInspectionsData = rTestData[testDataId];

		//setInspections
		std::vector<std::pair<int, int>> numbersOfRecords;
		std::string inspectionNumberStr;
		for (int j = 0; j < rInspectionsData.size(); j++)
		{
			numbersOfRecords.emplace_back(rInspectionsData[j].m_recordSize, rInspectionsData[j].m_recordInterestSize);
			inspectionNumberStr = SvUl::Format("%s%d/%d, ", inspectionNumberStr.c_str(), numbersOfRecords[j].first, numbersOfRecords[j].second);
		}
		retValue = setInspections(numbersOfRecords, rTrController, rLogClass, strTestWithMoreThreads);
		std::string logStr = SvUl::Format(_T("Writer Tests (%d): CreateInspections(%s)"), testDataId, inspectionNumberStr.c_str());

		//check if interest is set and set pauseStart and pauseStop values
		bool isInterestTest = false;
		int startInterestPause = 0;
		int stopInterestPause = 0;
		if (0 < numbersOfRecords[0].second)
		{
			isInterestTest = true;
			getInterestPausePoints(numbersOfRecords[0].first, numberOfRuns, 3, startInterestPause, stopInterestPause);
		}

		if (!retValue)
		{
			rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
			break;
		}
		else
		{
			rLogClass.Log(logStr.c_str(), LogLevel::Information_Level2, LogType::PASS, __LINE__, strTestWithMoreThreads);
		}

		//init Inspections
		std::vector<SVMatroxBufferCreateStruct> cameraStructList;
		for (int i = 0; i < rInspectionsData.size(); i++)
		{
			try
			{
				cameraStructList.emplace_back(specifyBufferFromImage(rInspectionsData[i].m_imageFilesList[0][0]));
				rTrController.startResetTriggerRecordStructure(i);

				for (const auto& rImageList : rInspectionsData[i].m_imageFilesList)
				{
					UuidCreateSequential(&guid);

					rTrController.addOrChangeImage(guid, specifyBufferFromImage(rImageList[0]));
				}
				//add a special buffer which will be set only once and then should all the run the same.
				UuidCreateSequential(&guid);
				rTrController.addOrChangeImage(guid, specifyBufferFromImage(rInspectionsData[i].m_imageFilesList[0][0]));

				rTrController.finishResetTriggerRecordStructure();
			}
			catch (const SvStl::MessageContainer& rExp)
			{
				logStr = SvUl::Format(_T("Writer Tests (%d): resetTriggerRecordStructure"), testDataId);
				rLogClass.logException(logStr.c_str(), rExp, __LINE__, strTestWithMoreThreads);
				retValue = false;
				break;
			}
			catch (...)
			{
				std::string errorStr = SvUl::Format(_T("Writer Tests (%d): resetTriggerRecordStructure throw an exception"), testDataId);
				rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
				retValue = false;
				break;
			}
		}
		if (!retValue)
		{
			break;
		}

		std::this_thread::sleep_for(1s);

		{
			std::lock_guard<std::mutex> lock(g_newInterestTrIpMutex);
			g_newInterestTrMap.clear();
		}

		//the run
		rTrController.pauseTrsOfInterest(0);
		std::vector<SvTrc::ITriggerRecordRPtr> lastTRList(rInspectionsData.size());
		for (int runId = 0; runId < numberOfRuns; runId++)
		{
			if (isInterestTest && (runId == startInterestPause || runId == stopInterestPause))
			{
				rTrController.pauseTrsOfInterest(runId == startInterestPause);
				std::string errorStr = SvUl::Format(_T("Writer Tests (%d): pauseTrsOfInterest to %d by run %d"), testDataId, runId == startInterestPause, runId);
				rLogClass.Log(errorStr.c_str(), LogLevel::Information_Level2, LogType::PASS, __LINE__, strTestWithMoreThreads);
			}


			for (int ipId = 0; ipId < rInspectionsData.size(); ipId++)
			{
				const auto& rIPData = rInspectionsData[ipId];
				auto tr2W = rTrController.createTriggerRecordObjectToWrite(ipId);
				if (nullptr == tr2W)
				{
					std::string errorStr = SvUl::Format(_T("Writer Tests (%d): createTriggerRecordObjectToWrite(%d) return nullptr by run %d!"), testDataId, ipId, runId);
					rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
					retValue = false;
					continue;
				}

				if (nullptr != lastTRList[ipId])
				{//set images from last run
					tr2W->setImages(*lastTRList[ipId].get());
				}

				SvTrc::TriggerData trData;
				trData.m_TriggerCount = triggerIdOffset*testDataId + runId;
				tr2W->setTriggerData(trData);
				//write image to buffer
				auto cameraImageHandle = rTrController.getImageBuffer(cameraStructList[ipId]);
				if (nullptr == cameraImageHandle || cameraImageHandle->isEmpty())
				{
					std::string errorStr = SvUl::Format(_T("Writer Tests (%d): getImageBuffer(%d) return nullptr by run %d!"), testDataId, ipId, runId);
					rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
					retValue = false;
					continue;
				}
				else
				{
					const auto imageId = rIPData.m_imageFilesList[0][runId%rIPData.m_imageFilesList[0].size()];
					MbufCopy(imageId, cameraImageHandle->getHandle()->GetBuffer().GetIdentifier());
					if (!areImageEqual(cameraImageHandle->getHandle()->GetBuffer().GetIdentifier(), imageId))
					{
						std::string errorStr = SvUl::Format(_T("Writer Tests (%d): ImageCopy(%d:0) failed. Images are not equal by run %d!"), testDataId, ipId, runId);
						rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
						retValue = false;
						continue;
					}
					try
					{
						tr2W->setImage(0, cameraImageHandle->getBufferPos());
					}
					catch (const SvStl::MessageContainer& rExp)
					{
						std::string errorStr = SvUl::Format(_T("Writer Tests (%d): setImage(%d) by run %d:"), testDataId, ipId, runId);
						rLogClass.logException(errorStr.c_str(), rExp, __LINE__, strTestWithMoreThreads);
						retValue = false;
						continue;
					}
					if (0 == runId)
					{//write special buffer only at the first run
						auto lastImageHandle = tr2W->createNewImageHandle(static_cast<int>(rIPData.m_imageFilesList.size()));
						if (nullptr == lastImageHandle || lastImageHandle->isEmpty())
						{
							std::string errorStr = SvUl::Format(_T("Writer Tests (%d): createNewImageHandle(%d:last) return nullptr by run %d!"), testDataId, ipId, runId);
							rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
							retValue = false;
							continue;
						}
						else
						{
							MbufCopy(imageId, lastImageHandle->getHandle()->GetBuffer().GetIdentifier());
							if (!areImageEqual(lastImageHandle->getHandle()->GetBuffer().GetIdentifier(), imageId))
							{
								std::string errorStr = SvUl::Format(_T("Writer Tests (%d): ImageCopy(%d:last) failed. Images are not equal by run %d!"), testDataId, ipId, runId);
								rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
								retValue = false;
								continue;
							}
						}
					}
				}
				for (int imagePos = 1; imagePos < rIPData.m_imageFilesList.size(); imagePos++)
				{
					const auto imageId = rIPData.m_imageFilesList[imagePos][runId%rIPData.m_imageFilesList[imagePos].size()];
					auto imageHandle = tr2W->createNewImageHandle(imagePos);
					if (nullptr == imageHandle || imageHandle->isEmpty())
					{
						std::string errorStr = SvUl::Format(_T("Writer Tests (%d): createNewImageHandle(%d:%d) return nullptr by run %d!"), testDataId, ipId, imagePos, runId);
						rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
						retValue = false;
						continue;
					}
					else
					{
						MbufCopy(imageId, imageHandle->getHandle()->GetBuffer().GetIdentifier());
						if (!areImageEqual(imageHandle->getHandle()->GetBuffer().GetIdentifier(), imageId))
						{
							std::string errorStr = SvUl::Format(_T("Writer Tests (%d): ImageCopy(%d:%d) failed. Images are not equal by run %d!"), testDataId, ipId, imagePos, runId);
							rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
							retValue = false;
							continue;
						}
					}
				}
				lastTRList[ipId] = rTrController.closeWriteAndOpenReadTriggerRecordObject(tr2W);

				if (0 == runId % 3 && nullptr != lastTRList[ipId])
				{
					rTrController.setTrsOfInterest({lastTRList[ipId]});
				}
			}

			std::this_thread::sleep_for(std::chrono::duration<int, std::micro>(sleepBetweenTrigger));
		}

		logStr = SvUl::Format(_T("Writer Tests for run %d finished"), testDataId);
		rLogClass.LogText(logStr.c_str(), LogLevel::Information_Level2, LogType::PASS);

		std::this_thread::sleep_for(10ms);
	}

	return retValue;
}

bool readerTest(LPCTSTR testName, LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData, bool isOtherProcess )
{
	bool retValue = true;
	std::string strResetEvent{_T("Global\\TRCModul_Reset")};
	strResetEvent += testName;
	g_resetEvent = createEvent(strResetEvent.c_str());
	std::string strReadyEvent{_T("Global\\TRCModul_Ready")};
	strReadyEvent += testName;
	g_readyEvent = createEvent(strReadyEvent.c_str());
	std::string strNewTrEvent{_T("Global\\TRCModul_NewTrid")};
	strNewTrEvent += testName;
	g_newTrEvent = createEvent(strNewTrEvent.c_str());
	if (nullptr != g_resetEvent && nullptr != g_readyEvent && nullptr != g_newTrEvent)
	{
		auto& rTrController = SvTrc::getTriggerRecordControllerRInstance();
		int resetCallbackHandle = rTrController.registerResetCallback(OnResetTRC);
		int readyCallbackHandle = rTrController.registerReadyCallback(OnReadyTRC);
		int newTrCallBackHandle = rTrController.registerNewTrCallback(OnNewTr);
		int newInterestCallBackHandle = rTrController.registerNewInterestTrCallback(OnNewInterestTr);
		if (rTrController.isValid())
		{
			OnReadyTRC();
		}

		int lastTestDataId = 0;
		std::vector<int> lastTrIds;

		//int numberOfRunsComplete = numberOfRuns * static_cast<int>(rTestData.size());
		for (int testDataPos = 0; testDataPos < rTestData.size(); testDataPos++)
		//for (int runId = 0; runId < numberOfRunsComplete; runId++)
		{
			DWORD waitValue = WAIT_FAILED;
			int isResetAgain = false;
			do
			{
				isResetAgain = false;
				do
				{
					HANDLE hChange[2];
					hChange[0] = g_resetEvent;
					hChange[1] = g_readyEvent;
					waitValue = WaitForMultipleObjects(2, hChange, false, 5000);
				} while (WAIT_OBJECT_0 == waitValue); //ignore resetEvent and wait again.
				if (WAIT_OBJECT_0 + 1 == waitValue)
				{
					if (WAIT_OBJECT_0 == WaitForMultipleObjects(1, &g_resetEvent, false, 5))
					{
						isResetAgain = true;
					}
				}
			} while (isResetAgain); //restart wait for ready if reset is done again (writer reset trc for every ip)

			if (WAIT_OBJECT_0 + 1 != waitValue)
			{
				rLogClass.Log(_T("Fail by waiting of Ready-Event from TRC"), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
				retValue = false;
				break;
			}

			auto inspectionList = rTrController.getInspections();
			auto inspectionSize = inspectionList.list_size();
			if (lastTrIds.size() != inspectionSize)
			{
				lastTrIds.resize(inspectionSize, -1);
			}

			int runId = -1;
			while (true)
			{
				HANDLE hChange[2];
				hChange[0] = g_resetEvent;
				hChange[1] = g_newTrEvent;
				waitValue = WaitForMultipleObjects(2, hChange, false, 1500);
				if (WAIT_OBJECT_0 == waitValue)
				{
					std::string logStr = SvUl::Format(_T("Reader Tests: Reset TRC after runId %d"), runId);
					bool isPass = (runId + 1 == numberOfRuns*inspectionSize);
					rLogClass.Log(logStr.c_str(), isPass ? LogLevel::Information_Level2 : LogLevel::Error, isPass ? LogType::PASS : LogType::FAIL, __LINE__, strTestWithMoreThreads);
					break;
				}

				std::vector<SvTrc::TrEventData> newTrInfoVec;
				{
					std::lock_guard<std::mutex> lock(g_newTrInfoVecMutex);
					if (0 == g_newTrInfoVec.size())
					{
						if (runId + 1 < numberOfRuns*inspectionSize)
						{
							std::string logStr = SvUl::Format(_T("Reader Tests: No new TR after runId %d"), runId);
							rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
						}
						else
						{
							std::string logStr = SvUl::Format(_T("Reader Tests: Finished run after runId %d"), runId);
							rLogClass.Log(logStr.c_str(), LogLevel::Information_Level2, LogType::PASS, __LINE__, strTestWithMoreThreads);
						}
						break;
					}
					//only for delete old events
					WaitForMultipleObjects(1, &g_newTrEvent, false, 0);
					newTrInfoVec.swap(g_newTrInfoVec);
					g_newTrInfoVec.clear();
				}

				for (const auto newTrInfo : newTrInfoVec)
				{
					runId++;
					int trIdLast = rTrController.getLastTrId(newTrInfo.m_inspectionPos);

					if (lastTrIds[newTrInfo.m_inspectionPos] == newTrInfo.m_trId)
					{
						std::string logStr = SvUl::Format(_T("Reader Tests: (%d) check trId %d again by run %d"), newTrInfo.m_inspectionPos, newTrInfo.m_trId, runId);
						rLogClass.Log(logStr.c_str(), LogLevel::Information_Level1, LogType::WARN, __LINE__, strTestWithMoreThreads);
					}
					else if (trIdLast != newTrInfo.m_trId)
					{
						std::string logStr = SvUl::Format(_T("Reader Tests: (%d) check trId %d not lastTrId %d again by run %d"), newTrInfo.m_inspectionPos, newTrInfo.m_trId, trIdLast, runId);
						rLogClass.Log(logStr.c_str(), LogLevel::Information_Level3, LogType::WARN, __LINE__, strTestWithMoreThreads);
					}

					lastTrIds[newTrInfo.m_inspectionPos] = newTrInfo.m_trId;

					auto tr2R = rTrController.createTriggerRecordObject(newTrInfo.m_inspectionPos, newTrInfo.m_trId);
					if (nullptr == tr2R)
					{
						std::string errorStr = SvUl::Format(_T("Reader Tests: createTriggerRecordObject(%d, %d) return nullptr by run %d!"), newTrInfo.m_inspectionPos, newTrInfo.m_trId, runId);
						rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
						retValue = false;
						std::this_thread::sleep_for(2us);
						continue;
					}
					int triggerCount = tr2R->getTriggerData().m_TriggerCount;
					int testDataId = triggerCount / triggerIdOffset;
					int writerRunId = triggerCount % triggerIdOffset;
					if (0 > triggerCount || rTestData.size() <= testDataId)
					{
						std::string errorStr = SvUl::Format(_T("Reader Tests: (%d) triggerCount (%d) do not fit: , called testDataId %d, max testDataId %d by run %d"), newTrInfo.m_inspectionPos, triggerCount, testDataId, static_cast<int>(rTestData.size()) - 1, runId);
						rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
						retValue = false;
						std::this_thread::sleep_for(1us);
						continue;
					}
					else
					{
						std::string logStr = SvUl::Format(_T("Reader Tests(%d): createTriggerRecordObject(%d) triggerCount %d by run %d"), testDataId, newTrInfo.m_inspectionPos, triggerCount, runId);
						rLogClass.Log(logStr.c_str(), LogLevel::Debug, LogType::PASS, __LINE__, strTestWithMoreThreads);
					}

					if (testDataId != lastTestDataId)
					{
						std::string logStr = SvUl::Format(_T("Reader Tests(%d): Moved to this testDataId with run %d"), testDataId, runId);
						rLogClass.Log(logStr.c_str(), LogLevel::Debug, LogType::PASS, __LINE__, strTestWithMoreThreads);
						lastTestDataId = testDataId;
					}

					retValue = checkImages(rTestData[testDataId][newTrInfo.m_inspectionPos], tr2R, rLogClass, writerRunId, testDataId, newTrInfo.m_inspectionPos, runId, triggerCount) && retValue;

					//check trofInterest
					int maxInterestSize = rTestData[testDataId][newTrInfo.m_inspectionPos].m_recordInterestSize;
					if (isOtherProcess)
					{
						maxInterestSize = inspectionList.list(newTrInfo.m_inspectionPos).numberrecordsofinterest();
					}
					if (0 < maxInterestSize)
					{
						std::random_device rd;
						std::uniform_int_distribution<int> dist(1, maxInterestSize);
						int interestNumber = dist(rd);
						if (0 == runId)
						{	//at the first run, give writer time to set tr of interest.
							std::this_thread::sleep_for(1ms);
						}
						auto interestTRVec = rTrController.getTrsOfInterest(newTrInfo.m_inspectionPos, interestNumber);
						if (0 < interestTRVec.size())
						{
							std::vector<int> interestList = getInterestList(isOtherProcess? inspectionList.list(newTrInfo.m_inspectionPos).numberofrecords():rTestData[testDataId][newTrInfo.m_inspectionPos].m_recordSize, numberOfRuns, 3);
							int currentPos = -1;
							for (auto pTr : interestTRVec)
							{
								if (nullptr != pTr)
								{
									int triggerCount = pTr->getTriggerData().m_TriggerCount;
									if (-1 == currentPos)
									{
										auto findIter = std::find(interestList.begin(), interestList.end(), triggerCount);
										if (interestList.end() != findIter)
										{
											currentPos = static_cast<int>(std::distance(interestList.begin(), findIter));
											int expectedNumber = std::min<int>(interestNumber, currentPos+1);
											if (interestTRVec.size() != expectedNumber)
											{
												retValue = false;
												std::string logStr = SvUl::Format(_T("Reader Tests(%d): getTRsOfInterest: Number of TRs %d, but expected %d (first triggerCount %d) for ip %d with run %d"), testDataId, interestTRVec.size(), expectedNumber, triggerCount, newTrInfo.m_inspectionPos, runId);
												rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
												break;
											}
										
										}
										else
										{
											retValue = false;
											std::string logStr = SvUl::Format(_T("Reader Tests(%d): getTRsOfInterest: First TR (%d) should not be an interest TR for ip %d with run %d"), testDataId, triggerCount, newTrInfo.m_inspectionPos, runId);
											rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
											break;
										}
									}

									if (interestList[currentPos] != triggerCount)
									{
										retValue = false;
										std::string logStr = SvUl::Format(_T("Reader Tests(%d): getTRsOfInterest: TR %d, but TR %d expected for ip %d with run %d"), testDataId, triggerCount, interestList[currentPos], newTrInfo.m_inspectionPos, runId);
										rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
										break;
									}
									currentPos--;
								}
								else
								{
									retValue = false;
									std::string logStr = SvUl::Format(_T("Reader Tests(%d): getTRsOfInterest: One of the TR is empty for ip %d with run %d"), testDataId, newTrInfo.m_inspectionPos, runId);
									rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
									break;
								}
							}
						}
						else
						{
							retValue = false;
							std::string logStr = SvUl::Format(_T("Reader Tests(%d): getTRsOfInterest: Return list is empty for ip %d with run %d"), testDataId, newTrInfo.m_inspectionPos, runId);
							rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
						}
					}
				}
				newTrInfoVec.clear();
			}

			//check newInterestEvents
			std::lock_guard<std::mutex> lock(g_newInterestTrIpMutex);
			if (0 < g_newInterestTrMap.size())
			{
				for (auto iter : g_newInterestTrMap)
				{
					size_t expectedSize = (numberOfRuns+2) / 3;
					if (0 < inspectionList.list(iter.first).numberrecordsofinterest())
					{
						std::vector<int> interestList = getInterestList(isOtherProcess ? inspectionList.list(iter.first).numberofrecords() : rTestData[testDataPos][iter.first].m_recordSize, numberOfRuns, 3);
						expectedSize = interestList.size();
					}
					
					std::string logStr;
					if (expectedSize == iter.second.size())
					{
						logStr = SvUl::Format(_T("Reader Tests(%d): %d TRsOfInterest-Events for ip %d"), testDataPos, iter.second.size(), iter.first);
						rLogClass.Log(logStr.c_str(), LogLevel::Information_Level3, LogType::PASS, __LINE__, strTestWithMoreThreads);
					}
					else if (expectedSize-1 == iter.second.size())
					{
						logStr = SvUl::Format(_T("Reader Tests(%d): %zu instead of %zu TRsOfInterest-Events for ip %d"), testDataPos, iter.second.size(), expectedSize, iter.first);
						rLogClass.Log(logStr.c_str(), LogLevel::Information_Level1, LogType::WARN, __LINE__, strTestWithMoreThreads);
					}
					else
					{
						logStr = SvUl::Format(_T("Reader Tests(%d): %zu instead of %zu TRsOfInterest-Events for ip %d"), testDataPos, iter.second.size(), expectedSize, iter.first);
						rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
					}		
					logStr.clear();
					for (auto trId : iter.second)
					{
						logStr = SvUl::Format(_T("%s;%d"), logStr, trId);
					}
					rLogClass.Log(logStr.c_str(), LogLevel::Debug, LogType::BLANK, __LINE__, strTestWithMoreThreads);
				}
			}
			else
			{
				if (0 < inspectionList.list(0).numberrecordsofinterest())
				{
					std::string logStr = SvUl::Format(_T("Reader Tests(%d): None TRsOfInterest-Events get"), testDataPos);
					rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
				}
			}
			g_newInterestTrMap.clear();
		}

		rTrController.unregisterResetCallback(resetCallbackHandle);
		rTrController.unregisterReadyCallback(readyCallbackHandle);
		rTrController.unregisterNewTrCallback(newTrCallBackHandle);
		rTrController.unregisterNewInterestTrCallback(newInterestCallBackHandle);
	}
	else
	{
		rLogClass.Log(_T("Set Events for ReaderTest failed"), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
		retValue = false;
	}
		
	return retValue;
}
#pragma endregion Global Function