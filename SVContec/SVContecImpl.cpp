// ******************************************************************************
// * COPYRIGHT (c) 2013 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVContecImpl
// * .File Name       : $Workfile:   SVContecImpl.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   30 Oct 2013 11:31:30  $
// ******************************************************************************

#include "stdafx.h"
#include <fstream>
#include <time.h>
#include <functional>
#include <algorithm>
#include <string>
#include "SVContecImpl.h"
#include "SVIOLibrary\SVIOParameterEnum.h"
#include "SVStatusLibrary\SVException.h"
#include "SVMessage\SVMessage.h"
#include "SVUtilityLibrary\SVString.h"

#define DEVICENAME "DIO000"
#define CHIPNUMBER 0
#define DATAPORT 0
#define STATUSPORT 1
#define CONTROLPORT 2
#define RETRY 2

// ******* Timeout for I/O board Acknowledge microseconds.
#define BOARD_SELECT_ACK_TIMEOUT 16000

// ******* Debug Data **********
#define MaxDebugData 500
//#define LogDebugData 

#ifdef LogDebugData

struct SVLargeInteger
{
	unsigned long LoWord;
	unsigned long HiWord;
};

struct TriggerDebugData
{
	INT64 m_lTimeStamp1;
	INT64 m_lTimeStamp2;
	INT64 m_64lParTimeStamp;
	unsigned char m_dStatusReg0;
	unsigned char m_dStatusReg1;
	unsigned char m_dStatusReg2;
};

long g_lCriticalSecInits= 0;
TriggerDebugData g_TDebugData[MaxDebugData];
long g_CallbackCount = 0;
INT64 g_perf;
INT64 g_perfFreq;

#endif

class SVContecTriggerFind : public std::binary_function< SVContecTriggerStruct, SVContecCallbackPtr, bool >
{
public:
	bool operator()( const SVContecTriggerStruct& triggerStruct, const SVContecCallbackPtr& pCallback) const
	{
		return (triggerStruct.pCallback == pCallback);
	}
};



SVContecImpl::SVContecImpl() 
: m_numPorts( 2 )
, m_numInputs( 12 )
, m_numOutputs( 24 )
, m_numTriggers( 3 )
, m_lBoardVersion( 0 )
, m_DeviceId( -1 )
, m_DataPortOutbound( true )
{
	try
	{
		#if( _WIN32_WINNT >= 0x0500 )
		  ::InitializeCriticalSectionAndSpinCount( &m_hCriticalSection, BOARD_SELECT_ACK_TIMEOUT + 100 );
		#else
		  ::InitializeCriticalSection( &m_hCriticalSection );
		#endif
		m_bCriticalSectionCreated = TRUE;
		DioInit( DEVICENAME, &m_DeviceId );
		HRESULT l_hr = SetDataPortOutputMode();
		l_hr = DioSetInterruptCallBackProc( m_DeviceId, CallBackProc, this );

		//BRW - START TEST
		//BRW - END TEST
	}
	catch( ... )
	{
		m_bCriticalSectionCreated = FALSE;
	}
	m_bUseSingleTrigger = false;
	m_lParallelBoardInterfaceBehavior = Function00ForWrite1;

	#ifdef SV_LOG_STATUS_INFO
		m_StatusLog.Create();
	#endif
}

SVContecImpl::~SVContecImpl()
{
	MaskIrq(); // BRW - TVic specific - disables interrupt
	DioExit( m_DeviceId );
	if( m_bCriticalSectionCreated )
	{
		try
		{
		  ::DeleteCriticalSection( &m_hCriticalSection );
		}
		catch( ... )
		{
		}
	}
}

unsigned long SVContecImpl::GetNumPorts()
{
	return m_numPorts;
}

unsigned long SVContecImpl::GetNumInputs()
{
	return m_numInputs;
}

unsigned long SVContecImpl::GetNumOutputs()
{
	return m_numOutputs;
}

HRESULT SVContecImpl::Initialize( bool bInit )
{
	// BRW - Disable debug strings.
	//OutputDebugString( "Start - Initialize\n" );

	HRESULT hr = S_OK;

#ifdef LogDebugData
	char l_szValue[128];
#endif

	if ( bInit && !GetActive() ) // BRW - TVic specific - gets the Active Property
	{
		SetActive( 1 ); // BRW - TVic specific - sets the Active Property
		//SetCurrentLPT( 1 ); // BRW - TVic specific - sets the LPT number

		if ( !GetActive() ) // BRW - TVic specific - gets the Active Property
		{
			hr = S_FALSE;
		}
		else
		{
			SetAcquired(1); // BRW - TVic specific - sets the Acquired Property
			if (!GetAcquired()) // BRW - TVic specific - gets the Acquired Property
			{
				hr = S_FALSE;
			}
			/*else
			{
				if( GetCurrentLptMode() < LPT_PS2_MODE ) // BRW - TVic specific - gets the CurrentLptMode Property
				{
					hr = S_FALSE;
				}
			}*/
		}

		// Initialize previous Output States
		for( int i = 0; i < SVNumOutputPorts ; i++ )
		{
			m_nPreviousOutputs[i] = 0xff;
		}

		// Initialize previous Trigger State
		m_lLastTriggerState = GetStatusPort(); // BRW - TVic specific - gets the StatusPort Property

		time_t l_time;
		time(&l_time);
		unsigned long l_lTime = static_cast<unsigned long>(l_time);
		SetRTC( l_lTime );	// 5 command 

//		SetLogValue( 0xF3 );
		// Unlock I/O Board
		if( WriteUnlockString() != S_OK )
		{
			WriteUnlockString();
		}

		// The Rabbit board version is read here.
		for( int i = 0 ; i < RETRY ; i++ )
		{
			HRESULT l_hrOK = GetBoardVersion( m_lBoardVersion ); // 5 commands
			if( l_hrOK == S_OK )
				break;
		}

		if( m_lBoardVersion == 0x1010101 || m_lBoardVersion == 0x7f7f7f7f )
		{
			m_lParallelBoardInterfaceBehavior = LegacyFunctions;
		}

#ifdef LogDebugData
		memset( &g_TDebugData[0], 0,  sizeof(  g_TDebugData )) ;
#endif
	}
	else
	{
#ifdef LogDebugData
		::QueryPerformanceFrequency( ( LARGE_INTEGER* )&g_perfFreq );
		g_perfFreq = g_perfFreq / 1000000;
		FILE* l_pfh = fopen("c:\\temp\\DebugCounts.txt","w");
		INT64 l_LastTimeStamp = g_TDebugData[0].m_lTimeStamp1;
		fprintf( l_pfh, "Critical Section inits %d", g_lCriticalSecInits );
		for( int i = 0 ; i < g_CallbackCount ; i++ )
		{
			fprintf( l_pfh, "\nTrigger %04d\n",i);
			fprintf( l_pfh, "%I64d TVic \n", g_TDebugData[i].m_64lParTimeStamp /  g_perfFreq );
			fprintf( l_pfh, "%I64d Start | Since last Int %I64d \n",g_TDebugData[i].m_lTimeStamp1 / g_perfFreq, ( g_TDebugData[i].m_lTimeStamp1 - l_LastTimeStamp)/g_perfFreq );
			fprintf( l_pfh, "%I64d End \n",g_TDebugData[i].m_lTimeStamp2 / g_perfFreq); 
			fprintf( l_pfh, "Status Register - %02x %02x %02x\n", g_TDebugData[i].m_dStatusReg0, g_TDebugData[i].m_dStatusReg1, g_TDebugData[i].m_dStatusReg2 );
			l_LastTimeStamp = g_TDebugData[i].m_lTimeStamp1; 
		}
		fclose( l_pfh );
#endif
//		SetLogValue( 0xF4 );
		if (GetAcquired()) // BRW - TVic specific - gets the Acquired Property
		{
			if( WriteLockString() != S_OK )
			{
				WriteLockString();
			}

			// Block the IRQ
			MaskIrq(); // BRW - TVic specific - disables interrupt

			SetAcquired(0); // BRW - TVic specific - sets the Acquired Property

			// Unload the driver
			if ( GetActive() ) // BRW - TVic specific - gets the Active Property
				SetActive(0); // BRW - TVic specific - sets the Active Property

			// Stop all triggers
			m_triggerList.clear();
		}
	}
	// BRW - Disable debug strings.
	//OutputDebugString( "End - Initialize\n" );

	return hr;
}

// WriteUnlockString 
// Sends a command 0x55 to the Parallel board
// to enable the outputs.
HRESULT SVContecImpl::WriteUnlockString()
{
	// BRW - Disable debug strings.
	//OutputDebugString( "Start - WriteUnlockString\n" );
	HRESULT l_hrOk = S_OK;

	unsigned long l_lCommand = 0;
	LPCSTR l_pString = "Unlock";

	long l_Retry = RETRY;
	bool l_bLocked = true;

	while( l_Retry > 0 )
	{
		// Resets parallel board sub command sequence.
		l_lCommand = NO_CMD;
		SVReadWriteLpt( l_lCommand, SVControlCommand );

		l_lCommand = UNLOCK_CMD;
		l_hrOk = SVReadWriteLpt( l_lCommand, SVControlCommand );
		for( int i = 0; i < 6 ; i++ )
		{
			HRESULT l_hr;
			l_lCommand = *(l_pString + i);
			if( (l_hr = SVReadWriteLpt( l_lCommand, SVControlCommand )) != S_OK )
			{
				l_hrOk = l_hr;
			}
		}

		if( l_hrOk == S_OK )	// Check if IO is actually unlocked
		{
			l_hrOk = GetLockState( l_bLocked );
			if( l_bLocked == false )
			{
				break;
			}
		}
		l_Retry--;
	}
	// BRW - Disable debug strings.
	//OutputDebugString( "End - WriteUnlockString\n" );
	return l_hrOk;
}

// WriteLockString 
// Sends a command 0x55 to the Parallel board
// to enable the outputs.
HRESULT SVContecImpl::WriteLockString()
{
	// BRW - Disable debug strings.
	//OutputDebugString( "Start - WriteLockString\n" );
	HRESULT l_hrOk = S_OK;

	unsigned long l_lCommand = 0;
	LPCSTR l_pString = "Lock";

	long l_Retry = RETRY;
	bool l_bLocked = false;

	while( l_Retry > 0 )
	{

		// Resets parallel board sub command sequence.
		l_lCommand = NO_CMD;
		SVReadWriteLpt( l_lCommand, SVControlCommand );

		l_lCommand = LOCK_CMD;
		l_hrOk = SVReadWriteLpt( l_lCommand, SVControlCommand );
		for( int i = 0; l_hrOk == S_OK && i < 4 ; i++ )
		{
			HRESULT l_hr;
			l_lCommand = *(l_pString + i);
			if( (l_hr = SVReadWriteLpt( l_lCommand, SVControlCommand )) != S_OK )
			{
				l_hrOk = l_hr;
			}
		}

		if( l_hrOk == S_OK )
		{
			l_hrOk = GetLockState( l_bLocked );
			if( l_bLocked == true )
			{
				break;
			}
		}
		l_Retry--;
	}
	// BRW - Disable debug strings.
	//OutputDebugString( "End - WriteLockString\n" );
	return l_hrOk;
}

// GetBoardVersion
// Get Board Version sends a Sub Command value of 0x56 (GET_BOARD_VERSION)
// This puts the parallel board in a mode that will send the 
// Board version on the next four ControlReadCommands.
HRESULT SVContecImpl::GetBoardVersion( long& p_rlVer )
{
	// BRW - Disable debug strings.
	//OutputDebugString( "Start - GetBoardVersion\n" );
	HRESULT l_hr;
	long l_lVersion = 0;
	unsigned long l_lCommand = NO_CMD;
	SVReadWriteLpt( l_lCommand, SVControlCommand ); // send a No Mode command to reset states.

	l_lCommand = GET_BOARD_VERSION;
	l_hr = SVReadWriteLpt( l_lCommand, SVControlCommand );

	HRESULT l_tmphr = S_OK;
	if( l_hr == S_OK )
	{
		for( int i = 0 ; i < 4 ; i++ )
		{
			unsigned long l_lValue;
			if( l_tmphr = SVReadWriteLpt( l_lValue, SVControlReadCommand ) != S_OK )
				l_hr = l_tmphr;
			l_lVersion = (l_lVersion << 8) | l_lValue ;
		}
	}
	p_rlVer = l_lVersion;
	// BRW - Disable debug strings.
	//OutputDebugString( "End - GetBoardVersion\n" );
	return l_hr;
}

unsigned long SVContecImpl::GetInputCount()
{
	return m_numInputs;
}

unsigned long SVContecImpl::GetOutputCount()
{
	return m_numOutputs;
}

unsigned long SVContecImpl::GetPortCount()
{
	return m_numPorts;
}

HRESULT SVContecImpl::GetInputValue( unsigned long* pVal )
{
	SVReadWriteLpt( *pVal, SVControlReadDigitalInputs );
	*pVal = *pVal & 0xff;
	return S_OK;
}

HRESULT SVContecImpl::GetInputBit( unsigned long bitNum, bool& bitVal )
{
	HRESULT hr = S_OK;
	unsigned long l_lValue;
	if( bitNum < 8 )
	{
		SVReadWriteLpt( l_lValue, SVControlReadDigitalInputs, bitNum );
	}
	bitVal = l_lValue ? true : false;

	return hr;
}

HRESULT SVContecImpl::Lock()
{
	HRESULT l_hr = S_OK;
	try
	{
	  ::EnterCriticalSection( &m_hCriticalSection );
	}
	catch( ... )
	{
		l_hr = S_FALSE;
//		::MessageBox(NULL, "SVReadWrite Locked", NULL, MB_OK);
	}
	return l_hr;
}

HRESULT SVContecImpl::UnLock()
{
	HRESULT l_hr = S_OK;
	try
	{
		::LeaveCriticalSection( &m_hCriticalSection );
	}
	catch( ... )
	{
		l_hr = S_FALSE;
	}
	return l_hr;
}

LPCTSTR SVContecImpl::GetControlText( long p_lControl )
{
	LPCTSTR l_pString = NULL;
	{
		switch( p_lControl )
		{
			case SVContecImpl::SVControlCameraSetup:
			{
				l_pString = "Camera Setup";
				break;
			}
			case SVContecImpl::SVControlWriteDigital0:
			{
				l_pString = "Write Digital Bank 0";
				break;
			}
			case SVContecImpl::SVControlWriteDigital1:
			{
				l_pString = "Write Digital Bank 1";
				break;
			}
			case SVContecImpl::SVControlReadDigitalInputs:
			{
				l_pString = "Read Digital Inputs";
				break;
			}
			case SVContecImpl::SVControlReadCommand:
			{
				l_pString = "Command Read";
				break;
			}
			case SVContecImpl::SVControlCommand:
			{
				l_pString = "Command Write";
				break;
			}
		}
	}
	return l_pString;
}

HRESULT SVContecImpl::SVReadWriteLpt( unsigned long& p_plValue, long p_lControl, long p_lBit )
{
	// BRW - Disable debug strings.
	//SVString l_Message;
	//l_Message.Format( _T( "Start - SVReadWriteLpt( %x, %x, %x )\r" ), p_plValue, p_lControl, p_lBit );
	//OutputDebugString( l_Message.ToString() );
	HRESULT l_hr = S_OK;

	__int64 l_i64Start;
	__int64 l_i64Check;
	__int64 l_i64Freq;

	if( Lock() == S_OK )
	{
		// Reset the Quantum
		HANDLE l_hThread = ::GetCurrentThread();
		DWORD l_dwPriority = ::GetThreadPriority( l_hThread );
		::SetThreadPriority( l_hThread, l_dwPriority );

		unsigned short l_nPrevControl = GetControlPort(); // BRW - TVic specific - gets the ControlPort Property
		if( (l_nPrevControl & 0xf) != 0 )
		{
			SVException e;
			SETEXCEPTION1(e, SVMSG_INVALID_LINE_STATE , _T("Select Line In wrong State") );
			// BRW - Disable debug strings.
			//OutputDebugString( "Select Line In wrong State\n" );
			e.LogException( );
		}
		// Get Value of control port interrupt Bit 
		BYTE l_nVal = SVControlEnableInterrupt;
		SetControlPort( ( unsigned short )( l_nVal ) );	// Set Control to known state with select line off... // BRW - TVic specific - sets the ControlPort Property

		unsigned short l_lPort = GetControlPort() & 0x37; // BRW - TVic specific - gets the ControlPort Property
		if( l_lPort != l_nVal )
		{
			l_hr = S_FALSE;
		}

		QueryPerformanceCounter( (LARGE_INTEGER*)&l_i64Start );
		QueryPerformanceFrequency( (LARGE_INTEGER*)&l_i64Freq );
		l_i64Freq = l_i64Freq/1000000;	// Divide frequency so results are in microsecends.

		// **** Wait for Acknowledge off...
		while( ( GetStatusPort() & 128 ) == 0 ) // BRW - TVic specific - gets the StatusPort Property
		{
			// BRW - Disable debug strings.
			//OutputDebugString( "  SVReadWriteLpt - Waiting for ACK\n" );
			QueryPerformanceCounter( (LARGE_INTEGER*)&l_i64Check );
			if( ( l_i64Check - l_i64Start )/l_i64Freq > BOARD_SELECT_ACK_TIMEOUT )
			{
				// BRW - Disable debug strings.
				//OutputDebugString( "    SVReadWriteLpt - BOARD_SELECT_ACK_TIMEOUT\n" );
				if( ( GetStatusPort() & 128 ) == 0 ) // BRW - TVic specific - gets the StatusPort Property
				{
					l_hr = S_FALSE;
				}
				break;
			}
		}

		BYTE l_lControl = static_cast< BYTE >( p_lControl );
		switch( p_lControl )
		{
			case SVControlWriteDigital1:
			{
				// BRW - Disable debug strings.
				//OutputDebugString( "  SVReadWriteLpt - SVControlWriteDigital1" );
				if( m_lParallelBoardInterfaceBehavior == Function00ForWrite1 )
				{
					l_lControl = SVControlWriteDigital1_110;
				}
			}
			case SVControlCameraSetup:
			case SVControlWriteDigital0:
			case SVControlCommand:
			{
				// BRW - Disable debug strings.
				//OutputDebugString( "  SVReadWriteLpt - SVControlCameraSetup / SVControlWriteDigital0 / SVControlCommand" );
				SetControlPort( ( l_nVal | l_lControl ) );	// Move Data to outputs // BRW - TVic specific - sets the ControlPort Property
				BYTE l_nNewOutput;
				if( p_lBit < 0 ) // no bits  -  use entire port.
				{
					l_nNewOutput = static_cast< BYTE >( p_plValue );
				}
				else
				{				// use bits.
					if( p_plValue )
					{
						l_nNewOutput = GetPreviousOutputs( p_lControl ) | ( 1 << p_lBit ) ;
					}
					else
					{
						l_nNewOutput = GetPreviousOutputs( p_lControl ) & ~( 1 << p_lBit );
					}
				}
				SetDataPort( l_nNewOutput );			// Write Data to Data Port // BRW - TVic specific - sets the DataPort Property
				SetPreviousOutputs( p_lControl, l_nNewOutput );
				SetControlPort( ( unsigned short )( l_nVal | l_lControl | SVControlSelectMode ) ); // BRW - TVic specific - sets the ControlPort Property
				// **** Wait for Acknowledge...
				QueryPerformanceCounter( (LARGE_INTEGER*)&l_i64Start );
				while( ( GetStatusPort() & 128 ) != 0 ) // BRW - TVic specific - gets the StatusPort Property
				{
					// BRW - Disable debug strings.
					//OutputDebugString( "  SVReadWriteLpt - Waiting for ACK\n" );
					QueryPerformanceCounter( (LARGE_INTEGER*)&l_i64Check );
					if( ( l_i64Check - l_i64Start )/l_i64Freq > BOARD_SELECT_ACK_TIMEOUT )
					{
						// BRW - Disable debug strings.
						//OutputDebugString( "    SVReadWriteLpt - BOARD_SELECT_ACK_TIMEOUT\n" );
						if( ( GetStatusPort() & 128 ) != 0 ) // BRW - TVic specific - gets the StatusPort Property
						{
							l_hr = S_FALSE;
						}
						break;
					}
				}
				SetControlPort( ( unsigned short )( l_nVal | l_lControl ) ); // BRW - TVic specific - sets the ControlPort Property
				break;
			}
			case SVControlReadDigitalInputs:
			case SVControlReadCommand:
			{
				// BRW - Disable debug strings.
				//OutputDebugString( "  SVReadWriteLpt - SVControlReadDigitalInputs / SVControlReadCommand" );
				SetControlPort( static_cast< BYTE >( l_nVal | SVControlBidirectional | p_lControl ) ); // BRW - TVic specific - sets the ControlPort Property
				SetDataPortInputMode();
				SetControlPort( static_cast< BYTE >( l_nVal | SVControlBidirectional | p_lControl | SVControlSelectMode ) ); // BRW - TVic specific - sets the ControlPort Property
				// **** Wait for Acknowledge...
				QueryPerformanceCounter( (LARGE_INTEGER*)&l_i64Start );
				while( ( GetStatusPort() & 128 ) != 0 ) // BRW - TVic specific - gets the StatusPort Property
				{
					// BRW - Disable debug strings.
					//OutputDebugString( "  SVReadWriteLpt - Waiting for ACK\n" );
					QueryPerformanceCounter( (LARGE_INTEGER*)&l_i64Check );
					if( ( l_i64Check - l_i64Start )/l_i64Freq > BOARD_SELECT_ACK_TIMEOUT )
					{
						// BRW - Disable debug strings.
						//OutputDebugString( "    SVReadWriteLpt - BOARD_SELECT_ACK_TIMEOUT\n" );
						if( ( GetStatusPort() & 128 ) != 0 ) // BRW - TVic specific - gets the StatusPort Property
						{
							l_hr = S_FALSE;
						}
						break;
					}
				}
				if( p_lBit < 0 )
				{
					p_plValue = GetDataPort(); // BRW - TVic specific - gets the DataPort Property
				}
				else
				{
					p_plValue = ( GetDataPort() & ( 1 << p_lBit ) ) ? 1 : 0; // BRW - TVic specific - gets the DataPort Property
				}
				SetControlPort( static_cast< BYTE >( l_nVal | SVControlBidirectional | p_lControl ) ); // BRW - TVic specific - sets the ControlPort Property
				SetDataPortOutputMode();
				SetControlPort( static_cast< BYTE >( l_nVal | SVControlBidirectional ) ); // BRW - TVic specific - sets the ControlPort Property
				break;
			}
			default:
			{
				// BRW - Disable debug strings.
				//OutputDebugString( "  SVReadWriteLpt - default" );
				l_hr = S_FALSE;
				break;
			}
		}
		// Put control port back in to No Mode 
		SetControlPort( l_nVal ); // BRW - TVic specific - sets the ControlPort Property
		UnLock();
	}
	else
	{
		l_hr = S_FALSE;
	}
	// BRW - Disable debug strings.
	//l_Message.Format( _T( "End - SVReadWriteLpt( %x, %x, %x )\r" ), p_plValue, p_lControl, p_lBit );
	//OutputDebugString( l_Message.ToString() );

	return l_hr;
}

short SVContecImpl::GetPreviousOutputs( long p_lControl )
{
	short l_nPrevOutputs = 0;
	switch( p_lControl )
	{
		case SVControlWriteDigital0:
		{
			l_nPrevOutputs = m_nPreviousOutputs[0];
			break;
		}
		case SVControlWriteDigital1:
		{
			l_nPrevOutputs = m_nPreviousOutputs[1];
			break;
		}
		case SVControlCameraSetup:
		{
			l_nPrevOutputs = m_nPreviousOutputs[2];
			break;
		}
	}
	return l_nPrevOutputs;
}

void SVContecImpl::SetPreviousOutputs( long p_lControl, short p_sValue )
{
	switch( p_lControl )
	{
		case SVControlWriteDigital0:
		{
			m_nPreviousOutputs[0] = p_sValue;
			break;
		}
		case SVControlWriteDigital1:
		{
			m_nPreviousOutputs[1] = p_sValue;
			break;
		}
		case SVControlCameraSetup:
		{
			m_nPreviousOutputs[2] = p_sValue;
			break;
		}
		default:
		{
			// Do nothing for other control types..
			break;
		}
	}
}

HRESULT SVContecImpl::SetOutputBit( unsigned long bitNum, bool bitVal )
{
	// BRW - Disable debug strings.
	//OutputDebugString( "Start - SetOutputBit\n" );
	HRESULT hr = S_FALSE;

	unsigned long l_lData = bitVal ? 1 : 0;

	for( int i = 0; i < RETRY ; i++ )
	{
		if( bitNum < 8 )
		{
			hr = SVReadWriteLpt( l_lData, SVControlWriteDigital0, bitNum );
		}
		else if( bitNum < 16 )
		{
			hr = SVReadWriteLpt( l_lData, SVControlWriteDigital1, bitNum - 8 );
		}
		else if( bitNum < 24 )
		{
			if( m_lBoardVersion == 0x7f7f7f7f && 20 <= bitNum )
			{
				l_lData = ( l_lData == 1 ) ? 0 : 1;
			}

			hr = SVReadWriteLpt( l_lData, SVControlCameraSetup, bitNum - 16 );
		}
		if( hr == S_OK )
		{
			break;
		}
		else
		{
			WriteUnlockString();
		}
	}
	// BRW - Disable debug strings.
	//OutputDebugString( "End - SetOutputBit\n" );
	return hr;
}

HRESULT SVContecImpl::SetOutputValue( unsigned long val )
{
	// BRW - Disable debug strings.
	//OutputDebugString( "Start - SetOutputValue\n" );
	// This function will only write the SVIMs 16 outputs

	HRESULT l_hr = S_OK;
	for( int i = 0 ; i < RETRY ; i++ )
	{
		unsigned long data = val & 0xff;
		if( SVReadWriteLpt( data, SVControlWriteDigital0 ) != S_OK )
		{
			l_hr = S_FALSE;
		}
		data = ( val >> 8 ) & 0xff;
		if( SVReadWriteLpt( data, SVControlWriteDigital1 ) != S_OK )
		{
			l_hr = S_FALSE;
		}
		if( l_hr == S_OK )
		{
			break;
		}
		else
		{
			WriteUnlockString();
		}
	}
	// The camera control bits could be written along with the outputs since
	// there are only 16 output bits.

//	data = ( val >> 16 ) & 0xff;
//	if( SVReadWriteLpt( data, SVControlCameraSetup ) != S_OK )
//	{
//		l_hr = S_FALSE;
//	}
	// BRW - Disable debug strings.
	//OutputDebugString( "End - SetOutputValue\n" );

	return l_hr;
}

HRESULT SVContecImpl::GetPortInputValue( unsigned portNo, unsigned long* pVal )
{
	return GetInputValue(pVal);
}

HRESULT SVContecImpl::SetPortOutputValue( unsigned portNo, unsigned long val )
{
	return SetOutputValue(val);
}

unsigned long SVContecImpl::GetTriggerCount()
{
	return m_numTriggers;
}

HANDLE SVContecImpl::GetTriggerHandle( unsigned long index )
{
	return (HANDLE)(index + 1);
}

BSTR SVContecImpl::GetTriggerName( HANDLE handle )
{
	char l_szTmp[128];
	l_szTmp[0]=0;

	int l_iIndex = (int) handle - 1;
	if( l_iIndex >= 0 && l_iIndex < m_numTriggers )
	{
		strcpy( &l_szTmp[0], "IO_Board_1.Dig_" );
		size_t l_iStrLen = strlen( &l_szTmp[0] );
		char* l_sz = _itoa( static_cast< int >( l_iIndex ), &l_szTmp[l_iStrLen], 10 );
		l_szTmp[ l_iStrLen + 1 ] = 0;
	}

	BSTR name = _com_util::ConvertStringToBSTR(l_szTmp);

	return name;
}

HRESULT SVContecImpl::AddTriggerCallback( HANDLE handle, SVContecCallbackPtr pCallback, void* pOwner, void* pData )
{
	HRESULT hr = S_FALSE;

	TriggerList::iterator it = m_triggerList.find( handle );
	if (it != m_triggerList.end())
	{
		TriggerCallbackList& list = it->second;

		// check for dups
		TriggerCallbackList::iterator callbackIt = std::find_if( list.begin(), list.end(), std::bind2nd( SVContecTriggerFind(), pCallback ) );
		
		if (callbackIt != list.end())
		{
			// DUPLICATE Entry!!!
		}
		else
		{
			// add it
			SVContecTriggerStruct triggerStruct;
			triggerStruct.bStarted = false;
			triggerStruct.pCallback = pCallback;
			triggerStruct.pOwner = pOwner;
			triggerStruct.pData = pData;

			list.push_back(triggerStruct);
			hr = S_OK;
		}
	}
	else
	{
		// add it
		TriggerCallbackList list;
		SVContecTriggerStruct triggerStruct;
		triggerStruct.bStarted = false;
		triggerStruct.pCallback = pCallback;
		triggerStruct.pOwner = pOwner;
		triggerStruct.pData = pData;

		list.push_back(triggerStruct);
		m_triggerList.insert( std::make_pair( handle, list ) );

		hr = S_OK;
	}
	return hr;
}

HRESULT SVContecImpl::RemoveTriggerCallback( HANDLE handle, SVContecCallbackPtr pCallback )
{
	HRESULT hr = S_FALSE;

	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		// check if it is in the list
		TriggerCallbackList& list = it->second;
		
		TriggerCallbackList::iterator callbackIt = std::find_if( list.begin(), list.end(), std::bind2nd( SVContecTriggerFind(), pCallback ) );
		if (callbackIt != list.end())
		{
			list.erase(callbackIt);
			hr = S_OK;
		}
	}
	return hr;
}

HRESULT SVContecImpl::RemoveAllTriggerCallbacks( HANDLE handle )
{
	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		TriggerCallbackList& list = it->second;
		
		for (size_t i = 0;i < list.size();i++)
		{
			list[i].bStarted = false;
		}
		m_triggerList.erase(it);
	}
	return S_OK;
}

HRESULT SVContecImpl::StartTrigger( HANDLE handle )
{
	HRESULT hr = S_FALSE;

	#ifdef SV_LOG_STATUS_INFO
		m_StatusLog.clear();
	#endif

	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		TriggerCallbackList& list = it->second;
		
		for (size_t i = 0;i < list.size();i++)
		{
			list[i].bStarted = true;
		}
		hr = S_OK;
	}
	if( hr == S_OK )
	{
		// Initialize previous Trigger State
		m_lLastTriggerState = GetStatusPort(); // BRW - TVic specific - gets the StatusPort Property

		hr = UnmaskIrq(); // BRW - TVic specific - enables interrupt
	}

	#ifdef SV_LOG_STATUS_INFO
		if( hr != S_OK )
		{
			SVString l_String;

			l_String.Format( _T( "StartTrigger - Status = 0x%X" ), hr );

			m_StatusLog.push_back( l_String );
		}
	#endif

	return hr;
}

HRESULT SVContecImpl::StopTrigger( HANDLE handle )
{
	HRESULT hr = S_FALSE;

	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		TriggerCallbackList& list = it->second;
		
		for (size_t i = 0;i < list.size();i++)
		{
			list[i].bStarted = false;
		}
		hr = S_OK;
	}
	if( hr == S_OK )
	{
		bool l_bDisableIrq = true;
		for( it = m_triggerList.begin(); it != m_triggerList.end(); ++it )
		{
			TriggerCallbackList& list = it->second;

			if( 0 == list.size() || list[0].bStarted )
			{
				l_bDisableIrq = false;
			}
		}
		if( l_bDisableIrq ) // Make sure all triggers are stopped before Masking the IRQ
		{
			hr = MaskIrq(); // BRW - TVic specific - disables interrupt
		}
	}

	#ifdef SV_LOG_STATUS_INFO
		SVString l_FileName;

		l_FileName.Format( _T( "C:\\SVObserver\\SVTVicLpt.log" ) );

		std::fstream l_Stream( l_FileName.ToString(), std::ios_base::trunc | std::ios_base::out );

		if( l_Stream.is_open() )
		{
			for( int i = 0; i < m_StatusLog.GetCount(); ++i )
			{
				SVString l_String;

				m_StatusLog.GetAt( i, &l_String );

				l_Stream << l_String.ToString() << std::endl;
			}

			l_Stream.close();

			m_StatusLog.clear();
		}
	#endif

	return hr;
}

/*void SVContecImpl::FireLptInterrupt( unsigned short DataReg, unsigned short StatusReg, unsigned long LoTime, unsigned long HiTime )
{
	::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL );

#ifdef LogDebugData
	g_TDebugData[g_CallbackCount].m_dStatusReg0 = StatusReg;
	g_TDebugData[g_CallbackCount].m_dStatusReg1 = GetStatusPort();
	::QueryPerformanceCounter( ( LARGE_INTEGER* )&g_perf );
	g_TDebugData[g_CallbackCount].m_lTimeStamp1 = g_perf;

	((SVLargeInteger*)&g_TDebugData[g_CallbackCount].m_64lParTimeStamp)->HiWord = HiTime ;
	((SVLargeInteger*)&g_TDebugData[g_CallbackCount].m_64lParTimeStamp)->LoWord = LoTime ;
#endif

	unsigned short l_StatusReg = GetStatusPort(); // BRW - TVic specific - gets the StatusPort Property

	#ifdef SV_LOG_STATUS_INFO
		SVString l_String;

		l_String.Format( _T( "FireLptInterrupt - StatusReg = 0x%X" ), l_StatusReg );

		m_StatusLog.push_back( l_String );
	#endif

	// call trigger callbacks
	TriggerList::iterator it;

	for (it = m_triggerList.begin();it != m_triggerList.end() ;it++)
	{

		long l_lTrigger = ( long ) it->first ; // Trigger = the 1 based handle.

		short l_nTriggerBit=SVTriggerNone;

		// Get the trigger bit 
		switch( l_lTrigger )
		{
			case 1: // trigger 1 Bit 3 of Status port
			{
				l_nTriggerBit = SVTrigger1;
				break;
			}
			case 2:
			{
				l_nTriggerBit = SVTrigger2;
				break;
			}
			case 3:
			{
				l_nTriggerBit = SVTrigger3;
				break;
			}
			default:
			{
				break;
			}
		}

		if (( (l_nTriggerBit != SVTriggerNone) &&
			( (l_nTriggerBit & l_StatusReg ) != ( l_nTriggerBit & m_lLastTriggerState )) && 
			 (( l_nTriggerBit & l_StatusReg ) == ( l_nTriggerBit & m_lLptTriggerEdge )) ) || m_bUseSingleTrigger )
		{
			#ifdef SV_LOG_STATUS_INFO
				l_String.Format( _T( "FireLptInterrupt - TriggerBit = 0x%X - TriggerEdge = 0x%X" ), l_nTriggerBit, m_lLptTriggerEdge );

				m_StatusLog.push_back( l_String );
			#endif

			TriggerCallbackList& list = it->second;

			for (size_t i = 0;i < list.size();i++)
			{
				if (list[i].bStarted)
				{
					( list[ i ].pCallback )( list[ i ].pOwner, list[ i ].pData );
				}
			}
		}
	}
	m_lLastTriggerState = l_StatusReg;
	unsigned short l_TmpStatusReg = GetStatusPort(); // BRW - TVic specific - gets the StatusPort Property
	if( l_TmpStatusReg != l_StatusReg)
	{
		FireLptInterrupt(DataReg, StatusReg, LoTime, HiTime);
	}

#ifdef LogDebugData
	// **** Debug logging ****
	::QueryPerformanceCounter( ( LARGE_INTEGER* )&g_perf );
	g_TDebugData[g_CallbackCount].m_dStatusReg2 = GetStatusPort();
	g_TDebugData[g_CallbackCount].m_lTimeStamp2 = g_perf ;
	if( g_CallbackCount < MaxDebugData )
		g_CallbackCount++; //*//*
#endif
}*/

void SVContecImpl::CallBackProc( short Id, WPARAM wParam, LPARAM lParam, void *Param )
{
	SVContecImpl* l_Impl = reinterpret_cast< SVContecImpl* >( Param );
	l_Impl->NewFireLptInterrupt( Id, wParam, lParam, Param );
}

void SVContecImpl::NewFireLptInterrupt( short Id, WPARAM wParam, LPARAM lParam, void *Param )
{
	::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL );

#ifdef LogDebugData
	g_TDebugData[g_CallbackCount].m_dStatusReg0 = StatusReg;
	g_TDebugData[g_CallbackCount].m_dStatusReg1 = GetStatusPort();
	::QueryPerformanceCounter( ( LARGE_INTEGER* )&g_perf );
	g_TDebugData[g_CallbackCount].m_lTimeStamp1 = g_perf;

	((SVLargeInteger*)&g_TDebugData[g_CallbackCount].m_64lParTimeStamp)->HiWord = HiTime ;
	((SVLargeInteger*)&g_TDebugData[g_CallbackCount].m_64lParTimeStamp)->LoWord = LoTime ;
#endif

	unsigned short l_StatusReg = GetStatusPort(); // BRW - TVic specific - gets the StatusPort Property

	#ifdef SV_LOG_STATUS_INFO
		SVString l_String;

		l_String.Format( _T( "FireLptInterrupt - StatusReg = 0x%X" ), l_StatusReg );

		m_StatusLog.push_back( l_String );
	#endif

	// call trigger callbacks
	TriggerList::iterator it;

	for (it = m_triggerList.begin();it != m_triggerList.end() ;it++)
	{

		long l_lTrigger = ( long ) it->first ; // Trigger = the 1 based handle.

		short l_nTriggerBit=SVTriggerNone;

		// Get the trigger bit 
		switch( l_lTrigger )
		{
			case 1: // trigger 1 Bit 3 of Status port
			{
				l_nTriggerBit = SVTrigger1;
				break;
			}
			case 2:
			{
				l_nTriggerBit = SVTrigger2;
				break;
			}
			case 3:
			{
				l_nTriggerBit = SVTrigger3;
				break;
			}
			default:
			{
				break;
			}
		}

		if (( (l_nTriggerBit != SVTriggerNone) &&
			( (l_nTriggerBit & l_StatusReg ) != ( l_nTriggerBit & m_lLastTriggerState )) && 
			 (( l_nTriggerBit & l_StatusReg ) == ( l_nTriggerBit & m_lLptTriggerEdge )) ) || m_bUseSingleTrigger )
		{
			#ifdef SV_LOG_STATUS_INFO
				l_String.Format( _T( "FireLptInterrupt - TriggerBit = 0x%X - TriggerEdge = 0x%X" ), l_nTriggerBit, m_lLptTriggerEdge );

				m_StatusLog.push_back( l_String );
			#endif

			TriggerCallbackList& list = it->second;

			for (size_t i = 0;i < list.size();i++)
			{
				if (list[i].bStarted)
				{
					( list[ i ].pCallback )( list[ i ].pOwner, list[ i ].pData );
				}
			}
		}
	}
	m_lLastTriggerState = l_StatusReg;
	unsigned short l_TmpStatusReg = GetStatusPort(); // BRW - TVic specific - gets the StatusPort Property
	if( l_TmpStatusReg != l_StatusReg)
	{
		NewFireLptInterrupt( Id, wParam, lParam, Param );
	}

#ifdef LogDebugData
	// **** Debug logging ****
	::QueryPerformanceCounter( ( LARGE_INTEGER* )&g_perf );
	g_TDebugData[g_CallbackCount].m_dStatusReg2 = GetStatusPort();
	g_TDebugData[g_CallbackCount].m_lTimeStamp2 = g_perf ;
	if( g_CallbackCount < MaxDebugData )
		g_CallbackCount++; //*/
#endif
}

// SetBoardType
// Set Board Type sends a sub command to tell the parallel board 
// to be setup for X1 hardware or to be setup for D-Series hardware
// The valid sub commands are 0x58 and 0x59
HRESULT SVContecImpl::SetBoardType( unsigned long p_lBoardType )
{
	HRESULT l_hr = S_OK;
	
	// Sub Command set board type 
	unsigned long l_lBoardType = SET_SYSTEM_TYPE;
	l_hr = SVReadWriteLpt( l_lBoardType, SVControlCommand );
	if( l_hr == S_OK )
	{
		l_hr = SVReadWriteLpt( p_lBoardType, SVControlCommand );
	}

	return l_hr;
}

// GetBoardType
// Get Board Type sends a sub command to tell the parallel board 
// to send the board type on the next SVControlReadCommand.
HRESULT SVContecImpl::GetBoardType( unsigned long& p_lBoardType )
{
	HRESULT l_hr = S_OK;
	
	unsigned long l_lCommand = GET_SYSTEM_TYPE;
	// Sub Command set board type 
	l_hr = SVReadWriteLpt( l_lCommand, SVControlCommand );
	if( l_hr == S_OK )
	{
		unsigned long l_lTmp;
		l_hr = SVReadWriteLpt( l_lTmp, SVControlReadCommand );
		p_lBoardType = l_lTmp;
	}

	return l_hr;
}

// GetFanState
// Get Fan State sends a sub command to tell the parallel board 
// to send the fan state on the next SVControlReadCommand.
HRESULT SVContecImpl::GetFanState( unsigned long& p_lFanState )
{
	HRESULT l_hr = S_OK;
	
	// Sub Command Read Fan State 
	unsigned long l_lCommand = READ_FANS;
	l_hr = SVReadWriteLpt( l_lCommand, SVControlCommand );
	if( l_hr == S_OK )
	{
		unsigned long l_lTmp;
		if( (l_hr = SVReadWriteLpt( l_lTmp, SVControlReadCommand )) == S_OK)
		{
			p_lFanState = l_lTmp;
		}
	}

	return l_hr;
}

// This function is for debugging fan monitoring.
HRESULT SVContecImpl::GetFanFreq( long* p_plFanFreq )
{
	HRESULT l_hr = S_OK;
	unsigned long l_lTmp = 0;

	unsigned long l_lCommand = NO_CMD;
	l_hr = SVReadWriteLpt( l_lCommand, SVControlCommand );

	l_lCommand = READ_FAN_FREQ;
	l_hr = SVReadWriteLpt( l_lCommand, SVControlCommand );
	if( l_hr == S_OK )
	{
		for( int i = 0 ; i < 4 ; i++ )
		{
			l_hr = SVReadWriteLpt( l_lTmp, SVControlReadCommand );
			if( l_hr == S_OK)
			{
				*(p_plFanFreq + i) = l_lTmp;
			}
			else
			{
				break;
			}
		}
	}

	return l_hr;
}

// This function is for debugging IO Bits.
// It reads the log from the rabbit up to 1024 elements
HRESULT SVContecImpl::GetLog( unsigned long* p_pulTS, unsigned long* p_pucLog, long& p_lSize )
{
	HRESULT l_hr = S_OK;
	unsigned long l_lTmp = 0;

	unsigned long l_lCommand = NO_CMD;
	l_hr = SVReadWriteLpt( l_lCommand, SVControlCommand );

	l_lCommand = READ_LOG;
	l_hr = SVReadWriteLpt( l_lCommand, SVControlCommand );
	if( l_hr == S_OK )
	{
		for( int i = 0 ; i < p_lSize && l_hr == S_OK ; i++ )
		{
			// Logged time stamp.
			unsigned long l_lTimeStamp=0;
			for( int k = 0 ; l_hr == S_OK && k < 4 ; k++ )
			{
				l_hr = SVReadWriteLpt( l_lTmp, SVControlReadCommand );
				l_lTimeStamp += l_lTmp << (k*8);
			}

			// Logged command
			if( l_hr == S_OK )
			{
				l_hr = SVReadWriteLpt( l_lTmp, SVControlReadCommand );
				if( l_hr == S_OK)
				{
					*( p_pulTS + i ) = l_lTimeStamp;
					*( p_pucLog + i ) = l_lTmp;
				}
				else
				{
					l_hr = S_OK;
					p_lSize = i;
					break;
				}
			}
			else
			{
				l_hr = S_OK;
				p_lSize = i;
				break;
			}
		}
	}

	return l_hr;
}

// This function is for debugging IO Bits.
HRESULT SVContecImpl::GetRTC( unsigned long& p_lValue )
{
	HRESULT l_hr = S_OK;
	unsigned long l_lTmp = 0;

	unsigned long l_lCommand = NO_CMD;
	l_hr = SVReadWriteLpt( l_lCommand, SVControlCommand );

	l_lCommand = READ_RTC;
	l_hr = SVReadWriteLpt( l_lCommand, SVControlCommand );
	if( l_hr == S_OK )
	{
		for( int i = 0 ; i < 4 ; i++ )
		{
			l_hr = SVReadWriteLpt( l_lTmp, SVControlReadCommand );
			if( l_hr == S_OK)
			{
				p_lValue += l_lTmp << (i*8);
			}
			else
			{
				break;
			}
		}
	}

	return l_hr;
}

// This function is for debugging IO Bits.
// This function sends the RTC in 6 bytes.
// byte 0 - reset value is zero.
// byte 1 - Setup command to Write RTC
// byte 2 - bits 0-7
// byte 3 - bits 8-15
// byte 4 - bits 16-23
// byte 5 - bits 24-31
HRESULT SVContecImpl::SetRTC( unsigned long p_lValue )
{
	HRESULT l_hr = S_OK;
	unsigned long l_lTmp = 0;

	unsigned long l_lCommand = NO_CMD;
	l_hr = SVReadWriteLpt( l_lCommand, SVControlCommand );

	l_lCommand = WRITE_RTC;	// Get the Rabbit ready to receive a string of bytes for RTC set
	l_hr = SVReadWriteLpt( l_lCommand, SVControlCommand );
	if( l_hr == S_OK )
	{
		for( int i = 0 ; i < 4 ; i++ )
		{
			l_lTmp = p_lValue >> (i*8);
			l_hr = SVReadWriteLpt( l_lTmp, SVControlCommand );	// Write 4 Bytes to fill a long in the Rabbit.
			if( l_hr != S_OK)
			{
				break;
			}
		}
	}

	return l_hr;
}

// This function is for debugging IO Bits.
// This function sends a value to the rabbit log in 2 bytes.
// byte 0 - Setup command to Write Log.
// byte 1 - data to write in log...
HRESULT SVContecImpl::SetLogValue( unsigned long p_lValue )
{
	HRESULT l_hr = S_OK;
	unsigned long l_lTmp = 0;

	unsigned long l_lCommand = WRITE_LOG;	// Get the Rabbit ready to receive a string of bytes for RTC set
	l_hr = SVReadWriteLpt( l_lCommand, SVControlCommand );
	if( l_hr == S_OK )
	{
		l_hr = SVReadWriteLpt( p_lValue, SVControlCommand );	// Write Log Byte
	}

	return l_hr;
}

// Reads the lock State
// Byte 0 - Command to Read Lock State
// Byte 1 - Read Back lock state.
HRESULT SVContecImpl::GetLockState( bool& p_bLocked )
{
	HRESULT l_hr = S_OK;
	
	unsigned long l_lCommand = READ_LOCK_STATE;
	// Sub Command set board type 
	l_hr = SVReadWriteLpt( l_lCommand, SVControlCommand );
	if( l_hr == S_OK )
	{
		unsigned long l_lTmp;
		l_hr = SVReadWriteLpt( l_lTmp, SVControlReadCommand );
		p_bLocked = (l_lTmp == 1) ;
	}

	return l_hr;
}

// SetTriggerEdge
// Set Trigger Edge sets the trigger edge in the parallel I/O board
// So we only get the interrupts for the proper edge.
HRESULT SVContecImpl::SetTriggerEdge( unsigned long p_lTriggerEdge )
{
	HRESULT l_hr = S_OK;
	
	if( !( m_lBoardVersion == 0 || m_lBoardVersion == 0x1010101 || m_lBoardVersion == 0x7f7f7f7f ) )
	{
		unsigned long l_lCommand = SET_TRIGGER_EDGE;
		// Sub Command set board type 
		l_hr = SVReadWriteLpt( l_lCommand, SVControlCommand );
		if( l_hr == S_OK )
		{
			l_lCommand = p_lTriggerEdge;
			l_hr = SVReadWriteLpt( l_lCommand, SVControlCommand );
		}
	}
	else
	{
		l_hr = S_FALSE; //SV_WRONG_BOARD_TYPE
	}

	return l_hr;
}
// end 


HRESULT SVContecImpl::TriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		if ( 0 < p_ulHandle )
		{
			*p_pulCount = 3;

			l_hrOk = S_OK;
		}
		else
		{
			*p_pulCount = 0;
		}
	}

	return l_hrOk;
}

HRESULT SVContecImpl::TriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pbstrName != NULL )
	{
		if ( *p_pbstrName != NULL )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = NULL;
		}

		if ( 0 < p_ulHandle )
		{
			// SVSignalEdge and SVBoardVersion enums are used here to make the code more clear.
			// however at some time in the future the Dll parameters may be implemented
			// as an array and therefore this enum may not apply.
			if ( p_ulIndex == SVSignalEdge )
			{
				*p_pbstrName = ::SysAllocString( L"Edge" );

			}
			if( p_ulIndex == SVSingleTriggerEdge )
			{
				*p_pbstrName = ::SysAllocString( L"Single Trigger Edge");
			}
			if( p_ulIndex == SVBoardVersion )
			{
				*p_pbstrName = ::SysAllocString( L"Board Version");
			}

			if ( *p_pbstrName != NULL )
			{
				l_hrOk = S_OK;
			}
		}
	}

	return l_hrOk;
}

HRESULT SVContecImpl::TriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pvarValue != NULL )
	{
		if ( ::VariantClear( p_pvarValue ) == S_OK )
		{
			if ( 0 < p_ulHandle )
			{
				// SVSignalEdge enum is used here to make the code more clear.
				// however at some time in the future the Dll parameters may be implemented
				// as an array and therefore this enum may not apply.
				if ( p_ulIndex == SVSignalEdge )
				{
					long l_lMask;
					switch( p_ulHandle )
					{
						case 1: 
						{
							l_lMask = SVContecImpl::SVTrigger1;
							break;
						}
						case 2:
						{
							l_lMask = SVContecImpl::SVTrigger2;
							break;
						}
						case 3:
						{
							l_lMask = SVContecImpl::SVTrigger3;
							break;
						}
						case 4:
						{
							l_lMask = SVContecImpl::SVTrigger4;
							break;
						}
					}

					p_pvarValue->vt = VT_I4;

					if ( ( m_lLptTriggerEdge & l_lMask ) == l_lMask )
					{
						p_pvarValue->lVal = 1;
					}
					else
					{
						p_pvarValue->lVal = -1;
					}

					l_hrOk = S_OK;
				}
				// SVBoardVersion enum is used here to make the code more clear.
				// however at some time in the future the Dll parameters may be implemented
				// as an array and therefore this enum may not apply.
				if( p_ulIndex == SVBoardVersion )
				{
					long l_lVer;
					WCHAR wbuf[256];
					p_pvarValue->vt = VT_BSTR;
					l_hrOk = GetBoardVersion( l_lVer );
				swprintf( wbuf, L"Dedicated I/O Processor board %d.%02d firmware %d.%02d",
						l_lVer >> 24 & 0xff,
						l_lVer >> 16 & 0xff,
						l_lVer >> 8 & 0xff,
						l_lVer & 0xff);
					p_pvarValue->bstrVal = ::SysAllocString( wbuf );
				}
			}
		}
	}

	return l_hrOk;
}

HRESULT SVContecImpl::TriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle )
	{
		if ( p_pvarValue != NULL )
		{
			// SVSignalEdge enum is used here to make the code more clear.
			// however at some time in the future the Dll parameters may be implemented
			// as an array and therefore this enum may not apply.
			if ( p_ulIndex == SVSignalEdge )
			{
				if ( p_pvarValue->vt == VT_I4 )
				{
					long l_lMask = 0; //1 << ( p_ulHandle - 1 );

					switch( p_ulHandle )
					{
						case 1: 
						{
							l_lMask = SVContecImpl::SVTrigger1;
							break;
						}
						case 2:
						{
							l_lMask = SVContecImpl::SVTrigger2;
							break;
						}
						case 3:
						{
							l_lMask = SVContecImpl::SVTrigger3;
							break;
						}
						case 4:
						{
							l_lMask = SVContecImpl::SVTrigger4;
							break;
						}
					}

					switch ( p_pvarValue->lVal )
					{
						case 1:
						{
							m_lLptTriggerEdge |= l_lMask;
							m_lIOBrdTriggerEdge |= 1 << (p_ulHandle -1);

							l_hrOk = S_OK;

							break;
						}
						case -1:
						{
							m_lLptTriggerEdge &= ~l_lMask;
							m_lIOBrdTriggerEdge &= ~(1 << (p_ulHandle -1));

							l_hrOk = S_OK;

							break;
						}
						default:
						{
							break;
						}
					}

					l_hrOk = SetTriggerEdge( m_lIOBrdTriggerEdge );
				}
			}
			if( p_ulIndex == SVSingleTriggerEdge ) // Use Single Trigger Methodology
			{
				if ( p_pvarValue->vt == VT_I4 )
				{
					if( p_pvarValue->lVal == 0 )
					{
						m_bUseSingleTrigger = false;
					}
					else
					{
						m_bUseSingleTrigger = true;
					}
				}
				if( p_pvarValue->vt == VT_BOOL )
				{
					m_bUseSingleTrigger = p_pvarValue->bVal == TRUE;
				}
			}

		}
	}

	return l_hrOk;
}

// Non-Trigger Parameter Functions

// GetParameterCount
// This function returns the number of available parameters.
HRESULT SVContecImpl::GetParameterCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		*p_pulCount = 3;
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

// GetParameterName
// This function creates a BSTR with the name of the parameter
// It is the responsibility of the caller to free the BSTR.
HRESULT SVContecImpl::GetParameterName( unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pbstrName != NULL )
	{
		if ( *p_pbstrName != NULL )
		{
			::SysFreeString( *p_pbstrName );

			*p_pbstrName = NULL;
		}

		if( p_ulIndex == SVBoardVersion )
		{
			*p_pbstrName = ::SysAllocString( L"Board Version(R)");
		}

		if ( p_ulIndex == SVFanState )
		{
			*p_pbstrName = ::SysAllocString( L"Fan State(R)" );
		}

		if ( p_ulIndex == SVBoardType )
		{
			*p_pbstrName = ::SysAllocString( L"Board Type(R/W)" );

		}

		if( p_ulIndex == SVFanFreq )
		{
			*p_pbstrName = ::SysAllocString( L"Fan Frequency");
		}
			

		if ( *p_pbstrName != NULL )
		{
			l_hrOk = S_OK;
		}

	}

	return l_hrOk;
}

// GetParameterValue
// This function Gets the parameter value specified by p_ulIndex.
HRESULT SVContecImpl::GetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pvarValue != NULL )
	{
		if ( ::VariantClear( p_pvarValue ) == S_OK )
		{
			// Board Version
			if( p_ulIndex == SVBoardVersion )
			{
				long l_lVer;
				WCHAR wbuf[256];
				p_pvarValue->vt = VT_BSTR;
				l_hrOk = GetBoardVersion( l_lVer );
				swprintf( wbuf, L"Dedicated I/O Processor board %d.%02d firmware %d.%02d",
					l_lVer >> 24 & 0xff,
					l_lVer >> 16 & 0xff,
					l_lVer >> 8 & 0xff,
					l_lVer & 0xff);
				p_pvarValue->bstrVal = ::SysAllocString( wbuf );
			}
			else
			// Board Type : X-Series, D-Series, or A-Series Boards.
			if( p_ulIndex == SVBoardType )
			{
				unsigned long l_lType;
				l_hrOk = GetBoardType( l_lType );
				p_pvarValue->vt = VT_I4;
				if( l_hrOk == S_OK )
				{
					p_pvarValue->lVal = l_lType;
				}
				
			}
			else
			// Fan State
			if( p_ulIndex == SVFanState )
			{
				unsigned long l_lFanState;
				p_pvarValue->vt = VT_I4;
				l_hrOk = GetFanState(l_lFanState);
				p_pvarValue->lVal = l_lFanState;
			}
			else
			if( p_ulIndex == SVFanFreq )
			{
				long l_lTmp[4];
				l_hrOk = GetFanFreq( l_lTmp );
				if( l_hrOk == S_OK )
				{	// The results are only 8 bits each so stuff all 4 into a 32bit long.
					// This is just for testing.
					// If this is something we want to support then it should be wrapped up in a safearray of longs.
					p_pvarValue->vt = VT_I4;
					p_pvarValue->lVal = (l_lTmp[0] & 0xff) |
						((l_lTmp[1] & 0xff) << 8 ) |
						(( l_lTmp[2] & 0xff ) << 16 ) | 
						(( l_lTmp[3] & 0xff ) << 24 );  
				}
			}
			else
			if( p_ulIndex == SVLogDump )
			{
				unsigned long LogCode[1024];
				unsigned long TimeStamp[1024];

				long l_Size = 2048;
				memset( LogCode, 0, l_Size );
				l_hrOk = GetLog( TimeStamp, LogCode, l_Size );
			

				// Instead of sending the data back through a SafeArray, put it directly into a file here.
				if( l_hrOk == S_OK )
				{
					FILE* fh = fopen( "c:\\SVObserver\\IOBoardLog.txt", "a");
					if( fh != NULL )
					{
						time_t l_time;
						time( &l_time );
						struct tm   *newTime;
						newTime = localtime( &l_time );
						char l_strTmp[50];
						strcpy_s( l_strTmp, 50, asctime( newTime ));
						size_t l_size = strlen(l_strTmp );
						if( l_size >= 25 ) // get rid of the newline from asctime.
							l_strTmp[24]=0;

						fprintf( fh, "\nNew Log Entry %s\n", l_strTmp);
						for( int i = 0 ; i < l_Size ; i++ )
						{
							l_time = TimeStamp[i];
							newTime = localtime( &l_time );
							strcpy_s( l_strTmp, 50, asctime( newTime ));
							l_size = strlen(l_strTmp );
							if( l_size >= 25 ) // get rid of the newline from asctime.
							{
								l_strTmp[24]=0;
							}
							fprintf( fh, "%s >function %02x\n", l_strTmp, LogCode[i]  );
						}
						fclose( fh );
					}
				}
			}
			else
			if( p_ulIndex == SVRabbitRTC )
			{
				unsigned long l_lRTC = 0;
				l_hrOk = GetRTC( l_lRTC );
				if( l_hrOk == S_OK )
				{
					// put data into variant safe array.
					p_pvarValue->lVal = l_lRTC;
					p_pvarValue->vt = VT_I4;
				}
			}
		}
	}

	return l_hrOk;
}

// This logic in this function is where we will match up SVObserver versions and Parallel board versions.
HRESULT SVContecImpl::TranslateBoardType( long& p_ulBoardType )
{
	// At this time the versions have only one split at Rabbit version 1.10 and SVObserver 5.21+ which will not
	// have this function 
	if( (m_lBoardVersion & 0xffff) >= 0x10a && m_lBoardVersion != 0x1010101 && m_lBoardVersion != 0x7f7f7f7f )
	{
		p_ulBoardType |= 0x40;
		m_lParallelBoardInterfaceBehavior = Function00ForWrite1;
	}
	else
	{
		m_lParallelBoardInterfaceBehavior = LegacyFunctions;
	}
	return S_OK;
}

// SetParameterValue
// This function sets the value specified by p_ulIndex.
// if the index is not supported for setting, then an error is returned.
HRESULT SVContecImpl::SetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pvarValue != NULL )
	{
		// Set Board Type
		if( p_ulIndex == SVBoardType ) 
		{
			if( p_pvarValue->vt == VT_I4 )
			{
				long lBoardType = p_pvarValue->lVal;
				if( TranslateBoardType( lBoardType ) == S_OK )
				{
					l_hrOk = SetBoardType( lBoardType );
				}
				else
				{
					l_hrOk = SetBoardType( p_pvarValue->lVal );
				}
			}
		}
		else if( p_ulIndex == SVRabbitRTC )
		{
			long l_lRTC = p_pvarValue->lVal;
			if( p_pvarValue->vt == VT_I4)
			{
				l_hrOk = SetRTC( l_lRTC );
			}
		}
		else if( p_ulIndex == SVRabbitWriteLog )
		{
			long l_lValue = p_pvarValue->lVal;
			l_hrOk = SetLogValue( l_lValue );
		}
	}

	return l_hrOk;
}

/*void SVContecImpl::CallBackProc( short Id, WPARAM wParam, LPARAM lParam, void *Param )
{
	WORD	BitNo;
	short	Kind;
	//char	TextTemp[100];

	//Create the class object
	//CInterruptCallbackDlg *dlg;
	//dlg = (CInterruptCallbackDlg *)Param;

	Id		= LOWORD(wParam);	// ID getted from DioInit function
	BitNo	= LOWORD(lParam);	// Interrupt BitNo
	Kind	= HIWORD(lParam);	// Rise: 1, Fall: 2, Both: 

	// BRW - Switch to handle BitNo other than 0-3.
	switch( BitNo )
	{
		case 14:
		{
			BitNo = 1;
			break;
		}
		default:
		{
			break;
		}
	}

	//if (Kind & DIO_INT_RISE) {
		//dlg->UpCount[BitNo]++;		// Up counter
	//}
	//if (Kind & DIO_INT_FALL) {
		//dlg->DownCount[BitNo]++;	// Down counter
	//}

	//-----------------------------
	// Expression
	//-----------------------------
	//wsprintf(TextTemp, "%d", dlg->UpCount[BitNo]);
	//dlg->m_Up[BitNo]->SetWindowText(TextTemp);
	//wsprintf(TextTemp, "%d", dlg->DownCount[BitNo]);
	//dlg->m_Down[BitNo]->SetWindowText(TextTemp);

	//return;
}*/

// Masking the interrupt disables it.
HRESULT SVContecImpl::MaskIrq()
{
	HRESULT l_hr = S_OK;

	// Disable interrupt.
	l_hr = DioNotifyInterrupt( m_DeviceId, 14, 0, NULL );

	return l_hr;
}

// Unmasking the interrupt enables it.
HRESULT SVContecImpl::UnmaskIrq()
{
	HRESULT l_hr = S_OK;

	// Enable interrupt on PortNo 1, BitNo 6
	// 1 or 0?  0->1 or 1->0?
	l_hr = DioNotifyInterrupt( m_DeviceId, 14, 2, NULL );

	return l_hr;
}

// PortNo 0 is the Data Port.
BYTE SVContecImpl::GetDataPort()
{
	BYTE l_Retval;
	HRESULT l_hr = SetDataPortInputMode();
	l_hr = DioInpByte( m_DeviceId, DATAPORT, &l_Retval );

	//if ( l_hr != S_OK )
	//{
		// BRW - Disable debug strings.
		//SVString l_Message;
		//l_Message.Format( _T( "l_hr = %x; %x = GetDataPort()\n" ), l_hr, l_Retval );
		//OutputDebugString( l_Message.c_str() );
	//}

	return l_Retval;
}

HRESULT SVContecImpl::SetDataPort( BYTE p_Value )
{
	HRESULT l_hr = SetDataPortOutputMode();
	l_hr = DioOutByte( m_DeviceId, DATAPORT, p_Value );

	//if ( l_hr != S_OK )
	//{
		// BRW - Disable debug strings.
		//SVString l_Message;
		//l_Message.Format( _T( "%x = SetDataPort( %x )\n" ), l_hr, p_Value );
		//OutputDebugString( l_Message.c_str() );
	//}

	return l_hr;
}

// PortNo 1 is the Status Port.
BYTE SVContecImpl::GetStatusPort()
{
	BYTE l_Retval;

	HRESULT l_hr = DioInpByte( m_DeviceId, STATUSPORT, &l_Retval );
	// Status Port bits 1 and 7 are inverted.
	l_Retval ^= 193;

	if ( l_Retval & 8 )
	{
		//OutputDebugString( _T( "---T1---\n" ) );
	}
	else if ( l_Retval & 16 )
	{
		OutputDebugString( _T( "---T2---\n" ) );
	}
	else if ( l_Retval & 32 )
	{
		OutputDebugString( _T( "---T3---\n" ) );
	}

	//if ( l_hr != S_OK )
	//{
		// BRW - Disable debug strings.
		//SVString l_Message;
		//l_Message.Format( _T( "l_hr = %x; %x = GetStatusPort()\n" ), l_hr, l_Retval );
		//OutputDebugString( l_Message.c_str() );
	//}

	return l_Retval;
}

// Status Port is input only.  Do we need this?
HRESULT SVContecImpl::SetStatusPort( BYTE p_Value )
{
	HRESULT l_hr = E_FAIL;

	l_hr = DioOutByte( m_DeviceId, STATUSPORT, p_Value );

	//if ( l_hr != S_OK )
	//{
		// BRW - Disable debug strings.
		//SVString l_Message;
		//l_Message.Format( _T( "%x = SetStatusPort( %x )\n" ), l_hr, p_Value );
		//OutputDebugString( l_Message.c_str() );
	//}

	return l_hr;
}

// PortNo 2 is the Control Port.
// Control Port is output only.  Do we need this?
BYTE SVContecImpl::GetControlPort()
{
	BYTE l_Retval;

	//HRESULT l_hr = SetControlPortInputMode();
	HRESULT l_hr = DioInpByte( m_DeviceId, CONTROLPORT, &l_Retval );
	// Control Port bits 0, 1, and 3 are inverted.
	l_Retval ^= 11;

	//if ( l_hr != S_OK || l_Retval == 0xff )
	//{
		// BRW - Disable debug strings.
		//SVString l_Message;
		//short l_Flags;
		//DioGet8255Mode( m_DeviceId, CHIPNUMBER, &l_Flags );
		//l_Message.Format( _T( "l_Flags:  %x\tGetControlPort:  %x\n" ), l_Flags, l_Retval );
		//OutputDebugString( l_Message.c_str() );
	//}

	return l_Retval;
}

HRESULT SVContecImpl::SetControlPort( BYTE p_Value )
{
	HRESULT l_hr = E_FAIL;

	// BRW - Disable debug strings.
	//SVString l_Message;
	//l_Message.Format( _T( "SetControlPort( %x ) -> " ), p_Value );
	//OutputDebugString( l_Message.c_str() );

	// Control Port bits 0, 1, and 3 are inverted.
	p_Value ^= 11;
	SetControlPortOutputMode();
	l_hr = DioOutByte( m_DeviceId, CONTROLPORT, p_Value );

	// BRW - Disable debug strings.
	//l_Message.Format( _T( "%x\n" ), p_Value );
	//OutputDebugString( l_Message.c_str() );
	return l_hr;
}

bool SVContecImpl::GetActive()
{
	return m_Active;
}

HRESULT SVContecImpl::SetActive( bool p_Active )
{
	HRESULT l_hr = E_FAIL;

	m_Active = p_Active;

	if ( p_Active == m_Active )
	{
		l_hr = S_OK;
	}

	return l_hr;
}

bool SVContecImpl::GetAcquired()
{
	return m_Acquired;
}

HRESULT SVContecImpl::SetAcquired( bool p_Acquired )
{
	HRESULT l_hr = E_FAIL;

	m_Acquired = p_Acquired;

	if ( p_Acquired == m_Acquired )
	{
		l_hr = S_OK;
	}

	return l_hr;
}

HRESULT SVContecImpl::SetControlPortInputMode()
{
	HRESULT l_hr = S_OK;

	if ( m_ControlPortOutbound )
	{
		m_ControlPortOutbound = false;
		SetIOPortsModes();
	}

	return l_hr;
}

HRESULT SVContecImpl::SetControlPortOutputMode()
{
	HRESULT l_hr = S_OK;

	if ( !m_ControlPortOutbound )
	{
		m_ControlPortOutbound = true;
		SetIOPortsModes();
	}

	return l_hr;
}

HRESULT SVContecImpl::SetDataPortInputMode()
{
	HRESULT l_hr = S_OK;

	if ( m_DataPortOutbound )
	{
		m_DataPortOutbound = false;
		SetIOPortsModes();
	}

	return l_hr;
}

HRESULT SVContecImpl::SetDataPortOutputMode()
{
	HRESULT l_hr = S_OK;

	if ( !m_DataPortOutbound )
	{
		m_DataPortOutbound = true;
		SetIOPortsModes();
	}

	return l_hr;
}

HRESULT SVContecImpl::SetIOPortsModes()
{
	short l_Flags = 0x82;

	if ( !m_ControlPortOutbound )
	{
		// Set Control Port to INPUT mode.
		l_Flags ^= 9;
	}

	if ( !m_DataPortOutbound )
	{
		// Set Data Port to INPUT mode.
		l_Flags ^= 0x10;
	}

	HRESULT l_hr = DioSet8255Mode( m_DeviceId, CHIPNUMBER, l_Flags );

	//if ( l_hr != S_OK )
	//{
		// BRW - Disable debug strings.
		//SVString l_Message;
		//l_Message.Format( _T( "DioSet8255Mode returned %x\n" ), l_hr );
		//OutputDebugString( l_Message.c_str() );
	//}

	return l_hr;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVContec\SVContecImpl.cpp_v  $
 * 
 *    Rev 1.0   30 Oct 2013 11:31:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in of Contec files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/