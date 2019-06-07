//*****************************************************************************
/// \copyright COPYRIGHT (c) 2018 by Seidenader Maschinenbau GmbH
/// \file DataControllerBase.h
/// All Rights Reserved
//*****************************************************************************
/// Base class to control of the Data.
//******************************************************************************
#pragma once

#pragma region Includes
#include "LocalConst.h"
#include "SVProtoBuf/TriggerRecordController.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#pragma endregion Includes


#pragma region Declarations
namespace SvTrc
{
struct TriggerRecordData;
class ITriggerRecordR;
class ITriggerRecordRW;
typedef std::shared_ptr< ITriggerRecordR > ITriggerRecordRPtr;
typedef std::shared_ptr< ITriggerRecordRW > ITriggerRecordRWPtr;
}
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
		volatile int m_TRofInterestNumber = 0;
		volatile long m_numberOfFreeTR = 0; // the number of free (unlocked) triggerRecords.
		volatile int m_lastFinishedTRID {-1};
		volatile int m_triggerRecordBufferSize {0}; //This is the size of the buffer reserved for one trigger Record.
		volatile int m_dataListSize {0}; //data List byte size
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
	virtual void setTRofInterestNumber(int number) { assert(false); throw E_NOTIMPL; };
	virtual void setTRofInterest(int inspectionPos, int pos) = 0;

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
	virtual void clearAll();

	TriggerRecordData& getTRData(int inspectionPos, int pos) const;

	virtual bool setInspections(const SvPb::InspectionList& rInspectionList) { assert(false); throw E_NOTIMPL; };
	virtual long getResetId() const = 0;
	virtual volatile long* getResetLockCounterRef() = 0;
	virtual const SvPb::InspectionList& getInspections() const = 0;

	void setLastFinishedTR(int inspectionPos, int id);
	int getLastTRId(int inspectionPos) const;

	const SvPb::ImageList& getImageDefList(int inspectionPos) const;
	virtual void setImageDefList(int inspectionPos, SvPb::ImageList&& imageList) { assert(false); throw E_NOTIMPL; };

	virtual const SvPb::ImageStructList& getImageStructList() const = 0;
	/// Set the ImageStructList
	/// ATTENTION: Throw exception if ImageStructList cannot set (e.g. in writer object to large for the space in sharedMemory).
	/// \param list [in]
	virtual void setImageStructList(SvPb::ImageStructList list) { assert(false); throw E_NOTIMPL; };

	virtual void resetImageRefCounter() { assert(false); throw E_NOTIMPL; };

	int getTriggerRecordNumber(int inspectionPos) const;

	const SvPb::DataDefinitionList& getDataDefList(int inspectionPos) const;
	virtual void changeDataDef(SvPb::DataDefinitionList&& rDataDefList, std::vector<_variant_t>&& rValueObjectList, int inspectionPos) { assert(false); throw E_NOTIMPL; };

	virtual ITriggerRecordRPtr createTriggerRecordObject(int inspectionPos, std::function<bool(TriggerRecordData&)> validFunc) = 0;
	virtual ITriggerRecordRWPtr createTriggerRecordObjectToWrite(int inspectionPos) { assert(false); throw E_NOTIMPL; };

	virtual std::vector<std::pair<int, int>> ResetTriggerRecordStructure(int inspectionId, int triggerRecordNumber, SvPb::ImageList imageList, SvPb::ImageStructList imageStructList) { assert(false); throw E_NOTIMPL; };

	virtual long* getImageRefCountPtr(int pos) = 0;

	virtual void removeImageMemory(std::string memoryName) { assert(false); throw E_NOTIMPL; };
	virtual int createMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos) { assert(false); throw E_NOTIMPL; };
	virtual int contractMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos) { assert(false); throw E_NOTIMPL; };

	std::vector<SVMatroxBuffer>& getBufferVectorRef() { return m_bufferVector; }

	int getMaxNumberOfRequiredBuffer() { return m_maxNumberOfRequiredBuffer; };

	virtual bool isWritable() const { return true; };

	void setResetCallback(std::function<void()>&& reloadCallback) { m_reloadCallback = reloadCallback; };
	void setReadyCallback(std::function<void()>&& readyCallback) { m_readyCallback = readyCallback; };
	void setNewTrIdCallback(std::function<void(int, int)>&& newTrIdCallback) { m_newTrIdCallback = newTrIdCallback; };

	/// Set the InspectionList
	/// \param rInspectionList [in]
	virtual void setInspectionList(const SvPb::InspectionList &rInspectionList) { assert(false); throw E_NOTIMPL; };

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
	/// \param trVec [in] The vector of pair<inspectionPos, pos> 
	bool setTRofInterest(std::vector<std::pair<int,int>> trVec);
#pragma endregion Public Methods

#pragma region Protected Methods
	virtual TRControllerBaseDataPerIP* getTRControllerData(int inspectionId) = 0;
	virtual const TRControllerBaseDataPerIP* getTRControllerData(int inspectionId) const = 0;
#pragma endregion Protected Methods

#pragma region Member variables
protected:
	bool m_isGlobalInit = false;

	std::vector<SVMatroxBuffer> m_bufferVector;
	int m_maxNumberOfRequiredBuffer = 9000;
	std::function<void()> m_reloadCallback;
	std::function<void()> m_readyCallback;
	std::function<void(int, int)> m_newTrIdCallback;
	HANDLE m_hResetEvent {nullptr};
	HANDLE m_hReadyEvent {nullptr};
	HANDLE m_hTridEvent {nullptr};
#pragma endregion Member variables
};
} //namespace SvTrc
