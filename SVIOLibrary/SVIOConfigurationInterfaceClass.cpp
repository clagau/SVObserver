// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIOConfigurationInterfaceClass
// * .File Name       : $Workfile:   SVIOConfigurationInterfaceClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   06 May 2013 19:13:16  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIOConfigurationInterfaceClass.h"

SVIOConfigurationInterfaceClass::SVIOConfigurationInterfaceClass()
{
	Init();
}

SVIOConfigurationInterfaceClass::~SVIOConfigurationInterfaceClass()
{
	Shutdown();
}

void SVIOConfigurationInterfaceClass::Shutdown()
{
	CloseDigital();
}

void SVIOConfigurationInterfaceClass::Init()
{
	m_Opto22InputInvert = true;
	m_Opto22OutputInvert = true;

	for (int i = 0; i < 4; i++)
	{
		m_pbSVIMTriggerRising[i] = true;
		m_pbSVIMStrobeRising[i] = true;
		m_abSVIMStrobeStartFrame[i] = false;
	}

	m_DigitalInputs.clear();
	m_DigitalOutputs.clear();

	unsigned long size {0UL};
	if (S_OK == m_DigitalBoard.GetInputCount(&size))
	{
		m_DigitalInputs.resize(size);
	}
	if (S_OK == m_DigitalBoard.GetOutputCount(&size))
	{
		m_DigitalOutputs.resize(size);
	}
}

HRESULT SVIOConfigurationInterfaceClass::OpenDigital( LPCTSTR pName )
{
	HRESULT Result{E_FAIL};

	Result = m_DigitalBoard.Open( pName );

	if ( S_OK == Result )
	{
		unsigned long size{0UL};
		if ( S_OK == ( Result = m_DigitalBoard.GetInputCount( &size ) ) )
		{
			m_DigitalInputs.resize(size);
		}
	}

	if ( S_OK == Result )
	{
		unsigned long size {0UL};
		if ( S_OK == ( Result = m_DigitalBoard.GetOutputCount( &size ) ) )
		{
			m_DigitalOutputs.resize(size);
		}
	}

	if ( S_OK == Result )
	{
		Result = ClearDigitalOutputs();
	}

	if ( S_OK != Result )
	{
		CloseDigital();
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::CloseDigital()
{
	return m_DigitalBoard.Close();
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputCount( unsigned long& rCount )
{
	rCount = static_cast<unsigned long> (m_DigitalInputs.size());

	return S_OK;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputIsInverted( unsigned long channel, bool& rValue )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalInputs.size());

	if ( channel < Count )
	{
		rValue = m_DigitalInputs[channel].m_bIsInverted;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalInputIsInverted( unsigned long channel, bool Value )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalInputs.size());

	if ( channel < Count )
	{
		m_DigitalInputs[channel].m_bIsInverted = Value;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputIsForced( unsigned long channel, bool& rValue )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalInputs.size());

	if ( channel < Count )
	{
		rValue = m_DigitalInputs[channel].m_bIsForced;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalInputIsForced( unsigned long channel, bool Value )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalInputs.size());

	if ( channel < Count )
	{
		m_DigitalInputs[channel].m_bIsForced = Value;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputForcedValue( unsigned long channel, bool& rValue )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalInputs.size());

	if ( channel < Count )
	{
		rValue = m_DigitalInputs[channel].m_bForcedValue;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalInputForcedValue( unsigned long channel, bool Value )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalInputs.size());

	if ( channel < Count )
	{
		m_DigitalInputs[ channel ].m_bForcedValue = Value;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputValue( unsigned long channel, bool& rValue )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalInputs.size());

	if ( channel < Count )
	{
		bool l_bValue = false;

		if (m_isBatchInputRead)
		{
			if (8 > channel)
			{
				l_bValue = (static_cast<long>(m_batchDigitalInputValue) & (1 << channel)) ? true : false;
				Result = S_OK;
			}
		}
		else
		{
			Result = m_DigitalBoard.GetInputValue(channel, &l_bValue);
		}

		if ( S_OK == Result )
		{
			m_DigitalInputs[ channel ].m_bValue = l_bValue;
		}

		if ( m_DigitalInputs[ channel ].m_bIsForced )
		{
			l_bValue = m_DigitalInputs[ channel ].m_bForcedValue;
		}

		if ( m_DigitalInputs[ channel ].m_bIsInverted == m_Opto22InputInvert )
		{
			l_bValue = ! l_bValue;
		}

		rValue = l_bValue;
	}

	return Result;
}

void SVIOConfigurationInterfaceClass::readDigitalInputBatch()
{
	m_DigitalBoard.GetInputValues(&m_batchDigitalInputValue);
	m_isBatchInputRead = true;
}

void SVIOConfigurationInterfaceClass::clearDigitalInputBatch()
{
	m_isBatchInputRead = false;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputCount( unsigned long& rCount )
{
	rCount = static_cast<unsigned long> (m_DigitalOutputs.size());

	return S_OK;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputIsInverted( unsigned long channel, bool& rValue )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( channel < Count )
	{
		rValue = m_DigitalOutputs[ channel ].m_bIsInverted;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputIsInverted( unsigned long channel, bool Value )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( channel < Count )
	{
		m_DigitalOutputs[ channel ].m_bIsInverted = Value;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputIsForced( unsigned long channel, bool& rValue )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( channel < Count )
	{
		rValue = m_DigitalOutputs[ channel ].m_bIsForced;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputIsForced( unsigned long channel, bool Value )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( channel < Count )
	{
		m_DigitalOutputs[ channel ].m_bIsForced = Value;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputForcedValue( unsigned long channel, bool& rValue )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( channel < Count )
	{
		rValue = m_DigitalOutputs[channel].m_bForcedValue;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputForcedValue( unsigned long channel, bool Value )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( channel < Count )
	{
		m_DigitalOutputs[channel].m_bForcedValue = Value;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputValue( unsigned long channel, bool Value )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( channel < Count )
	{
		bool l_bValue = Value;

		if ( m_DigitalOutputs[ channel ].m_bIsForced )
		{
			l_bValue = m_DigitalOutputs[ channel ].m_bForcedValue;
		}

		if ( m_DigitalOutputs[ channel ].m_bIsInverted == m_Opto22OutputInvert )
		{
			l_bValue = ! l_bValue;
		}

		m_DigitalOutputs[ channel ].m_bValue = l_bValue;

		Result = m_DigitalBoard.SetOutputValue( channel, l_bValue );
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputPortValue( unsigned long port, unsigned long Value )
{
	return m_DigitalBoard.SetOutputPortValue( port, Value );
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputData(unsigned long channel, const IntVariantMap& rData)
{
	return m_DigitalBoard.SetOutputData(channel, rData);
}

HRESULT SVIOConfigurationInterfaceClass::ClearDigitalOutputs()
{
	unsigned long i = 0;
	
	unsigned long l_iSize = static_cast<unsigned long> (m_DigitalOutputs.size());

	for ( i = 0; i < l_iSize; i++ )
	{
		SetDigitalOutputIsInverted( i, false );
		SetDigitalOutputForcedValue( i, false );
		SetDigitalOutputIsForced( i, false );

		SetDigitalOutputValue( i, true );
	}

	for ( i = 0; i < 4; i++ )
	{
		m_DigitalBoard.SetOutputValue( 19 - i, false );
		m_DigitalBoard.SetOutputValue( 23 - i, false );
	}

	return S_OK;
}

HRESULT SVIOConfigurationInterfaceClass::TestDigitalOutputs()
{
	const unsigned long l_ulPortCount = 2;

	for(unsigned long i = 0; i < 10; ++ i )
	{
		for (unsigned long j = 0; j < l_ulPortCount; j++)
		{
			SetDigitalOutputPortValue( j, 0x00000000 );
		}

		Sleep( 100 );

		for (unsigned long j = 0; j < l_ulPortCount; j++)
		{
			SetDigitalOutputPortValue( j, 0xFFFFFFFF );
		}

		Sleep( 100 );
	}

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());
	for(unsigned long i = 0; i < Count; ++ i )
	{
		for(unsigned long j = 0; j < 20; ++ j )
		{

			SetDigitalOutputValue( i, true );
			Sleep( 100 );

			SetDigitalOutputValue( i, false );
			Sleep( 100 );
		}
	}

	ClearDigitalOutputs();

	return S_OK;
}

HRESULT SVIOConfigurationInterfaceClass::SetSVIMTriggerValue( unsigned long channel, bool bRising )
{
	HRESULT Result{E_FAIL};

	if ( channel < 4 )
	{
		m_pbSVIMTriggerRising[ channel ] = bRising;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetSVIMStrobeValue( unsigned long channel, bool bRising )
{
	HRESULT Result{E_FAIL};

	if ( channel < 4 )
	{
		m_pbSVIMStrobeRising[ channel ] = bRising;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetSVIMStrobeStartFrameActive( unsigned long channel, bool bActive )
{
	HRESULT Result{E_FAIL};

	if ( channel < 4 )
	{
		m_abSVIMStrobeStartFrame[ channel ] = bActive;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetIOTriggerValue( unsigned long channel, bool& rbRising )
{
	HRESULT Result{E_FAIL};

	if ( channel < 4 )
	{
		if ( m_pbSVIMTriggerRising[ channel ] )
		{
			// Rising

			rbRising = ! m_Opto22InputInvert;
		}
		else
		{
			// Falling

			rbRising = m_Opto22InputInvert;
		}

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetIOStrobeValue( unsigned long channel, bool& rbRising )
{
	HRESULT Result{E_FAIL};

	if ( channel < 4 )
	{
		if ( m_pbSVIMStrobeRising[ channel ] )
		{
			// Rising

			rbRising = ! m_Opto22OutputInvert;
		}
		else
		{
			// Falling

			rbRising = m_Opto22OutputInvert;
		}

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetCameraTriggerValue( unsigned long channel, bool bRising )
{
	HRESULT Result{E_FAIL};

	if ( channel < 4 )
	{
		bool l_bRising = false;

		Result = GetIOTriggerValue( channel, l_bRising );

		if ( S_OK == Result )
		{
			// JMS - Set Output to FALSE to invert signal
			// JMS - Set Output to TRUE to not invert signal

			Result = m_DigitalBoard.SetOutputValue( 23 - channel, l_bRising == bRising );
		}
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetCameraStrobeValue( unsigned long channel, bool bRising )
{
	HRESULT Result{E_FAIL};

	if ( channel < 4 )
	{
		bool l_bRising = false;

		Result = GetIOStrobeValue( channel, l_bRising );

		if ( S_OK == Result )
		{
			// JMS - Set Output to FALSE to invert signal
			// JMS - Set Output to TRUE to not invert signal

			Result = m_DigitalBoard.SetOutputValue( 19 - channel, l_bRising == bRising );
		}
	}

	return Result;
}

// Parameter related functions. Not implemented in all dlls.
HRESULT SVIOConfigurationInterfaceClass::GetParameterCount( unsigned long* pCount )
{
	return m_DigitalBoard.GetParameterCount( pCount );
}

HRESULT SVIOConfigurationInterfaceClass::GetParameterName( unsigned long index, BSTR* pName )
{
	return m_DigitalBoard.GetParameterName( index, pName );
}

HRESULT SVIOConfigurationInterfaceClass::GetParameterValue( unsigned long index, VARIANT* pValue )
{
	return m_DigitalBoard.GetParameterValue(index, pValue );
}

HRESULT SVIOConfigurationInterfaceClass::SetParameterValue( unsigned long index, VARIANT* pValue )
{
	return m_DigitalBoard.SetParameterValue(index, pValue );
}
