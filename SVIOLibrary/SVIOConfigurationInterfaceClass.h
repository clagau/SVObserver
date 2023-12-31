// ******************************************************************************
// * COPYRIGHT (c) 2003 by K�rber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOConfigurationInterfaceClass
// * .File Name       : $Workfile:   SVIOConfigurationInterfaceClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   06 May 2013 19:13:20  $
// ******************************************************************************

#pragma once

#include "SVIODigitalStruct.h"
#include "SVIODigitalLoadLibraryClass.h"

namespace SvTrig
{
struct ResultData;
}

class SVIOConfigurationInterfaceClass  
{
public:
	virtual ~SVIOConfigurationInterfaceClass();

	static SVIOConfigurationInterfaceClass& Instance()
	{
		static SVIOConfigurationInterfaceClass Object; 
		return Object;
	}

	void Shutdown();
	void Init();
	void initializeIO(int inputSize, int outputSize);

	HRESULT OpenDigital(HMODULE libraryHandle);
	HRESULT CloseDigital();

	long GetDigitalInputCount();
	HRESULT GetDigitalInputIsInverted( unsigned long channel, bool& rbValue );
	HRESULT SetDigitalInputIsInverted( unsigned long channel, bool bValue );
	HRESULT GetDigitalInputIsForced( unsigned long channel, bool& rbValue );
	HRESULT SetDigitalInputIsForced( unsigned long channel, bool bValue );
	HRESULT GetDigitalInputForcedValue( unsigned long channel, bool& rbValue );
	HRESULT SetDigitalInputForcedValue( unsigned long channel, bool rbValue );
	HRESULT GetDigitalInputValue( unsigned long channel, bool& rbValue );
	
	/// Read the DIO and set it intern. If used GetDigitalInputValue is used then, this values will used until clearDigitalInput is called.
	void readDigitalInputBatch();
	void clearDigitalInputBatch();

	long GetDigitalOutputCount();
	HRESULT GetDigitalOutputIsInverted( unsigned long channel, bool& rbValue );
	HRESULT SetDigitalOutputIsInverted( unsigned long channel, bool bValue );
	HRESULT GetDigitalOutputIsForced( unsigned long channel, bool& rbValue );
	HRESULT SetDigitalOutputIsForced( unsigned long channel, bool bValue );
	HRESULT GetDigitalOutputForcedValue( unsigned long channel, bool& rbValue );
	HRESULT SetDigitalOutputForcedValue( unsigned long channel, bool bValue );
	HRESULT SetDigitalOutputValue( unsigned long channel, bool bValue );

	HRESULT SetDigitalOutputPortValue( unsigned long port, unsigned long value );

	HRESULT SetDigitalOutputData(const SvTrig::ResultData& rResultData);

	HRESULT ClearDigitalOutputs();
	HRESULT TestDigitalOutputs();

	HRESULT SetSVIMTriggerValue( unsigned long channel, bool bRising );
	HRESULT SetSVIMStrobeValue( unsigned long channel, bool bRising );
	HRESULT SetSVIMStrobeStartFrameActive( unsigned long channel, bool bActive );

	HRESULT GetIOTriggerValue( unsigned long channel, bool& rbRising );
	HRESULT GetIOStrobeValue( unsigned long channel, bool& rbRising );

	HRESULT SetCameraTriggerValue( unsigned long channel, bool bRising );
	HRESULT SetCameraStrobeValue( unsigned long channel, bool bRising );

	// Parameter related functions. Not implemented in all dlls.
	unsigned long GetParameterCount() const;
	_variant_t GetParameterName( unsigned long index) const;
	_variant_t GetParameterValue( unsigned long index) const;
	HRESULT SetParameterValue( unsigned long index, const _variant_t& rValue);

private:
	SVIOConfigurationInterfaceClass();
	SVIOConfigurationInterfaceClass( const SVIOConfigurationInterfaceClass& p_rObject );

	SVIODigitalLoadLibraryClass m_DigitalBoard;

	bool m_Opto22InputInvert;
	bool m_Opto22OutputInvert;
	bool m_pbSVIMTriggerRising[4];
	bool m_pbSVIMStrobeRising[4];
	bool m_abSVIMStrobeStartFrame[4];

	bool m_isBatchInputRead {false};
	unsigned long m_batchDigitalInputValue {0ul};

	SVIODigitalStructVector m_DigitalInputs;
	SVIODigitalStructVector m_DigitalOutputs;
};
