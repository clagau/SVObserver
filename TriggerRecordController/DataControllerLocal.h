//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file DataControllerLocal.h
/// All Rights Reserved
//*****************************************************************************
/// Control of the Data in local mode.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVProtoBuf/TriggerRecordController.h"
#include "DataControllerBase.h"
#include "ImageBufferLocalHelper.h"
#pragma endregion Includes


namespace SvTrc
{

class TRControllerLocalDataPerIP : public TRControllerBaseDataPerIP
{
public:
	TRControllerLocalDataPerIP() = default;
	~TRControllerLocalDataPerIP();


	BasicData getBasicData() const override { return m_basicData; };
	BasicData& getMutableBasicData() { return m_basicData; };
	void setLastFinishedTRID(int id) override { m_basicData.m_lastFinishedTRID = id; };
	void setDataDefList(SvPb::DataDefinitionList&& dataDefList)	{ m_DataDefList.Swap(&dataDefList);	}
	const SvPb::DataDefinitionList& getDataDefList() const { return m_DataDefList; };
	int getLastStartedTRID() const { return m_lastStartedTRID; };
	void setLastStartedTRID(int id) { m_lastStartedTRID = id; };
	int getNextPosForFreeCheck() const { return m_nextPosForFreeCheck; };
	void setNextPosForFreeCheck(int id) { m_nextPosForFreeCheck = id; };
	TriggerRecordData& getTRData(int pos) const override;
	const SvPb::ImageList& getImageList() const override { return m_ImageList; };
	const SvPb::DataDefinitionList& getDataList() const override { return m_DataDefList; };
	SvPb::ImageList& getMutableImageList() { return m_ImageList; };
	void setImageList(SvPb::ImageList&& imageList) { m_ImageList = imageList; };
	void* getTriggerRecords() { return m_pTriggerRecords; };
	void setTriggerRecords(void* pTR);

private:
	BasicData m_basicData;
	void* m_pTriggerRecords = nullptr;

	int m_lastStartedTRID {-1};
	int m_nextPosForFreeCheck {0}; //This variable contains which position it should start searching for a free slot.
	SvPb::ImageList m_ImageList;
	SvPb::DataDefinitionList m_DataDefList;
};

class DataControllerLocal : public DataControllerBase
{
#pragma region Constructor
public:
	DataControllerLocal();
	virtual ~DataControllerLocal();

	DataControllerLocal(const DataControllerLocal&) = delete;
	DataControllerLocal& operator=(const DataControllerLocal&) = delete;
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual void clearAll() override;

	virtual bool setInspections(const SvPb::InspectionList& rInspectionList) override;

	virtual long getResetId() const override { return m_resetId; };

	virtual long* getResetLockCounterRef() override { return &m_resetLockCounter; };

	virtual const SvPb::InspectionList& getInspections() const override { return m_inspectionList; }

	virtual void setImageDefList(int inspectionPos, SvPb::ImageList&& imageList) override;

	virtual const SvPb::ImageStructList& getImageStructList() const override { return m_imageStructList; };
	virtual void setImageStructList(SvPb::ImageStructList list) override { m_imageStructList.Swap(&list); };

	virtual void resetImageRefCounter() override;
	
	virtual void removeImageMemory(std::string memoryName) override { /*For local nothing to do, for SM is something to do */ };
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
	void ResetInspectionData(TRControllerLocalDataPerIP& rData);
#pragma endregion Private Methods

#pragma region Member variables
private:
	std::vector<TRControllerLocalDataPerIP> m_dataVector; //IP data blocks (should be the same as number of IPs)
	long m_resetId = 0; //id of the last reset
	long m_lastResetId = 0; //id of the last reset
	long m_resetLockCounter = 0; //counter of current used methods of ITriggerRecordR-instance 
	SvPb::InspectionList m_inspectionList;

	//image data
	SvPb::ImageStructList m_imageStructList;
	long* m_imageRefCountArray = nullptr; //an array of the reference counts.
	int m_imageRefCountSize = 0; //the numbers of refCounts reserved in m_imageRefCountArray.

	ImageBufferLocalHelper m_imageMemoryHelper;

	int m_nextTRID = 0; //the next ID for next created trigger record.
#pragma endregion Member variables
};
} //namespace SvTrc