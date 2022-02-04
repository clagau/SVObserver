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
#include "TriggerDevice.h"
#include "SVTriggerInfoStruct.h"
#pragma endregion Includes


namespace SvTrig
{
class SVIOTriggerLoadLibraryClass;
class SVDigitizerLoadLibraryClass;

struct AcquisitionParameter
{
	bool operator==(const AcquisitionParameter& rRhs)
	{
		bool result{ m_pDllDigitizer == rRhs.m_pDllDigitizer };
		result &= (m_triggerChannel == rRhs.m_triggerChannel);
		//m_active needs not to be the same
		return result;
	}

	SVDigitizerLoadLibraryClass* m_pDllDigitizer {nullptr};
	unsigned long m_triggerChannel {0UL};
	bool m_active{ false };
};

class SVTriggerClass : public TriggerDevice  
{
public:
	explicit SVTriggerClass(LPCTSTR deviceName);
	virtual ~SVTriggerClass() = default;

	void __stdcall triggerCallback(SvTrig::TriggerData&& triggerData);

	void addAcquisitionTrigger(AcquisitionParameter&& acqParameter);
	void clearAcquisitionTriggers();
	const std::vector<AcquisitionParameter>& getAcquisitionTriggers()const  { return m_acqTriggerParameters; }
	void setTriggerType(bool isTestMode) const;

	virtual HRESULT RegisterCallback(PpqTriggerCallBack pPpqTriggerCallback) override;
	virtual HRESULT UnregisterCallback() override;

	virtual HRESULT Start() override;
	virtual HRESULT Stop() override;

	void setDigitizerNumber(int digitizerNr) { m_digitizerNumber = digitizerNr; }
	int getDigitizerNumber() const { return m_digitizerNumber; }
	void setTriggerChannel(unsigned long triggerChannel) { m_triggerChannel = triggerChannel; }
	unsigned long getTriggerChannel() const { return m_triggerChannel; }

	void setDLLTrigger(SVIOTriggerLoadLibraryClass* pDllTrigger) { m_pDLLTrigger = pDllTrigger; }
	SVIOTriggerLoadLibraryClass* getDLLTrigger() { return m_pDLLTrigger; }

	SvDef::TriggerType getType() const { return m_type; }
	void setPause(bool pauseState);
	bool getPause() const { return m_pause; }
protected:
	virtual void preProcessTriggers(SvTrig::SVTriggerInfoStruct& rTriggerInfo) override;
	virtual void postProcessTriggers(DWORD sleepDuration, bool softwareTrigger) override;

private:
	int m_digitizerNumber{ -1 };
	uint8_t m_triggerIndex{ 0 };
	SVIOTriggerLoadLibraryClass* m_pDLLTrigger{nullptr};
	unsigned long m_triggerChannel {0UL};
	bool m_pause {false};
	SvDef::TriggerType m_type{SvDef::TriggerType::HardwareTrigger};

	std::vector<AcquisitionParameter> m_acqTriggerParameters;
};
} //namespace SvTrig