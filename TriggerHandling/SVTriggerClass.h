//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerClass
//* .File Name       : $Workfile:   SVTriggerClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 12:28:10  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "Definitions/TriggerType.h"
#include "SVOLibrary/SVODeviceClass.h"
#pragma endregion Includes

class SVIOTriggerLoadLibraryClass;

namespace SvTh
{
	class SVDigitizerLoadLibraryClass;

	struct AcquisitionParameter
	{
		bool operator==(const AcquisitionParameter& rRhs)
		{
			return (m_pDllDigitizer == rRhs.m_pDllDigitizer) && (m_triggerChannel == rRhs.m_triggerChannel);
		}

		SVDigitizerLoadLibraryClass* m_pDllDigitizer {nullptr};
		unsigned long m_triggerChannel {0UL};
	};

	class SVTriggerClass : public SVODeviceClass  
	{
	public:
		explicit SVTriggerClass(LPCTSTR deviceName);
		virtual ~SVTriggerClass();

		void addAcquisitionTrigger(SVDigitizerLoadLibraryClass* pDllDigitizer, unsigned long triggerChannel);
		void clearAcquisitionTriggers();
		void enableInternalTrigger() const;

		virtual HRESULT RegisterCallback( SVOCallbackPtr pCallback, 
										  void *pvOwner, void *pvCaller ) override;
		virtual HRESULT UnregisterCallback( SVOCallbackPtr pCallback,
											void *pvOwner, void *pvCaller ) override;
		virtual HRESULT UnregisterAllCallbacks() override;

		virtual HRESULT Start() override;
		virtual HRESULT Stop() override;

		void setDigitizerNumber(int digitizerNr) { m_digitizerNumber = digitizerNr; }
		int getDigitizerNumber() const { return m_digitizerNumber; }
		void setTriggerChannel(unsigned long triggerChannel) { m_triggerChannel = triggerChannel; }
		unsigned long getTriggerChannel() const { return m_triggerChannel; }

		void setDLLTrigger(SVIOTriggerLoadLibraryClass* pDllTrigger) { m_pDLLTrigger = pDllTrigger; }
		SVIOTriggerLoadLibraryClass* getDLLTrigger() { return m_pDLLTrigger; }

		SvDef::TriggerType getType() const { return m_type; }

	protected:
		virtual HRESULT processAcquisitionTriggers(const SVOResponseClass& rResponse) const override;

	private:
		int m_digitizerNumber {-1};

		SVIOTriggerLoadLibraryClass* m_pDLLTrigger{nullptr};
		unsigned long m_triggerChannel {0UL};
		SvDef::TriggerType m_type{SvDef::TriggerType::HardwareTrigger};

		std::vector<AcquisitionParameter> m_acqTriggerParameters;
#ifdef SV_LOG_STATUS_INFO
			std::vector<std::string> m_StatusLog;
		#endif
	};
} //namespace SvTh