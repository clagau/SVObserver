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
#include "SVOLibrary/TriggerDevice.h"
#include "SVTriggerInfoStruct.h"
#pragma endregion Includes

class SVIOTriggerLoadLibraryClass;

namespace SvTrig
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

class SVTriggerClass : public TriggerDevice  
{
public:
	explicit SVTriggerClass(LPCTSTR deviceName);
	virtual ~SVTriggerClass() = default;

	void __stdcall triggerCallback(const SvTrig::IntVariantMap& rTriggerData);

	void addAcquisitionTrigger(SVDigitizerLoadLibraryClass* pDllDigitizer, unsigned long triggerChannel);
	void clearAcquisitionTriggers();
	void enableInternalTrigger() const;

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

protected:
	virtual void preProcessTriggers(SvTrig::SVTriggerInfoStruct& rTriggerInfo) override;
	virtual void postProcessTriggers() override;

private:
	int m_digitizerNumber{ -1 };
	long m_currentObjectID{ 0L };
	long m_triggerIndex{ 0L };
	SVIOTriggerLoadLibraryClass* m_pDLLTrigger{nullptr};
	unsigned long m_triggerChannel {0UL};
	SvDef::TriggerType m_type{SvDef::TriggerType::HardwareTrigger};

	std::vector<AcquisitionParameter> m_acqTriggerParameters;
};
} //namespace SvTrig