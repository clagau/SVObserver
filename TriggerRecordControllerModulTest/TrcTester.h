//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file TrcTester.h
/// All Rights Reserved
//*****************************************************************************
/// Declaration of the Trigger Record controller tester class (and a configuration 
/// struct for it).
//******************************************************************************
#pragma once

#include <winerror.h>
#include <intsafe.h>

#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#include "SVProtoBuf/TriggerRecordController.h"
#include "SVLibrary/SVOINIClass.h"
#include "LogClass.h"

namespace SvTrc
{
class ITriggerRecordControllerRW;
}

/// configures the unit test
class TrcTesterConfiguration
{
public:
	TrcTesterConfiguration(LogClass& rLogClass, SvLib::SVOINIClass iniFile, bool isLocal);
	~TrcTesterConfiguration();

	typedef std::vector<MIL_ID> ImageList;
	typedef std::vector<std::pair<std::string, variant_t>> ParamDefList;
	typedef std::vector<std::vector<variant_t>> ParamValueList;

	struct InspectionsDef
	{
		InspectionsDef(int minRecord, int maxRecord, const std::vector<ImageList>& rImages, const ParamDefList& rParamDefs, const ParamValueList& rParamValues)
			: m_minRecordSize(minRecord)
			, m_maxRecordSize(maxRecord)
			, m_imageFilesList(rImages)
			, m_ParamDefs(rParamDefs)
			, m_ParamValues(rParamValues)
		{
		}
		int m_minRecordSize;
		int m_maxRecordSize;
		const std::vector<ImageList> m_imageFilesList;
		const ParamDefList& m_ParamDefs;
		const ParamValueList& m_ParamValues; //(Size of m_ParamDefs and any m_ParamValue[n] must be equal)
	};

	typedef std::vector<std::vector<InspectionsDef>> TestDataList;

	int getNumberOfRuns() const { return m_NumberOfRuns; };
	int getNoOfRepetitionsPerStep() const { return m_NoOfRepetitionsPerStep; };
	int getNumberOfInspections() const { return m_NumberOfInspections; };
	int getNumberOfImagesPerInspection() const { return m_NumberOfImagesPerInspection; };
	int getNumberOfBuffersPerInspection() const { return m_NumberOfBuffersPerInspection; };
	int getNumberOfIndependentBuffers() const { return m_NumberOfIndependentBuffers; };
	double getMaxTimeSetBuffer() const { return m_NoOfRepetitionsPerStep * m_maxTimeSetBufferPerIter; };
	double getMaxTimeCheckBufferPerBuffer() const { return m_maxTimeCheckBufferPerBuffer; };
	double getMaxTimesetAndReadImage() const { return m_maxTimesetAndReadImage; };
	const std::vector<ImageList>& getImageLists() const { return m_imageLists; };
	const ParamDefList& getValueObjectSet() const { return m_valueObjectSet; };
	const ParamValueList& getValueSet() const { return m_valueSet; };
	const TestDataList& getTestData() const { return m_testData; };
	int getNumberOfRecordsAddOne() const { return m_numberOfRecordsAddOne; };
	int getNumberOfKeepFreeRecords() const { return m_numberOfKeepFreeRecords; };

private:
	int m_NumberOfRuns = 20;
	const int m_NoOfRepetitionsPerStep = 200;

	int m_NumberOfImagesPerInspection = 99;
	int m_NumberOfInspections = 5;
	int m_NumberOfBuffersPerInspection = 10;
	int m_NumberOfIndependentBuffers = 5;

	//performance times
	double m_maxTimeSetBufferPerIter = 0.0018;
	double m_maxTimeCheckBufferPerBuffer = 0.0031;
	double m_maxTimesetAndReadImage = 0.00065;

	int m_numberOfRecordsAddOne = 5;
	int m_numberOfKeepFreeRecords = 2;

	const std::vector<std::vector<std::string>> m_imageFileNameLists = {{R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0000.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0001.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0002.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0003.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0004.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0005.bmp)",
													R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0006.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0007.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0008.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0009.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0010.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0011.bmp)"},
													{R"(C:\Images\Mono8\1000x1000\Blob_0001.bmp)",R"(C:\Images\Mono8\1000x1000\Blob_0002.bmp)",R"(C:\Images\Mono8\1000x1000\Blob_0003.bmp)",R"(C:\Images\Mono8\1000x1000\Blob_0004.bmp)"},
													{R"(C:\Images\RGB\656x416\CrimpWave\CrimpWave_000001.bmp)",R"(C:\Images\RGB\656x416\CrimpWave\CrimpWave_000002.bmp)",R"(C:\Images\RGB\656x416\CrimpWave\CrimpWave_000003.bmp)",R"(C:\Images\RGB\656x416\CrimpWave\CrimpWave_000004.bmp)", R"(C:\Images\RGB\656x416\CrimpWave\CrimpWave_000005.bmp)",R"(C:\Images\RGB\656x416\CrimpWave\CrimpWave_000006.bmp)",R"(C:\Images\RGB\656x416\CrimpWave\CrimpWave_000007.bmp)",R"(C:\Images\RGB\656x416\CrimpWave\CrimpWave_000008.bmp)"}};
	std::vector<ImageList> m_imageLists;

	const ParamDefList m_valueObjectSet = {{"Value_int", 4}, {"Value_double", 4.0}, {"Value_string", "Test100"}};
	const ParamValueList m_valueSet = {{4,4.0,"Test1.0"}, {7,3.1,"Test2.0"}, {1,0.5,"Test3.0"}, {5,5.5,"Test4.0"}, {2,3.0,"Test5.0"}, {100,1000.0,"Test6.0"}, {4,4.0,"Test7.0"}};

	TestDataList m_testData;
};

/// controls the flow of the test
class TrcTester
{
public:
	TrcTester(const TrcTesterConfiguration& rConfig, LogClass& rLogClass);
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
#pragma endregion test methods

	bool setInspectionBuffers(LPCSTR testAreaStr);
	bool setIndependentBuffers(LPCSTR testAreaStr);
	
	SvTrc::ITriggerRecordControllerRW &m_TRController;
	const TrcTesterConfiguration& m_config;
	LogClass& m_rLogClass;
};

