//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017 by Seidenader Maschinenbau GmbH
/// \file TriggerRecord.h
/// All Rights Reserved
//*****************************************************************************
/// The class have access to a TriggerRecord.
//******************************************************************************
#pragma once

#pragma region Includes
#include "ObjectInterfaces/ITriggerRecordR.h"
#include "ObjectInterfaces/ITriggerRecordRW.h"
#pragma endregion Includes

namespace SvPb
{
class DataDefinitionList;
class ImageList;
}

namespace SvTrc
{
struct TriggerRecordData;

class TriggerRecord final : public SvOi::ITriggerRecordRW
{
#pragma region Constructor
public:
	TriggerRecord(int inspectionPos, int trPos, TriggerRecordData& rData, const SvPb::ImageList& rImageList, const SvPb::DataDefinitionList& rDataDefList, long dataListSize, long resetId);
	TriggerRecord() = delete;
	~TriggerRecord();
#pragma endregion Constructor

#pragma region Public Methods
public:
#pragma region ITriggerRecordR Methods
	virtual int getId() const override;
	virtual int getInspectionPos() const override { return m_inspectionPos; };

	virtual const SvOi::TriggerData& getTriggerData() const override;

	virtual SvOi::ITRCImagePtr getImage(int pos, bool lockImage = false) const override;
	virtual SvOi::ITRCImagePtr getChildImage(int childPos, bool lockImage = false) const override;

	virtual _variant_t getDataValue(int pos) const override;

	virtual bool isValueDataValid() const override;
	virtual bool isObjectUpToTime() const override;

	virtual long getResetId() const override { return m_ResetId; };
#pragma endregion ITriggerRecordR Methods

#pragma region ITriggerRecordRW Methods
	virtual void setTriggerData(const SvOi::TriggerData& data) override;

	virtual void initImages() override;
	virtual void setImages(const ITriggerRecordR& rDestTr) override;

	virtual void setImage(int pos, const SvOi::ITRCImagePtr& pImage) override;
	virtual void setImage(int pos, int bufferPos) override;

	virtual SvOi::ITRCImagePtr createNewImageHandle(int pos) override;

	virtual void initValueData() override;
	virtual void writeValueData(const uint8_t* pMemSource, int32_t memBytes) override;
#pragma endregion ITriggerRecordRW Methods

	void blockUpdateLastTrId() { m_blockUpdateLastId = true; };
	bool isBlockUpdateLastTrId() { return m_blockUpdateLastId; };
	int getTrPos() { return m_trPos; };
#pragma endregion Public Methods

#pragma region Private Methods
private:
	const TriggerRecordData& getTrData() const { return m_rData; };

	_variant_t readValue(VARTYPE vtType, const BYTE* pMemBlock) const;
	_variant_t readArrayValue(VARTYPE vtType, const BYTE* pMemBlock) const;
	int getElementSize(VARTYPE vtType) const;
#pragma endregion Private Methods

#pragma region Member variables
private:
	TriggerRecordData& m_rData;
	const SvPb::ImageList& m_rImageList;
	const SvPb::DataDefinitionList& m_rDataDefList;
	const long m_dataListSize;
	const long m_ResetId{0L};
	const int m_inspectionPos{-1};
	const int m_trPos {-1};
	bool m_blockUpdateLastId = false;
#pragma endregion Member variables
};

void removeTrReferenceCount(int ipPos, long& rReferenceCount);
} //namespace SvTrc