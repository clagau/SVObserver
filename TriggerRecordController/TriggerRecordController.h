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
#pragma warning( push )
#pragma warning( disable : 4800 ) 
#include "SVProtoBuf\TriggerRecordController.pb.h"
#pragma warning( pop )
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
		virtual int getLastTRId() override { return m_lastTRID; }

		virtual const SvPB::ImageList& GetImageDefList() override;

		virtual ITriggerRecordRPtr CreateTriggerRecordObject(int trId) override;

		virtual ITriggerRecordRWPtr CreateTriggerRecordObjectToWrite() override;

		virtual bool StartResetTriggerRecordStructure(int TriggerRecordSize) override;

		virtual bool AddImage2TriggerRecordStructure(const GUID& rImageId, SVMatroxBufferCreateStruct bufferStruct) override;

		virtual bool FinishResetTriggerRecordStructure() override;
#pragma endregion Public Methods

#pragma region Private Methods
	private:
		TriggerRecord::TriggerRecordData& getTRData(int pos) const;
#pragma endregion Private Methods

#pragma region Member variables
	private:
		const int m_TriggerRecordAddOn = 2; //number of additional slots for internal use
		const int m_maxTriggerRecords = 100;
		int m_TriggerRecordNumber = 50 + m_TriggerRecordAddOn;  //maximal number of trigger record to use
		bool m_bInit = false;
		bool m_bReset = false;
		void* m_triggerRecords = nullptr;
		int m_triggerRecordBufferSize = 0; //This is the size of the buffer reservered for one trigger Record.
		int m_nextPosForFreeCheck = 0; //This variable contains on which position it should be started for searching for a free slot.
		int m_nextTRID = 0;
		int m_lastTRID = -1;
		SvPB::ImageSizeList m_imageSizeListResetTmp; //This imageSizeList is only temporary during reset process. In normal run don't use this, but use it from ImageBufferController.
		SvPB::ImageList m_imageListResetTmp; //This imageList is only temporary during reset process. In normal run don't use this, but use it from ImageBufferController.
#pragma endregion Member variables
	};
} //namespace SvTRC