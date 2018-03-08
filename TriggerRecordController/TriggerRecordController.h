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

namespace SvTRC
{
	class TriggerRecordController : public ITriggerRecordControllerRW
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

#pragma region ITriggerRecordControllerR Methods
		virtual const SvPB::InspectionList& getInspections() const override { return m_inspectionList; }

		virtual int getLastTRId(int inspectionPos) const override;

		virtual const SvPB::ImageList& GetImageDefList(int inspectionPos) override;

		virtual ITriggerRecordRPtr CreateTriggerRecordObject(int inspectionPos, int trId) override;
#pragma endregion ITriggerRecordControllerR Methods

#pragma region ITriggerRecordControllerRW Methods
		virtual void setInspections(const SvPB::InspectionList& rInspectionList) override;

		virtual ITriggerRecordRWPtr CreateTriggerRecordObjectToWrite(int inspectionPos) override;

		virtual bool StartResetTriggerRecordStructure(int inspectionPos, int TriggerRecordSize) override;

		virtual bool AddImage2TriggerRecordStructure(const GUID& rImageId, SVMatroxBufferCreateStruct bufferStruct) override;

		virtual bool FinishResetTriggerRecordStructure() override;
#pragma endregion ITriggerRecordControllerRW Methods

		time_t getResetTime() { return m_resetTime; };

		long& getResetLockCounterRef() { return m_resetLockCounter; };
#pragma endregion Public Methods

#pragma region Private Methods
	private:
		TriggerRecord::TriggerRecordData& getTRData(int inspectionPos, int pos) const;

		/// Reset all data for one Inspection and clear data.
		/// \param inspectionID [in]
		void ResetInspectionData(int inspectionID);
#pragma endregion Private Methods

#pragma region Member variables
	private:
		static const int m_TriggerRecordAddOn = 2; //number of additional slots for internal use
		static const int m_maxTriggerRecords = 100;

		struct TRControllerDataPerIP
		{
			bool m_bInit = false;
			int m_TriggerRecordNumber = 50 + m_TriggerRecordAddOn;  //maximal number of trigger record to use
			int m_lastTRID = -1;
			int m_triggerRecordBufferSize = 0; //This is the size of the buffer reserved for one trigger Record.
			int m_nextPosForFreeCheck = 0; //This variable contains on which position it should be started for searching for a free slot.
			SvPB::ImageList* m_pImageList = nullptr;
			void* m_pTriggerRecords = nullptr;
		};
		
		TRControllerDataPerIP* m_pData = nullptr;
		int m_IPDataNumber = 0; //number of IP data blocks (should be the same as number of IPs)
		int m_nextTRID = 0; //the next ID for next created trigger record.
		time_t m_resetTime = 0; //time of the last reset
		long m_resetLockCounter = 0; //counter of current used methods of ITriggerRecordR-instance 
		SvPB::InspectionList m_inspectionList;

		int m_resetStarted4IP = -1; //Position of IP with is in reset state. If m_resetStarted4IP == -1, no reset started.
		SvPB::ImageStructList m_imageStructListResetTmp; //This ImageStructList is only temporary during reset process. In normal run don't use this.
		SvPB::ImageList m_imageListResetTmp; //This imageList is only temporary during reset process. In normal run don't use this.
		int m_TriggerRecordNumberResetTmp = 0; //This parameter is only temporary during reset process. In normal run don't use this.
#pragma endregion Member variables
	};
} //namespace SvTRC