//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file DataControllerBase.h
/// All Rights Reserved
//*****************************************************************************
/// Base class to control of the Data.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVProtoBuf/TriggerRecordController.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#pragma endregion Includes


#pragma region Declarations
namespace SvTrc
{
struct TriggerRecordData;
class ITriggerRecordR;
class ITriggerRecordRW;
struct TrEventData;
typedef std::shared_ptr< ITriggerRecordR > ITriggerRecordRPtr;
typedef std::shared_ptr< ITriggerRecordRW > ITriggerRecordRWPtr;
}

constexpr long long c_MBInBytes = 1048576LL;//1024 * 1024
#pragma endregion Declarations

namespace SvTrc
{
class Locker
{
public:
	explicit Locker(volatile long& rLocker)
		: m_rLocker(rLocker)
	{
		int count = 0;
		while (InterlockedCompareExchange(&rLocker, 1, 0) != 0)
		{
			std::this_thread::yield();
			count++;
			if (100 < count)
			{
				throw 0;
			}
		}
	}

	~Locker()
	{
		InterlockedExchange(&m_rLocker, 0l);
	}

	typedef std::unique_ptr<Locker> LockerPtr;
	static LockerPtr lockReset(volatile long& rLocker)
	{
		try
		{
			return std::make_unique<Locker>(rLocker);
		}
		catch (...)
		{
			return nullptr;
		}
	}

private:
	volatile long& m_rLocker;
};

class TRControllerBaseDataPerIP
{
public:
	TRControllerBaseDataPerIP() = default;
	virtual ~TRControllerBaseDataPerIP() = default;
	
	struct BasicData
	{
		volatile bool m_bInit = false;
		volatile int m_TriggerRecordNumber = 50;  //maximal number of trigger record to use
		volatile int m_TrOfInterestNumber = 0;
		volatile long m_numberOfFreeTR = 0; // the number of free (unlocked) triggerRecords.
		volatile int m_lastFinishedTRID {-1};
		volatile long m_triggerRecordBufferSize {0}; //This is the size of the buffer reserved for one trigger Record.
		volatile long m_dataListSize {0}; //data List byte size
		volatile long m_mutexTrOfInterest {false};
		volatile int m_TrOfInterestCurrentPos {-1};
	};

	virtual BasicData getBasicData() const = 0;
	virtual void setLastFinishedTRID(int id) = 0;
	virtual TriggerRecordData& getTRData(int pos) const = 0;
	virtual const SvPb::ImageList& getImageList() const = 0;
	virtual const SvPb::DataDefinitionList& getDataList() const = 0;
	virtual void resetFreeTrNumber() {};
	virtual void increaseFreeTrNumber() {};
	virtual void decreaseFreeTrNumber() {};
	virtual bool isEnoughFreeForLock() const { return true; };
	virtual void setTrOfInterestNumber(int number) { assert(false); throw E_NOTIMPL; };
	virtual void setTrOfInterest(int inspectionPos, int pos) = 0;

protected:
	int getNumberOfTRKeepFreeForWrite() const;
private:
};

class DataControllerBase
{
#pragma region Constructor
public:
	DataControllerBase();
	virtual ~DataControllerBase();

	DataControllerBase(const DataControllerBase&) = delete;
	DataControllerBase& operator=(const DataControllerBase&) = delete;
#pragma endregion Constructor

#pragma region Public Methods
public:
	virtual void clearImageBuffer(bool shouldResetImageStruct = false);
	virtual void clearAll();

	TriggerRecordData& getTRData(int inspectionPos, int pos) const;

	virtual bool setInspections(SvPb::InspectionList&& rInspectionList) { assert(false); throw E_NOTIMPL; };
	virtual long getResetId() const = 0;
	virtual volatile long* getResetLockCounterRef() = 0;
	virtual const SvPb::InspectionList& getInspections() const = 0;

	void setLastFinishedTr(const TrEventData& data);
	int getLastTrId(int inspectionPos) const;

	/// Get ImageDefList. ATTENTION: Throw exception if get imageDef failed.
	/// \param inspectionPos [in] Position of inspection.
	/// \param onlyIfInit [in] If true it checked if inspection is initialized and not initialized throw an exception. If false not check if initialized.
	/// \returns const SvPb::ImageList&
	const SvPb::ImageList& getImageDefList(int inspectionPos, bool onlyIfInit = true) const;
	virtual void setImageDefList(int inspectionPos, SvPb::ImageList&& imageList) { assert(false); throw E_NOTIMPL; };

	virtual const SvPb::ImageStructList& getImageStructList() const = 0;
	/// Set the ImageStructList
	/// ATTENTION: Throw exception if ImageStructList cannot set (e.g. in writer object to large for the space in sharedMemory).
	/// \param list [in]
	virtual void setImageStructList(SvPb::ImageStructList&& list) { assert(false); throw E_NOTIMPL; };

	virtual void resetImageRefCounter() { assert(false); throw E_NOTIMPL; };

	int getTriggerRecordNumber(int inspectionPos) const;

	const SvPb::DataDefinitionList& getDataDefList(int inspectionPos) const;
	virtual void changeDataDef(SvPb::DataDefinitionList&& rDataDefList, long valueObjectMemSize, int inspectionPos) { assert(false); throw E_NOTIMPL; };

	virtual ITriggerRecordRPtr createTriggerRecordObject(int inspectionPos, std::function<bool(TriggerRecordData&)> validFunc) = 0;
	virtual ITriggerRecordRWPtr createTriggerRecordObjectToWrite(int inspectionPos) { assert(false); throw E_NOTIMPL; };

	virtual std::vector<std::pair<int, int>> ResetTriggerRecordStructure(int inspectionId, int triggerRecordNumber, SvPb::ImageList&& rImageList, SvPb::ImageStructList&& rImageStructList) { assert(false); throw E_NOTIMPL; };

	virtual long* getImageRefCountPtr(int pos) = 0;

	virtual void removeImageMemory(std::string memoryName) { assert(false); throw E_NOTIMPL; };
	virtual int createMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos) { assert(false); throw E_NOTIMPL; };
	virtual int contractMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos) { assert(false); throw E_NOTIMPL; };

	std::vector<SVMatroxBuffer>& getBufferVectorRef() { return m_bufferVector; }

	int getMaxNumberOfRequiredBuffer() const { return m_maxNumberOfRequiredBuffer; };
	long long getMaxBufferSizeInBytes() const {	return m_maxBufferSizeInBytes; }

	virtual bool isWritable() const { return true; };

	void setResetCallback(std::function<void()>&& reloadCallback) { m_reloadCallback = reloadCallback; };
	void setReadyCallback(std::function<void()>&& readyCallback) { m_readyCallback = readyCallback; };
	void setNewTrIdCallback(std::function<void(TrEventData)>&& newTrIdCallback) { m_newTrIdCallback = newTrIdCallback; };
	void setNewInterestTrIdsCallback(std::function<void(std::vector<TrEventData>)>&& newTrIdCallback) { m_newInterestTrIdsCallback = newTrIdCallback; };

	/// Set the InspectionList
	/// \param rInspectionList [in]
	virtual void setInspectionList(SvPb::InspectionList&& rInspectionList) { assert(false); throw E_NOTIMPL; };

	bool isIPInit(int inspectionPos);

	/// Set the flag for global initialization. If this flag is set the writer version will not recreate every time the memory, but if finished the global initialization.
	/// \param flag [in] True, means global initialization is one.
	virtual void setGlobalInitFlag(bool flag) { m_isGlobalInit = flag; };

	/// Reset resetId to 0 and wait until all reader finished his function.
	virtual void prepareReset();
	/// Set resetId to a new number and send reset event.
	virtual void finishedReset() { assert(false); throw E_NOTIMPL; };

	virtual void setPauseTrsOfInterest(bool flag) = 0;
	virtual bool getPauseTrsOfInterest() const = 0;
	virtual std::vector<ITriggerRecordRPtr> getTRsOfInterest(int inspectionPos, int n) = 0;

	void increaseNumberOfFreeTr(int inspectionPos);

	/// Set the TRs to the Interest list.
	/// \param rTrVec [in] The vector of pair<inspectionPos, pos> 
	bool setTrOfInterest(const std::vector<std::pair<int,int>>& rTrVec);

	void setIpToInitFlagList(int pos) { m_setInitFlagAfterResetSet.insert(pos); };
#pragma endregion Public Methods

#pragma region Protected Methods
	virtual TRControllerBaseDataPerIP* getTRControllerData(int inspectionId) = 0;
	virtual const TRControllerBaseDataPerIP* getTRControllerData(int inspectionId) const = 0;
#pragma endregion Protected Methods

#pragma region Member variables
protected:
	//GlobalInit and m_setInitFlagAfterResetSet will only work for Writer (for Local it is not implemented)
	bool m_isGlobalInit = false;
	std::unordered_set<int> m_setInitFlagAfterResetSet;

	std::vector<SVMatroxBuffer> m_bufferVector;
	int m_maxNumberOfRequiredBuffer = 9000;
	long long m_maxBufferSizeInBytes = 8'000 * c_MBInBytes;
	std::function<void()> m_reloadCallback;
	std::function<void()> m_readyCallback;
	std::function<void(TrEventData)> m_newTrIdCallback;
	std::function<void(std::vector<TrEventData>)> m_newInterestTrIdsCallback;
	HANDLE m_hResetEvent {nullptr};
	HANDLE m_hReadyEvent {nullptr};
	HANDLE m_hInterestTridEvent {nullptr};
#pragma endregion Member variables
};
} //namespace SvTrc
