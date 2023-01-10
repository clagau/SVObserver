//*****************************************************************************
/// \copyright COPYRIGHT (c) 2019 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file DataControllerReader.h
/// All Rights Reserved
//*****************************************************************************
/// Control of the Data in reader mode.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVProtoBuf/TriggerRecordController.h"
#include "SVSharedMemoryLibrary/SharedDataStore.h"
#include "DataControllerBase.h"
#pragma endregion Includes


namespace SvTrc
{
struct CommonDataStruct;
struct SMData;


class TRControllerReaderDataPerIP : public TRControllerBaseDataPerIP
{
public:
	TRControllerReaderDataPerIP() = default;
	virtual ~TRControllerReaderDataPerIP();

	void init(const std::string& smName);
	void reloadData();

	BasicData getBasicData() const override { return m_pBasicData ? *m_pBasicData : BasicData(); };
	const SvPb::DataDefinitionList& getDataDefList() const { return m_DataDefList; };
	LPCTSTR getSMName() const { return m_smName.c_str(); };
	TriggerRecordData& getTRData(int pos) const override;
	const SvPb::ImageList& getImageList() const override { return m_ImageList; };
	const SvPb::DataDefinitionList& getDataList() const override { return m_DataDefList; };
	void* getTriggerRecords() { return m_pTriggerRecords; };
	virtual void setLastFinishedTRID(int) override { Log_Assert(false); throw E_NOTIMPL; };
	void setLastSetOfInterestFlagPos(int pos) override;
	int getTrId2Send();
	SvOi::TrInterestEventData getInterestTrId2Send();
	virtual void resetFreeTrNumber() override {	throw E_NOTIMPL; };
	virtual void increaseFreeTrNumber() override;
	virtual void decreaseFreeTrNumber() override;
	virtual bool isEnoughFreeForLock() const override;
	virtual void setTrOfInterest(int inspectionPos, int pos) override;
	std::vector<int> getTRofInterestPos(int n);

private:
	SvSml::DataStorePointer m_SMHandle;
	BasicData* m_pBasicData = nullptr;
	SMData* m_pSmData = nullptr;
	int* m_pTRofInterestArray = nullptr;

	SvPb::ImageList m_ImageList;
	void* m_pImageListInSM = nullptr;
	SvPb::DataDefinitionList m_DataDefList;
	void* m_pDataDefListInSM = nullptr;
	int m_lastSendTrId = -1;
	int m_lastSendInterestPos = -1;

	void* m_pTriggerRecords = nullptr;
	std::string m_smName = {};
};

class DataControllerReader : public DataControllerBase
{
#pragma region Constructor
public:
	DataControllerReader();
	virtual ~DataControllerReader();

	DataControllerReader(const DataControllerReader&) = delete;
	DataControllerReader& operator=(const DataControllerReader&) = delete;
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual long getResetId() const override;

	virtual volatile long* getResetLockCounterRef() override;

	virtual const SvPb::InspectionList& getInspections() const override;

	virtual const SvPb::ImageStructList& getImageStructList() const override;

	virtual SvOi::ITriggerRecordRPtr createTriggerRecordObject(int inspectionPos, std::function<bool(TriggerRecordData&)> validFunc) override;

	virtual bool isWritable() const override { return false; };

	virtual void setPauseTrsOfInterest(bool flag, int inspectionPos) override;
	virtual bool getPauseTrsOfInterest(int inspectionPos) const override;
	virtual std::vector<SvOi::ITriggerRecordRPtr> getTRsOfInterest(int inspectionPos, int n) override;
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	virtual TRControllerBaseDataPerIP* getTRControllerData(int inspectionId) override;
	virtual const TRControllerBaseDataPerIP* getTRControllerData(int inspectionId) const override;

	virtual long* getImageRefCountPtr(int pos) override;
	virtual bool isReady() const override { return isUpToDate(); };
#pragma endregion Protected Methods

#pragma region Private Methods
private:

	/// It checked if class already initialed and up to date. 
	/// \returns bool True, if all correct
	bool isUpToDate() const;

	void initAndreloadData();

	void newTrIdThread();

	void addBuffer(const SvPb::ImageStructData &imageStruct);

	void sendNewTrId();
#pragma endregion Private Methods

#pragma region Member variables
private:
	CommonDataStruct* m_pCommonData = nullptr;
	long* m_imageRefCountArray = nullptr; //an array of the reference counts.

	SvPb::InspectionList m_inspectionList;
	void* m_pInspectionListInSM = nullptr;

	SvPb::ImageStructList m_imageStructList;
	void* m_pImageStructListInSM = nullptr;

	std::vector<std::shared_ptr<TRControllerReaderDataPerIP>> m_dataVector;
	mutable std::shared_mutex m_dataVectorMutex;

	std::map<std::string, SvSml::DataStorePointer> m_sharedMemoryMap;

	SvSml::SharedDataStore m_commonSHHandle;

	std::atomic_long m_lastResetId = 0; //id of the last reset
	std::atomic_bool m_isInit = false;

	bool m_stopThread = false;
	std::future<void> m_reloadFuture;
	std::future<void> m_newTrIdFuture;
	HANDLE m_stopThreads {nullptr};
#pragma endregion Member variables
};
} //namespace SvTrc