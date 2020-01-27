//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file TrcTester.h
/// All Rights Reserved
//*****************************************************************************
/// Declaration of the Trigger Record controller tester class (and a configuration 
/// struct for it).
//******************************************************************************
#pragma once
class TrcTesterConfiguration;
class LogClass;

namespace SvTrc
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
	
	SvTrc::ITriggerRecordControllerRW &m_TRController;
	TrcTesterConfiguration& m_config;
	LogClass& m_rLogClass;
};

