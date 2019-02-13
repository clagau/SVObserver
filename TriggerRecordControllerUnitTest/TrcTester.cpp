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

#include "TriggerRecordControllerUnitTest.h"

#include "TrcTester.h"
#include "LogClass.h"
#include "SVStatusLibrary\MessageContainer.h"
#include "SVStatusLibrary\SVRegistry.h"
#include "TriggerRecordController\LocalConst.h"
#include "SVProtoBuf\ConverterHelper.h"

constexpr int triggerIdOffset = 100'000;

bool setInspections(std::vector<int> numbersOfRecords, SvTrc::ITriggerRecordControllerRW& rTrController, LogClass& rLogClass);
void runWriterTest(std::promise<bool>&& intPromise, LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData);
void runReaderTest(std::promise<bool>&& intPromise, LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData);
SVMatroxBufferCreateStruct specifyBuffer(int sizeFactor);
SVMatroxBufferCreateStruct specifyBufferRandom();
SVMatroxBufferCreateStruct specifyBufferFromImage(MIL_ID imageId);
int calcMaxBuffer();
std::vector<MIL_ID> loadImages(const std::vector<std::string> fileNames);
void freeImages(std::vector<MIL_ID> images);
bool areImageEqual(MIL_ID image1, MIL_ID image2);
int getLastTrId(int ipId, SvTrc::ITriggerRecordControllerR& rTrController);
bool checkImages(const TrcTesterConfiguration::InspectionsDef& rIPData, SvTrc::ITriggerRecordRPtr tr2R, LogClass& rLogClass, int writerRunId, int testDataId, int ipId, int runId, int triggerCount);

TrcTesterConfiguration::TrcTesterConfiguration(LogClass& rLogClass) 
{
	for (auto imageFileList : m_imageFileNameLists)
	{
		std::vector<MIL_ID> imageIds;
		try
		{
			imageIds = loadImages(imageFileList);
		}
		catch (CString& e)
		{
			rLogClass.LogText(e, LogLevel::Error, LogType::FAIL);
		}
		if (imageIds.size() <= 0)
		{
			rLogClass.LogText(_T("setAndReadImage: loadImage failed. 0 handle available!"), LogLevel::Error, LogType::FAIL);
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
		bool createInspectionsOk = setInspections(numbersOfRecords, m_TRController, m_rLogClass);
		if (!createInspectionsOk)
		{
			CString errorStr;
			errorStr.Format(_T("CreateInspections(%d, %d) ERROR on %d. iteration!"), m_config.getNumberOfImagesPerInspection(), m_config.getNumberOfInspections(), i);
			m_rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
			return false;
		}
	}
	m_rLogClass.LogText("CreateInspection", LogLevel::Information_Level1, LogType::PASS);
	return true;
}

bool TrcTester::setBuffers()
{
	CString logStr;
	bool setBufferOk = true;
	auto start = std::chrono::system_clock::now();
	for (int i = 0; i < m_config.getNoOfRepetitionsPerStep(); i++)
	{
		setBufferOk = setInspectionBuffers();
		if (!setBufferOk)
		{
			logStr.Format(_T("setInspectionBuffers(%d, %d) ERROR on %d. iteration!"), m_config.getNumberOfImagesPerInspection(), m_config.getNumberOfInspections(), i);
			m_rLogClass.LogText(logStr, LogLevel::Error, LogType::FAIL);
			break;
		}

		setBufferOk = setIndependentBuffers();
		if (!setBufferOk)
		{
			logStr.Format(_T("setIndependentBuffers(%d, %d) ERROR on %d. iteration!"), m_config.getNumberOfImagesPerInspection(), m_config.getNumberOfInspections(), i);
			m_rLogClass.LogText(logStr, LogLevel::Error, LogType::FAIL);
			break;
		}
	}
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_nano = end - start;
	if (setBufferOk)
	{
		bool isTimeOK = (elapsed_nano.count() <= m_config.getMaxTimeSetBuffer());
		logStr.Format(_T("setBuffers - time (%f s, max time is %f)"), elapsed_nano.count(), m_config.getMaxTimeSetBuffer());
		m_rLogClass.LogText(logStr, isTimeOK ? LogLevel::Information_Level1 : LogLevel::Error, isTimeOK ? LogType::PASS : LogType::FAIL);
	}
	return setBufferOk;
}

bool TrcTester::checkBufferMaximum()
{
	constexpr int numberOfRecords = 100;
	
	bool retValue = setInspections({numberOfRecords}, m_TRController, m_rLogClass);
	m_rLogClass.LogText(_T("checkBufferMaximum: setInspections!"), retValue ? LogLevel::Information_Level3 : LogLevel::Error, retValue ? LogType::PASS : LogType::FAIL);
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
		logStr.Format(_T("checkBufferMaximum: set images good case (%f s/ %f s)"), elapsed_nano.count(), m_config.getMaxTimeCheckBufferMaximumPerBuffer()*numberOfImages);
		bool isError = (!independentOk || m_config.getMaxTimeCheckBufferMaximumPerBuffer()*numberOfImages < elapsed_nano.count());
		m_rLogClass.LogText(logStr, !isError ? LogLevel::Information_Level3 : LogLevel::Error, !isError ? LogType::PASS : LogType::FAIL);
		if (!independentOk)
		{
			return false;
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		m_rLogClass.logException("checkBufferMaximum: set images good case", rExp);
		return false;
	}
	catch (...)
	{
		m_rLogClass.LogText(_T("Unknown exception when checkBufferMaximum: set images good case!"), LogLevel::Error, LogType::FAIL);
		return false;
	}

	try
	{
		m_TRController.startResetTriggerRecordStructure();
		//m_TRController.removeAllImageBuffer();
		UuidCreateSequential(&guid);
		m_TRController.addImageBuffer(guid, specifyBuffer(3), 1);
		m_TRController.finishResetTriggerRecordStructure();

		m_rLogClass.LogText(_T("checkBufferMaximum: set images too many buffers, but no exception"), LogLevel::Error, LogType::FAIL);
		return false;
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		m_rLogClass.logException("checkBufferMaximum: set images too many buffers", rExp, LogLevel::Information_Level3, LogType::PASS);
	}
	catch (...)
	{
		m_rLogClass.LogText(_T("Unknown exception when checkBufferMaximum: set images too many buffers!"), LogLevel::Error, LogType::FAIL);
		return false;
	}

	m_rLogClass.LogText("checkBufferMaximum", LogLevel::Information_Level1, LogType::PASS);
	return true;
}

bool TrcTester::createTR2WriteAndRead()
{
	std::random_device rd;
	std::uniform_int_distribution<int> dist(1, SvTrc::cMaxTriggerRecords);
	constexpr int numberOfInspection = 2;
	std::vector<int> numbersOfRecords = {dist(rd), dist(rd)};
	bool retValue = setInspections(numbersOfRecords, m_TRController, m_rLogClass);
	if (!retValue)
	{
		m_rLogClass.LogText(_T("createTR2WriteAndRead: setInspections!"),  LogLevel::Error, LogType::FAIL);
		return false;
	}
	else
	{
		CString errorStr;
		errorStr.Format(_T("createTR2WriteAndRead: setInspections with the TR-size of %d/%d"), numbersOfRecords[0], numbersOfRecords[1]);
		m_rLogClass.LogText(errorStr, LogLevel::Information_Level3, LogType::PASS);
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
				m_rLogClass.LogText(_T("createTR2WriteAndRead: init Inspection"), LogLevel::Error, LogType::FAIL);
				return false;
			}
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		m_rLogClass.logException("createTR2WriteAndRead: init Inspection", rExp);
		return false;
	}
	catch (...)
	{
		m_rLogClass.LogText(_T("Unknown exception when createTR2WriteAndRead: init Inspection!"), LogLevel::Error, LogType::FAIL);
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
				m_rLogClass.LogText(_T("createTR2WriteAndRead: could create read version of TR, but write version still open"), LogLevel::Error, LogType::FAIL);
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
						errorStr.Format(_T("createTR2WriteAndRead: createTriggerRecordObjectToWrite in insp %d run %d"), j, i);
						m_rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
					}
					else
					{
						CString errorStr;
						errorStr.Format(_T("createTR2WriteAndRead: createTriggerRecordObjectToWrite failed ( in insp %d run %d) as excepted because to many read version logged."), j, i);
						m_rLogClass.LogText(errorStr, LogLevel::Information_Level3, LogType::PASS);
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
						errorStr.Format(_T("createTR2WriteAndRead: createTriggerRecordObjectToWrite possible(insp %d run %d), but failed expected because to many read-version logged."), j, i);
						m_rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
					}
				}
			}
			int id = m_TRController.getLastTRId(j);
			auto tr2R = m_TRController.createTriggerRecordObject(j, id);
			if (nullptr == tr2R)
			{
				CString errorStr;
				errorStr.Format(_T("createTR2WriteAndRead: could not create read version of TR (id%d) in inspection %d"), id, j);
				m_rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
			}
			readTRVector[j].emplace_back(tr2R);
		}
	}

	m_rLogClass.LogText("createTR2WriteAndRead", LogLevel::Information_Level1, LogType::PASS);
	return retValue;
}

bool TrcTester::setAndReadImage()
{
	bool retValue = setInspections({12}, m_TRController, m_rLogClass);
	if (!retValue)
	{
		m_rLogClass.LogText(_T("setAndReadImage: setInspections!"), LogLevel::Error, LogType::FAIL);
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
			m_rLogClass.LogText(_T("setAndReadImage: init Inspection"), LogLevel::Error, LogType::FAIL);
			return false;
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		m_rLogClass.logException("setAndReadImage: init Inspection", rExp);
		return false;
	}
	catch (...)
	{
		m_rLogClass.LogText(_T("Unknown exception when setAndReadImage: init Inspection!"), LogLevel::Error, LogType::FAIL);
		return false;
	}

	constexpr int numberOfRuns = 100;
	auto start = std::chrono::system_clock::now();
	for (int i = 0; i < numberOfRuns; i++)
	{
		//write image to buffer
		auto imageHandle = m_TRController.getImageBuffer(bufferStruct);
		if (nullptr == imageHandle || imageHandle->isEmpty())
		{
			m_rLogClass.LogText(_T("setAndReadImage: getImageBuffer return nullptr!"), LogLevel::Error, LogType::FAIL);
			return false;
		}
		else
		{
			MbufCopy(imageIds[i%imageIds.size()], imageHandle->getHandle()->GetBuffer().GetIdentifier());
			if (!areImageEqual(imageHandle->getHandle()->GetBuffer().GetIdentifier(), imageIds[i%imageIds.size()]))
			{
				m_rLogClass.LogText(_T("setAndReadImage: ImageCopy failed. Images are not equal!"), LogLevel::Error, LogType::FAIL);
			}
		}

		{//scope for tr2W
			//get TR and set new image
			auto tr2W = m_TRController.createTriggerRecordObjectToWrite(0);
			if (nullptr == tr2W)
			{
				m_rLogClass.LogText(_T("setAndReadImage: createTriggerRecordObjectToWrite return nullptr!"), LogLevel::Error, LogType::FAIL);
				return false;
			}
			try
			{
				tr2W->setImage(0, imageHandle->getBufferPos());
			}
			catch (const SvStl::MessageContainer& rExp)
			{
				m_rLogClass.logException("setAndReadImage: ", rExp);
				return false;
			}
		}
		int id = m_TRController.getLastTRId(0);
		auto tr2R = m_TRController.createTriggerRecordObject(0, id);
		if (nullptr == tr2R)
		{
			CString errorStr;
			errorStr.Format(_T("setAndReadImage: could not create read version of TR (id%d) in run %d"), id, i);
			m_rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
			continue;
		}
		imageHandle = tr2R->getImage(0);
		if (nullptr == imageHandle || imageHandle->isEmpty())
		{
			m_rLogClass.LogText(_T("setAndReadImage: getImageBuffer return nullptr!"), LogLevel::Error, LogType::FAIL);
			return false;
		}
		else
		{
			if (!areImageEqual(imageHandle->getHandle()->GetBuffer().GetIdentifier(), imageIds[i%imageIds.size()]))
			{
				m_rLogClass.LogText(_T("setAndReadImage: ImageCopy failed. Images are not equal!"), LogLevel::Error, LogType::FAIL);
			}
			//else
			//{
			//	CString logStr;
			//	logStr.Format(_T("setAndReadImage: Read image is equal to set image in run %d"), i);
			//	m_rLogClass.LogText(logStr, LogLevel::Information_Level3, LogType::PASS);
			//}
		}
	}
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_nano = end - start;

	CString logStr;
	logStr.Format(_T("setAndReadImage: (%f s/ %f s)"), elapsed_nano.count(), numberOfRuns*m_config.getMaxTimesetAndReadImage());
	bool isTimeOk = elapsed_nano.count() < numberOfRuns*m_config.getMaxTimesetAndReadImage();
	m_rLogClass.LogText(logStr, isTimeOk ? LogLevel::Information_Level1 : LogLevel::Error, isTimeOk ? LogType::PASS : LogType::FAIL);
	return true;
}

bool TrcTester::setAndReadValues()
{
	bool retValue = setInspections({21}, m_TRController, m_rLogClass);
	if (!retValue)
	{
		m_rLogClass.LogText(_T("setAndReadValues: setInspections!"), LogLevel::Error, LogType::FAIL);
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
		pValueObjectDef->set_guidid(uniqueIdBytes.c_str());
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
		if (!retValue)
		{
			m_rLogClass.LogText(_T("setAndReadValues: init Inspection"), LogLevel::Error, LogType::FAIL);
			return false;
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		m_rLogClass.logException("setAndReadValues: init Inspection", rExp);
		return false;
	}
	catch (...)
	{
		m_rLogClass.LogText(_T("Unknown exception when setAndReadValues: init Inspection!"), LogLevel::Error, LogType::FAIL);
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
				m_rLogClass.LogText(_T("setAndReadValues: createTriggerRecordObjectToWrite return nullptr!"), LogLevel::Error, LogType::FAIL);
				return false;
			}
			try
			{
				auto tmpData = runData;
				tr2W->writeValueData(std::move(tmpData));
			}
			catch (const SvStl::MessageContainer& rExp)
			{
				m_rLogClass.logException("setAndReadValues: ", rExp);
				return false;
			}
		}
		int id = m_TRController.getLastTRId(0);
		auto tr2R = m_TRController.createTriggerRecordObject(0, id);
		if (nullptr == tr2R)
		{
			CString errorStr;
			errorStr.Format(_T("setAndReadValues: could not create read version of TR (id%d) in run %d"), id, i);
			m_rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
			continue;
		}
		
		int pos = i%dataDefList.list_size();
		variant_t testValue = tr2R->getDataValue(SvPb::GetGuidFromProtoBytes(dataDefList.list(pos).guidid()));
		if (runData[pos] != testValue)
		{
			CString errorStr;
			errorStr.Format(_T("setAndReadValues: value unexpect. Run %d, value = %s, expect = %s"), i, SvUl::createStdString(testValue).c_str(), SvUl::createStdString(runData[pos]).c_str());
			m_rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
			return false;
		}
		else
		{
			CString errorStr;
			errorStr.Format(_T("setAndReadValues: value expected. Run %d, value = %s"), i, SvUl::createStdString(testValue).c_str());
			m_rLogClass.LogText(errorStr, LogLevel::Information_Level3, LogType::PASS);
		}
	}
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_nano = end - start;

	CString logStr;
	logStr.Format(_T("setAndReadValues: (%f s/ %f s)"), elapsed_nano.count(), numberOfRuns*m_config.getMaxTimesetAndReadImage());
	bool isTimeOk = elapsed_nano.count() < numberOfRuns*m_config.getMaxTimesetAndReadImage();
	m_rLogClass.LogText(logStr, isTimeOk ? LogLevel::Information_Level1 : LogLevel::Error, isTimeOk ? LogType::PASS : LogType::FAIL);
	return true;
}

bool TrcTester::testWithMoreThreads()
{
	auto& rAllImageLists = m_config.getImageLists();
	std::promise<bool> writerPromise;
	std::promise<bool> readerPromise;
	std::future<bool> writerResult = writerPromise.get_future();
	std::future<bool> readerResult = readerPromise.get_future();
	std::thread writerThread(runWriterTest, std::move(writerPromise), std::ref(m_rLogClass), 100, m_config.getTestData());
	std::thread readerThread(runReaderTest, std::move(readerPromise), std::ref(m_rLogClass), 100, m_config.getTestData());
	bool retValue = writerResult.get() && readerResult.get();
	{
		m_rLogClass.LogText(_T("testWithMoreThreads"), retValue ? LogLevel::Information_Level1 : LogLevel::Error, retValue ? LogType::PASS : LogType::FAIL);
	}
	writerThread.join();
	readerThread.join();
	return retValue;
}
#pragma endregion test methods

bool TrcTester::setInspectionBuffers()
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
			m_rLogClass.logException("Parts of setInspectionBuffer", rExp);
			return false;
		}
		catch (...)
		{
			CString errorStr;
			errorStr.Format(_T("Parts of setInspectionBuffer throw an exception: #%d!"), i);
			m_rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
			return false;
		}
	}

	return true;
}


bool TrcTester::setIndependentBuffers()
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
			m_rLogClass.LogText(_T("ERROR when creating independent buffer!"), LogLevel::Error, LogType::FAIL);
			return false;
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		m_rLogClass.logException("Parts of setIndependentBuffers", rExp);
		return false;
	}
	catch (...)
	{
		m_rLogClass.LogText(_T("Unknown exception when creating independent buffer!"), LogLevel::Error, LogType::FAIL);
		return false;
	}

	return true;
}

bool setInspections(std::vector<int> numbersOfRecords, SvTrc::ITriggerRecordControllerRW& rTrController, LogClass& rLogClass)
{
	SvPb::InspectionList inspList;
	for (int numberOfRecords : numbersOfRecords)
	{
		auto* pInspStruct = inspList.add_list();
		if (nullptr == pInspStruct)
		{
			rLogClass.LogText(_T("createTR2WriteAndRead: Could not create inspection!"), LogLevel::Error, LogType::FAIL);
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
		retValue = setInspections(numbersOfRecords, rTrController, rLogClass);
		logStr.Format(_T("Writer Tests (%d): CreateInspections(%s)"), testDataId, inspectionNumberStr);
		
		if (!retValue)
		{
			rLogClass.LogText(logStr, LogLevel::Error, LogType::FAIL);
			break;
		}
		else
		{
			rLogClass.LogText(logStr, LogLevel::Information_Level3, LogType::PASS);
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
				rLogClass.logException(logStr, rExp);
				retValue = false;
				break;
			}
			catch (...)
			{
				CString errorStr;
				errorStr.Format(_T("Writer Tests (%d): resetTriggerRecordStructure throw an exception"), testDataId);
				rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
				retValue = false;
				break;
			}
		}
		if (!retValue)
		{
			break;
		}

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
					rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
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
					rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
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
						rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
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
						rLogClass.logException(errorStr, rExp);
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
							rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
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
								rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
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
						rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
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
							rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
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

void runReaderTest(std::promise<bool>&& intPromise, LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData)
{
	bool retValue = true;
	auto& rTrController = SvTrc::getTriggerRecordControllerRInstance();
	int lastTestDataId = 0;
	std::vector<int> lastTrIds;
	//done a delay to get WriterTest finished with init.
	getLastTrId(0, rTrController);

	int numberOfRunsComplete = numberOfRuns * static_cast<int>(rTestData.size());

	for (int runId = 0; runId < numberOfRunsComplete; runId++)
	{
		auto inspectionSize = rTrController.getInspections().list_size();
		if (lastTrIds.size() != inspectionSize)
		{
			lastTrIds.resize(inspectionSize, -1);
		}

		for (int ipId = 0; ipId < inspectionSize; ipId++)
		{
			int trId = -1;
			for (int i=0; i<20; i++)
			{
				trId = getLastTrId(ipId, rTrController);
				if (lastTrIds[ipId] != trId)
				{
					break;
				}
				//add a sleep if last id the same as by the last to avoid to check one TR to often.
				std::chrono::duration<int, std::nano> t(1);
				std::this_thread::sleep_for(t);
			}
			if (lastTrIds[ipId] == trId)
			{
				CString logStr;
				logStr.Format(_T("Reader Tests: (%d) check trId %d again by run %d"), ipId, trId, runId);
				rLogClass.LogText(logStr, LogLevel::Information_Level3, LogType::WARN);
			}
			lastTrIds[ipId] = trId;

			auto tr2R = rTrController.createTriggerRecordObject(ipId, trId);
			if (nullptr == tr2R)
			{
				CString errorStr;
				errorStr.Format(_T("Reader Tests: createTriggerRecordObject(%d, %d) return nullptr by run %d!"), ipId, trId, runId);
				rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
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
				errorStr.Format(_T("Reader Tests: (%d) triggerCount (%d) do not fit: , called testDataId %d, max testDataId %d by run %d"), ipId, triggerCount, testDataId, rTestData.size()-1, runId);
				rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
				retValue = false;
				std::chrono::duration<int, std::micro> t(1);
				std::this_thread::sleep_for(t);
				continue;
			}
			//else
			//{
			//	CString logStr;
			//	logStr.Format(_T("Reader Tests(%d): createTriggerRecordObject(%d) triggerCount %d by run %d"), testDataId, ipId, triggerCount, runId);
			//	rLogClass.LogText(logStr, LogLevel::Information_Level3, LogType::PASS);
			//}

			if (testDataId != lastTestDataId)
			{
				CString logStr;
				logStr.Format(_T("Reader Tests(%d): Moved to this testDataId with run %d"), testDataId, runId);
				rLogClass.LogText(logStr, LogLevel::Information_Level3, LogType::PASS);
				lastTestDataId = testDataId;
			}

			retValue = checkImages(rTestData[testDataId][ipId], tr2R, rLogClass, writerRunId, testDataId, ipId, runId, triggerCount) && retValue;
		}
		
		//std::this_thread::sleep_for(std::chrono::duration<int, std::nano>(1));
	}
		
	intPromise.set_value(retValue);
	rLogClass.LogText("Finished runReaderTest", LogLevel::Information_Level1, LogType::PASS);
}

int calcMaxBuffer()
{
	constexpr int handleForReset = 1000;
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

std::vector<MIL_ID> loadImages(const std::vector<std::string> fileNames)
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

int getLastTrId(int ipId, SvTrc::ITriggerRecordControllerR& rTrController)
{
	int trId = -1;
	for (int i = 0; i < 200; i++)
	{
		trId = rTrController.getLastTRId(ipId);
		if (0 <= trId)
		{
			break;
		}
		std::chrono::duration<int, std::nano> t(100);
		std::this_thread::sleep_for(t);
	}
	return trId;
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
				rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
				retValue = false;
				continue;
			}
			else
			{
				CString errorStr;
				errorStr.Format(_T("Reader Tests (%d): getImage(%d:%d) Tr is not longer up to time by run %d!"), testDataId, ipId, imagePos, runId);
				rLogClass.LogText(errorStr, LogLevel::Error, LogType::WARN);
				return retValue;
			}
		}
		else
		{
			if (!areImageEqual(imageHandle->getHandle()->GetBuffer().GetIdentifier(), imageId))
			{
				CString errorStr;
				errorStr.Format(_T("Reader Tests (%d): ImageCopy(%d:%d) failed. TriggerCount = %d. Images are not equal by run %d!"), testDataId, ipId, imagePos, triggerCount, runId);
				rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
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
				rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
				retValue = false;
			}
			else
			{
				CString errorStr;
				errorStr.Format(_T("Reader Tests (%d): getImage(%d:last) Tr is not longer up to time by run %d!"), testDataId, ipId, runId);
				rLogClass.LogText(errorStr, LogLevel::Error, LogType::WARN);
				return retValue;
			}
		}
		else
		{
			if (!areImageEqual(imageHandle->getHandle()->GetBuffer().GetIdentifier(), imageId))
			{
				CString errorStr;
				errorStr.Format(_T("Reader Tests (%d): ImageCopy(%d:last) failed. TriggerCount = %d. Images are not equal by run %d!"), testDataId, ipId, triggerCount, runId);
				rLogClass.LogText(errorStr, LogLevel::Error, LogType::FAIL);
				retValue = false;
			}
		}
	}
	return retValue;
}
