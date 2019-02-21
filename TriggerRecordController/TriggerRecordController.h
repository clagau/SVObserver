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
	TriggerRecordController(std::unique_ptr<DataControllerBase> pDataController);
	~TriggerRecordController();

	TriggerRecordController(const TriggerRecordController&) = delete;
	TriggerRecordController& operator=(const TriggerRecordController&) = delete;
#pragma endregion Constructor

#pragma region Public Methods
public:
	void setLastFinishedTR(int inspectionPos, int id);
	ImageBufferController& getImageBufferControllerInstance() { return m_imageBufferController; };

#pragma region ITriggerRecordControllerR Methods
	virtual const SvPb::InspectionList& getInspections() const override { return m_pDataController->getInspections(); }

	virtual int getLastTRId(int inspectionPos) const override { return m_pDataController->getLastTRId(inspectionPos); };

	virtual const SvPb::ImageList& getImageDefList(int inspectionPos) override;

	virtual const SvPb::DataDefinitionList& getDataDefList(int inspectionPos) override;

	virtual ITriggerRecordRPtr createTriggerRecordObject(int inspectionPos, int trId) override;

	virtual int registerResetCallback(std::function<void()> pCallback) override;
	virtual void unregisterResetCallback(int handleId) override;
	virtual int registerNewTrCallback(std::function<void(int, int)> pCallback) override;
	virtual void unregisterNewTrCallback(int handleId) override;
#pragma endregion ITriggerRecordControllerR Methods

#pragma region ITriggerRecordControllerRW Methods
	virtual void clearAll() override;

	virtual bool setInspections(const SvPb::InspectionList& rInspectionList) override;

	virtual ITriggerRecordRWPtr createTriggerRecordObjectToWrite(int inspectionPos) override;

	virtual ITriggerRecordRPtr closeWriteAndOpenReadTriggerRecordObject(ITriggerRecordRWPtr& pTriggerRecord) override;

	virtual IImagePtr getImageBuffer(const SVMatroxBufferCreateStruct& bufferStruct, bool createBufferExternIfNecessary = false) const override;

	virtual void startResetTriggerRecordStructure(int inspectionPos = m_cResetStartedAddBuffer) override;
	virtual void finishResetTriggerRecordStructure() override;

	virtual int addOrChangeImage(const GUID& rImageId, const SVMatroxBufferCreateStruct& rBufferStruct, int inspectionPos = -1) override;
	virtual int addOrChangeChildImage(const GUID& rImageId, const GUID& rParentId, const MatroxBufferChildDataStruct& rBufferStruct, int inspectionPos = -1) override;

	virtual bool addImageBuffer(const GUID& ownerID, const SVMatroxBufferCreateStruct& bufferStruct, int numberOfBuffers) override;
	virtual bool removeImageBuffer(const GUID& ownerID, const SVMatroxBufferCreateStruct& bufferStruct) override;
	virtual bool removeAllImageBuffer(const GUID& ownerID) override;
	virtual bool removeAllImageBuffer() override;

	virtual void changeDataDef(SvPb::DataDefinitionList&& rDataDefList, std::vector<_variant_t>&& rValueObjectList, int inspectionPos = -1) override;

	virtual bool lockReset() override;
	virtual void unlockReset() override { m_isResetLocked = false; };
	virtual bool isResetLocked() const override { return m_isResetLocked; };
	virtual bool isResetStarted() const override { return -1 != m_resetStarted4IP; };
#pragma endregion ITriggerRecordControllerRW Methods

	long getResetId() const { return m_pDataController->getResetId(); };

	long* getResetLockCounterRef() { return m_pDataController->getResetLockCounterRef(); };

	bool isWritable() const { return m_pDataController->isWritable(); };
#pragma endregion Public Methods

#pragma region Private Methods
private:
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

	void sendTrIdCall(int inspectionPos, int trId);

	void reduceRequiredImageBuffer(const std::map<int, int>& bufferMap);
#pragma endregion Private Methods

#pragma region Member variables
public:
	static const int cMaxTriggerRecords = 305;

private:
	//This Map contains per (non image- (e.g. AcqusitionObject))object a map of required buffers.
	//First parameter of the second map is the structID of the buffer and the second parameter is the required number.
	std::map<GUID, std::map<int, int>> m_additionalBufferMap;

	int m_resetStarted4IP = -1; //Position of IP with is in reset state. If m_resetStarted4IP == -1, no reset started.
	SvPb::ImageStructList m_imageStructListResetTmp; //This ImageStructList is only temporary during reset process. In normal run don't use this.
	SvPb::ImageList m_imageListResetTmp; //This imageList is only temporary during reset process. In normal run don't use this.
	int m_TriggerRecordNumberResetTmp = 0; //This parameter is only temporary during reset process. In normal run don't use this.

	std::vector<std::pair<int, std::function<void()>>> m_resetCallbacks;
	std::vector<std::pair<int, std::function<void(int, int)>>> m_newTRCallbacks;
	bool m_isResetLocked = false;

	mutable std::mutex m_callbackMutex;

	std::unique_ptr<DataControllerBase> m_pDataController;
	ImageBufferController m_imageBufferController;
#pragma endregion Member variables
};



TriggerRecordController& getTriggerRecordControllerInstance();
ImageBufferController& getImageBufferControllerInstance();
} //namespace SvTrc
