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
	m_Opto22InputInvert = true;
	m_Opto22OutputInvert = true;

	for ( int i = 0; i < 4; i++ )
	{
		m_pbSVIMTriggerRising[i] = true;
		m_pbSVIMStrobeRising[i] = true;
		m_abSVIMStrobeStartFrame[i] = false;
	}

	m_svDigitalInputs.SetSize( 8 );
	m_svDigitalOutputs.SetSize( 16 );
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
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svDigitalBoard.Open( p_szName );

	if ( S_OK == l_hrOk )
	{
		unsigned long l_ulSize = 0;

		if ( S_OK == ( l_hrOk = m_svDigitalBoard.GetInputCount( &l_ulSize ) ) )
		{
			if ( l_ulSize < static_cast< unsigned long >( m_svDigitalInputs.GetSize() ) )
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	if ( S_OK == l_hrOk )
	{
		unsigned long l_ulSize = 0;

		if ( S_OK == ( l_hrOk = m_svDigitalBoard.GetOutputCount( &l_ulSize ) ) )
		{
			if ( l_ulSize < static_cast< unsigned long >( m_svDigitalOutputs.GetSize() ) )
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	if ( S_OK == l_hrOk )
	{
		l_hrOk = ClearDigitalOutputs();
	}

	if ( S_OK != l_hrOk )
	{
		CloseDigital();
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::CloseDigital()
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svDigitalBoard.Close();

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputCount( unsigned long &p_rulCount )
{
	HRESULT l_hrOk = S_OK;

	p_rulCount = static_cast< unsigned long >( m_svDigitalInputs.GetSize() );

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputIsInverted( unsigned long p_ulChannel, bool &p_rbValue )
{
	HRESULT l_hrOk = S_FALSE;

	unsigned long l_ulCount = static_cast< unsigned long >( m_svDigitalInputs.GetSize() );

	if ( 0 <= p_ulChannel && p_ulChannel < l_ulCount )
	{
		p_rbValue = m_svDigitalInputs[ p_ulChannel ].m_bIsInverted;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalInputIsInverted( unsigned long p_ulChannel, bool p_bValue )
{
	HRESULT l_hrOk = S_FALSE;

	unsigned long l_ulCount = static_cast< unsigned long >( m_svDigitalInputs.GetSize() );

	if ( 0 <= p_ulChannel && p_ulChannel < l_ulCount )
	{
		m_svDigitalInputs[ p_ulChannel ].m_bIsInverted = p_bValue;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputIsForced( unsigned long p_ulChannel, bool &p_rbValue )
{
	HRESULT l_hrOk = S_FALSE;

	unsigned long l_ulCount = static_cast< unsigned long >( m_svDigitalInputs.GetSize() );

	if ( 0 <= p_ulChannel && p_ulChannel < l_ulCount )
	{
		p_rbValue = m_svDigitalInputs[ p_ulChannel ].m_bIsForced;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalInputIsForced( unsigned long p_ulChannel, bool p_bValue )
{
	HRESULT l_hrOk = S_FALSE;

	unsigned long l_ulCount = static_cast< unsigned long >( m_svDigitalInputs.GetSize() );

	if ( 0 <= p_ulChannel && p_ulChannel < l_ulCount )
	{
		m_svDigitalInputs[ p_ulChannel ].m_bIsForced = p_bValue;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputForcedValue( unsigned long p_ulChannel, bool &p_rbValue )
{
	HRESULT l_hrOk = S_FALSE;

	unsigned long l_ulCount = static_cast< unsigned long >( m_svDigitalInputs.GetSize() );

	if ( 0 <= p_ulChannel && p_ulChannel < l_ulCount )
	{
		p_rbValue = m_svDigitalInputs[ p_ulChannel ].m_bForcedValue;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalInputForcedValue( unsigned long p_ulChannel, bool p_bValue )
{
	HRESULT l_hrOk = S_FALSE;

	unsigned long l_ulCount = static_cast< unsigned long >( m_svDigitalInputs.GetSize() );

	if ( 0 <= p_ulChannel && p_ulChannel < l_ulCount )
	{
		m_svDigitalInputs[ p_ulChannel ].m_bForcedValue = p_bValue;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputValue( unsigned long p_ulChannel, bool &p_rbValue )
{
	HRESULT l_hrOk = S_FALSE;

	unsigned long l_ulCount = static_cast< unsigned long >( m_svDigitalInputs.GetSize() );

	if ( 0 <= p_ulChannel && p_ulChannel < l_ulCount )
	{
		bool l_bValue = false;

		l_hrOk = m_svDigitalBoard.GetInputValue( p_ulChannel, &l_bValue );

		if ( S_OK == l_hrOk )
		{
			m_svDigitalInputs[ p_ulChannel ].m_bValue = l_bValue;
		}

		if ( m_svDigitalInputs[ p_ulChannel ].m_bIsForced )
		{
			l_bValue = m_svDigitalInputs[ p_ulChannel ].m_bForcedValue;
		}

		if ( m_svDigitalInputs[ p_ulChannel ].m_bIsInverted == m_Opto22InputInvert )
		{
			l_bValue = ! l_bValue;
		}

		p_rbValue = l_bValue;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputPortCount( unsigned long &p_rulCount )
{
	HRESULT l_hrOk = S_OK;

	l_hrOk = m_svDigitalBoard.GetInputPortCount( &p_rulCount );

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalInputPortValue( unsigned long p_ulPort, unsigned long &p_rulValue )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svDigitalBoard.GetInputPortValue( p_ulPort, &p_rulValue );

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputCount( unsigned long &p_rulCount )
{
	HRESULT l_hrOk = S_OK;

	p_rulCount = static_cast< unsigned long >( m_svDigitalOutputs.GetSize() );

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputIsInverted( unsigned long p_ulChannel, bool &p_rbValue )
{
	HRESULT l_hrOk = S_FALSE;

	unsigned long l_ulCount = static_cast< unsigned long >( m_svDigitalOutputs.GetSize() );

	if ( 0 <= p_ulChannel && p_ulChannel < l_ulCount )
	{
		p_rbValue = m_svDigitalOutputs[ p_ulChannel ].m_bIsInverted;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputIsInverted( unsigned long p_ulChannel, bool p_bValue )
{
	HRESULT l_hrOk = S_FALSE;

	unsigned long l_ulCount = static_cast< unsigned long >( m_svDigitalOutputs.GetSize() );

	if ( 0 <= p_ulChannel && p_ulChannel < l_ulCount )
	{
		m_svDigitalOutputs[ p_ulChannel ].m_bIsInverted = p_bValue;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputIsForced( unsigned long p_ulChannel, bool &p_rbValue )
{
	HRESULT l_hrOk = S_FALSE;

	unsigned long l_ulCount = static_cast< unsigned long >( m_svDigitalOutputs.GetSize() );

	if ( 0 <= p_ulChannel && p_ulChannel < l_ulCount )
	{
		p_rbValue = m_svDigitalOutputs[ p_ulChannel ].m_bIsForced;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputIsForced( unsigned long p_ulChannel, bool p_bValue )
{
	HRESULT l_hrOk = S_FALSE;

	unsigned long l_ulCount = static_cast< unsigned long >( m_svDigitalOutputs.GetSize() );

	if ( 0 <= p_ulChannel && p_ulChannel < l_ulCount )
	{
		m_svDigitalOutputs[ p_ulChannel ].m_bIsForced = p_bValue;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputForcedValue( unsigned long p_ulChannel, bool &p_rbValue )
{
	HRESULT l_hrOk = S_FALSE;

	unsigned long l_ulCount = static_cast< unsigned long >( m_svDigitalOutputs.GetSize() );

	if ( 0 <= p_ulChannel && p_ulChannel < l_ulCount )
	{
		p_rbValue = m_svDigitalOutputs[ p_ulChannel ].m_bForcedValue;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputForcedValue( unsigned long p_ulChannel, bool p_bValue )
{
	HRESULT l_hrOk = S_FALSE;

	unsigned long l_ulCount = static_cast< unsigned long >( m_svDigitalOutputs.GetSize() );

	if ( 0 <= p_ulChannel && p_ulChannel < l_ulCount )
	{
		m_svDigitalOutputs[ p_ulChannel ].m_bForcedValue = p_bValue;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputValue( unsigned long p_ulChannel, bool &p_rbValue )
{
	HRESULT l_hrOk = S_FALSE;

	unsigned long l_ulCount = static_cast< unsigned long >( m_svDigitalOutputs.GetSize() );

	if ( 0 <= p_ulChannel && p_ulChannel < l_ulCount )
	{
		bool l_bValue = m_svDigitalOutputs[ p_ulChannel ].m_bValue;

		if ( m_Opto22OutputInvert )
		{
			l_bValue = ! l_bValue;
		}

		p_rbValue = l_bValue;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputValue( unsigned long p_ulChannel, bool p_bValue )
{
	HRESULT l_hrOk = S_FALSE;

	unsigned long l_ulCount = static_cast< unsigned long >( m_svDigitalOutputs.GetSize() );

	if ( 0 <= p_ulChannel && p_ulChannel < l_ulCount )
	{
		bool l_bValue = p_bValue;

		if ( m_svDigitalOutputs[ p_ulChannel ].m_bIsForced )
		{
			l_bValue = m_svDigitalOutputs[ p_ulChannel ].m_bForcedValue;
		}

		if ( m_svDigitalOutputs[ p_ulChannel ].m_bIsInverted == m_Opto22OutputInvert )
		{
			l_bValue = ! l_bValue;
		}

		m_svDigitalOutputs[ p_ulChannel ].m_bValue = l_bValue;

		l_hrOk = m_svDigitalBoard.SetOutputValue( p_ulChannel, l_bValue );
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputState( unsigned long p_ulSizeInChars, unsigned char *p_pucBlock )
{
	HRESULT l_hrOk = S_FALSE;

	if( 0 < p_ulSizeInChars && nullptr != p_pucBlock )
	{
		unsigned long l_ulCount = 0;

		l_hrOk = GetDigitalOutputCount( l_ulCount );

		if ( p_ulSizeInChars * 8 < l_ulCount )
		{
			l_hrOk = S_FALSE;
		}

		memset( (void *)p_pucBlock, 0x00, p_ulSizeInChars );

		for ( unsigned long l = 0; S_OK == l_hrOk && l < l_ulCount; l++ )
		{
			bool l_bValue = false;

			l_hrOk = GetDigitalOutputValue( l, l_bValue );

			if ( S_OK == l_hrOk )
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

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::GetDigitalOutputPortCount( unsigned long &p_rulCount )
{
	HRESULT l_hrOk = S_OK;

	l_hrOk = m_svDigitalBoard.GetOutputPortCount( &p_rulCount );

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::SetDigitalOutputPortValue( unsigned long p_ulPort, unsigned long p_ulValue )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svDigitalBoard.SetOutputPortValue( p_ulPort, p_ulValue );

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::ClearDigitalOutputs()
{
	HRESULT l_hrOk = S_OK;

	unsigned long i = 0;
	
	unsigned long l_iSize = static_cast< unsigned long >( m_svDigitalOutputs.GetSize() );

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

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::TestDigitalOutputs()
{
	HRESULT l_hrOk = S_OK;

	unsigned long i = 0;
	unsigned long j = 0;
	unsigned long l_ulCount = static_cast< unsigned long >( m_svDigitalOutputs.GetSize() );
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

	for( i = 0; i < l_ulCount; ++ i )
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

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::SetSVIMTriggerValue( unsigned long p_ulChannel, bool p_bRising )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulChannel < 4 )
	{
		m_pbSVIMTriggerRising[ p_ulChannel ] = p_bRising;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::SetSVIMStrobeValue( unsigned long p_ulChannel, bool p_bRising )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulChannel < 4 )
	{
		m_pbSVIMStrobeRising[ p_ulChannel ] = p_bRising;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::SetSVIMStrobeStartFrameActive( unsigned long p_ulChannel, bool p_bActive )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulChannel < 4 )
	{
		m_abSVIMStrobeStartFrame[ p_ulChannel ] = p_bActive;

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::GetSVIMStrobeStartFrameActive( unsigned long p_ulChannel, bool &p_rbActive )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulChannel < 4 )
	{
		p_rbActive = m_abSVIMStrobeStartFrame[ p_ulChannel ];

		l_hrOk = S_OK;
	}

	return l_hrOk;
}


HRESULT SVIOConfigurationInterfaceClass::GetIOTriggerValue( unsigned long p_ulChannel, bool &p_rbRising )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulChannel < 4 )
	{
		if ( m_pbSVIMTriggerRising[ p_ulChannel ] )
		{
			// Rising

			p_rbRising = ! m_Opto22InputInvert;
		}
		else
		{
			// Falling

			p_rbRising = m_Opto22InputInvert;
		}

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::GetIOStrobeValue( unsigned long p_ulChannel, bool &p_rbRising )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulChannel < 4 )
	{
		if ( m_pbSVIMStrobeRising[ p_ulChannel ] )
		{
			// Rising

			p_rbRising = ! m_Opto22OutputInvert;
		}
		else
		{
			// Falling

			p_rbRising = m_Opto22OutputInvert;
		}

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::SetCameraTriggerValue( unsigned long p_ulChannel, bool p_bRising )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulChannel < 4 )
	{
		bool l_bRising = false;

		l_hrOk = GetIOTriggerValue( p_ulChannel, l_bRising );

		if ( S_OK == l_hrOk )
		{
			// JMS - Set Output to FALSE to invert signal
			// JMS - Set Output to TRUE to not invert signal

			l_hrOk = m_svDigitalBoard.SetOutputValue( 23 - p_ulChannel, l_bRising == p_bRising );
		}
	}

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::SetCameraStrobeValue( unsigned long p_ulChannel, bool p_bRising )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulChannel < 4 )
	{
		bool l_bRising = false;

		l_hrOk = GetIOStrobeValue( p_ulChannel, l_bRising );

		if ( S_OK == l_hrOk )
		{
			// JMS - Set Output to FALSE to invert signal
			// JMS - Set Output to TRUE to not invert signal

			l_hrOk = m_svDigitalBoard.SetOutputValue( 19 - p_ulChannel, l_bRising == p_bRising );
		}
	}

	return l_hrOk;
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
