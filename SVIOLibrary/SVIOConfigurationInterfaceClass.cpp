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

constexpr size_t InputsDefaultNr = 8;
constexpr size_t OutputsDefaultNr = 16;

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
	m_DigitalInputs.resize(InputsDefaultNr);
	m_DigitalOutputs.resize(OutputsDefaultNr);
}

HRESULT SVIOConfigurationInterfaceClass::OpenDigital( LPCTSTR pName )
{
	HRESULT Result{E_FAIL};

	Result = m_DigitalBoard.Open( pName );

	if ( S_OK == Result )
	{
		unsigned long l_ulSize = 0;

		if ( S_OK == ( Result = m_DigitalBoard.GetInputCount( &l_ulSize ) ) )
		{
			if ( l_ulSize < static_cast< unsigned long >( m_DigitalInputs.size() ) )
			{
				Result = E_FAIL;
			}
		}
	}

	if ( S_OK == Result )
	{
		unsigned long l_ulSize = 0;

		if ( S_OK == ( Result = m_DigitalBoard.GetOutputCount( &l_ulSize ) ) )
		{
			if ( l_ulSize < static_cast< unsigned long >( m_DigitalOutputs.size() ) )
			{
				Result = E_FAIL;
			}
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

	if ( 0 <= channel && channel < Count )
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

	if ( 0 <= channel && channel < Count )
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

	if ( 0 <= channel && channel < Count )
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

	if ( 0 <= channel && channel < Count )
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

	if ( 0 <= channel && channel < Count )
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

	if ( 0 <= channel && channel < Count )
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

	if ( 0 <= channel && channel < Count )
	{
		bool l_bValue = false;

		Result = m_DigitalBoard.GetInputValue( channel, &l_bValue );

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

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputPortCount( unsigned long& rCount )
{
	return m_DigitalBoard.GetInputPortCount( &rCount );
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputPortValue( unsigned long port, unsigned long& rValue )
{
	return m_DigitalBoard.GetInputPortValue(port, &rValue);
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

	if ( 0 <= channel && channel < Count )
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

	if ( 0 <= channel && channel < Count )
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

	if ( 0 <= channel && channel < Count )
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

	if ( 0 <= channel && channel < Count )
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

	if ( 0 <= channel && channel < Count )
	{
		rValue = m_DigitalOutputs[ channel ].m_bForcedValue;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputForcedValue( unsigned long channel, bool Value )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( 0 <= channel && channel < Count )
	{
		m_DigitalOutputs[ channel ].m_bForcedValue = Value;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputValue( unsigned long channel, bool& rValue )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( 0 <= channel && channel < Count )
	{
		bool l_bValue = m_DigitalOutputs[ channel ].m_bValue;

		if ( m_Opto22OutputInvert )
		{
			l_bValue = ! l_bValue;
		}

		rValue = l_bValue;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputValue( unsigned long channel, bool Value )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( 0 <= channel && channel < Count )
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

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputState( unsigned long sizeInChars, unsigned char* pBlock )
{
	HRESULT Result{E_FAIL};

	if( 0 < sizeInChars && nullptr != pBlock )
	{
		unsigned long Count = 0;

		Result = GetDigitalOutputCount( Count );

		if ( sizeInChars * 8 < Count )
		{
			Result = E_FAIL;
		}

		memset( (void *)pBlock, 0x00, sizeInChars );

		for ( unsigned long l = 0; S_OK == Result && l < Count; l++ )
		{
			bool l_bValue = false;

			Result = GetDigitalOutputValue( l, l_bValue );

			if ( S_OK == Result )
			{
				if ( l_bValue ) 
				{
					unsigned char l_ucBlock = static_cast<unsigned char >(l / 8);
					unsigned char l_ucBit = static_cast<unsigned char >(l % 8);

					pBlock[ l_ucBlock ] |= 0x01 << l_ucBit;
				}
			}
		}
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputPortCount( unsigned long& rCount )
{
	return m_DigitalBoard.GetOutputPortCount( &rCount );
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
	unsigned long i = 0;
	unsigned long j = 0;
	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());
	unsigned long l_ulPortCount = 2;

	for( i = 0; i < 10; ++ i )
	{
		for ( j = 0; j < l_ulPortCount; j++)
		{
			SetDigitalOutputPortValue( j, 0x00000000 );
		}

		Sleep( 100 );

		for ( j = 0; j < l_ulPortCount; j++)
		{
			SetDigitalOutputPortValue( j, 0xFFFFFFFF );
		}

		Sleep( 100 );
	}

	for( i = 0; i < Count; ++ i )
	{
		for( j = 0; j < 20; ++ j )
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

HRESULT SVIOConfigurationInterfaceClass::GetSVIMStrobeStartFrameActive( unsigned long channel, bool& rbActive )
{
	HRESULT Result{E_FAIL};

	if ( channel < 4 )
	{
		rbActive = m_abSVIMStrobeStartFrame[ channel ];

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
