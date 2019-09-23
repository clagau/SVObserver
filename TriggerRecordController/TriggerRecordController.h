//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file TriggerRecordController.h
/// All Rights Reserved
//*****************************************************************************
/// The TriggerRecordController: control the triggerRecords.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ITriggerRecordControllerRW.h"
#include "ITriggerRecordRW.h"
#include "ImageBufferController.h"
#include "SVProtoBuf/TriggerRecordController.h"
#pragma endregion Includes

namespace SvTrc
{
class TriggerRecordController final : public ITriggerRecordControllerRW
{
#pragma region Constructor
public:
	explicit TriggerRecordController(std::unique_ptr<DataControllerBase> pDataController);
	~TriggerRecordController();

	TriggerRecordController(const TriggerRecordController&) = delete;
	TriggerRecordController& operator=(const TriggerRecordController&) = delete;
#pragma endregion Constructor

#pragma region Public Methods
public:
	void setLastFinishedTr(TrEventData data);
	void increaseNumberOfFreeTr(int inspectionPos) { return m_pDataController->increaseNumberOfFreeTr(inspectionPos); };
	ImageBufferController& getImageBufferControllerInstance() { return m_imageBufferController; };

#pragma region ITriggerRecordControllerR Methods
	virtual const SvPb::InspectionList& getInspections() const override { return m_pDataController->getInspections(); }

	virtual bool isValid() const override { return 0 < getResetId(); };

	virtual int getLastTrId(int inspectionPos) const override { return m_pDataController->getLastTrId(inspectionPos); };

	virtual const SvPb::ImageList& getImageDefList(int inspectionPos) override;

	virtual const SvPb::DataDefinitionList& getDataDefList(int inspectionPos) override;

	virtual ITriggerRecordRPtr createTriggerRecordObject(int inspectionPos, int trId) override;
	virtual ITriggerRecordRPtr createTriggerRecordObjectPerTriggerCount(int inspectionPos, int triggerCount) override;

	virtual int registerResetCallback(std::function<void()> pCallback) override;
	virtual void unregisterResetCallback(int handleId) override;
	virtual int registerReadyCallback(std::function<void()> pCallback) override;
	virtual void unregisterReadyCallback(int handleId) override;
	virtual int registerNewTrCallback(std::function<void(TrEventData)> pCallback) override;
	virtual void unregisterNewTrCallback(int handleId) override;
	virtual int registerNewInterestTrCallback(std::function<void(std::vector<TrEventData>)> pCallback) override;
	virtual void unregisterNewInterestTrCallback(int handleId) override;

	virtual bool setTrsOfInterest(const std::vector<ITriggerRecordRPtr>& trVector) override;
	virtual std::vector<ITriggerRecordRPtr> getTrsOfInterest(int inspectionPos, int n) override;
	virtual void pauseTrsOfInterest(bool pauseFlag) override;
	virtual bool isPauseTrsOfInterest() const override { return m_pDataController->getPauseTrsOfInterest(); };
#pragma endregion ITriggerRecordControllerR Methods

#pragma region ITriggerRecordControllerRW Methods
	virtual void clearAll() override;

	virtual bool setInspections(SvPb::InspectionList&& rInspectionList) override;

	virtual void resizeIPNumberOfRecords(const std::vector<int>& inspectionPosVec, long newSizeTr, long newSizeTrOfIntereset) override;

	virtual ITriggerRecordRWPtr createTriggerRecordObjectToWrite(int inspectionPos) override;

	virtual ITriggerRecordRPtr closeWriteAndOpenReadTriggerRecordObject(ITriggerRecordRWPtr& pTriggerRecord) override;

	virtual void closeWriteObjectWithoutUpdateLastTrId(ITriggerRecordRWPtr& pTriggerRecord) override;

	virtual IImagePtr getImageBuffer(const SVMatroxBufferCreateStruct& bufferStruct, bool createBufferExternIfNecessary = false) const override;

	virtual void startResetTriggerRecordStructure(int inspectionPos = m_cResetStartedAddBuffer) override;
	virtual void finishResetTriggerRecordStructure() override;

	virtual void setGlobalInit() override;
	virtual void finishGlobalInit() override;

	virtual int addOrChangeImage(const GUID& rImageId, const SVMatroxBufferCreateStruct& rBufferStruct, int inspectionPos = -1) override;
	virtual int addOrChangeChildImage(const GUID& rImageId, const GUID& rParentId, const MatroxBufferChildDataStruct& rBufferStruct, int inspectionPos = -1) override;

	virtual void addImageBuffer(const GUID& ownerID, const SVMatroxBufferCreateStruct& bufferStruct, int numberOfBuffers, bool clearBuffer = false) override;
	virtual bool removeImageBuffer(const GUID& ownerID, const SVMatroxBufferCreateStruct& bufferStruct) override;
	virtual bool removeAllImageBuffer(const GUID& ownerID) override;
	virtual bool removeAllImageBuffer() override;

	virtual void changeDataDef(SvPb::DataDefinitionList&& rDataDefList, long valueObjectMemSize, int inspectionPos = -1) override;

	virtual bool lockReset() override;
	virtual void unlockReset() override { m_isResetLocked = false; };
	virtual bool isResetLocked() const override { return m_isResetLocked; };
	virtual bool isResetStarted() const override { return -1 != m_resetStarted4IP; };
#pragma endregion ITriggerRecordControllerRW Methods

	long getResetId() const { return m_pDataController->getResetId(); };

	volatile long* getResetLockCounterRef() { return m_pDataController->getResetLockCounterRef(); };

	bool isWritable() const { return m_pDataController->isWritable(); };

	static int needNumberOfTr(SvPb::Inspection ipData);
#pragma endregion Public Methods

#pragma region Private Methods
private:
	/// Recalculate the required buffer completely. 
	/// Normally it will only the required buffer fit depending of the changed image, but if went reset wrong, a recalculate should done, because the numbers can be wrong.
	void recalcRequiredBuffer();

	/// Reset the trigger record structure with the tmp-member-parameter.
	void ResetTriggerRecordStructure();

	/// Set "numberOfBuffersrequired" to pStructData if != nullptr or create new struct to m_imageStructListResetTmp and fill it. Add type and id to pImageDefinition.
	/// \param pStructData [in] image struct data if already available, else nullptr.
	/// \param pImageDefinition [in,out] Image definition object.
	/// \param typeStr [in] TypeStruct to set to image struct, if struct is new added.
	void setStructData(SvPb::ImageStructData* pStructData, SvPb::ImageDefinition* pImageDefinition, const std::string& typeStr);

	enum ResetEnum
	{
		Invalid,
		ResetRunning,
		NewReset
	};
	ResetEnum calcResetEnum(int inspectionPos);

	void sendResetCall();
	void sendReadyCall();
	void sendTrIdCall(TrEventData data);
	void sendInterestTrIdCall(std::vector<TrEventData> data);

	void reduceRequiredImageBuffer(const std::map<int, int>& bufferMap);
#pragma endregion Private Methods

#pragma region Member variables
private:
	//This Map contains per (non image- (e.g. AcqusitionObject))object a map of required buffers.
	//First parameter of the second map is the structID of the buffer and the second parameter is the required number.
	std::map<GUID, std::map<int, int>> m_additionalBufferMap;

	int m_resetStarted4IP = -1; //Position of IP with is in reset state. If m_resetStarted4IP == -1, no reset started.
	bool m_mustRecalcRequiredBuffers = false;
	SvPb::ImageStructList m_imageStructListResetTmp; //This ImageStructList is only temporary during reset process. In normal run don't use this.
	SvPb::ImageList m_imageListResetTmp; //This imageList is only temporary during reset process. In normal run don't use this.
	int m_TriggerRecordNumberResetTmp = 0; //This parameter is only temporary during reset process. In normal run don't use this.

	std::vector<std::pair<int, std::function<void()>>> m_resetCallbacks;
	std::vector<std::pair<int, std::function<void()>>> m_readyCallbacks;
	std::vector<std::pair<int, std::function<void(TrEventData)>>> m_newTrCallbacks;
	std::vector<std::pair<int, std::function<void(std::vector<TrEventData>)>>> m_newInterestTrCallbacks;
	bool m_isResetLocked = false;

	bool m_isGlobalInit = false;

	mutable std::mutex m_callbackMutex;

	std::unique_ptr<DataControllerBase> m_pDataController;
	ImageBufferController m_imageBufferController;
#pragma endregion Member variables
};



TriggerRecordController& getTriggerRecordControllerInstance();
ImageBufferController& getImageBufferControllerInstance();
} //namespace SvTrc
