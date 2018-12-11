//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file TriggerRecord.h
/// All Rights Reserved
//*****************************************************************************
/// The class have access to a TriggerRecord.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ITriggerRecordR.h"
#include "ITriggerRecordRW.h"
#include "SVProtoBuf/TriggerRecordController.h"
#pragma endregion Includes

namespace SvTrc
{
class TriggerRecord final : public ITriggerRecordRW
{
	friend class TriggerRecordController;

	struct TriggerRecordData
	{
		//All constructor are deleted, because this struct should not create with the normal way. 
		// It must be allocated a buffer with space behind this struct for further data (image position and data)
		TriggerRecordData() = delete;
		TriggerRecordData(const TriggerRecordData&) = delete;
		TriggerRecordData& operator = (const TriggerRecordData&) = delete;

		void init(int imageCount)
		{
			m_trId = cInvalidTrId;
			m_referenceCount = cInvalidData;
			m_ImageCount = imageCount;
			m_triggerData = TriggerData();
		}

		int* const getImagePos()
		{
			return reinterpret_cast<int*const>(reinterpret_cast<char*>(this) + sizeof(TriggerRecordData));
		}

		const int* const getImagePos() const
		{
			return reinterpret_cast<const int*const>(reinterpret_cast<const char*>(this) + sizeof(TriggerRecordData));
		}

		void* getValueData()
		{
			byte* pResult = reinterpret_cast<byte*>(this) + sizeof(TriggerRecordData);
			pResult += sizeof(int) * m_ImageCount;
			return reinterpret_cast<void*> (pResult);
		}

		int m_trId = cInvalidTrId; //This should be unique ID for the trigger record
		long m_referenceCount = cInvalidData; //The ReferenceCount for this TriggerRecord. m_InvalidData = invalid; m_WriteBlocked = write blocked; 0 = readable, but can be "deleted"; >0 = readable and blocked 
		int m_ImageCount {0};
		TriggerData m_triggerData;
	};
#pragma region Constructor
public:
	TriggerRecord(int inspectionPos, TriggerRecordData& rData, const SvPb::ImageList& rImageList, const SvPb::DataDefinitionList& rDataDefList, const int& rDataListSize, long resetId);
	TriggerRecord() = delete;
	~TriggerRecord();
#pragma endregion Constructor

#pragma region Public Methods
public:
#pragma region ITriggerRecordR Methods
	virtual int getId() const override { return m_rData.m_trId; };
	virtual int getInspectionPos() const override { return m_inspectionPos; };

	virtual const TriggerData& getTriggerData() const override { return m_rData.m_triggerData; }

	virtual IImagePtr getImage(const GUID& imageId, bool lockImage = false) const override;
	virtual IImagePtr getImage(int pos, bool lockImage = false) const override;
	virtual IImagePtr getChildImage(int childPos, bool lockImage = false) const override;

	virtual _variant_t getDataValue(const GUID& dataId) const override;

	virtual bool isValueDataValid() const override;
	virtual bool isObjectUpToTime() const override;
#pragma endregion ITriggerRecordR Methods

#pragma region ITriggerRecordRW Methods
	virtual void setTriggerData(const TriggerData& data) override { m_rData.m_triggerData = data; };

	virtual void initImages() override;
	virtual void setImages(const ITriggerRecordR& rDestTR) override;

	virtual void setImage(const GUID& rImageId, const IImagePtr& pImage) override;
	virtual void setImage(int pos, const IImagePtr& pImage) override;
	virtual void setImage(const GUID& rImageId, int bufferPos) override;
	virtual void setImage(int pos, int bufferPos) override;

	virtual IImagePtr createNewImageHandle(const GUID& imageId) override;
	virtual IImagePtr createNewImageHandle(int pos) override;

	virtual void writeValueData(std::vector<_variant_t>&& rValueObjectList) override;
#pragma endregion ITriggerRecordRW Methods

	const TriggerRecordData& getTRData() const { return m_rData; };
#pragma endregion Public Methods

#pragma region Private Methods
private:
#pragma endregion Private Methods

#pragma region Member variables
private:
	static const int cInvalidTrId = -1;
	static const int cInvalidData = -1;
	static const int cWriteBlocked = -2;

	TriggerRecordData& m_rData;
	const SvPb::ImageList& m_rImageList;
	const SvPb::DataDefinitionList& m_rDataDefList;
	const int& m_rDataListSize;
	const long m_ResetId{0L};
	int m_inspectionPos{-1};
#pragma endregion Member variables
};

template<typename Container>
int findGuidPos(const Container& rContainer, const std::string& rGuidIdBytes)
{
	int pos = -1;
	auto imageIter = std::find_if(rContainer.begin(), rContainer.end(), [&rGuidIdBytes](auto data)->bool
	{
		return (0 == data.guidid().compare(rGuidIdBytes));
	});
	if (rContainer.end() != imageIter)
	{
		pos = static_cast<int>(std::distance(rContainer.begin(), imageIter));
	}

	return pos;
}
} //namespace SvTrc