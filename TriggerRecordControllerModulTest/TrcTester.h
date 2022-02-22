//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file TrcTester.h
/// All Rights Reserved
//*****************************************************************************
/// Declaration of the Trigger Record controller tester class (and a configuration 
/// struct for it).
//******************************************************************************
#pragma once
class TrcTesterConfiguration;
class LogClass;

namespace SvOi
{
class ITriggerRecordControllerRW;
}

namespace SvPb
{
class DataDefinitionList;
}

/// controls the flow of the test
class TrcTester
{
public:
	TrcTester(TrcTesterConfiguration& rConfig, LogClass& rLogClass);
	~TrcTester();
	bool fullTest();

private:
#pragma region test methods
	bool createInspections();
	bool setBuffers();
	bool checkBufferMaximum();
	bool createTR2WriteAndRead();
	bool setAndReadImage();
	bool setAndReadValues();
	bool testWithMoreThreads();
	template <int readerSize>
	bool testWithReaderApps();
#pragma endregion test methods

	bool setInspectionBuffers(LPCSTR testAreaStr);
	bool setIndependentBuffers(LPCSTR testAreaStr);
	bool writeAndReadSingleValue(int index, int listSize, const std::vector<std::vector<uint8_t>>& rDataVector);
	int createDataDefContainer(std::vector<std::vector<uint8_t>>& rDataMemVector, SvPb::DataDefinitionList& rDataDefList);
	
	SvOi::ITriggerRecordControllerRW* m_pTrcRW;
	TrcTesterConfiguration& m_config;
	LogClass& m_rLogClass;
};

inline uint32_t getNextObjectId()
{
	static uint32_t nextId = 1;
	return nextId++;
}