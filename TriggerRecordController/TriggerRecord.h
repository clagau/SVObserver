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
#include "TriggerRecordData.h"
#include "SVProtoBuf/TriggerRecordController.h"
#pragma endregion Includes

namespace SvTrc
{
class TriggerRecord final : public ITriggerRecordRW
{
#pragma region Constructor
public:
	TriggerRecord(int inspectionPos, TriggerRecordData& rData, const SvPb::ImageList& rImageList, const SvPb::DataDefinitionList& rDataDefList, int dataListSize, long resetId);
	TriggerRecord() = delete;
	~TriggerRecord();
#pragma endregion Constructor

#pragma region Public Methods
public:
#pragma region ITriggerRecordR Methods
	virtual int getId() const override;
	virtual int getInspectionPos() const override { return m_inspectionPos; };

	virtual const TriggerData& getTriggerData() const override;

	virtual IImagePtr getImage(const GUID& imageId, bool lockImage = false) const override;
	virtual IImagePtr getImage(int pos, bool lockImage = false) const override;
	virtual IImagePtr getChildImage(int childPos, bool lockImage = false) const override;

	virtual _variant_t getDataValue(const GUID& dataId) const override;
	virtual _variant_t getDataValue(int pos) const override;

	virtual bool isValueDataValid() const override;
	virtual bool isObjectUpToTime() const override;
#pragma endregion ITriggerRecordR Methods

#pragma region ITriggerRecordRW Methods
	virtual void setTriggerData(const TriggerData& data) override;

	virtual void initImages() override;
	virtual void setImages(const ITriggerRecordR& rDestTR) override;

	virtual void setImage(const GUID& rImageId, const IImagePtr& pImage) override;
	virtual void setImage(int pos, const IImagePtr& pImage) override;
	virtual void setImage(const GUID& rImageId, int bufferPos) override;
	virtual void setImage(int pos, int bufferPos) override;

	virtual IImagePtr createNewImageHandle(const GUID& imageId) override;
	virtual IImagePtr createNewImageHandle(int pos) override;

	virtual void initValueData() override;
	virtual void writeValueData(std::vector<_variant_t>&& rValueObjectList) override;
#pragma endregion ITriggerRecordRW Methods

#pragma endregion Public Methods

#pragma region Private Methods
private:

	const TriggerRecordData& getTRData() const { return m_rData; };
#pragma endregion Private Methods

#pragma region Member variables
private:
	TriggerRecordData& m_rData;
	const SvPb::ImageList& m_rImageList;
	const SvPb::DataDefinitionList& m_rDataDefList;
	const int m_dataListSize;
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