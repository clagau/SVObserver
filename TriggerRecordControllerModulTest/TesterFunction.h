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
namespace SvTrc
{
class ITriggerRecordControllerRW;
}

extern LPTSTR strTestWithMoreThreads;

bool areImageEqual(MIL_ID image1, MIL_ID image2);
SVMatroxBufferCreateStruct specifyBufferFromImage(MIL_ID imageId);

//numbersOfRecords are list of pairs (one per inspection) with first = numberOfRecords and second = number”fInterest
bool setInspections(std::vector<std::pair<int, int>> numbersOfRecords, SvTrc::ITriggerRecordControllerRW& rTrController, LogClass& rLogClass, LPCSTR testAreaStr);

bool writerTest(LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData);
bool readerTest(LogClass& rLogClass, const int numberOfRuns, const TrcTesterConfiguration::TestDataList& rTestData, bool isOtherProcess = false);