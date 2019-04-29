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

#include "TriggerRecordControllerModulTest.h"

#include "TrcTester.h"
#include "LogClass.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVStatusLibrary\SVRegistry.h"
#include "TriggerRecordController\LocalConst.h"
#include "SVProtoBuf\ConverterHelper.h"

constexpr int triggerIdOffset = 100'000;
LPCSTR strTestConfig = _T("Config");
LPTSTR strTestCreateInspections = _T("createInspections");
LPTSTR strTestSetBuffers = _T("setBuffers");
LPTSTR strTestCheckBufferMaximum = _T("checkBufferMaximum");
LPTSTR strTestCreateTR2WriteAndRead = _T("createTR2WriteAndRead");
LPTSTR strTestSetAndReadImage = _T("setAndReadImage");
LPTSTR strTestSetAndReadValues = _T("setAndReadValues");
LPTSTR strTestWithMoreThreads = _T("testWithMoreThreads");
HANDLE g_resetEvent = nullptr;
HANDLE g_readyEvent = nullptr;
HANDLE g_newTrEvent = nullptr;

bool setInspections(std::vector<int> numbersOfRecords, SvTrc::ITriggerRecordControllerRW& rTrController, LogClass& rLogClass, LPCSTR testAreaStr);
void runWriterTest(std::promise<bool>&& intPromise, LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData);
void runReaderTest(std::promise<bool>&& intPromise, LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData);
SVMatroxBufferCreateStruct specifyBuffer(int sizeFactor);
SVMatroxBufferCreateStruct specifyBufferRandom();
SVMatroxBufferCreateStruct specifyBufferFromImage(MIL_ID imageId);
int calcMaxBuffer();
std::vector<MIL_ID> loadImages(const std::vector<std::string>& fileNames);
void freeImages(std::vector<MIL_ID> images);
bool areImageEqual(MIL_ID image1, MIL_ID image2);
bool checkImages(const TrcTesterConfiguration::InspectionsDef& rIPData, SvTrc::ITriggerRecordRPtr tr2R, LogClass& rLogClass, int writerRunId, int testDataId, int ipId, int runId, int triggerCount);
HANDLE createEvent(LPCTSTR eventName);

TrcTesterConfiguration::TrcTesterConfiguration(LogClass& rLogClass, SvLib::SVOINIClass iniFile)
{
	m_NumberOfRuns = iniFile.GetValueInt(_T("General"), _T("NumberOfRuns"), m_NumberOfRuns);
	m_maxTimeSetBufferPerIter = iniFile.GetValueDouble(_T("MaxTime"), _T("SetBufferPerIter"), m_maxTimeSetBufferPerIter);
	m_maxTimeCheckBufferPerBuffer = iniFile.GetValueDouble(_T("MaxTime"), _T("CheckBufferPerBuffer"), m_maxTimeCheckBufferPerBuffer);
	m_maxTimesetAndReadImage = iniFile.GetValueDouble(_T("MaxTime"), _T("SetAndReadImage"), m_maxTimesetAndReadImage);
	
	for (auto imageFileList : m_imageFileNameLists)
	{
		std::vector<MIL_ID> imageIds;
		try
		{
			imageIds = loadImages(imageFileList);
		}
		catch (CString& e)
		{
			rLogClass.Log(e, LogLevel::Error, LogType::FAIL, __LINE__, strTestConfig);
		}
		if (imageIds.size() == 0)
		{
			rLogClass.Log(_T("setAndReadImage: loadImage failed. 0 handle available!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestConfig);
		}
		else
		{
			m_imageLists.push_back(imageIds);
		}		
	}

	m_testData = {{{10, 300, {m_imageLists[0],m_imageLists[1], m_imageLists[2]}, getValueObjectSet(), m_valueSet}},
		{{10,30, {m_imageLists[1], m_imageLists[2]}, getValueObjectSet(), m_valueSet}, {10, 300, {m_imageLists[0]}, getValueObjectSet(), m_valueSet}}};
};

TrcTesterConfiguration::~TrcTesterConfiguration()
{
	for (auto imageList : m_imageLists)
	{
		freeImages(imageList);
	}
	m_imageLists.clear();
}

TrcTester::TrcTester(const TrcTesterConfiguration& rConfig, LogClass& rLogClass) :
	m_config(rConfig),
	m_rLogClass(rLogClass),
	m_TRController(SvTrc::getTriggerRecordControllerRWInstance())
{

}

TrcTester::~TrcTester()
{
	m_TRController.clearAll(); // avoids error message when MappFree() is called.
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
	return isRunOk;
}

#pragma region test methods
bool TrcTester::createInspections()
{
	std::vector<int> numbersOfRecords(m_config.getNumberOfInspections(), m_config.getNumberOfImagesPerInspection());
	for (int i = 0; i < m_config.getNoOfRepetitionsPerStep(); i++)
	{
		bool createInspectionsOk = setInspections(numbersOfRecords, m_TRController, m_rLogClass, strTestCreateInspections);
		if (!createInspectionsOk)
		{
			CString errorStr;
			errorStr.Format(_T("(%d, %d) ERROR on %d. iteration!"), m_config.getNumberOfImagesPerInspection(), m_config.getNumberOfInspections(), i);
			m_rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestCreateInspections);
			return false;
		}
	}
	m_rLogClass.Log("Successfully", LogLevel::Information_Level1, LogType::PASS, __LINE__, strTestCreateInspections);
	return true;
}

bool TrcTester::setBuffers()
{
	CString logStr;
	bool setBufferOk = true;
	auto start = std::chrono::system_clock::now();
	for (int i = 0; i < m_config.getNoOfRepetitionsPerStep(); i++)
	{
		setBufferOk = setInspectionBuffers(strTestSetBuffers);
		if (!setBufferOk)
		{
			logStr.Format(_T("setInspectionBuffers(%d, %d) ERROR on %d. iteration!"), m_config.getNumberOfImagesPerInspection(), m_config.getNumberOfInspections(), i);
			m_rLogClass.Log(logStr, LogLevel::Error, LogType::FAIL,  __LINE__, strTestSetBuffers);
			break;
		}

		setBufferOk = setIndependentBuffers(strTestSetBuffers);
		if (!setBufferOk)
		{
			logStr.Format(_T("setIndependentBuffers(%d, %d) ERROR on %d. iteration!"), m_config.getNumberOfImagesPerInspection(), m_config.getNumberOfInspections(), i);
			m_rLogClass.Log(logStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestSetBuffers);
			break;
		}
	}
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_nano = end - start;
	if (setBufferOk)
	{
		bool isTimeOK = (elapsed_nano.count() <= m_config.getMaxTimeSetBuffer());
		logStr.Format(_T("time (%f s, max time is %f)"), elapsed_nano.count(), m_config.getMaxTimeSetBuffer());
		m_rLogClass.Log(logStr, isTimeOK ? LogLevel::Information_Level1 : LogLevel::Error, isTimeOK ? LogType::PASS : LogType::FAIL, __LINE__, strTestSetBuffers);
	}
	return setBufferOk;
}

bool TrcTester::checkBufferMaximum()
{
	constexpr int numberOfRecords = 100;
	
	bool retValue = setInspections({numberOfRecords}, m_TRController, m_rLogClass, strTestCheckBufferMaximum);
	m_rLogClass.Log(_T("setInspections!"), retValue ? LogLevel::Information_Level3 : LogLevel::Error, retValue ? LogType::PASS : LogType::FAIL, __LINE__, strTestCheckBufferMaximum);
	if (!retValue)
	{
		return false;
	}

	GUID guid = GUID_NULL;
	const int maxBuffer = calcMaxBuffer();
	const int numberOfRecordsAddOne = numberOfRecords + 2;
	const int numberOfImages = maxBuffer / numberOfRecordsAddOne;
	const int numberOfAddBuffer = maxBuffer%numberOfRecordsAddOne;
	//create maximum number of possible image buffers (no exception should happen.
	try
	{
		auto start = std::chrono::system_clock::now();
		m_TRController.startResetTriggerRecordStructure(0);
		bool independentOk = m_TRController.removeAllImageBuffer();
		for (int i = 0; i < numberOfImages; i++)
		{
			UuidCreateSequential(&guid);
			independentOk &= ( 0 <= m_TRController.addOrChangeImage(guid, specifyBuffer(i / 3)));
		}
		m_TRController.addImageBuffer(guid, specifyBuffer(1), numberOfAddBuffer);
		m_TRController.finishResetTriggerRecordStructure();
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_nano = end - start;
		CString logStr;
		logStr.Format(_T("set images good case (%f s/ %f s)"), elapsed_nano.count(), m_config.getMaxTimeCheckBufferPerBuffer()*numberOfImages);
		bool isError = (!independentOk || m_config.getMaxTimeCheckBufferPerBuffer()*numberOfImages < elapsed_nano.count());
		m_rLogClass.Log(logStr, !isError ? LogLevel::Information_Level3 : LogLevel::Error, !isError ? LogType::PASS : LogType::FAIL, __LINE__, strTestCheckBufferMaximum);
		if (!independentOk)
		{
			return false;
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		m_rLogClass.logException("set images good case", rExp, __LINE__, strTestCheckBufferMaximum);
		return false;
	}
	catch (...)
	{
		m_rLogClass.Log(_T("Unknown exception: set images good case!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestCheckBufferMaximum);
		return false;
	}

	try
	{
		m_TRController.startResetTriggerRecordStructure();
		//m_TRController.removeAllImageBuffer();
		UuidCreateSequential(&guid);
		m_TRController.addImageBuffer(guid, specifyBuffer(3), 1);
		m_TRController.finishResetTriggerRecordStructure();

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
	std::random_device rd;
	std::uniform_int_distribution<int> dist(1, SvTrc::cMaxTriggerRecords);
	constexpr int numberOfInspection = 2;
	std::vector<int> numbersOfRecords = {dist(rd), dist(rd)};
	bool retValue = setInspections(numbersOfRecords, m_TRController, m_rLogClass, strTestCreateTR2WriteAndRead);
	if (!retValue)
	{
		m_rLogClass.Log(_T("setInspections!"),  LogLevel::Error, LogType::FAIL, __LINE__, strTestCreateTR2WriteAndRead);
		return false;
	}
	else
	{
		CString errorStr;
		errorStr.Format(_T("setInspections with the TR-size of %d/%d"), numbersOfRecords[0], numbersOfRecords[1]);
		m_rLogClass.Log(errorStr, LogLevel::Information_Level3, LogType::PASS, __LINE__, strTestCreateTR2WriteAndRead);
	}

	try
	{
		for (int i = 0; i < numberOfInspection; i++)
		{
			m_TRController.startResetTriggerRecordStructure(i);
			GUID guid = GUID_NULL;
			UuidCreateSequential(&guid);
			retValue = (0 <= m_TRController.addOrChangeImage(guid, specifyBufferRandom()));
			m_TRController.finishResetTriggerRecordStructure();
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

	const int maxRecords = *std::max_element(numbersOfRecords.begin(), numbersOfRecords.end());
	const int minRecords = *std::min_element(numbersOfRecords.begin(), numbersOfRecords.end());
	{
		auto tr2W = m_TRController.createTriggerRecordObjectToWrite(0);
		if (nullptr != tr2W)
		{
			auto tr2R = m_TRController.createTriggerRecordObject(0, tr2W->getId());
			if (nullptr != tr2R)
			{
				m_rLogClass.Log(_T("could create read version of TR, but write version still open"), LogLevel::Error, LogType::FAIL, __LINE__, strTestCreateTR2WriteAndRead);
			}
		}
	}

	std::array<std::vector<SvTrc::ITriggerRecordRPtr>, numberOfInspection> readTRVector;
	for (int i = 0; i < maxRecords+10; i++)
	{
		for (int j = 0; j < numberOfInspection; j++)
		{
			{ //scope for tr
				auto tr = m_TRController.createTriggerRecordObjectToWrite(j);
				if (nullptr == tr)
				{
					if (readTRVector[j].size() <= numbersOfRecords[j])
					{
						CString errorStr;
						errorStr.Format(_T("createTriggerRecordObjectToWrite in insp %d run %d"), j, i);
						m_rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestCreateTR2WriteAndRead);
					}
					else
					{
						CString errorStr;
						errorStr.Format(_T("createTriggerRecordObjectToWrite failed ( in insp %d run %d) as excepted because to many read version logged."), j, i);
						m_rLogClass.Log(errorStr, LogLevel::Information_Level3, LogType::PASS, __LINE__, strTestCreateTR2WriteAndRead);
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
					if (readTRVector[j].size() > numbersOfRecords[j] + 4) //add some buffer, because the TRC have a few more TR than required.
					{
						CString errorStr;
						errorStr.Format(_T("createTriggerRecordObjectToWrite possible(insp %d run %d), but failed expected because to many read-version logged."), j, i);
						m_rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestCreateTR2WriteAndRead);
					}
				}
			}
			int id = m_TRController.getLastTRId(j);
			auto tr2R = m_TRController.createTriggerRecordObject(j, id);
			if (nullptr == tr2R)
			{
				CString errorStr;
				errorStr.Format(_T("could not create read version of TR (id%d) in inspection %d"), id, j);
				m_rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestCreateTR2WriteAndRead);
			}
			readTRVector[j].emplace_back(tr2R);
		}
	}

	m_rLogClass.Log("Successfully", LogLevel::Information_Level1, LogType::PASS, __LINE__, strTestCreateTR2WriteAndRead);
	return retValue;
}

bool TrcTester::setAndReadImage()
{
	bool retValue = setInspections({12}, m_TRController, m_rLogClass, strTestSetAndReadImage);
	if (!retValue)
	{
		m_rLogClass.Log(_T("setInspections!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadImage);
		return false;
	}

	//init triggerRecord
	const auto& imageIds = m_config.getImageLists()[0];
	auto bufferStruct = specifyBufferFromImage(imageIds[0]);
	GUID imageGuid = GUID_NULL;
	UuidCreateSequential(&imageGuid);
	try
	{
		m_TRController.startResetTriggerRecordStructure(0);
		m_TRController.removeAllImageBuffer();
		GUID guid = GUID_NULL;
		UuidCreateSequential(&guid);
		m_TRController.addImageBuffer(guid, bufferStruct, 1);
		
		retValue = (0 <= m_TRController.addOrChangeImage(imageGuid, bufferStruct));
		m_TRController.finishResetTriggerRecordStructure();
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

	//constexpr int numberOfRuns = 100;
	const int numberOfRuns = m_config.getNoOfRepetitionsPerStep();
	auto start = std::chrono::system_clock::now();
	for (int i = 0; i < numberOfRuns; i++)
	{
		//write image to buffer
		auto imageHandle = m_TRController.getImageBuffer(bufferStruct);
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
			auto tr2W = m_TRController.createTriggerRecordObjectToWrite(0);
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
		int id = m_TRController.getLastTRId(0);
		auto tr2R = m_TRController.createTriggerRecordObject(0, id);
		if (nullptr == tr2R)
		{
			CString errorStr;
			errorStr.Format(_T("could not create read version of TR (id%d) in run %d"), id, i);
			m_rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadImage);
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
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_nano = end - start;

	CString logStr;
	logStr.Format(_T("(%f s/ %f s)"), elapsed_nano.count(), numberOfRuns*m_config.getMaxTimesetAndReadImage());
	bool isTimeOk = elapsed_nano.count() < numberOfRuns*m_config.getMaxTimesetAndReadImage();
	m_rLogClass.Log(logStr, isTimeOk ? LogLevel::Information_Level1 : LogLevel::Error, isTimeOk ? LogType::PASS : LogType::FAIL, __LINE__, strTestSetAndReadImage);
	return true;
}

bool TrcTester::setAndReadValues()
{
	bool retValue = setInspections({21}, m_TRController, m_rLogClass, strTestSetAndReadValues);
	if (!retValue)
	{
		m_rLogClass.Log(_T("setInspections!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadValues);
		return false;
	}

	//create ValueList
	std::vector<_variant_t> valueList;
	SvPb::DataDefinitionList dataDefList;
	auto* pList = dataDefList.mutable_list();
	for (const auto valueObject : m_config.getValueObjectSet())
	{
		auto* pValueObjectDef = pList->Add();
		std::string uniqueIdBytes;
		GUID guid = GUID_NULL;
		UuidCreateSequential(&guid);
		SvPb::SetGuidInProtoBytes(&uniqueIdBytes, guid);
		pValueObjectDef->set_guidid(uniqueIdBytes);
		pValueObjectDef->set_name(valueObject.first);
		valueList.push_back(valueObject.second);
	}

	//init triggerRecord
	try
	{
		auto tmpDefList = dataDefList;
		auto tmpValueList = valueList;
		m_TRController.startResetTriggerRecordStructure(0);
		m_TRController.changeDataDef(std::move(tmpDefList), std::move(tmpValueList), 0);
		m_TRController.finishResetTriggerRecordStructure();
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

	constexpr int numberOfRuns = 10;
	auto start = std::chrono::system_clock::now();
	for (int i = 0; i < numberOfRuns; i++)
	{
		auto runData = m_config.getValueSet()[i%m_config.getValueSet().size()];
		{//scope for tr2W
		 //get TR and set new image
			auto tr2W = m_TRController.createTriggerRecordObjectToWrite(0);
			if (nullptr == tr2W)
			{
				m_rLogClass.Log(_T("createTriggerRecordObjectToWrite return nullptr!"), LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadValues);
				return false;
			}
			try
			{
				auto tmpData = runData;
				tr2W->writeValueData(std::move(tmpData));
			}
			catch (const SvStl::MessageContainer& rExp)
			{
				m_rLogClass.logException("", rExp, __LINE__, strTestSetAndReadValues);
				return false;
			}
		}
		int id = m_TRController.getLastTRId(0);
		auto tr2R = m_TRController.createTriggerRecordObject(0, id);
		if (nullptr == tr2R)
		{
			CString errorStr;
			errorStr.Format(_T("could not create read version of TR (id%d) in run %d"), id, i);
			m_rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadValues);
			continue;
		}
		
		int pos = i%dataDefList.list_size();
		variant_t testValue = tr2R->getDataValue(SvPb::GetGuidFromProtoBytes(dataDefList.list(pos).guidid()));
		if (runData[pos] != testValue)
		{
			CString errorStr;
			errorStr.Format(_T("value unexpected. Run %d, value = %s, expect = %s"), i, SvUl::createStdString(testValue).c_str(), SvUl::createStdString(runData[pos]).c_str());
			m_rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestSetAndReadValues);
			return false;
		}
		else
		{
			CString errorStr;
			errorStr.Format(_T("value expected. Run %d, value = %s"), i, SvUl::createStdString(testValue).c_str());
			m_rLogClass.Log(errorStr, LogLevel::Information_Level3, LogType::PASS, __LINE__, strTestSetAndReadValues);
		}
	}
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_nano = end - start;

	CString logStr;
	logStr.Format(_T("(%f s/ %f s)"), elapsed_nano.count(), numberOfRuns*m_config.getMaxTimesetAndReadImage());
	bool isTimeOk = elapsed_nano.count() < numberOfRuns*m_config.getMaxTimesetAndReadImage();
	m_rLogClass.Log(logStr, isTimeOk ? LogLevel::Information_Level1 : LogLevel::Error, isTimeOk ? LogType::PASS : LogType::FAIL, __LINE__, strTestSetAndReadValues);
	return true;
}

bool TrcTester::testWithMoreThreads()
{
	std::promise<bool> writerPromise;
	std::promise<bool> readerPromise;
	std::future<bool> writerResult = writerPromise.get_future();
	std::future<bool> readerResult = readerPromise.get_future();
	std::thread writerThread(runWriterTest, std::move(writerPromise), std::ref(m_rLogClass), 100, m_config.getTestData());
	std::thread readerThread(runReaderTest, std::move(readerPromise), std::ref(m_rLogClass), 100, m_config.getTestData());
	bool retValue = writerResult.get() && readerResult.get();
	{
		m_rLogClass.Log(_T("End test"), retValue ? LogLevel::Information_Level1 : LogLevel::Error, retValue ? LogType::PASS : LogType::FAIL, __LINE__, strTestWithMoreThreads);
	}
	writerThread.join();
	readerThread.join();
	return retValue;
}
#pragma endregion test methods

bool TrcTester::setInspectionBuffers(LPCSTR testAreaStr)
{
	GUID guid = GUID_NULL;

	for (int i = 0; i < m_config.getNumberOfInspections(); i++)
	{
		try
		{
			m_TRController.startResetTriggerRecordStructure(i);

			for (int j = 0; j < m_config.getNumberOfBuffersPerInspection(); j++)
			{
				UuidCreateSequential(&guid);

				m_TRController.addOrChangeImage(guid, specifyBuffer(1 + j));
			}

			m_TRController.finishResetTriggerRecordStructure(); //ABXX die Änderung der in Arbeit befindlichen Inspektion wird abgeschlossen
		}
		catch (const SvStl::MessageContainer& rExp)
		{
			m_rLogClass.logException("Parts of setInspectionBuffer", rExp, __LINE__, testAreaStr);
			return false;
		}
		catch (...)
		{
			CString errorStr;
			errorStr.Format(_T("Parts of setInspectionBuffer throw an exception: #%d!"), i);
			m_rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, testAreaStr);
			return false;
		}
	}

	return true;
}


bool TrcTester::setIndependentBuffers(LPCSTR testAreaStr)
{
	GUID guid = GUID_NULL;

	try
	{
		m_TRController.startResetTriggerRecordStructure();
		bool independentOk = true;
		for (int i = 0; i < m_config.getNumberOfBuffersPerInspection(); i++)
		{
			UuidCreateSequential(&guid);
			independentOk &= m_TRController.removeAllImageBuffer();
			independentOk &= m_TRController.addImageBuffer(guid, specifyBuffer(m_config.getNumberOfIndependentBuffers()), 1);
		}
		m_TRController.finishResetTriggerRecordStructure();

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

bool setInspections(std::vector<int> numbersOfRecords, SvTrc::ITriggerRecordControllerRW& rTrController, LogClass& rLogClass, LPCSTR testAreaStr)
{
	SvPb::InspectionList inspList;
	for (int numberOfRecords : numbersOfRecords)
	{
		auto* pInspStruct = inspList.add_list();
		if (nullptr == pInspStruct)
		{
			rLogClass.Log(_T("createTR2WriteAndRead: Could not create inspection!"), LogLevel::Error, LogType::FAIL, __LINE__, testAreaStr);
			return false;
		}
		pInspStruct->set_numberofrecords(numberOfRecords);
	}
	return rTrController.setInspections(inspList);
}

void runWriterTest(std::promise<bool>&& intPromise, LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData)
{
	bool retValue = true;
	GUID guid = GUID_NULL;
	CString logStr;
	std::random_device rd;
	auto& rTrController = SvTrc::getTriggerRecordControllerRWInstance();

	for (int testDataId = 0; testDataId < rTestData.size(); testDataId++)
	{
		const auto& rInspectionsData = rTestData[testDataId];

		//setInspections
		std::vector<int> numbersOfRecords;
		CString inspectionNumberStr;
		for (int j = 0; j < rInspectionsData.size(); j++)
		{
			std::uniform_int_distribution<int> dist(rInspectionsData[j].m_minRecordSize, rInspectionsData[j].m_maxRecordSize);
			numbersOfRecords.emplace_back(dist(rd));
			inspectionNumberStr.Format("%s%d, ", inspectionNumberStr, numbersOfRecords[j]);
		}
		retValue = setInspections(numbersOfRecords, rTrController, rLogClass, strTestWithMoreThreads);
		logStr.Format(_T("Writer Tests (%d): CreateInspections(%s)"), testDataId, inspectionNumberStr);
		
		if (!retValue)
		{
			rLogClass.Log(logStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
			break;
		}
		else
		{
			rLogClass.Log(logStr, LogLevel::Information_Level3, LogType::PASS, __LINE__, strTestWithMoreThreads);
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
				logStr.Format(_T("Writer Tests (%d): resetTriggerRecordStructure"), testDataId);
				rLogClass.logException(logStr, rExp, __LINE__, strTestWithMoreThreads);
				retValue = false;
				break;
			}
			catch (...)
			{
				CString errorStr;
				errorStr.Format(_T("Writer Tests (%d): resetTriggerRecordStructure throw an exception"), testDataId);
				rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
				retValue = false;
				break;
			}
		}
		if (!retValue)
		{
			break;
		}

		std::this_thread::sleep_for(std::chrono::duration<int, std::micro>(500));

		//the run
		auto start = std::chrono::system_clock::now();
		std::vector<SvTrc::ITriggerRecordRPtr> lastTRList(rInspectionsData.size());
		for (int runId = 0; runId < numberOfRuns; runId++)
		{
			for (int ipId = 0; ipId < rInspectionsData.size(); ipId++)
			{
				const auto& rIPData = rInspectionsData[ipId];
				auto tr2W = rTrController.createTriggerRecordObjectToWrite(ipId);
				if (nullptr == tr2W)
				{
					CString errorStr;
					errorStr.Format(_T("Writer Tests (%d): createTriggerRecordObjectToWrite(%d) return nullptr by run %d!"), testDataId, ipId, runId);
					rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
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
					CString errorStr;
					errorStr.Format(_T("Writer Tests (%d): getImageBuffer(%d) return nullptr by run %d!"), testDataId, ipId, runId);
					rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
					retValue = false;
					continue;
				}
				else
				{
					const auto imageId = rIPData.m_imageFilesList[0][runId%rIPData.m_imageFilesList[0].size()];
					MbufCopy(imageId, cameraImageHandle->getHandle()->GetBuffer().GetIdentifier());
					if (!areImageEqual(cameraImageHandle->getHandle()->GetBuffer().GetIdentifier(), imageId))
					{
						CString errorStr;
						errorStr.Format(_T("Writer Tests (%d): ImageCopy(%d:0) failed. Images are not equal by run %d!"), testDataId, ipId, runId);
						rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
						retValue = false;
						continue;
					}
					try
					{
						tr2W->setImage(0, cameraImageHandle->getBufferPos());
					}
					catch (const SvStl::MessageContainer& rExp)
					{
						CString errorStr;
						errorStr.Format(_T("Writer Tests (%d): setImage(%d) by run %d:"), testDataId, ipId, runId);
						rLogClass.logException(errorStr, rExp, __LINE__, strTestWithMoreThreads);
						retValue = false;
						continue;
					}		
					if (0 == runId)
					{//write special buffer only at the first run
						auto lastImageHandle = tr2W->createNewImageHandle(static_cast<int>(rIPData.m_imageFilesList.size()));
						if (nullptr == lastImageHandle || lastImageHandle->isEmpty())
						{
							CString errorStr;
							errorStr.Format(_T("Writer Tests (%d): createNewImageHandle(%d:last) return nullptr by run %d!"), testDataId, ipId, runId);
							rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
							retValue = false;
							continue;
						}
						else
						{
							MbufCopy(imageId, lastImageHandle->getHandle()->GetBuffer().GetIdentifier());
							if (!areImageEqual(lastImageHandle->getHandle()->GetBuffer().GetIdentifier(), imageId))
							{
								CString errorStr;
								errorStr.Format(_T("Writer Tests (%d): ImageCopy(%d:last) failed. Images are not equal by run %d!"), testDataId, ipId, runId);
								rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
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
						CString errorStr;
						errorStr.Format(_T("Writer Tests (%d): createNewImageHandle(%d:%d) return nullptr by run %d!"), testDataId, ipId, imagePos, runId);
						rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
						retValue = false;
						continue;
					}
					else
					{
						MbufCopy(imageId, imageHandle->getHandle()->GetBuffer().GetIdentifier());
						if (!areImageEqual(imageHandle->getHandle()->GetBuffer().GetIdentifier(), imageId))
						{
							CString errorStr;
							errorStr.Format(_T("Writer Tests (%d): ImageCopy(%d:%d) failed. Images are not equal by run %d!"), testDataId, ipId, imagePos, runId);
							rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
							retValue = false;
							continue;
						}
					}
				}
				lastTRList[ipId] = rTrController.closeWriteAndOpenReadTriggerRecordObject(tr2W);
			}
			
			std::this_thread::sleep_for(std::chrono::duration<int, std::micro>(5500));
		}

		logStr.Format(_T("Writer Tests for run %d finished"), testDataId);
		rLogClass.LogText(logStr, LogLevel::Information_Level3, LogType::PASS);
		
		//std::this_thread::sleep_for(std::chrono::duration<int, std::micro>(1));
	}

	intPromise.set_value(retValue);
	rLogClass.LogText("Finished runWriterTest", LogLevel::Information_Level1, LogType::PASS);
}

void OnResetTRC()
{
	::SetEvent(g_resetEvent);
}

void OnReadyTRC()
{
	::SetEvent(g_readyEvent);
}

std::set<int> g_newTrIpSet;
std::mutex g_newTrIpSetMutex;
void OnNewTr(int ipId, int)
{
	std::lock_guard<std::mutex> lock(g_newTrIpSetMutex);
	g_newTrIpSet.emplace(ipId);
	::SetEvent(g_newTrEvent);
}

void runReaderTest(std::promise<bool>&& intPromise, LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData)
{
	bool retValue = true;
	const LPCTSTR strResetEvent = _T("Global\\TRCModul_Reset");
	g_resetEvent = createEvent(strResetEvent);
	const LPCTSTR strReadyEvent = _T("Global\\TRCModul_Ready");
	g_readyEvent = createEvent(strReadyEvent);
	const LPCTSTR strNewTrEvent = _T("Global\\TRCModul_NewTrid");
	g_newTrEvent = createEvent(strNewTrEvent);
	if (nullptr != g_resetEvent && nullptr != g_readyEvent && nullptr != g_newTrEvent)
	{
		auto& rTrController = SvTrc::getTriggerRecordControllerRInstance();
		int resetCallbackHandle = rTrController.registerResetCallback(OnResetTRC);
		int readyCallbackHandle = rTrController.registerReadyCallback(OnReadyTRC);
		int newTrCallBackHandle = rTrController.registerNewTrCallback(OnNewTr);
		int lastTestDataId = 0;
		std::vector<int> lastTrIds;

		int numberOfRunsComplete = numberOfRuns * static_cast<int>(rTestData.size());
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

			auto inspectionSize = rTrController.getInspections().list_size();
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
				waitValue = WaitForMultipleObjects(2, hChange, false, 1000);
				if (WAIT_OBJECT_0 == waitValue)
				{
					CString logStr;
					logStr.Format(_T("Reader Tests: Reset TRC after runId %d"), runId);
					rLogClass.Log(logStr, LogLevel::Information_Level3, (runId+1 == numberOfRuns*inspectionSize) ? LogType::PASS : LogType::FAIL, __LINE__, strTestWithMoreThreads);
					break;
				}

				std::lock_guard<std::mutex> lock(g_newTrIpSetMutex);
				if (0 == g_newTrIpSet.size())
				{
					if (runId+1 < numberOfRuns*inspectionSize)
					{
						CString logStr;
						logStr.Format(_T("Reader Tests: No new TR after runId %d"), runId);
						rLogClass.Log(logStr, LogLevel::Information_Level3, LogType::FAIL, __LINE__, strTestWithMoreThreads);
					}
					else
					{
						CString logStr;
						logStr.Format(_T("Reader Tests: Finished run after runId %d"), runId);
						rLogClass.Log(logStr, LogLevel::Information_Level3, LogType::PASS, __LINE__, strTestWithMoreThreads);
					}
					break;
				}
				for (int ipId : g_newTrIpSet)
				{
					runId++;
					int trId = rTrController.getLastTRId(ipId);

					if (lastTrIds[ipId] == trId)
					{
						CString logStr;
						logStr.Format(_T("Reader Tests: (%d) check trId %d again by run %d"), ipId, trId, runId);
						rLogClass.Log(logStr, LogLevel::Information_Level3, LogType::WARN, __LINE__, strTestWithMoreThreads);
					}
					lastTrIds[ipId] = trId;

					auto tr2R = rTrController.createTriggerRecordObject(ipId, trId);
					if (nullptr == tr2R)
					{
						CString errorStr;
						errorStr.Format(_T("Reader Tests: createTriggerRecordObject(%d, %d) return nullptr by run %d!"), ipId, trId, runId);
						rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
						retValue = false;
						std::chrono::duration<int, std::micro> t(2);
						std::this_thread::sleep_for(t);
						continue;
					}
					int triggerCount = tr2R->getTriggerData().m_TriggerCount;
					int testDataId = triggerCount / triggerIdOffset;
					int writerRunId = triggerCount % triggerIdOffset;
					if (0 > triggerCount || rTestData.size() <= testDataId)
					{
						CString errorStr;
						errorStr.Format(_T("Reader Tests: (%d) triggerCount (%d) do not fit: , called testDataId %d, max testDataId %d by run %d"), ipId, triggerCount, testDataId, static_cast<int>(rTestData.size()) - 1, runId);
						rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
						retValue = false;
						std::chrono::duration<int, std::micro> t(1);
						std::this_thread::sleep_for(t);
						continue;
					}
					//else
					//{
					//	CString logStr;
					//	logStr.Format(_T("Reader Tests(%d): createTriggerRecordObject(%d) triggerCount %d by run %d"), testDataId, ipId, triggerCount, runId);
					//	rLogClass.Log(logStr, LogLevel::Information_Level3, LogType::PASS, __LINE__, strTestWithMoreThreads);
					//}

					//if (testDataId != lastTestDataId)
					//{
					//	CString logStr;
					//	logStr.Format(_T("Reader Tests(%d): Moved to this testDataId with run %d"), testDataId, runId);
					//	rLogClass.Log(logStr, LogLevel::Information_Level3, LogType::PASS, __LINE__, strTestWithMoreThreads);
					//	lastTestDataId = testDataId;
					//}

					retValue = checkImages(rTestData[testDataId][ipId], tr2R, rLogClass, writerRunId, testDataId, ipId, runId, triggerCount) && retValue;
				}
				g_newTrIpSet.clear();
			}
			//std::this_thread::sleep_for(std::chrono::duration<int, std::nano>(1));
		}

		rTrController.unregisterResetCallback(resetCallbackHandle);
		rTrController.unregisterReadyCallback(readyCallbackHandle);
		rTrController.unregisterNewTrCallback(newTrCallBackHandle);
	}
	else
	{
		rLogClass.Log(_T("Set Events for ReaderTest failed"), LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
		retValue = false;
	}
		
	intPromise.set_value(retValue);
	rLogClass.LogText("Finished runReaderTest", LogLevel::Information_Level1, LogType::PASS);
}

constexpr int handleForReset = 1000;
int calcMaxBuffer()
{
	SVRegistryClass reg(_T(R"(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\\Windows\)"));
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

SVMatroxBufferCreateStruct specifyBuffer(int sizeFactor)
{
	if (sizeFactor < 1)
	{
		sizeFactor = 1;
	}
	SVMatroxBufferCreateStruct bufferStruct;
	int sizeX = sizeFactor * 64;
	int sizeY = sizeFactor * 48;
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

SVMatroxBufferCreateStruct specifyBufferFromImage(MIL_ID imageId)
{
	SVMatroxBufferCreateStruct bufferStruct;
	bufferStruct.m_lSizeX = static_cast<long>(MbufInquire(imageId, M_SIZE_X, M_NULL));
	bufferStruct.m_lSizeY = static_cast<long>(MbufInquire(imageId, M_SIZE_Y, M_NULL));
	bufferStruct.m_eAttribute = SVBufAttImageProcDib;
	bufferStruct.m_eType = SV8BitUnsigned;
	return bufferStruct;
}

std::vector<MIL_ID> loadImages(const std::vector<std::string>& fileNames)
{
	std::vector<MIL_ID> retValue(fileNames.size());
	for (int i=0; i<fileNames.size(); i++)
	{
		MbufRestore(fileNames[i], M_DEFAULT_HOST, &retValue[i]);
		if (M_NULL == retValue[i])
		{
			retValue.clear();
			CString errorStr;
			errorStr.Format(_T("LoadImages failed by image %d with \"%s\""), i, fileNames[i].c_str());
			throw errorStr;
		}
	}
	return retValue;
}

void freeImages(std::vector<MIL_ID> images)
{
	for (MIL_ID id : images)
	{
		MbufFree(id);
	}
}

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
				CString errorStr;
				errorStr.Format(_T("Reader Tests (%d): getImage(%d:%d) return nullptr by run %d!"), testDataId, ipId, imagePos, runId);
				rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
				retValue = false;
				continue;
			}
			else
			{
				CString errorStr;
				errorStr.Format(_T("Reader Tests (%d): getImage(%d:%d) Tr is not longer up to time by run %d!"), testDataId, ipId, imagePos, runId);
				rLogClass.Log(errorStr, LogLevel::Error, LogType::WARN, __LINE__, strTestWithMoreThreads);
				return retValue;
			}
		}
		else
		{
			if (!areImageEqual(imageHandle->getHandle()->GetBuffer().GetIdentifier(), imageId))
			{
				CString errorStr;
				errorStr.Format(_T("Reader Tests (%d): ImageCopy(%d:%d) failed. TriggerCount = %d. Images are not equal by run %d!"), testDataId, ipId, imagePos, triggerCount, runId);
				rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
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
				CString errorStr;
				errorStr.Format(_T("Reader Tests (%d): getImage(%d:last) (TriggerCount = %d) return nullptr by run %d. Is TR upToTime: %d!"), testDataId, ipId, triggerCount, runId, tr2R->isObjectUpToTime());
				rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
				retValue = false;
			}
			else
			{
				CString errorStr;
				errorStr.Format(_T("Reader Tests (%d): getImage(%d:last) Tr is not longer up to time by run %d!"), testDataId, ipId, runId);
				rLogClass.Log(errorStr, LogLevel::Error, LogType::WARN, __LINE__, strTestWithMoreThreads);
				return retValue;
			}
		}
		else
		{
			if (!areImageEqual(imageHandle->getHandle()->GetBuffer().GetIdentifier(), imageId))
			{
				CString errorStr;
				errorStr.Format(_T("Reader Tests (%d): ImageCopy(%d:last) failed. TriggerCount = %d. Images are not equal by run %d!"), testDataId, ipId, triggerCount, runId);
				rLogClass.Log(errorStr, LogLevel::Error, LogType::FAIL, __LINE__, strTestWithMoreThreads);
				retValue = false;
			}
		}
	}
	return retValue;
}

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