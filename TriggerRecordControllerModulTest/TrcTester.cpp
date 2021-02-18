//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file TrcTester.h
/// All Rights Reserved
//*****************************************************************************
/// Definition of the Trigger Record controller tester class (and a configuration 
/// struct for it).
//******************************************************************************
#pragma once

#include "stdafx.h"
#include "TrcTester.h"
#include "TesterFunction.h"
#include "LogClass.h"
#include "TrcTesterConfiguration.h"
#include "SVMatroxLibrary\SVMatroxBufferCreateStruct.h"
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVStatusLibrary\RegistryAccess.h"
#include "SVUtilityLibrary/SVClock.h"
#include "SVUtilityLibrary\StringHelper.h"
#include "ObjectInterfaces/ITriggerRecordControllerRW.h"
#include "ObjectInterfaces/ITriggerRecordRW.h"

constexpr int triggerIdOffset = 100'000;
static int g_maxSizeFactor = 32;
LPTSTR strTestCreateInspections = _T("createInspections");
LPTSTR strTestSetBuffers = _T("setBuffers");
LPTSTR strTestCheckBufferMaximum = _T("checkBufferMaximum");
LPTSTR strTestCreateTR2WriteAndRead = _T("createTR2WriteAndRead");
LPTSTR strTestSetAndReadImage = _T("setAndReadImage");
LPTSTR strTestSetAndReadValues = _T("setAndReadValues");
LPTSTR strTestWithReaderApps = _T("testWithReaderApps");

struct ReaderProcessData
{
	std::string m_name;
	STARTUPINFO m_info = {sizeof(m_info)};
	PROCESS_INFORMATION m_processInfo;
};

int calcMaxBuffer();
void runWriterTest(std::promise<bool>&& intPromise, LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData);
void runReaderTest(std::promise<bool>&& intPromise, LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData);
SVMatroxBufferCreateStruct specifyBuffer(int sizeFactor);
SVMatroxBufferCreateStruct specifyBufferRandom();
bool startReaderApp(ReaderProcessData& data, int logLevel);
bool finishedReaderApp(ReaderProcessData data, int timeoutinMs, LogClass& rLogClass);

TrcTester::TrcTester(TrcTesterConfiguration& rConfig, LogClass& rLogClass) :
	m_config(rConfig),
	m_rLogClass(rLogClass),
	m_pTrcRW(SvOi::getTriggerRecordControllerRWInstance())
{
	g_maxSizeFactor = m_config.getMaxSpecifyBufferFactor();
}

TrcTester::~TrcTester()
{
	if (nullptr != m_pTrcRW)
	{
		m_pTrcRW->clearAll(); // avoids error message when MappFree() is called.
	}

	google::protobuf::ShutdownProtobufLibrary();
}

bool TrcTester::fullTest()
{
	bool isRunOk = createInspections();
	isRunOk = setBuffers() && isRunOk;
	isRunOk = checkBufferMaximum() && isRunOk;
	isRunOk = createTR2WriteAndRead() && isRunOk;
	isRunOk = setAndReadImage() && isRunOk;
	isRunOk = setAndReadValues() && isRunOk;
	isRunOk = testWithMoreThreads() && isRunOk;
	if (!m_config.isLocal())
	{
		isRunOk = testWithReaderApps<1>() && isRunOk;
		isRunOk = testWithReaderApps<2>() && isRunOk;
	}
	return isRunOk;
}

#pragma region test methods
bool TrcTester::createInspections()
{
	if (nullptr == m_pTrcRW)
	{
		return false;
	}
	std::vector<std::pair<int, int>> numbersOfRecords(m_config.getNumberOfInspections(), {m_config.getNumberOfImagesPerInspection(), 0});
	for (int i = 0; i < m_config.getNoOfRepetitionsPerStep(); i++)
	{
		bool createInspectionsOk = setInspections(numbersOfRecords, *m_pTrcRW, m_rLogClass, strTestCreateInspections);
		if (!createInspectionsOk)
		{
			std::string errorStr = SvUl::Format(_T("(%d, %d) ERROR on %d. iteration!"), m_config.getNumberOfImagesPerInspection(), m_config.getNumberOfInspections(), i);
			m_rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestCreateInspections);
			return false;
		}
	}
	m_rLogClass.Log("Successfully", LogLevel::Information_Level1, LogType::PASS, __LINE__, strTestCreateInspections);
	return true;
}

bool TrcTester::setBuffers()
{
	if (nullptr == m_pTrcRW)
	{
		return false;
	}
	std::string logStr;
	bool setBufferOk = true;
	double elapsed_ms_sum = 0.;
	std::vector<std::pair<int, int>> numbersOfRecords(m_config.getNumberOfInspections(), {m_config.getNumberOfImagesPerInspection(), 0});
	for (int i = 0; i < m_config.getNoOfRepetitionsPerStep(); i++)
	{
		setInspections(numbersOfRecords, *m_pTrcRW, m_rLogClass, strTestCreateInspections);
		m_pTrcRW->setGlobalInit();
		double start = SvUl::GetTimeStamp();
		setBufferOk = setInspectionBuffers(strTestSetBuffers);
		if (!setBufferOk)
		{
			logStr = SvUl::Format(_T("setInspectionBuffers(%d, %d) ERROR on %d. iteration!"), m_config.getNumberOfImagesPerInspection(), m_config.getNumberOfInspections(), i);
			m_rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL,  __LINE__, strTestSetBuffers);
			break;
		}

		setBufferOk = setIndependentBuffers(strTestSetBuffers);
		if (!setBufferOk)
		{
			logStr = SvUl::Format(_T("setIndependentBuffers(%d, %d) ERROR on %d. iteration!"), m_config.getNumberOfImagesPerInspection(), m_config.getNumberOfInspections(), i);
			m_rLogClass.Log(logStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetBuffers);
			break;
		}
		m_pTrcRW->finishGlobalInit();
		double end = SvUl::GetTimeStamp();
		double elapsed_ms = end - start;
		elapsed_ms_sum += elapsed_ms;
		m_pTrcRW->clearAll();
	}

	if (setBufferOk)
	{
		bool isTimeOK = (elapsed_ms_sum <= m_config.getMaxTimeSetBuffer());
		logStr = SvUl::Format(_T("time (%f ms, max time is %f)"), elapsed_ms_sum, m_config.getMaxTimeSetBuffer());
		m_rLogClass.Log(logStr.c_str(), isTimeOK ? LogLevel::Information_Level1 : LogLevel::Error, isTimeOK ? LogType::PASS : LogType::FAIL, __LINE__, strTestSetBuffers);
	}
	return setBufferOk;
}

bool TrcTester::checkBufferMaximum()
{
	constexpr int numberOfRecords = 100;
	
	if (nullptr == m_pTrcRW)
	{
		return false;
	}
	bool retValue = setInspections({{numberOfRecords, 0}}, *m_pTrcRW, m_rLogClass, strTestCheckBufferMaximum);
	m_rLogClass.Log(_T("setInspections!"), retValue ? LogLevel::Information_Level3 : LogLevel::Error, retValue ? LogType::PASS : LogType::FAIL, __LINE__, strTestCheckBufferMaximum);
	if (false == retValue)
	{
		return false;
	}

	const int maxBuffer = calcMaxBuffer();
	const int numberOfRecordsAddOne = numberOfRecords + m_config.getNumberOfRecordsAddOne();
	const int numberOfImages = maxBuffer / numberOfRecordsAddOne;
	const int numberOfAddBuffer = maxBuffer%numberOfRecordsAddOne;
	//create maximum number of possible image buffers (no exception should happen.
	try
	{
		double start = SvUl::GetTimeStamp();
		m_pTrcRW->startResetTriggerRecordStructure(0);
		bool independentOk = m_pTrcRW->removeAllImageBuffer();
		for (int i = 0; i < numberOfImages; i++)
		{
			independentOk &= ( 0 <= m_pTrcRW->addOrChangeImage(getNextObjectId(), specifyBuffer(i / m_config.getSpecifyBufferDiv())));
		}
		m_pTrcRW->addImageBuffer(getNextObjectId(), specifyBuffer(1), numberOfAddBuffer);
		m_pTrcRW->finishResetTriggerRecordStructure();
		double end = SvUl::GetTimeStamp();
		double elapsed_ms = end - start;
		std::string logStr = SvUl::Format(_T("set images (maxBuffer = %d) good case (%f ms/ %f ms)"), maxBuffer, elapsed_ms, m_config.getMaxTimeCheckBufferPerBuffer()*numberOfImages);
		bool isError = (!independentOk || m_config.getMaxTimeCheckBufferPerBuffer()*numberOfImages < elapsed_ms);
		m_rLogClass.Log(logStr.c_str(), !isError ? LogLevel::Information_Level3 : LogLevel::Error, !isError ? LogType::PASS : LogType::FAIL, __LINE__, strTestCheckBufferMaximum);
		if (!independentOk)
		{
			return false;
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		std::string logStr = SvUl::Format(_T("set images good case (MaxBuffer = %d)"), maxBuffer);
		m_rLogClass.logException(logStr.c_str(), rExp, __LINE__, strTestCheckBufferMaximum);
		return false;
	}
	catch (...)
	{
		m_rLogClass.Log(_T("Unknown exception: set images good case!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestCheckBufferMaximum);
		return false;
	}

	try
	{
		m_pTrcRW->startResetTriggerRecordStructure();
		//pTrcRW->removeAllImageBuffer();
		m_pTrcRW->addImageBuffer(getNextObjectId(), specifyBuffer(3), 1);
		m_pTrcRW->finishResetTriggerRecordStructure();

		m_rLogClass.Log(_T("set images too many buffers, but no exception"), LogLevel::Error, LogType::FAIL, __LINE__, strTestCheckBufferMaximum);
		return false;
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		m_rLogClass.logException("set images too many buffers", rExp, __LINE__, strTestCheckBufferMaximum, LogLevel::Information_Level3, LogType::PASS);
	}
	catch (...)
	{
		m_rLogClass.Log(_T("Unknown exception: set images too many buffers!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestCheckBufferMaximum);
		return false;
	}

	m_rLogClass.Log("Successfully", LogLevel::Information_Level1, LogType::PASS, __LINE__, strTestCheckBufferMaximum);
	return true;
}

bool TrcTester::createTR2WriteAndRead()
{
	if (nullptr == m_pTrcRW)
	{
		return false;
	}
	std::random_device rd;
	std::uniform_int_distribution<int> dist(1, SvOi::ITriggerRecordControllerRW::ITriggerRecordControllerRW::cMaxTriggerRecords);
	constexpr int numberOfInspection = 2;
	std::vector<std::pair<int, int>> numbersOfRecords = {{dist(rd),0}, {dist(rd),0}};
	bool retValue = setInspections(numbersOfRecords, *m_pTrcRW, m_rLogClass, strTestCreateTR2WriteAndRead);
	if (!retValue)
	{
		m_rLogClass.Log(_T("setInspections!"),  LogLevel::Error, LogType::FAIL, __LINE__, strTestCreateTR2WriteAndRead);
		return false;
	}
	else
	{
		std::string errorStr = SvUl::Format(_T("setInspections with the TR-size of %d/%d"), numbersOfRecords[0].first, numbersOfRecords[1].first);
		m_rLogClass.Log(errorStr.c_str(), LogLevel::Information_Level3, LogType::PASS, __LINE__, strTestCreateTR2WriteAndRead);
	}

	try
	{
		for (int i = 0; i < numberOfInspection; i++)
		{
			m_pTrcRW->startResetTriggerRecordStructure(i);
			retValue = (0 <= m_pTrcRW->addOrChangeImage(getNextObjectId(), specifyBufferRandom()));
			m_pTrcRW->finishResetTriggerRecordStructure();
			if (!retValue)
			{
				m_rLogClass.Log(_T("init Inspection"), LogLevel::Error, LogType::FAIL, __LINE__, strTestCreateTR2WriteAndRead);
				return false;
			}
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		m_rLogClass.logException("init Inspection", rExp, __LINE__, strTestCreateTR2WriteAndRead);
		return false;
	}
	catch (...)
	{
		m_rLogClass.Log(_T("Unknown exception: init Inspection!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestCreateTR2WriteAndRead);
		return false;
	}

	int maxRecords = 0;
	for (auto valuePair : numbersOfRecords)
	{
		maxRecords = std::max(maxRecords, valuePair.first);
	}

	{
		SvOi::ITriggerRecordRWPtr tr2W = m_pTrcRW->createTriggerRecordObjectToWrite(0);
		if (nullptr != tr2W)
		{
			auto tr2R = m_pTrcRW->createTriggerRecordObject(0, tr2W->getId());
			if (nullptr != tr2R)
			{
				m_rLogClass.Log(_T("could create read version of TR, but write version still open"), LogLevel::Error, LogType::FAIL, __LINE__, strTestCreateTR2WriteAndRead);
			}
		}
	}

	int numberOfFails = 0;
	std::array<std::vector<SvOi::ITriggerRecordRPtr>, numberOfInspection> readTRVector;
	for (int i = 0; i < maxRecords+10; i++)
	{
		for (int j = 0; j < numberOfInspection; j++)
		{
			{ //scope for tr
				auto tr = m_pTrcRW->createTriggerRecordObjectToWrite(j);
				if (nullptr == tr)
				{
					if (readTRVector[j].size() <= numbersOfRecords[j].first)
					{
						std::string errorStr = SvUl::Format(_T("createTriggerRecordObjectToWrite in insp %d run %d"), j, i);
						m_rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestCreateTR2WriteAndRead);
					}
					else
					{
						std::string errorStr = SvUl::Format(_T("createTriggerRecordObjectToWrite failed ( in insp %d run %d) as excepted because to many read version logged."), j, i);
						m_rLogClass.Log(errorStr.c_str(), LogLevel::Information_Level2, LogType::PASS, __LINE__, strTestCreateTR2WriteAndRead);
						size_t countToDelete = 22;
						if (readTRVector[j].size() <= countToDelete)
						{
							readTRVector[j].clear();
						}
						else
						{
							readTRVector[j].erase(readTRVector[j].begin(), readTRVector[j].begin() + countToDelete);
						}
					}
					continue;
				}
				else
				{
					if (readTRVector[j].size() > numbersOfRecords[j].first + m_config.getNumberOfRecordsAddOne()) //add some buffer, because the TRC have a few more TR than required.
					{
						std::string errorStr = SvUl::Format(_T("createTriggerRecordObjectToWrite possible(insp %d run %d), but failed expected because to many read-version logged."), j, i);
						m_rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestCreateTR2WriteAndRead);
					}
				}
			}
			int id = m_pTrcRW->getLastTrId(j);
			auto tr2R = m_pTrcRW->createTriggerRecordObject(j, id);
			bool shouldFail = readTRVector[j].size() >= numbersOfRecords[j].first + m_config.getNumberOfRecordsAddOne() - m_config.getNumberOfKeepFreeRecords();
			if (nullptr == tr2R)
			{
				if (!shouldFail)
				{
					std::string errorStr = SvUl::Format(_T("could not create read version of TR (id%d) in inspection %d"), id, j);
					m_rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestCreateTR2WriteAndRead);
				}
				numberOfFails++;
				if (0 == numberOfFails % 11)
				{
					if (13 < readTRVector[j].size())
					{
						readTRVector[j].erase(readTRVector[j].begin(), readTRVector[j].begin() + 13);
					}
					else
					{
						readTRVector[j].clear();
					}
				}
			}
			else
			{
				if (shouldFail)
				{
					std::string errorStr = SvUl::Format(_T("createTriggerRecordObject should fail, because to many records logged. (id%d in inspection %d)"), id, j);
					m_rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestCreateTR2WriteAndRead);
				}
				readTRVector[j].emplace_back(tr2R);
			}
		}
	}

	m_rLogClass.Log("Successfully", LogLevel::Information_Level1, LogType::PASS, __LINE__, strTestCreateTR2WriteAndRead);
	return retValue;
}

bool TrcTester::setAndReadImage()
{
	if (nullptr == m_pTrcRW)
	{
		return false;
	}

	bool retValue = setInspections({{12,0}}, *m_pTrcRW, m_rLogClass, strTestSetAndReadImage);
	if (false == retValue)
	{
		m_rLogClass.Log(_T("setInspections!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadImage);
		return false;
	}

	//init triggerRecord
	const auto& imageIds = m_config.getImageLists()[0];
	auto bufferStruct = specifyBufferFromImage(imageIds[0]);
	try
	{
		m_pTrcRW->startResetTriggerRecordStructure(0);
		m_pTrcRW->removeAllImageBuffer();
		m_pTrcRW->addImageBuffer(getNextObjectId(), bufferStruct, 1);
		
		retValue = (0 <= m_pTrcRW->addOrChangeImage(getNextObjectId(), bufferStruct));
		m_pTrcRW->finishResetTriggerRecordStructure();
		if (!retValue)
		{
			m_rLogClass.Log(_T("init Inspection"), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadImage);
			return false;
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		m_rLogClass.logException("init Inspection", rExp, __LINE__, strTestSetAndReadImage);
		return false;
	}
	catch (...)
	{
		m_rLogClass.Log(_T("Unknown exception: init Inspection!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadImage);
		return false;
	}

	//first time of the next commands needs much longer. Run this before check performance
	{
		auto imageHandle = m_pTrcRW->getImageBuffer(bufferStruct);
		MbufCopy(imageIds[0], imageHandle->getHandle()->GetBuffer().GetIdentifier());
		areImageEqual(imageHandle->getHandle()->GetBuffer().GetIdentifier(), imageIds[0]);
	}

	//constexpr int numberOfRuns = 100;
	const int numberOfRuns = m_config.getNoOfRepetitionsPerStep();
	double start = SvUl::GetTimeStamp();
	for (int i = 0; i < numberOfRuns; i++)
	{
		//write image to buffer
		auto imageHandle = m_pTrcRW->getImageBuffer(bufferStruct);
		if (nullptr == imageHandle || imageHandle->isEmpty())
		{
			m_rLogClass.Log(_T("getImageBuffer return nullptr!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadImage);
			return false;
		}
		else
		{
			MbufCopy(imageIds[i%imageIds.size()], imageHandle->getHandle()->GetBuffer().GetIdentifier());
			if (!areImageEqual(imageHandle->getHandle()->GetBuffer().GetIdentifier(), imageIds[i%imageIds.size()]))
			{
				m_rLogClass.Log(_T("ImageCopy failed. Images are not equal!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadImage);
			}
		}

		{//scope for tr2W
			//get TR and set new image
			auto tr2W = m_pTrcRW->createTriggerRecordObjectToWrite(0);
			if (nullptr == tr2W)
			{
				m_rLogClass.Log(_T("createTriggerRecordObjectToWrite return nullptr!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadImage);
				return false;
			}
			try
			{
				tr2W->setImage(0, imageHandle->getBufferPos());
			}
			catch (const SvStl::MessageContainer& rExp)
			{
				m_rLogClass.logException("", rExp, __LINE__, strTestSetAndReadImage);
				return false;
			}
		}
		int id = m_pTrcRW->getLastTrId(0);
		auto tr2R = m_pTrcRW->createTriggerRecordObject(0, id);
		if (nullptr == tr2R)
		{
			std::string errorStr = SvUl::Format(_T("could not create read version of TR (id%d) in run %d"), id, i);
			m_rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadImage);
			continue;
		}
		imageHandle = tr2R->getImage(0);
		if (nullptr == imageHandle || imageHandle->isEmpty())
		{
			m_rLogClass.Log(_T("getImageBuffer return nullptr!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadImage);
			return false;
		}
		else
		{
			if (!areImageEqual(imageHandle->getHandle()->GetBuffer().GetIdentifier(), imageIds[i%imageIds.size()]))
			{
				m_rLogClass.Log(_T("ImageCopy failed. Images are not equal!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadImage);
			}
			//else
			//{
			//	CString logStr;
			//	logStr.Format(_T("Read image is equal to set image in run %d"), i);
			//	m_rLogClass.Log(logStr, LogLevel::Information_Level3, LogType::PASS, __LINE__, strTestSetAndReadImage);
			//}
		}
	}
	double end = SvUl::GetTimeStamp();
	double elapsed_ms = end - start;

	std::string logStr = SvUl::Format(_T("(%f ms/ %f ms)"), elapsed_ms, numberOfRuns*m_config.getMaxTimesetAndReadImage());
	bool isTimeOk = elapsed_ms < numberOfRuns*m_config.getMaxTimesetAndReadImage();
	m_rLogClass.Log(logStr.c_str(), isTimeOk ? LogLevel::Information_Level1 : LogLevel::Error, isTimeOk ? LogType::PASS : LogType::FAIL, __LINE__, strTestSetAndReadImage);
	return true;
}

bool TrcTester::setAndReadValues()
{
	if (nullptr == m_pTrcRW)
	{
		return false;
	}
	bool retValue = setInspections({{21, 0}}, *m_pTrcRW, m_rLogClass, strTestSetAndReadValues);
	if (false == retValue)
	{
		m_rLogClass.Log(_T("setInspections!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadValues);
		return false;
	}

	//create ValueList
	SvPb::DataDefinitionList dataDefList;
	std::vector<std::vector<uint8_t>> dataMemVector;
	long memOffset = createDataDefContainer(dataMemVector, dataDefList);

	//init triggerRecord
	try
	{
		auto tmpDefList = dataDefList;
		m_pTrcRW->startResetTriggerRecordStructure(0);
		m_pTrcRW->changeDataDef(std::move(tmpDefList), memOffset, 0);
		m_pTrcRW->finishResetTriggerRecordStructure();
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		m_rLogClass.logException("init Inspection", rExp, __LINE__, strTestSetAndReadValues);
		return false;
	}
	catch (...)
	{
		m_rLogClass.Log(_T("Unknown exception: init Inspection!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadValues);
		return false;
	}

	//first run empty run, because the first two runs can last longer.
	for (int i = 0; i < 2; i++)
	{
		writeAndReadSingleValue(i, dataDefList.list_size(), dataMemVector);
	}

	constexpr int numberOfRuns = 22;
	double start = SvUl::GetTimeStamp();
	for (int i = 0; i < numberOfRuns; i++)
	{
		bool retVal = writeAndReadSingleValue(i, dataDefList.list_size(), dataMemVector);
		if (!retVal)
		{
			return retVal;
		}		
	}
	double end = SvUl::GetTimeStamp();
	double elapsed_ms = end - start;

	std::string logStr = SvUl::Format(_T("(%f ms/ %f ms)"), elapsed_ms, numberOfRuns*m_config.getMaxTimesetAndReadValue());
	bool isTimeOk = elapsed_ms < numberOfRuns*m_config.getMaxTimesetAndReadValue();
	m_rLogClass.Log(logStr.c_str(), isTimeOk ? LogLevel::Information_Level1 : LogLevel::Error, isTimeOk ? LogType::PASS : LogType::FAIL, __LINE__, strTestSetAndReadValues);
	return true;
}

bool TrcTester::testWithMoreThreads()
{
	std::promise<bool> writerPromise;
	std::promise<bool> readerPromise;
	std::future<bool> writerResult = writerPromise.get_future();
	std::future<bool> readerResult = readerPromise.get_future();
	m_config.recalcRecordSizes();
	std::thread writerThread(runWriterTest, std::move(writerPromise), std::ref(m_rLogClass), 100, m_config.getTestData());
	std::thread readerThread(runReaderTest, std::move(readerPromise), std::ref(m_rLogClass), 100, m_config.getTestData());
	bool retValue = writerResult.get() && readerResult.get();
	{
		m_rLogClass.Log(_T("----End test-----"), retValue ? LogLevel::Information_Level1 : LogLevel::Error, retValue ? LogType::PASS : LogType::FAIL, __LINE__, strTestWithMoreThreads);
	}
	writerThread.join();
	readerThread.join();
	return retValue;
}

template <int readerSize>
bool TrcTester::testWithReaderApps()
{
	bool retValue = true;
	int timeoutInS = 10;

	std::array<std::pair<bool, ReaderProcessData>, readerSize> readerArray;
	std::string tmpStr = SvUl::Format("Start testWithReaderApps with %d reader", static_cast<int>(readerArray.size()));
	m_rLogClass.LogText(tmpStr.c_str(), LogLevel::Information_Level3, LogType::BLANK);
	for (int i = 0; i < readerArray.size(); i++)
	{
		readerArray[i].second.m_name = "reader"+ std::to_string(i+1);
		readerArray[i].first = startReaderApp(readerArray[i].second, static_cast<int>(m_rLogClass.GetLogLevel()));
		if (!readerArray[i].first)
		{
			tmpStr = SvUl::Format("Create Reader-Process %s", readerArray[i].second.m_name.c_str());
			m_rLogClass.Log(tmpStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithReaderApps);
		}
	}

	bool retWriterTest = writerTest(m_rLogClass, 100, m_config.getTestData(), 1 < readerSize ? 5500 : 3000);
	retValue = retValue && retWriterTest;
	m_rLogClass.Log("Finished writerTest", retWriterTest ? LogLevel::Information_Level1 : LogLevel::Error, retWriterTest ? LogType::PASS : LogType::FAIL, __LINE__, strTestWithReaderApps);

	for (auto readerPair : readerArray)
	{
		if (readerPair.first)
		{
			retValue = finishedReaderApp(readerPair.second, timeoutInS * 1000, m_rLogClass) && retValue;
		}
	}

	{
		m_rLogClass.Log(_T("----End test-----"), retValue ? LogLevel::Information_Level1 : LogLevel::Error, retValue ? LogType::PASS : LogType::FAIL, __LINE__, strTestWithReaderApps);
	}

	return retValue;
}
#pragma endregion test methods

bool TrcTester::setInspectionBuffers(LPCSTR testAreaStr)
{
	if (nullptr == m_pTrcRW)
	{
		return false;
	}
	for (int i = 0; i < m_config.getNumberOfInspections(); i++)
	{
		try
		{
			m_pTrcRW->startResetTriggerRecordStructure(i);

			for (int j = 0; j < m_config.getNumberOfBuffersPerInspection(); j++)
			{
				m_pTrcRW->addOrChangeImage(getNextObjectId(), specifyBuffer(1 + j));
			}

			m_pTrcRW->finishResetTriggerRecordStructure(); // i.e the current inspection will be completed

		}
		catch (const SvStl::MessageContainer& rExp)
		{
			m_rLogClass.logException("Parts of setInspectionBuffer", rExp, __LINE__, testAreaStr);
			return false;
		}
		catch (...)
		{
			std::string errorStr = SvUl::Format(_T("Parts of setInspectionBuffer throw an exception: #%d!"), i);
			m_rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, testAreaStr);
			return false;
		}
	}

	return true;
}


bool TrcTester::setIndependentBuffers(LPCSTR testAreaStr)
{
	if (nullptr == m_pTrcRW)
	{
		return false;
	}
	try
	{
		m_pTrcRW->startResetTriggerRecordStructure();
		bool independentOk = true;
		for (int i = 0; i < m_config.getNumberOfBuffersPerInspection(); i++)
		{
			independentOk &= m_pTrcRW->removeAllImageBuffer();
			m_pTrcRW->addImageBuffer(getNextObjectId(), specifyBuffer(m_config.getNumberOfIndependentBuffers()), 1);
		}
		m_pTrcRW->finishResetTriggerRecordStructure();

		if (!independentOk)
		{
			m_rLogClass.Log(_T("ERROR when creating independent buffer!"), LogLevel::Error, LogType::FAIL, __LINE__, testAreaStr);
			return false;
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		m_rLogClass.logException("Parts of setIndependentBuffers", rExp, __LINE__, testAreaStr);
		return false;
	}
	catch (...)
	{
		m_rLogClass.Log(_T("Unknown exception when creating independent buffer!"), LogLevel::Error, LogType::FAIL, __LINE__, testAreaStr);
		return false;
	}

	return true;
}

constexpr int handleForReset = 1000;
int calcMaxBuffer()
{
	RegistryAccess reg(_T(R"(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\\Windows\)"));
	DWORD value = 0;
	if (reg.GetRegistryValue(_T("GDIProcessHandleQuota"), &value) || 0 < value)
	{
		if (handleForReset < value)
		{
			return static_cast<int>(value) - handleForReset;
		}
		else
		{
			return static_cast<int>(value * 0.9);
		}
	}
	return 0;
}

void runWriterTest(std::promise<bool>&& intPromise, LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData)
{
	bool retValue = writerTest(rLogClass, numberOfRuns, rTestData);
	intPromise.set_value(retValue);
	rLogClass.LogText("Finished runWriterTest", LogLevel::Information_Level1, LogType::PASS);
}

void runReaderTest(std::promise<bool>&& intPromise, LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData)
{
	bool retValue = readerTest(_T("Local"), {rLogClass, numberOfRuns, rTestData});
	intPromise.set_value(retValue);
	rLogClass.LogText("Finished runReaderTest", LogLevel::Information_Level1, LogType::PASS);
}

SVMatroxBufferCreateStruct specifyBuffer(int sizeFactor)
{
	sizeFactor = std::max<int>(std::min<int>(sizeFactor, g_maxSizeFactor), 1);
	SVMatroxBufferCreateStruct bufferStruct;
	int sizeX = sizeFactor * 32;
	int sizeY = sizeFactor * 24;
	bufferStruct.m_lSizeX = sizeX;
	bufferStruct.m_lSizeY = sizeY;
	bufferStruct.m_eAttribute = SVBufAttImageProcDib;
	bufferStruct.m_eType = SV8BitUnsigned;

	return bufferStruct;
}

SVMatroxBufferCreateStruct specifyBufferRandom()
{
	std::random_device rd;
	std::uniform_int_distribution<int> dist(1, 2000);
	SVMatroxBufferCreateStruct bufferStruct;
	int sizeX = dist(rd);
	int sizeY = dist(rd);
	bufferStruct.m_lSizeX = sizeX;
	bufferStruct.m_lSizeY = sizeY;
	bufferStruct.m_eAttribute = SVBufAttImageProcDib;
	bufferStruct.m_eType = SV8BitUnsigned;

	return bufferStruct;
}

bool startReaderApp(ReaderProcessData& data, int logLevel)
{
	std::string commandStr = SvUl::Format(R"(TriggerRecordControllerReaderModulTest.exe %s %d)", data.m_name.c_str(), logLevel);
	if (!CreateProcess(R"(TriggerRecordControllerReaderModulTest.exe)", const_cast<char*> (commandStr.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &data.m_info, &data.m_processInfo))
	{
		return false;
	}
	return true;
}

bool finishedReaderApp(ReaderProcessData data, int timeoutinMs, LogClass& rLogClass)
{
	bool retValue = true;
	if (WAIT_OBJECT_0 == WaitForSingleObject(data.m_processInfo.hProcess, timeoutinMs))
	{
		std::ifstream logFile((data.m_name + "_log.txt").c_str());
		if (logFile.is_open())
		{
			std::string tmpString = SvUl::Format("Reader (%s) ready read logFile:", data.m_name.c_str());
			rLogClass.LogText0(tmpString.c_str(), LogLevel::Information_Level3);
			std::string line;
			bool bFindString = false;
			while (getline(logFile, line))
			{
				retValue = rLogClass.convertAndLogString(line) && retValue;
				bFindString = true;
			}
			logFile.close();
			tmpString = SvUl::Format("Reader (%s) finished, read logFile finished", data.m_name.c_str());
			rLogClass.Log(tmpString.c_str(), (retValue&&bFindString) ? LogLevel::Information_Level1 : LogLevel::Error, retValue ? LogType::PASS : LogType::FAIL, __LINE__, strTestWithReaderApps);
		}
		else
		{
			std::string tmpString = SvUl::Format("Reader (%s) finished, but logFile not found", data.m_name.c_str());
			rLogClass.Log(tmpString.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithReaderApps);
		}
	}
	else
	{
		std::string tmpString = SvUl::Format("Timeout Reader (%s) finished not in %d s", data.m_name.c_str(), timeoutinMs/1000);
		rLogClass.Log(tmpString.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithReaderApps);
		TerminateProcess(data.m_processInfo.hProcess, 0);
		retValue = false;
	}
	// Close process and thread handles. 
	CloseHandle(data.m_processInfo.hProcess);
	CloseHandle(data.m_processInfo.hThread);
	return retValue;
}

bool TrcTester::writeAndReadSingleValue(int index, int listSize, const std::vector<std::vector<uint8_t>>& rDataVector)
{
	if (nullptr == m_pTrcRW)
	{
		return false;
	}
	const auto& rRunData = m_config.getValueSet()[index%m_config.getValueSet().size()];
	const auto& rMemData = rDataVector[index%rDataVector.size()];
	{//scope for tr2W
	 //get TR and set new image
		auto tr2W = m_pTrcRW->createTriggerRecordObjectToWrite(0);
		if (nullptr == tr2W)
		{
			m_rLogClass.Log(_T("createTriggerRecordObjectToWrite return nullptr!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadValues);
			return false;
		}
		try
		{
			auto tmpData = rMemData;
			tr2W->writeValueData(&tmpData[0], static_cast<int32_t>(tmpData.size()));
		}
		catch (const SvStl::MessageContainer& rExp)
		{
			m_rLogClass.logException("", rExp, __LINE__, strTestSetAndReadValues);
			return false;
		}
	}
	std::this_thread::yield();
	int id = m_pTrcRW->getLastTrId(0);
	auto tr2R = m_pTrcRW->createTriggerRecordObject(0, id);
	if (nullptr == tr2R)
	{
		std::string errorStr = SvUl::Format(_T("could not create read version of TR (id%d) in run %d"), id, index);
		m_rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadValues);
		return true;
	}

	int pos = index%listSize;
	variant_t testValue = tr2R->getDataValue(pos);
	if (rRunData[pos] != testValue)
	{
		std::string errorStr = SvUl::Format(_T("value unexpected. Run %d, value = %s, expect = %s"), index, SvUl::createStdString(testValue).c_str(), SvUl::createStdString(rRunData[pos]).c_str());
		m_rLogClass.Log(errorStr.c_str(), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadValues);
		return false;
	}
	else
	{
		std::string errorStr = SvUl::Format(_T("value expected. Run %d, value = %s"), index, SvUl::createStdString(testValue).c_str());
		m_rLogClass.Log(errorStr.c_str(), LogLevel::Information_Level3, LogType::PASS, __LINE__, strTestSetAndReadValues);
	}
	return true;
}

int TrcTester::createDataDefContainer(std::vector<std::vector<BYTE>>& rDataMemVector, SvPb::DataDefinitionList& rDataDefList)
{
	rDataMemVector.clear();
	auto* pList = rDataDefList.mutable_list();
	long memOffset {0};
	const auto& rParamDefVec = m_config.getValueObjectSet();
	for (const auto valueObject : rParamDefVec)
	{
		long memSize = valueObject.getByteSize();
		auto* pValueObjectDef = pList->Add();
		pValueObjectDef->set_objectid(getNextObjectId());
		pValueObjectDef->set_name(valueObject.m_name);
		pValueObjectDef->set_arraysize(1);
		pValueObjectDef->set_vttype(valueObject.getVarType());
		pValueObjectDef->set_memoffset(memOffset);
		memOffset += memSize;
	}

	const auto& rValueVec = m_config.getValueSet();
	for (const auto& rValues : rValueVec)
	{
		std::vector<BYTE> memData;
		memData.resize(memOffset);
		for (int i = 0; i < rParamDefVec.size(); i++)
		{
			switch (rValues[i].vt)
			{
				case VT_I4:
				case VT_INT:
					memcpy(&memData[pList->Get(i).memoffset()], &rValues[i].intVal, rParamDefVec[i].getByteSize());
					break;
				case VT_R8:
					memcpy(&memData[pList->Get(i).memoffset()], &rValues[i].dblVal, rParamDefVec[i].getByteSize());
					break;
				case VT_BSTR:
				{
					std::string tmpStr = SvUl::createStdString(rValues[i].bstrVal);
					memcpy(&memData[pList->Get(i).memoffset()], tmpStr.c_str(), rParamDefVec[i].getByteSize());
					break;
				}
				default:
					break;
			}

		}
		rDataMemVector.push_back(std::move(memData));
	}
	return memOffset;
}