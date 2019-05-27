//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file DataControllerWriter.h
/// All Rights Reserved
//*****************************************************************************
/// Control of the Data in write mode.
//******************************************************************************
#pragma once

#pragma region Includes
#include "DataControllerBase.h"
#include "ImageBufferSMHelper.h"
#include "SharedMemoryStructs.h"
#include "SVProtoBuf/TriggerRecordController.h"
#include "SVSharedMemoryLibrary/SharedDataStore.h"
#pragma endregion Includes


namespace SvTrc
{
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
	const SvPb::DataDefinitionList& getDataList() const override { return m_DataDefList; };
	void setImageList(SvPb::ImageList&& imageList);
	void* getTriggerRecords() { return m_pTriggerRecords; };
	void* createTriggerRecordsBuffer(int trBufferSize, int trNumbers);
	virtual void resetFreeTrNumber() override;
	virtual void increaseFreeTrNumber() override;
	virtual void decreaseFreeTrNumber() override;
	virtual bool isEnoughFreeForLock() const override;
	
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

	virtual volatile long* getResetLockCounterRef() override { return &m_pCommonData->m_resetLockCounter; };

	virtual const SvPb::InspectionList& getInspections() const override { return m_inspectionList; }

	virtual void setImageDefList(int inspectionPos, SvPb::ImageList&& imageList) override;

	/// Return the image struct list. If globalInit active it return the temporary list which will be set if globalInit finished.
	/// \returns const SvPb::ImageStructList&
	virtual const SvPb::ImageStructList& getImageStructList() const override; 
	/// Set the new image struct list and copy it to the SM. If globalInit active it set only the temporary list and the other list and the SM will be set if global finished.
	/// \param list [in] New list.
	virtual void setImageStructList(SvPb::ImageStructList list) override;

	virtual void resetImageRefCounter() override;

	virtual void removeImageMemory(std::string memoryName) override { m_imageMemoryHelper.removeMemory(memoryName); };
	virtual int createMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos) override { return m_imageMemoryHelper.createMilBufferinMemory(requiredNumbers, rImageStruct, vectorPos); };
	virtual int contractMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos) override { return m_imageMemoryHelper.contractMilBufferinMemory(requiredNumbers, rImageStruct, vectorPos); };

	virtual void changeDataDef(SvPb::DataDefinitionList&& rDataDefList, std::vector<_variant_t>&& rValueObjectList, int inspectionPos) override;

	virtual ITriggerRecordRPtr createTriggerRecordObject(int inspectionPos, std::function<bool(TriggerRecordData&)> validFunc) override;
	virtual ITriggerRecordRWPtr createTriggerRecordObjectToWrite(int inspectionPos) override;

	virtual std::vector<std::pair<int, int>> ResetTriggerRecordStructure(int inspectionId, int triggerRecordNumber, SvPb::ImageList imageList, SvPb::ImageStructList imageStructList) override;

	/// Set the InspectionList
	/// ATTENTION: Throw exception if InspectionList to large for the space in sharedMemory.
	/// \param rInspectionList [in]
	virtual void setInspectionList(const SvPb::InspectionList &rInspectionList) override;

	virtual void setGlobalInitFlag(bool flag) override;

	/// Reset resetId to 0 and wait until all reader finished his function.
	virtual void prepareReset() override;
	/// Set resetId to a new number and send reset event.
	virtual void finishedReset() override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual TRControllerBaseDataPerIP* getTRControllerData(int inspectionId) override;
	virtual const TRControllerBaseDataPerIP* getTRControllerData(int inspectionId) const override;

	virtual long* getImageRefCountPtr(int pos) override;
#pragma endregion Protected Methods

#pragma region Private Methods
private:
	void ResetInspectionData(TRControllerWriterDataPerIP& rData);

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
	SvPb::ImageStructList m_imageStructListGlobalInitTmp;
	void* m_pImageStructListInSM = nullptr;

	std::vector<std::shared_ptr<TRControllerWriterDataPerIP>> m_dataVector;
	std::unordered_set<int> m_initAfterGlobalInitSet;

	ImageBufferSMHelper m_imageMemoryHelper;
	SvSml::SharedDataStore m_commonSHHandle;

	int m_nextTRID = 0; //the next ID for next created trigger record.
	long m_lastResetId = 0; //id of the last reset
#pragma endregion Member variables
};
} //namespace SvTrc
