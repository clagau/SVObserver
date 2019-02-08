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
#include "TriggerRecord.h"
#include "SVProtoBuf/TriggerRecordController.h"
#pragma endregion Includes

namespace SvTrc
{
struct StartCallbackStruct
{
	StartCallbackStruct(void* pOwner, StartResetCallbackPtr pCallback) : m_pOwner(pOwner), m_pCallback(pCallback) {};
	void* m_pOwner;
	StartResetCallbackPtr m_pCallback;
};

class TriggerRecordController final : public ITriggerRecordControllerRW
{
#pragma region Constructor
public:
	static TriggerRecordController& getTriggerRecordControllerInstance();

private:
	TriggerRecordController();
	~TriggerRecordController();

	TriggerRecordController(const TriggerRecordController&) = delete;
	TriggerRecordController& operator=(const TriggerRecordController&) = delete;
#pragma endregion Constructor

#pragma region Public Methods
public:
	void setLastFinishedTR(int inspectionPos, int id);

#pragma region ITriggerRecordControllerR Methods
	virtual const SvPb::InspectionList& getInspections() const override { return m_inspectionList; }

	virtual int getLastTRId(int inspectionPos) const override;

	virtual const SvPb::ImageList& getImageDefList(int inspectionPos) override;

	virtual ITriggerRecordRPtr createTriggerRecordObject(int inspectionPos, int trId) override;

	virtual void registerCallback(void* pOwner, StartResetCallbackPtr pCallback) override;
	virtual void unregisterCallback(void* pOwner, StartResetCallbackPtr pCallback) override;

	virtual bool isResetStarted() const override { return -1 != m_resetStarted4IP; };
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

	virtual void changeDataDef(SvPb::DataDefinitionList&& rDataDefList, std::vector<_variant_t>&& rValueObjectList, int inspectionPos = -1) override;

	virtual bool lockReset() override;
	virtual void unlockReset() override { m_isResetLocked = false; };
	virtual bool isResetLocked() const override { return m_isResetLocked; };
#pragma endregion ITriggerRecordControllerRW Methods

	long getResetId() { return m_resetId; };

	long& getResetLockCounterRef() { return m_resetLockCounter; };
#pragma endregion Public Methods

#pragma region Private Methods
private:
	TriggerRecord::TriggerRecordData& getTRData(int inspectionPos, int pos) const;

	/// Reset all data for one Inspection and clear data.
	/// \param inspectionID [in]
	void ResetInspectionData(int inspectionID);

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
#pragma endregion Private Methods

#pragma region Member variables
public:
	static const int cMaxTriggerRecords = 305;

private:
	static const int m_TriggerRecordAddOn = 2; //number of additional slots for internal use
	
	struct TRControllerDataPerIP
	{
		~TRControllerDataPerIP()
		{
			if(nullptr != m_pTriggerRecords)
			{
				::free(m_pTriggerRecords);
			}
		}

		bool m_bInit = false;
		int m_TriggerRecordNumber = 50 + m_TriggerRecordAddOn;  //maximal number of trigger record to use
		int m_lastFinishedTRID{-1};
		int m_lastStartedTRID{-1};
		int m_triggerRecordBufferSize{0}; //This is the size of the buffer reserved for one trigger Record.
		int m_nextPosForFreeCheck{0}; //This variable contains which position it should start searching for a free slot.
		int m_dataListSize{0}; //data List byte size
		SvPb::ImageList m_ImageList;
		SvPb::DataDefinitionList m_DataDefList;
		void* m_pTriggerRecords = nullptr;
	};

	TRControllerDataPerIP* m_pData = nullptr;
	int m_IPDataNumber = 0; //number of IP data blocks (should be the same as number of IPs)
	int m_nextTRID = 0; //the next ID for next created trigger record.
	long m_resetId = 0; //id of the current resetStruct
	long m_lastResetId = 0; //id of the last resetId
	long m_resetLockCounter = 0; //counter of current used methods of ITriggerRecordR-instance 
	SvPb::InspectionList m_inspectionList;

	//This Map contains per (non image- (e.g. AcqusitionObject))object a map of required buffers.
	//First parameter of the second map is the structID of the buffer and the second parameter is the required number.
	std::map<GUID, std::map<int, int>> m_additionalBufferMap;

	int m_resetStarted4IP = -1; //Position of IP with is in reset state. If m_resetStarted4IP == -1, no reset started.
	SvPb::ImageStructList m_imageStructListResetTmp; //This ImageStructList is only temporary during reset process. In normal run don't use this.
	SvPb::ImageList m_imageListResetTmp; //This imageList is only temporary during reset process. In normal run don't use this.
	int m_TriggerRecordNumberResetTmp = 0; //This parameter is only temporary during reset process. In normal run don't use this.

	std::vector<StartCallbackStruct> m_ResetCallbacks;
	bool m_isResetLocked = false;

	mutable CRITICAL_SECTION m_hCriticalSectionCallback;
#pragma endregion Member variables
};
} //namespace SvTrc
