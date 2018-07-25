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
		// It must be allocated a buffer with space behind this struct for further data (image position)
		TriggerRecordData() = delete;
		TriggerRecordData(const TriggerRecordData&) = delete;
		TriggerRecordData& operator = (const TriggerRecordData&) = delete;

		void init()
		{
			m_trId = m_InvalidTrId;
			m_referenceCount = m_InvalidData;
			m_triggerData = TriggerData();
		}

		int*const getImagePos()
		{
			return reinterpret_cast<int*const>(reinterpret_cast<char*>(this) + sizeof(TriggerRecordData));
		}

		const int*const getImagePos() const
		{
			return reinterpret_cast<const int*const>(reinterpret_cast<const char*>(this) + sizeof(TriggerRecordData));
		}

		int m_trId = m_InvalidTrId; //This should be unique ID for the trigger record
		long m_referenceCount = m_InvalidData; //The ReferenceCount for this TriggerRecord. m_InvalidData = invalid; m_WriteBlocked = write blocked; 0 = readable, but can be "deleted"; >0 = readable and blocked 
		TriggerData m_triggerData;
	};
#pragma region Constructor
public:
	TriggerRecord() = delete;
	~TriggerRecord();

private:
	TriggerRecord(int inspectionPos, TriggerRecordData& rData, const SvPb::ImageList& rImageList, const long& rResetId);
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
#pragma endregion ITriggerRecordRW Methods

	const TriggerRecordData& getTRData() const { return m_rData; };
#pragma endregion Public Methods

#pragma region Private Methods
private:
#pragma endregion Private Methods

#pragma region Member variables
private:
	static const int m_InvalidTrId = -1;
	TriggerRecordData& m_rData;
	const SvPb::ImageList& m_rImageList;
	const long m_ResetId = 0;
	int m_inspectionPos = -1;

	static const int m_InvalidData = -1;
	static const int m_WriteBlocked = -2;
#pragma endregion Member variables
};

template<typename Container>
int findImagePos(const Container& rContainer, const std::string& rImageIdBytes)
{
	int pos = -1;
	auto imageIter = std::find_if(rContainer.begin(), rContainer.end(), [&rImageIdBytes](auto data)->bool
	{
		return (0 == data.imageid().compare(rImageIdBytes));
	});
	if (rContainer.end() != imageIter)
	{
		pos = static_cast<int>(std::distance(rContainer.begin(), imageIter));
	}

	return pos;
}
} //namespace SvTrc