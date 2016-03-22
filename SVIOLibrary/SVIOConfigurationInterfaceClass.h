// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOConfigurationInterfaceClass
// * .File Name       : $Workfile:   SVIOConfigurationInterfaceClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   06 May 2013 19:13:20  $
// ******************************************************************************

#pragma once

#include "SVIODigitalArrayClass.h"
#include "SVIODigitalLoadLibraryClass.h"

class SVIOConfigurationInterfaceClass  
{
public:
	inline static SVIOConfigurationInterfaceClass& Instance();

	virtual ~SVIOConfigurationInterfaceClass();

	void Shutdown();

	HRESULT SetOpto22InvertInputValue( bool p_bInvert );
	HRESULT SetOpto22InvertOutputValue( bool p_bInvert );

	HRESULT OpenDigital( LPCTSTR p_szName );
	HRESULT CloseDigital();

	HRESULT GetDigitalInputCount( unsigned long &p_rulCount );
	HRESULT GetDigitalInputIsInverted( unsigned long p_ulChannel, bool &p_rbValue );
	HRESULT SetDigitalInputIsInverted( unsigned long p_ulChannel, bool p_rbValue );
	HRESULT GetDigitalInputIsForced( unsigned long p_ulChannel, bool &p_rbValue );
	HRESULT SetDigitalInputIsForced( unsigned long p_ulChannel, bool p_rbValue );
	HRESULT GetDigitalInputForcedValue( unsigned long p_ulChannel, bool &p_rbValue );
	HRESULT SetDigitalInputForcedValue( unsigned long p_ulChannel, bool p_rbValue );
	HRESULT GetDigitalInputValue( unsigned long p_ulChannel, bool &p_rbValue );

	HRESULT GetDigitalInputPortCount( unsigned long &p_rulCount );
	HRESULT GetDigitalInputPortValue( unsigned long p_ulPort, unsigned long &p_rulValue );

	HRESULT GetDigitalOutputCount( unsigned long &p_rulCount );
	HRESULT GetDigitalOutputIsInverted( unsigned long p_ulChannel, bool &p_rbValue );
	HRESULT SetDigitalOutputIsInverted( unsigned long p_ulChannel, bool p_rbValue );
	HRESULT GetDigitalOutputIsForced( unsigned long p_ulChannel, bool &p_rbValue );
	HRESULT SetDigitalOutputIsForced( unsigned long p_ulChannel, bool p_rbValue );
	HRESULT GetDigitalOutputForcedValue( unsigned long p_ulChannel, bool &p_rbValue );
	HRESULT SetDigitalOutputForcedValue( unsigned long p_ulChannel, bool p_rbValue );
	HRESULT GetDigitalOutputValue( unsigned long p_ulChannel, bool &p_rbValue );
	HRESULT SetDigitalOutputValue( unsigned long p_ulChannel, bool p_bValue );
	HRESULT GetDigitalOutputState( unsigned long p_ulSizeInChars, unsigned char *p_pucBlock );

	HRESULT GetDigitalOutputPortCount( unsigned long &p_rulCount );
	HRESULT SetDigitalOutputPortValue( unsigned long p_ulPort, unsigned long p_ulValue );

	HRESULT ClearDigitalOutputs();
	HRESULT TestDigitalOutputs();

	HRESULT SetSVIMTriggerValue( unsigned long p_ulChannel, bool p_bRising );
	HRESULT SetSVIMStrobeValue( unsigned long p_ulChannel, bool p_bRising );
	HRESULT SetSVIMStrobeStartFrameActive( unsigned long p_ulChannel, bool p_bActive );
	HRESULT GetSVIMStrobeStartFrameActive( unsigned long p_ulChannel, bool &p_rbActive );

	HRESULT GetIOTriggerValue( unsigned long p_ulChannel, bool &p_rbRising );
	HRESULT GetIOStrobeValue( unsigned long p_ulChannel, bool &p_rbRising );

	HRESULT SetCameraTriggerValue( unsigned long p_ulChannel, bool p_bRising );
	HRESULT SetCameraStrobeValue( unsigned long p_ulChannel, bool p_bRising );

	// Parameter related functions. Not implemented in all dlls.
	HRESULT GetParameterCount( unsigned long *p_pulCount );
	HRESULT GetParameterName( unsigned long p_ulIndex, BSTR *p_pbstrName );
	HRESULT GetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue );
	HRESULT SetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue );

private:
	SVIOConfigurationInterfaceClass();
	SVIOConfigurationInterfaceClass( const SVIOConfigurationInterfaceClass& p_rObject );
	const SVIOConfigurationInterfaceClass& operator=( const SVIOConfigurationInterfaceClass& p_rObject );

	SVIODigitalLoadLibraryClass m_svDigitalBoard;

	bool m_Opto22InputInvert;
	bool m_Opto22OutputInvert;
	bool m_pbSVIMTriggerRising[4];
	bool m_pbSVIMStrobeRising[4];
	bool m_abSVIMStrobeStartFrame[4];

	SVIODigitalArrayClass m_svDigitalInputs;
	SVIODigitalArrayClass m_svDigitalOutputs;

};

#include "SVIOConfigurationInterfaceClass.inl"

