//*****************************************************************************
/// \copyright (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// \file TesterFunction.h
/// All Rights Reserved 
//*****************************************************************************
/// Function used for the test.
//******************************************************************************
#pragma once
#include "TrcTesterConfiguration.h"
struct SVMatroxBufferCreateStruct;
class LogClass;
namespace SvOi
{
class ITriggerRecordControllerRW;
}

constexpr const char* strTestWithMoreThreads = _T("testWithMoreThreads");

bool areImageEqual(MIL_ID image1, MIL_ID image2);
SVMatroxBufferCreateStruct specifyBufferFromImage(MIL_ID imageId);

//numbersOfRecords are list of pairs (one per inspection) with first = numberOfRecords and second = number”fInterest
bool setInspections(std::vector<std::pair<int, int>> numbersOfRecords, SvOi::ITriggerRecordControllerRW& rTrcRW, LogClass& rLogClass, LPCSTR testAreaStr);

/// Start the writer test
/// \param rLogClass [in] Reference to logClass
/// \param numberOfRuns [in] Number of runs
/// \param rTestData [in] TestData
/// \param sleepBetweenTrigger [in] The sleep time between two trigger in microseconds.
/// \returns bool
bool writerTest(LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData, int sleepBetweenTrigger = 3000);

struct ReaderTestData
{
	ReaderTestData(LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData, bool isOtherProcess = false)
		:m_rLogClass(rLogClass)
		,m_numberOfRuns(numberOfRuns)
		,m_rTestDataList(rTestData)
		,m_isOtherProcess(isOtherProcess)
	{

	}
	LogClass& m_rLogClass;
	const int m_numberOfRuns;
	const TrcTesterConfiguration::TestDataList& m_rTestDataList;
	bool m_isOtherProcess = false;
};
bool readerTest(LPCTSTR testName, ReaderTestData testData);