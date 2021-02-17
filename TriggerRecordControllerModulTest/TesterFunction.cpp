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
#include "TrcTester.h"
#include "LogClass.h"
#include "TrcTesterConfiguration.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary\SVMatroxBuffer.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVUtilityLibrary\StringHelper.h"
#include "ObjectInterfaces/ITriggerRecordControllerR.h"
#include "ObjectInterfaces/ITriggerRecordControllerRW.h"
#include "ObjectInterfaces/ITriggerRecordR.h"
#include "ObjectInterfaces/ITriggerRecordRW.h"
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
constexpr int divFac = 3;

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

bool checkImages(const TrcTesterConfiguration::InspectionsDef& rIPData, SvOi::ITriggerRecordRPtr tr2R, LogClass& rLogClass, int writerRunId, int testDataId, int ipId, int runId, int triggerCount)
{
	bool retValue = true;
	for (int imagePos = 0; imagePos < rIPData.m_imageFilesList.size(); imagePos++)
	{
		auto imageId = rIPData.m_imageFilesList[imagePos]->at(writerRunId%rIPData.m_imageFilesList[imagePos]->size());
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
		auto imageId = rIPData.m_imageFilesList[0]->at(0);
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

std::vector<int> calcExpectedInterestList(int numberOfRecords, int numberOfRuns, int divValue)
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

std::vector<SvOi::TrEventData> g_newTrInfoVec;
std::mutex g_newTrInfoVecMutex;
void OnNewTr(SvOi::TrEventData data)
{
	std::lock_guard<std::mutex> lock(g_newTrInfoVecMutex);
	g_newTrInfoVec.emplace_back(data);
	::SetEvent(g_newTrEvent);
}

std::mutex g_newInterestTrMapMutex;
std::map<int,std::vector<int>> g_newInterestTrMap;
void OnNewInterestTr(LogClass* pLogClass, const std::vector<SvOi::TrInterestEventData>& rDataVec)
{
	std::lock_guard<std::mutex> lock(g_newInterestTrMapMutex);
	for (const auto& rData : rDataVec)
	{
		if (0 <= rData.m_inspectionPos && rData.m_isInterest)
		{
			g_newInterestTrMap[rData.m_inspectionPos].emplace_back(rData.m_trId);
			pLogClass->Log(SvUl::Format(_T("OnNewInterestTr: IP%d / TRId%d"), rData.m_inspectionPos, rData.m_trId).c_str(), LogLevel::Debug, LogType::BLANK, __LINE__);
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

bool setInspections(std::vector < std::pair <int, int> > numbersOfRecords, SvOi::ITriggerRecordControllerRW* pTrcRW, LogClass& rLogClass, LPCSTR testAreaStr)
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
	bool result{ false };
	if (nullptr != pTrcRW)
	{
		result = pTrcRW->setInspections(std::move(inspList));
	}
	return result;
}

bool writerTest(LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData, int sleepBetweenTrigger)
{
	auto* pTrcRW = SvOi::getTriggerRecordControllerRWInstance();
	if (nullptr == pTrcRW)
	{
		return false;
	}
	bool retValue = true;

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
		retValue = setInspections(numbersOfRecords, pTrcRW, rLogClass, strTestWithMoreThreads);
		std::string logStr = SvUl::Format(_T("Writer Tests (%d): CreateInspections(%s)"), testDataId, inspectionNumberStr.c_str());

		//check if interest is set and set pauseStart and pauseStop values
		bool isInterestTest = false;
		int startInterestPause = 0;
		int stopInterestPause = 0;
		if (0 < numbersOfRecords[0].second)
		{
			isInterestTest = true;
			getInterestPausePoints(numbersOfRecords[0].first, numberOfRuns, divFac, startInterestPause, stopInterestPause);
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
				cameraStructList.emplace_back(specifyBufferFromImage(rInspectionsData[i].m_imageFilesList[0]->at(0)));
				pTrcRW->startResetTriggerRecordStructure(i);

				for (const auto* pImageList : rInspectionsData[i].m_imageFilesList)
				{
					pTrcRW->addOrChangeImage(getNextObjectId(), specifyBufferFromImage(pImageList->at(0)));
				}
				//add a special buffer which will be set only once and then should all the run the same.
				pTrcRW->addOrChangeImage(getNextObjectId(), specifyBufferFromImage(rInspectionsData[i].m_imageFilesList[0]->at(0)));

				pTrcRW->finishResetTriggerRecordStructure();
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
			std::lock_guard<std::mutex> lock(g_newInterestTrMapMutex);
			g_newInterestTrMap.clear();
		}

		//the run
		pTrcRW->pauseTrsOfInterest(0);
		std::vector<SvOi::ITriggerRecordRPtr> lastTRList(rInspectionsData.size());
		for (int runId = 0; runId < numberOfRuns; runId++)
		{
			if (isInterestTest && (runId == startInterestPause || runId == stopInterestPause))
			{
				pTrcRW->pauseTrsOfInterest(runId == startInterestPause);
				std::string errorStr = SvUl::Format(_T("Writer Tests (%d): pauseTrsOfInterest to %d by run %d"), testDataId, runId == startInterestPause, runId);
				rLogClass.Log(errorStr.c_str(), LogLevel::Information_Level2, LogType::PASS, __LINE__, strTestWithMoreThreads);
			}


			for (int ipId = 0; ipId < rInspectionsData.size(); ipId++)
			{
				const auto& rIPData = rInspectionsData[ipId];
				auto tr2W = pTrcRW->createTriggerRecordObjectToWrite(ipId);
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

				SvOi::TriggerData trData;
				trData.m_TriggerCount = triggerIdOffset*testDataId + runId;
				tr2W->setTriggerData(trData);
				//write image to buffer
				auto cameraImageHandle = pTrcRW->getImageBuffer(cameraStructList[ipId]);
				if (nullptr == cameraImageHandle || cameraImageHandle->isEmpty())
				{
					std::string errorStr = SvUl::Format(_T("Writer Tests (%d): getImageBuffer(%d) return nullptr by run %d!"), testDataId, ipId, runId);
					rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
					retValue = false;
					continue;
				}
				else
				{
					auto imageId = rIPData.m_imageFilesList[0]->at(runId%rIPData.m_imageFilesList[0]->size());
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
					auto imageId = rIPData.m_imageFilesList[imagePos]->at(runId%rIPData.m_imageFilesList[imagePos]->size());
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
				lastTRList[ipId] = pTrcRW->closeWriteAndOpenReadTriggerRecordObject(tr2W);

				if (nullptr != lastTRList[ipId])
				{
					logStr = SvUl::Format(_T("Writer Tests (%d): ip%d: Set InterestEvent = %d / %d"), testDataId, ipId, lastTRList[ipId]->getId(), 0 == runId % divFac);
					rLogClass.LogText(logStr.c_str(), LogLevel::Debug, LogType::BLANK);
					pTrcRW->setTrsOfInterest({lastTRList[ipId]}, 0 == runId % divFac);
				}
			}

			std::this_thread::sleep_for(std::chrono::duration<int, std::micro>(sleepBetweenTrigger));
		}

		logStr = SvUl::Format(_T("Writer Tests for run %d finished"), testDataId);
		rLogClass.LogText(logStr.c_str(), LogLevel::Information_Level2, LogType::PASS);

		std::this_thread::sleep_for(15ms);
	}

	return retValue;
}

namespace
{
	void setEvents(LPCTSTR testName)
	{
		std::string strResetEvent{ _T("Global\\TRCModul_Reset") };
		strResetEvent += testName;
		g_resetEvent = createEvent(strResetEvent.c_str());
		std::string strReadyEvent{ _T("Global\\TRCModul_Ready") };
		strReadyEvent += testName;
		g_readyEvent = createEvent(strReadyEvent.c_str());
		std::string strNewTrEvent{ _T("Global\\TRCModul_NewTrid") };
		strNewTrEvent += testName;
		g_newTrEvent = createEvent(strNewTrEvent.c_str());
	}

	bool waitForReaderEvents(LogClass& rLogClass)
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
			return true;
		}
		return false;
	}

	bool testInterestTr(ReaderTestData& testData, SvOi::ITriggerRecordControllerR* pTRC, SvPb::InspectionList& rInspectionList, int inspectionPos, int testDataId, int runId, int interestNumber)
	{
		if (nullptr == pTRC)
		{
			return false;
		}
		bool retValue = true;
		auto interestTRVec = pTRC->getTrsOfInterest(inspectionPos, interestNumber);
		if (0 < interestTRVec.size())
		{
			std::vector<int> interestList = calcExpectedInterestList(testData.m_isOtherProcess ? rInspectionList.list(inspectionPos).numberofrecords() : testData.m_rTestDataList[testDataId][inspectionPos].m_recordSize, testData.m_numberOfRuns, divFac);
			int currentPos = -1;
			for (const auto& pTrInterest : interestTRVec)
			{
				if (nullptr != pTrInterest)
				{
					int triggerCountInterest = pTrInterest->getTriggerData().m_TriggerCount;
					if (-1 == currentPos)
					{
						auto findIter = std::find(interestList.begin(), interestList.end(), triggerCountInterest);
						if (interestList.end() != findIter)
						{
							currentPos = static_cast<int>(std::distance(interestList.begin(), findIter));
							int expectedNumber = std::min<int>(interestNumber, currentPos + 1);
							if (interestTRVec.size() != expectedNumber)
							{
								retValue = false;
								std::string logStr = SvUl::Format(_T("Reader Tests(%d): getTRsOfInterest: Number of TRs %d, but expected %d (last triggerCount %d, requested interest %d) for ip %d with run %d"), testDataId, interestTRVec.size(), expectedNumber, triggerCountInterest, interestNumber, inspectionPos, runId);
								testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
								break;
							}

						}
						else
						{
							retValue = false;
							std::string logStr = SvUl::Format(_T("Reader Tests(%d): getTRsOfInterest: First TR (%d) should not be an interest TR for ip %d with run %d"), testDataId, triggerCountInterest, inspectionPos, runId);
							testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
							break;
						}
					}

					if (interestList[currentPos] != triggerCountInterest)
					{
						retValue = false;
						std::string logStr = SvUl::Format(_T("Reader Tests(%d): getTRsOfInterest: TR %d, but TR %d expected for ip %d with run %d"), testDataId, triggerCountInterest, interestList[currentPos], inspectionPos, runId);
						testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
						break;
					}
					currentPos--;
				}
				else
				{
					retValue = false;
					std::string logStr = SvUl::Format(_T("Reader Tests(%d): getTRsOfInterest: One of the TR is empty for ip %d with run %d"), testDataId, inspectionPos, runId);
					testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
					break;
				}
			}
		}
		else
		{
			retValue = false;
			std::string logStr = SvUl::Format(_T("Reader Tests(%d): getTRsOfInterest: Return list is empty for ip %d with run %d"), testDataId, inspectionPos, runId);
			testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
		}
		return retValue;
	}

	bool readAllDataFromOneWriterBlock(ReaderTestData& testData, SvOi::ITriggerRecordControllerR* pTRC, SvPb::InspectionList& rInspectionList)
	{
		if (nullptr == pTRC)
		{
			return false;
		}
		bool retValue = true;
		auto inspectionSize = rInspectionList.list_size();
		std::vector<int> lastTrIds(inspectionSize, -1);

		int lastTestDataId = 0;		
		int runId = -1;
		while (true)
		{
			HANDLE hChange[2];
			hChange[0] = g_resetEvent;
			hChange[1] = g_newTrEvent;
			DWORD waitValue = WaitForMultipleObjects(2, hChange, false, 1500);
			if (WAIT_OBJECT_0 == waitValue)
			{
				std::string logStr = SvUl::Format(_T("Reader Tests: Reset TRC after runId %d"), runId);
				bool isPass = (runId + 1 == testData.m_numberOfRuns * inspectionSize);
				testData.m_rLogClass.Log(logStr.c_str(), isPass ? LogLevel::Information_Level2 : LogLevel::Error, isPass ? LogType::PASS : LogType::FAIL, __LINE__, strTestWithMoreThreads);
				break;
			}

			std::vector<SvOi::TrEventData> newTrInfoVec;
			{
				std::lock_guard<std::mutex> lock(g_newTrInfoVecMutex);
				if (0 == g_newTrInfoVec.size())
				{
					if (runId + 1 < testData.m_numberOfRuns * inspectionSize)
					{
						std::string logStr = SvUl::Format(_T("Reader Tests: No new TR after runId %d"), runId);
						testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
					}
					else
					{
						std::string logStr = SvUl::Format(_T("Reader Tests: Finished run after runId %d"), runId);
						testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Information_Level2, LogType::PASS, __LINE__, strTestWithMoreThreads);
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
				int trIdLast = pTRC->getLastTrId(newTrInfo.m_inspectionPos);

				if (lastTrIds[newTrInfo.m_inspectionPos] == newTrInfo.m_trId)
				{
					std::string logStr = SvUl::Format(_T("Reader Tests: IP%d: check trId %d again by run %d"), newTrInfo.m_inspectionPos, newTrInfo.m_trId, runId);
					testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Information_Level1, LogType::WARN, __LINE__, strTestWithMoreThreads);
				}
				else if (trIdLast != newTrInfo.m_trId)
				{
					std::string logStr = SvUl::Format(_T("Reader Tests: IP%d: check trId %d not lastTrId %d again by run %d"), newTrInfo.m_inspectionPos, newTrInfo.m_trId, trIdLast, runId);
					testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Information_Level3, LogType::WARN, __LINE__, strTestWithMoreThreads);
				}

				lastTrIds[newTrInfo.m_inspectionPos] = newTrInfo.m_trId;

				auto tr2R = pTRC->createTriggerRecordObject(newTrInfo.m_inspectionPos, newTrInfo.m_trId);
				if (nullptr == tr2R)
				{
					std::string errorStr = SvUl::Format(_T("Reader Tests: createTriggerRecordObject(%d, %d) return nullptr by run %d!"), newTrInfo.m_inspectionPos, newTrInfo.m_trId, runId);
					testData.m_rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
					retValue = false;
					std::this_thread::sleep_for(2us);
					continue;
				}
				int triggerCount = tr2R->getTriggerData().m_TriggerCount;
				int testDataId = triggerCount / triggerIdOffset;
				int writerRunId = triggerCount % triggerIdOffset;
				if (0 > triggerCount || testData.m_rTestDataList.size() <= testDataId)
				{
					std::string errorStr = SvUl::Format(_T("Reader Tests: IP%d: triggerCount (%d) do not fit: , called testDataId %d, max testDataId %d by run %d"), newTrInfo.m_inspectionPos, triggerCount, testDataId, static_cast<int>(testData.m_rTestDataList.size()) - 1, runId);
					testData.m_rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
					retValue = false;
					std::this_thread::sleep_for(1us);
					continue;
				}
				else
				{
					std::string logStr = SvUl::Format(_T("Reader Tests(%d): createTriggerRecordObject(%d) triggerCount %d by run %d"), testDataId, newTrInfo.m_inspectionPos, triggerCount, runId);
					testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Debug, LogType::PASS, __LINE__, strTestWithMoreThreads);
				}

				if (testDataId != lastTestDataId)
				{
					std::string logStr = SvUl::Format(_T("Reader Tests(%d): Moved to this testDataId with run %d"), testDataId, runId);
					testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Debug, LogType::PASS, __LINE__, strTestWithMoreThreads);
					lastTestDataId = testDataId;
				}

				retValue = checkImages(testData.m_rTestDataList[testDataId][newTrInfo.m_inspectionPos], tr2R, testData.m_rLogClass, writerRunId, testDataId, newTrInfo.m_inspectionPos, runId, triggerCount) && retValue;

				//check trofInterest
				int maxInterestSize = testData.m_rTestDataList[testDataId][newTrInfo.m_inspectionPos].m_recordInterestSize;
				if (testData.m_isOtherProcess)
				{
					maxInterestSize = rInspectionList.list(newTrInfo.m_inspectionPos).numberrecordsofinterest();
				}
				if (0 < maxInterestSize)
				{
					std::random_device rd;
					std::uniform_int_distribution<int> dist(1, maxInterestSize);
					int interestNumber = dist(rd);
					std::string logStr = SvUl::Format(_T("Reader Tests(%d): getTRsOfInterest: Requested Number of Interest %d"), testDataId, interestNumber);
					testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Debug, LogType::BLANK, __LINE__, strTestWithMoreThreads);
					if (0 == runId)
					{	//at the first run, give writer time to set tr of interest.
						std::this_thread::sleep_for(1ms);
					}

					testInterestTr(testData, pTRC, rInspectionList, newTrInfo.m_inspectionPos, testDataId, runId, interestNumber);
				}
			}
			newTrInfoVec.clear();
		}
		return retValue;
	}

	void checkInterestEvents(ReaderTestData& testData, SvPb::InspectionList& rInspectionList, int testDataPos)
	{
		std::map<int, std::vector<int>> newInterestTrMap;
		{
			std::lock_guard<std::mutex> lock(g_newInterestTrMapMutex);
			newInterestTrMap.swap(g_newInterestTrMap);
			g_newInterestTrMap.clear();
		}

		if (0 < newInterestTrMap.size())
		{
			for (auto iter : newInterestTrMap)
			{
				std::string logStr;
				size_t expectedSize = (testData.m_numberOfRuns + 2) / divFac;
				if (0 < rInspectionList.list(iter.first).numberrecordsofinterest())
				{
					std::vector<int> interestList = calcExpectedInterestList(testData.m_isOtherProcess ? rInspectionList.list(iter.first).numberofrecords() : testData.m_rTestDataList[testDataPos][iter.first].m_recordSize, testData.m_numberOfRuns, divFac);
					expectedSize = interestList.size();
					logStr = SvUl::Format(_T("Reader Tests(%d): Interest-expected: "), testDataPos);
					for (auto trId : interestList)
					{
						logStr = SvUl::Format(_T("%s;%d"), logStr.c_str(), trId);
					}
					testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Debug, LogType::BLANK, __LINE__, strTestWithMoreThreads);
				}
				else
				{
					logStr = SvUl::Format(_T("Reader Tests(%d): Get %d interests where numberOfInterest is null for ip %d: "), testDataPos, iter.second.size(), iter.first);
					for (auto trId : iter.second)
					{
						logStr = SvUl::Format(_T("%s;%d"), logStr.c_str(), trId);
					}
					testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
					continue;
				}

				if (expectedSize == iter.second.size())
				{
					logStr = SvUl::Format(_T("Reader Tests(%d): %d TRsOfInterest-Events for ip %d"), testDataPos, iter.second.size(), iter.first);
					testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Information_Level3, LogType::PASS, __LINE__, strTestWithMoreThreads);
				}
				else if (expectedSize - 1 == iter.second.size())
				{
					logStr = SvUl::Format(_T("Reader Tests(%d): %zu instead of %zu TRsOfInterest-Events for ip %d"), testDataPos, iter.second.size(), expectedSize, iter.first);
					testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Information_Level1, LogType::WARN, __LINE__, strTestWithMoreThreads);
				}
				else
				{
					logStr = SvUl::Format(_T("Reader Tests(%d): %zu instead of %zu TRsOfInterest-Events for ip %d"), testDataPos, iter.second.size(), expectedSize, iter.first);
					testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
				}
				logStr = _T("Interest: ");
				for (auto trId : iter.second)
				{
					logStr = SvUl::Format(_T("%s;%d"), logStr.c_str(), trId);
				}
				testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Debug, LogType::BLANK, __LINE__, strTestWithMoreThreads);
			}
		}
		else
		{
			if (0 < rInspectionList.list(0).numberrecordsofinterest())
			{
				std::string logStr = SvUl::Format(_T("Reader Tests(%d): None TRsOfInterest-Events get"), testDataPos);
				testData.m_rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
			}
		}
		newInterestTrMap.clear();
	}

	bool readerTestLoop(ReaderTestData& testData, SvOi::ITriggerRecordControllerR* pTRC)
	{
		if (nullptr == pTRC)
		{
			return false;
		}
		bool retValue = true;
		for (int testDataPos = 0; testDataPos < testData.m_rTestDataList.size(); testDataPos++)
			//for (int runId = 0; runId < numberOfRunsComplete; runId++)
		{
			bool isErrorThanBreakRun = waitForReaderEvents(testData.m_rLogClass);
			if (isErrorThanBreakRun)
			{
				break;
			}
			auto inspectionList = pTRC->getInspections();
			retValue = readAllDataFromOneWriterBlock(testData, pTRC, inspectionList);

			//check newInterestEvents
			checkInterestEvents(testData, inspectionList, testDataPos);
		}
		return retValue;
	}
}

bool readerTest(LPCTSTR testName, ReaderTestData testData)
{
	bool retValue = true;
	setEvents(testName);
	if (nullptr != g_resetEvent && nullptr != g_readyEvent && nullptr != g_newTrEvent)
	{
		auto pTRC = SvOi::getTriggerRecordControllerRInstance();
		if (nullptr != pTRC)
		{
			int resetCallbackHandle = pTRC->registerResetCallback(OnResetTRC);
			int readyCallbackHandle = pTRC->registerReadyCallback(OnReadyTRC);
			int newTrCallBackHandle = pTRC->registerNewTrCallback(OnNewTr);
			int newInterestCallBackHandle = pTRC->registerNewInterestTrCallback(std::bind(OnNewInterestTr, &testData.m_rLogClass, std::placeholders::_1));
			if (pTRC->isValid())
			{
				OnReadyTRC();
			}

			retValue = readerTestLoop(testData, pTRC);

			pTRC->unregisterResetCallback(resetCallbackHandle);
			pTRC->unregisterReadyCallback(readyCallbackHandle);
			pTRC->unregisterNewTrCallback(newTrCallBackHandle);
			pTRC->unregisterNewInterestTrCallback(newInterestCallBackHandle);
		}
	}
	else
	{
		testData.m_rLogClass.Log(_T("Set Events for ReaderTest failed"), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
		retValue = false;
	}
		
	return retValue;
}
#pragma endregion Global Function