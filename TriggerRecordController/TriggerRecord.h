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
#pragma endregion Includes

namespace SvTRC
{
	class TriggerRecord : public ITriggerRecordRW
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
			int m_referenceCount = m_InvalidData; //The ReferenceCount for this TriggerRecord. m_InvalidData = invalid; m_WriteBlocked = write blocked; 0 = readable, but can be "deleted"; >0 = readable and blocked 
			TriggerData m_triggerData;
		};
#pragma region Constructor
	public:
		TriggerRecord() = delete;
		~TriggerRecord();

	private:
		TriggerRecord(TriggerRecordData& data);
#pragma endregion Constructor

#pragma region Public Methods
	public:
#pragma region ITriggerRecordR Methods
		virtual int getId() const override { return m_data.m_trId; };

		virtual const TriggerData& getTriggerData() const override { return m_data.m_triggerData; }

		virtual IImagePtr getImage(const GUID& imageId) const override;
		virtual IImagePtr getImage(int pos) const override;
#pragma endregion ITriggerRecordR Methods

#pragma region ITriggerRecordRW Methods
		virtual void setTriggerData(const TriggerData& data) override { m_data.m_triggerData = data; };

		virtual void setImages(const ITriggerRecordR& rDestTR) override;

		virtual void setImage(int pos, int bufferPos) override;

		virtual IImagePtr createNewImageHandle(int pos) override;
#pragma endregion ITriggerRecordRW Methods

		const TriggerRecordData& getTRData() const { return m_data;	};
#pragma endregion Public Methods

#pragma region Member variables
	private:
		static const int m_InvalidTrId = -1;
		TriggerRecordData& m_data;
		static const int m_InvalidData = -2;
		static const int m_WriteBlocked = -1;
#pragma endregion Member variables
	};
} //namespace SvTRC