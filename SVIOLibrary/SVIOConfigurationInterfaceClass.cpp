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
	m_Opto22InputInvert = true;
	m_Opto22OutputInvert = true;

	for ( int i = 0; i < 4; i++ )
	{
		m_pbSVIMTriggerRising[i] = true;
		m_pbSVIMStrobeRising[i] = true;
		m_abSVIMStrobeStartFrame[i] = false;
	}

	m_DigitalInputs.resize(InputsDefaultNr);
	m_DigitalOutputs.resize(OutputsDefaultNr);
}

SVIOConfigurationInterfaceClass::~SVIOConfigurationInterfaceClass()
{
	Shutdown();
}

void SVIOConfigurationInterfaceClass::Shutdown()
{
	CloseDigital();
}

HRESULT SVIOConfigurationInterfaceClass::OpenDigital( LPCTSTR p_szName )
{
	HRESULT Result{E_FAIL};

	Result = m_svDigitalBoard.Open( p_szName );

	if ( S_OK == Result )
	{
		unsigned long l_ulSize = 0;

		if ( S_OK == ( Result = m_svDigitalBoard.GetInputCount( &l_ulSize ) ) )
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

		if ( S_OK == ( Result = m_svDigitalBoard.GetOutputCount( &l_ulSize ) ) )
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
	return m_svDigitalBoard.Close();
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputCount( unsigned long &rCount )
{
	rCount = static_cast<unsigned long> (m_DigitalInputs.size());

	return S_OK;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputIsInverted( unsigned long Channel, bool &rValue )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalInputs.size());

	if ( 0 <= Channel && Channel < Count )
	{
		rValue = m_DigitalInputs[Channel].m_bIsInverted;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalInputIsInverted( unsigned long Channel, bool Value )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalInputs.size());

	if ( 0 <= Channel && Channel < Count )
	{
		m_DigitalInputs[Channel].m_bIsInverted = Value;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputIsForced( unsigned long Channel, bool& rValue )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalInputs.size());

	if ( 0 <= Channel && Channel < Count )
	{
		rValue = m_DigitalInputs[Channel].m_bIsForced;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalInputIsForced( unsigned long Channel, bool Value )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalInputs.size());

	if ( 0 <= Channel && Channel < Count )
	{
		m_DigitalInputs[Channel].m_bIsForced = Value;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputForcedValue( unsigned long Channel, bool &rValue )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalInputs.size());

	if ( 0 <= Channel && Channel < Count )
	{
		rValue = m_DigitalInputs[Channel].m_bForcedValue;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalInputForcedValue( unsigned long Channel, bool Value )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalInputs.size());

	if ( 0 <= Channel && Channel < Count )
	{
		m_DigitalInputs[ Channel ].m_bForcedValue = Value;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputValue( unsigned long Channel, bool &rValue )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalInputs.size());

	if ( 0 <= Channel && Channel < Count )
	{
		bool l_bValue = false;

		Result = m_svDigitalBoard.GetInputValue( Channel, &l_bValue );

		if ( S_OK == Result )
		{
			m_DigitalInputs[ Channel ].m_bValue = l_bValue;
		}

		if ( m_DigitalInputs[ Channel ].m_bIsForced )
		{
			l_bValue = m_DigitalInputs[ Channel ].m_bForcedValue;
		}

		if ( m_DigitalInputs[ Channel ].m_bIsInverted == m_Opto22InputInvert )
		{
			l_bValue = ! l_bValue;
		}

		rValue = l_bValue;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputPortCount( unsigned long &p_rulCount )
{
	return m_svDigitalBoard.GetInputPortCount( &p_rulCount );
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputPortValue( unsigned long p_ulPort, unsigned long &p_rulValue )
{
	return m_svDigitalBoard.GetInputPortValue( p_ulPort, &p_rulValue );
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputCount( unsigned long& rCount )
{
	rCount = static_cast<unsigned long> (m_DigitalOutputs.size());

	return S_OK;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputIsInverted( unsigned long Channel, bool &rValue )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( 0 <= Channel && Channel < Count )
	{
		rValue = m_DigitalOutputs[ Channel ].m_bIsInverted;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputIsInverted( unsigned long Channel, bool Value )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( 0 <= Channel && Channel < Count )
	{
		m_DigitalOutputs[ Channel ].m_bIsInverted = Value;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputIsForced( unsigned long Channel, bool &rValue )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( 0 <= Channel && Channel < Count )
	{
		rValue = m_DigitalOutputs[ Channel ].m_bIsForced;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputIsForced( unsigned long Channel, bool Value )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( 0 <= Channel && Channel < Count )
	{
		m_DigitalOutputs[ Channel ].m_bIsForced = Value;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputForcedValue( unsigned long Channel, bool &rValue )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( 0 <= Channel && Channel < Count )
	{
		rValue = m_DigitalOutputs[ Channel ].m_bForcedValue;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputForcedValue( unsigned long Channel, bool Value )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( 0 <= Channel && Channel < Count )
	{
		m_DigitalOutputs[ Channel ].m_bForcedValue = Value;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputValue( unsigned long Channel, bool &rValue )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( 0 <= Channel && Channel < Count )
	{
		bool l_bValue = m_DigitalOutputs[ Channel ].m_bValue;

		if ( m_Opto22OutputInvert )
		{
			l_bValue = ! l_bValue;
		}

		rValue = l_bValue;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputValue( unsigned long Channel, bool Value )
{
	HRESULT Result{E_FAIL};

	unsigned long Count = static_cast<unsigned long> (m_DigitalOutputs.size());

	if ( 0 <= Channel && Channel < Count )
	{
		bool l_bValue = Value;

		if ( m_DigitalOutputs[ Channel ].m_bIsForced )
		{
			l_bValue = m_DigitalOutputs[ Channel ].m_bForcedValue;
		}

		if ( m_DigitalOutputs[ Channel ].m_bIsInverted == m_Opto22OutputInvert )
		{
			l_bValue = ! l_bValue;
		}

		m_DigitalOutputs[ Channel ].m_bValue = l_bValue;

		Result = m_svDigitalBoard.SetOutputValue( Channel, l_bValue );
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputState( unsigned long p_ulSizeInChars, unsigned char *p_pucBlock )
{
	HRESULT Result{E_FAIL};

	if( 0 < p_ulSizeInChars && nullptr != p_pucBlock )
	{
		unsigned long Count = 0;

		Result = GetDigitalOutputCount( Count );

		if ( p_ulSizeInChars * 8 < Count )
		{
			Result = E_FAIL;
		}

		memset( (void *)p_pucBlock, 0x00, p_ulSizeInChars );

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

					p_pucBlock[ l_ucBlock ] |= 0x01 << l_ucBit;
				}
			}
		}
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputPortCount( unsigned long &p_rulCount )
{
	return m_svDigitalBoard.GetOutputPortCount( &p_rulCount );
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputPortValue( unsigned long p_ulPort, unsigned long p_ulValue )
{
	return m_svDigitalBoard.SetOutputPortValue( p_ulPort, p_ulValue );
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
		m_svDigitalBoard.SetOutputValue( 19 - i, false );
		m_svDigitalBoard.SetOutputValue( 23 - i, false );
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

HRESULT SVIOConfigurationInterfaceClass::SetSVIMTriggerValue( unsigned long Channel, bool p_bRising )
{
	HRESULT Result{E_FAIL};

	if ( Channel < 4 )
	{
		m_pbSVIMTriggerRising[ Channel ] = p_bRising;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetSVIMStrobeValue( unsigned long Channel, bool p_bRising )
{
	HRESULT Result{E_FAIL};

	if ( Channel < 4 )
	{
		m_pbSVIMStrobeRising[ Channel ] = p_bRising;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetSVIMStrobeStartFrameActive( unsigned long Channel, bool p_bActive )
{
	HRESULT Result{E_FAIL};

	if ( Channel < 4 )
	{
		m_abSVIMStrobeStartFrame[ Channel ] = p_bActive;

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetSVIMStrobeStartFrameActive( unsigned long Channel, bool &p_rbActive )
{
	HRESULT Result{E_FAIL};

	if ( Channel < 4 )
	{
		p_rbActive = m_abSVIMStrobeStartFrame[ Channel ];

		Result = S_OK;
	}

	return Result;
}


HRESULT SVIOConfigurationInterfaceClass::GetIOTriggerValue( unsigned long Channel, bool &p_rbRising )
{
	HRESULT Result{E_FAIL};

	if ( Channel < 4 )
	{
		if ( m_pbSVIMTriggerRising[ Channel ] )
		{
			// Rising

			p_rbRising = ! m_Opto22InputInvert;
		}
		else
		{
			// Falling

			p_rbRising = m_Opto22InputInvert;
		}

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::GetIOStrobeValue( unsigned long Channel, bool &p_rbRising )
{
	HRESULT Result{E_FAIL};

	if ( Channel < 4 )
	{
		if ( m_pbSVIMStrobeRising[ Channel ] )
		{
			// Rising

			p_rbRising = ! m_Opto22OutputInvert;
		}
		else
		{
			// Falling

			p_rbRising = m_Opto22OutputInvert;
		}

		Result = S_OK;
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetCameraTriggerValue( unsigned long Channel, bool p_bRising )
{
	HRESULT Result{E_FAIL};

	if ( Channel < 4 )
	{
		bool l_bRising = false;

		Result = GetIOTriggerValue( Channel, l_bRising );

		if ( S_OK == Result )
		{
			// JMS - Set Output to FALSE to invert signal
			// JMS - Set Output to TRUE to not invert signal

			Result = m_svDigitalBoard.SetOutputValue( 23 - Channel, l_bRising == p_bRising );
		}
	}

	return Result;
}

HRESULT SVIOConfigurationInterfaceClass::SetCameraStrobeValue( unsigned long Channel, bool p_bRising )
{
	HRESULT Result{E_FAIL};

	if ( Channel < 4 )
	{
		bool l_bRising = false;

		Result = GetIOStrobeValue( Channel, l_bRising );

		if ( S_OK == Result )
		{
			// JMS - Set Output to FALSE to invert signal
			// JMS - Set Output to TRUE to not invert signal

			Result = m_svDigitalBoard.SetOutputValue( 19 - Channel, l_bRising == p_bRising );
		}
	}

	return Result;
}

// Parameter related functions. Not implemented in all dlls.
HRESULT SVIOConfigurationInterfaceClass::GetParameterCount( unsigned long *p_pulCount )
{
	return m_svDigitalBoard.GetParameterCount( p_pulCount );
}

HRESULT SVIOConfigurationInterfaceClass::GetParameterName( unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	return m_svDigitalBoard.GetParameterName( p_ulIndex, p_pbstrName );
}

HRESULT SVIOConfigurationInterfaceClass::GetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	return m_svDigitalBoard.GetParameterValue( p_ulIndex, p_pvarValue );
}

HRESULT SVIOConfigurationInterfaceClass::SetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	return m_svDigitalBoard.SetParameterValue( p_ulIndex, p_pvarValue );
}
