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

HRESULT SVIOConfigurationInterfaceClass::SetOpto22InvertInputValue( bool p_bInvert )
{
	HRESULT l_hrOk = S_OK;

	m_Opto22InputInvert = p_bInvert;

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::SetOpto22InvertOutputValue( bool p_bInvert )
{
	HRESULT l_hrOk = S_OK;

	m_Opto22OutputInvert = p_bInvert;

	return l_hrOk;
}

HRESULT SVIOConfigurationInterfaceClass::OpenDigital( LPCTSTR p_szName )
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = m_svDigitalBoard.Open( p_szName );

	if ( l_hrOk == S_OK )
	{
		unsigned long l_ulSize = 0;

		if ( ( l_hrOk = m_svDigitalBoard.GetInputCount( &l_ulSize ) ) == S_OK )
		{
			if ( l_ulSize < static_cast< unsigned long >( m_svDigitalInputs.GetSize() ) )
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	if ( l_hrOk == S_OK )
	{
		unsigned long l_ulSize = 0;

		if ( ( l_hrOk = m_svDigitalBoard.GetOutputCount( &l_ulSize ) ) == S_OK )
		{
			if ( l_ulSize < static_cast< unsigned long >( m_svDigitalOutputs.GetSize() ) )
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	if ( l_hrOk == S_OK )
	{
		l_hrOk = ClearDigitalOutputs();
	}

	if ( l_hrOk != S_OK )
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

		if ( l_hrOk == S_OK )
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

	if( 0 < p_ulSizeInChars && p_pucBlock != NULL )
	{
		unsigned long l_ulCount = 0;

		l_hrOk = GetDigitalOutputCount( l_ulCount );

		if ( p_ulSizeInChars * 8 < l_ulCount )
		{
			l_hrOk = S_FALSE;
		}

		memset( (void *)p_pucBlock, 0x00, p_ulSizeInChars );

		for ( unsigned long l = 0; l_hrOk == S_OK && l < l_ulCount; l++ )
		{
			bool l_bValue = false;

			l_hrOk = GetDigitalOutputValue( l, l_bValue );

			if ( l_hrOk == S_OK )
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

		if ( l_hrOk == S_OK )
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

		if ( l_hrOk == S_OK )
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

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVIOLibrary\SVIOConfigurationInterfaceClass.cpp_v  $
 * 
 *    Rev 1.1   06 May 2013 19:13:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Apr 2013 14:00:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 12:44:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Aug 2012 16:41:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated WriteDDE method parameter to be const correct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Aug 2012 14:21:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVIOConfigurationInterfaceClass to become a singleton.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   13 Dec 2010 08:12:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update source code to use new Xcede-AN board and Rabbit IO.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.18   15 Oct 2010 14:49:46   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  701
   SCR Title:  Add New Font Functionality to the Existing Command Interface
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated code to remove casting isssues and invalid return values.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.17   20 Jun 2007 12:59:34   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  598
   SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
   Checked in by:  jSpila;  Joseph Spila
   Change Description:  
     These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.16   27 Jun 2006 08:49:56   tbair
   Project:  SVObserver
   Change Request (SCR) nbr:  564
   SCR Title:  Add configurability to SVTVicLpt DLL to set the system type
   Checked in by:  tBair;  Tom Bair
   Change Description:  
     Added Set and Get Parameter Functions.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.15   22 Feb 2005 10:56:48   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  472
   SCR Title:  Fix problems with Basler cameras
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Switched logic to set flag for trigger and strobe inverters.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.14   15 Jun 2004 13:56:48   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  410
   SCR Title:  Fix DDE IO
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated ReadDDEInput and WriteDDEOutput methods to handle changes in SVDDEItemData struct.
   
   
   /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Jun 2004 13:47:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  410
 * SCR Title:  Fix DDE IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated ReadDDEInput and WriteDDEOutput methods to use the changes to the SVDDEItemData structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.13   11 Jun 2004 14:06:52   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  410
   SCR Title:  Fix DDE IO
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated GetDigitalOutput method to fix the indexing problem.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.12   08 Jun 2004 11:24:18   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  410
   SCR Title:  Fix DDE IO
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated GetDigitalOutputValue, SetDigitalOutputValue, and GetDigitalOutputState methods to fix a DDE issue with the data collected from the Digital IO and sent to the DDE State output.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.11   22 Jan 2004 12:25:34   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  399
   SCR Title:  Increase 1394 SVIM Performance
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     changed name of function
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.10   21 Jan 2004 15:24:20   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  399
   SCR Title:  Increase 1394 SVIM Performance
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added code to make strobe start frame optional
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.9   19 Jan 2004 17:19:10   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  399
   SCR Title:  Increase 1394 SVIM Performance
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Fixed bug with IO startup system.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.8   16 Jan 2004 07:19:50   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  399
   SCR Title:  Increase 1394 SVIM Performance
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Added functions to allow configuring IO to work with special SVIM hardware
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.7   24 Nov 2003 09:34:18   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated Trigger and Strobe invert functions to select the correct lines.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.6   24 Nov 2003 07:02:28   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated Trigger and Strobe invert methods to handle all outputs instead of only the first 16.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.5   13 Nov 2003 09:20:12   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated ClearDigitalOutputs method to fix the problem with inverted or forced values set in I/O document.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.4   11 Nov 2003 10:44:14   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Updated both Invert Trigger and Invert Strobe methods to fix a variable problem.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   16 Oct 2003 14:52:30   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     Update digital output port methods to fix problems with not setting enough data and added methods to handle the inverters for the tiggers and strobs.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
