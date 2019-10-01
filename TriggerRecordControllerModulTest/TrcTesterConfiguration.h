//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file TrcTester.h
/// All Rights Reserved
//*****************************************************************************
/// configures the unit test
/// This class will be added also to TriggerRecordControllerReaderModuleTest-project.
//******************************************************************************
#pragma once
class LogClass;
namespace SvLib
{
class  SVOINIClass;
}

struct ParamDef
{
	ParamDef(std::string name, variant_t value, int byteSize = 0);

	std::string m_name;
	variant_t m_value;
	int m_byteSize = 0; //only used if string, else size of object

	DWORD getVarType() const { return m_value.vt; };
	int getByteSize() const { return m_byteSize; };
};

class TrcTesterConfiguration
{
public:
	TrcTesterConfiguration(LogClass& rLogClass, SvLib::SVOINIClass iniFile, bool isLocal);
	explicit TrcTesterConfiguration(LogClass& rLogClass);

	~TrcTesterConfiguration();

	typedef std::vector<MIL_ID> ImageList;
	typedef std::vector<ParamDef> ParamDefList;
	typedef std::vector<std::vector<variant_t>> ParamValueList;

	struct InspectionsDef
	{
		InspectionsDef(int minRecord, int maxRecord, int minInterest, int maxInterest, const std::vector<ImageList>& rImages, const ParamDefList& rParamDefs, const ParamValueList& rParamValues)
			: m_imageFilesList(rImages)
			, m_ParamDefs(rParamDefs)
			, m_ParamValues(rParamValues)
			, m_minRecordSize(minRecord)
			, m_maxRecordSize(maxRecord)
			, m_minRecordInterestSize(minInterest)
			, m_maxRecordInterestSize(maxInterest)
		{
			recalcRecordSizes();
		}

		void recalcRecordSizes();

		const int m_minRecordSize;
		const int m_maxRecordSize;
		int m_recordSize;
		const int m_minRecordInterestSize;
		const int m_maxRecordInterestSize;
		int m_recordInterestSize;
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
	int getMaxSpecifyBufferFactor() const { return m_maxSpecifyBufferFactor; };
	int getSpecifyBufferDiv() const { return m_SpecifyBufferDiv; };
	double getMaxTimeSetBuffer() const { return m_NoOfRepetitionsPerStep * m_maxTimeSetBufferPerIter; };
	double getMaxTimeCheckBufferPerBuffer() const { return m_maxTimeCheckBufferPerBuffer; };
	double getMaxTimesetAndReadImage() const { return m_maxTimesetAndReadImage; };
	double getMaxTimesetAndReadValue() const { return m_maxTimesetAndReadValue; };
	const std::vector<ImageList>& getImageLists() const { return m_imageLists; };
	const ParamDefList& getValueObjectSet() const { return m_valueObjectSet; };
	const ParamValueList& getValueSet() const { return m_valueSet; };
	const TestDataList& getTestData() const { return m_testData; };
	void recalcRecordSizes();
	int getNumberOfRecordsAddOne() const { return m_numberOfRecordsAddOne; };
	int getNumberOfKeepFreeRecords() const { return m_numberOfKeepFreeRecords; };
	bool isLocal() const { return m_isLocal; };

private:
	void init(LogClass& rLogClass);

private:
	bool m_isLocal = false;
	int m_NumberOfRuns = 20;
	const int m_NoOfRepetitionsPerStep = 20;
	
	int m_NumberOfImagesPerInspection = 99;
	int m_NumberOfInspections = 5;
	int m_NumberOfBuffersPerInspection = 10;
	int m_NumberOfIndependentBuffers = 5;
	int m_maxSpecifyBufferFactor = 32;
	int m_SpecifyBufferDiv = 5;

	//performance times
	double m_maxTimeSetBufferPerIter = 1.8;
	double m_maxTimeCheckBufferPerBuffer = 3.1;
	double m_maxTimesetAndReadImage = 0.65;
	double m_maxTimesetAndReadValue = 0.01;

	int m_numberOfRecordsAddOne = 5;
	int m_numberOfKeepFreeRecords = 2;

	const std::vector<std::vector<std::string>> m_imageFileNameLists = {{R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0000.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0001.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0002.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0003.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0004.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0005.bmp)",
		R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0006.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0007.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0008.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0009.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0010.bmp)",R"(C:\IMAGES\Grayscale\640x480\seq Rotate\Rotate640x480_0011.bmp)"},
		{R"(C:\Images\Mono8\1000x1000\Blob_0001.bmp)",R"(C:\Images\Mono8\1000x1000\Blob_0002.bmp)",R"(C:\Images\Mono8\1000x1000\Blob_0003.bmp)",R"(C:\Images\Mono8\1000x1000\Blob_0004.bmp)"},
		{R"(C:\Images\RGB\656x416\CrimpWave\CrimpWave_000001.bmp)",R"(C:\Images\RGB\656x416\CrimpWave\CrimpWave_000002.bmp)",R"(C:\Images\RGB\656x416\CrimpWave\CrimpWave_000003.bmp)",R"(C:\Images\RGB\656x416\CrimpWave\CrimpWave_000004.bmp)", R"(C:\Images\RGB\656x416\CrimpWave\CrimpWave_000005.bmp)",R"(C:\Images\RGB\656x416\CrimpWave\CrimpWave_000006.bmp)",R"(C:\Images\RGB\656x416\CrimpWave\CrimpWave_000007.bmp)",R"(C:\Images\RGB\656x416\CrimpWave\CrimpWave_000008.bmp)"}};
	std::vector<ImageList> m_imageLists;

	const ParamDefList m_valueObjectSet = {{"Value_int", 4}, {"Value_double", 4.0}, {"Value_string", "Test100", 9}};
	const ParamValueList m_valueSet = {{4,4.0,"Test1.0"}, {7,3.1,"Test2.0"}, {1,0.5,"Test3.0"}, {5,5.5,"Test4.0"}, {2,3.0,"Test5.0"}, {100,1000.0,"Test116."}, {4,4.0,"Test17.0"}};

	TestDataList m_testData;
};