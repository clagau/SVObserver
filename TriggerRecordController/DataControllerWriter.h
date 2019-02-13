//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file DataControllerWriter.h
/// All Rights Reserved
//*****************************************************************************
/// Control of the Data in write mode.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVProtoBuf/TriggerRecordController.h"
#include "SVSharedMemoryLibrary/SharedDataStore.h"
#include "ImageBufferSMHelper.h"
#pragma endregion Includes



namespace SvTrc
{

struct CommonDataStruct
{
	long m_resetId = 0; //id of the last reset
	long m_resetLockCounter = 0; //counter of current used methods of ITriggerRecordR-instance 
	int m_imageRefCountSize = 0; //the numbers of refCounts reserved in m_imageRefCountArray.
	int m_inspectionListPBSize = 0;
	int m_imageStructListPBSize = 0;
};

class TRControllerWriterDataPerIP : public TRControllerBaseDataPerIP
{
public:
	TRControllerWriterDataPerIP() = default;
	~TRControllerWriterDataPerIP();

	void init(std::function<void(const std::string&, int)> smDataCBFunc);

	BasicData getBasicData() const override { return m_pBasicData ? *m_pBasicData : BasicData(); };
	void setDataListSize(int dataSize);
	void setInitFlag(bool flag);
	void setLastFinishedTRID(int id) override;
	void setDataDefList(SvPb::DataDefinitionList&& dataDefList);
	const SvPb::DataDefinitionList& getDataDefList() const { return m_DataDefList; };
	int getLastStartedTRID() const { return m_lastStartedTRID; };
	void setLastStartedTRID(int id) { m_lastStartedTRID = id; };
	int getNextPosForFreeCheck() const { return m_nextPosForFreeCheck; };
	void setNextPosForFreeCheck(int id) { m_nextPosForFreeCheck = id; };
	TriggerRecordData& getTRData(int pos) const override;
	const SvPb::ImageList& getImageList() const override { return m_ImageList; };
	void setImageList(SvPb::ImageList&& imageList);
	void* getTriggerRecords() { return m_pTriggerRecords; };
	void* createTriggerRecordsBuffer(int trBufferSize, int trNumbers);

	struct SMData
	{
		int m_maxImageListSize = 1000;
		int m_imageListSize = 0;
		int m_maxDataDefListSize = 10000;
		int m_dataDefListSize = 0;
		int m_maxTriggerRecordBufferSize = 100'000;
	};
	
private:
	void createSMBuffer(BasicData basicData, SMData smData);

	SvSml::DataStorePointer m_SMHandle;
	BasicData* m_pBasicData = nullptr;
	SMData* m_pSmData = nullptr;

	SvPb::ImageList m_ImageList;
	void* m_pImageListInSM = nullptr;
	SvPb::DataDefinitionList m_DataDefList;
	void* m_pDataDefListInSM = nullptr;
	
	void* m_pTriggerRecords = nullptr;
	std::function <void(const std::string&, int)> m_smDataCBFunc;

	int m_lastStartedTRID {-1};
	int m_nextPosForFreeCheck {0}; //This variable contains which position it should start searching for a free slot.
	const int m_imageListStepSize = 1000;
	const int m_dataDefListStepSize = 1000;
	const int m_triggerRecordBufferStepSize = 10000;
};


constexpr LPCTSTR cCommonParameterSM = _T("SVO-CommonParameter");

class DataControllerWriter : public DataControllerBase
{
#pragma region Constructor
public:
	DataControllerWriter();
	virtual ~DataControllerWriter();

	DataControllerWriter(const DataControllerWriter&) = delete;
	DataControllerWriter& operator=(const DataControllerWriter&) = delete;
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual void clearAll() override;

	virtual bool setInspections(const SvPb::InspectionList& rInspectionList) override;

	virtual long getResetId() const override { return m_pCommonData->m_resetId; };

	virtual long& getResetLockCounterRef() override { return m_pCommonData->m_resetLockCounter; };

	virtual const SvPb::InspectionList& getInspections() const override { return m_inspectionList; }

	virtual void setImageDefList(int inspectionPos, SvPb::ImageList&& imageList) override;

	virtual const SvPb::ImageStructList& getImageStructList() const override { return m_imageStructList; }; 
	virtual void setImageStructList(SvPb::ImageStructList list) override;

	virtual void resetImageRefCounter() override;

	virtual void removeImageMemory(std::string memoryName) override { m_imageMemoryHelper.removeMemory(memoryName); };
	virtual int createMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos) override { return m_imageMemoryHelper.createMilBufferinMemory(requiredNumbers, rImageStruct, vectorPos); };
	virtual int contractMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos) override { return m_imageMemoryHelper.contractMilBufferinMemory(requiredNumbers, rImageStruct, vectorPos); };

	virtual void changeDataDef(SvPb::DataDefinitionList&& rDataDefList, std::vector<_variant_t>&& rValueObjectList, int inspectionPos) override;

	virtual ITriggerRecordRPtr createTriggerRecordObject(int inspectionPos, int trId) override;
	virtual ITriggerRecordRWPtr createTriggerRecordObjectToWrite(int inspectionPos) override;

	virtual std::vector<std::pair<int, int>> ResetTriggerRecordStructure(int inspectionId, int triggerRecordNumber, SvPb::ImageList imageList, SvPb::ImageStructList imageStructList) override;
#pragma endregion Public Methods

#pragma region Protected Methods
	virtual TRControllerBaseDataPerIP* getTRControllerData(int inspectionId) override;
	virtual const TRControllerBaseDataPerIP* getTRControllerData(int inspectionId) const override;

	virtual long* getImageRefCountPtr(int pos) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void ResetInspectionData(TRControllerWriterDataPerIP& rData);

	/// Set the InspectionList
	/// ATTENTION: Throw exception if InspectionList to large for the space in sharedMemory.
	/// \param rInspectionList [in]
	void setInspectionList(const SvPb::InspectionList &rInspectionList);

	void setInspectionSMData(int ipPos, const std::string& rSmName, int smSize);
#pragma endregion Private Methods

#pragma region Member variables
private:
	const int m_cFullSizeOfCommonParameterSM;
	CommonDataStruct* m_pCommonData = nullptr;
	long* m_imageRefCountArray = nullptr; //an array of the reference counts.
	
	SvPb::InspectionList m_inspectionList;
	void* m_pInspectionListInSM = nullptr;

	SvPb::ImageStructList m_imageStructList;
	void* m_pImageStructListInSM = nullptr;

	std::vector<std::shared_ptr<TRControllerWriterDataPerIP>> m_dataVector;

	ImageBufferSMHelper m_imageMemoryHelper;
	SvSml::SharedDataStore m_commonSHHandle;

	int m_nextTRID = 0; //the next ID for next created trigger record.
	long m_lastResetId = 0; //id of the last reset
#pragma endregion Member variables
};
} //namespace SvTrc
