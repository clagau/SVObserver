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
class TRControllerBaseDataPerIP
{
public:
	TRControllerBaseDataPerIP() = default;
	virtual ~TRControllerBaseDataPerIP() = default;
	
	struct BasicData
	{
		bool m_bInit = false;
		int m_TriggerRecordNumber = 50 + cTriggerRecordAddOn;  //maximal number of trigger record to use
		int m_lastFinishedTRID {-1};
		int m_triggerRecordBufferSize {0}; //This is the size of the buffer reserved for one trigger Record.
		int m_dataListSize {0}; //data List byte size
	};

	virtual BasicData getBasicData() const = 0;
	virtual void setLastFinishedTRID(int id) = 0;
	virtual TriggerRecordData& getTRData(int pos) const = 0;
	virtual const SvPb::ImageList& getImageList() const = 0;

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

	virtual bool setInspections(const SvPb::InspectionList& rInspectionList) = 0;
	virtual long getResetId() const = 0;
	virtual long& getResetLockCounterRef() = 0;
	virtual const SvPb::InspectionList& getInspections() const = 0;

	void setLastFinishedTR(int inspectionPos, int id);
	int getLastTRId(int inspectionPos) const;

	const SvPb::ImageList& getImageDefList(int inspectionPos) const;
	virtual void setImageDefList(int inspectionPos, SvPb::ImageList&& imageList) = 0;

	virtual const SvPb::ImageStructList& getImageStructList() const = 0;
	/// Set the ImageStructList
	/// ATTENTION: Throw exception if ImageStructList cannot set (e.g. in writer object to large for the space in sharedMemory).
	/// \param list [in]
	virtual void setImageStructList(SvPb::ImageStructList list) = 0;

	virtual void resetImageRefCounter() = 0;

	int getTriggerRecordNumber(int inspectionPos) const;

	virtual void changeDataDef(SvPb::DataDefinitionList&& rDataDefList, std::vector<_variant_t>&& rValueObjectList, int inspectionPos) = 0;

	virtual ITriggerRecordRPtr createTriggerRecordObject(int inspectionPos, int trId) = 0;
	virtual ITriggerRecordRWPtr createTriggerRecordObjectToWrite(int inspectionPos) = 0;

	virtual std::vector<std::pair<int, int>> ResetTriggerRecordStructure(int inspectionId, int triggerRecordNumber, SvPb::ImageList imageList, SvPb::ImageStructList imageStructList) = 0;

	virtual long* getImageRefCountPtr(int pos) = 0;

	virtual void removeImageMemory(std::string memoryName) = 0;
	virtual int createMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos) = 0;
	virtual int contractMilBufferinMemory(int requiredNumbers, SvPb::ImageStructData& rImageStruct, int vectorPos) = 0;

	std::vector<SVMatroxBuffer>& getBufferVectorRef() { return m_bufferVector;	}

	int getMaxNumberOfRequiredBuffer() { return m_maxNumberOfRequiredBuffer; };
#pragma endregion Public Methods

#pragma region Protected Methods
	virtual TRControllerBaseDataPerIP* getTRControllerData(int inspectionId) = 0;
	virtual const TRControllerBaseDataPerIP* getTRControllerData(int inspectionId) const = 0;
#pragma endregion Protected Methods

#pragma region Member variables
protected:
	std::vector<SVMatroxBuffer> m_bufferVector;
	int m_maxNumberOfRequiredBuffer = 9000;
#pragma endregion Member variables
};
} //namespace SvTrc
