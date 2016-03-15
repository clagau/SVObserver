// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTVICLPTIMPL
// * .File Name       : $Workfile:   SVTVicLptImpl.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   13 May 2013 12:00:42  $
// ******************************************************************************

#include "stdafx.h"
#include <fstream>
#include <time.h>
#include <functional>
#include <algorithm>
#include <string>
#include "SVTVicLptImpl.h"
#include "SVIOLibrary\SVIOParameterEnum.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVTimerLibrary\SVClock.h"
#include "SVMessage\SVMessage.h"


#define RETRY 2

// ******* Timeout for I/O board Acknowledge microseconds.
#define BOARD_SELECT_ACK_TIMEOUT 16000


// ******* Debug Data **********
#define MaxDebugData 500
//#define LogDebugData 

#ifdef LogDebugData

struct TriggerDebugData
{
	SVClock::SVTimeStamp m_lTimeStamp1;
	SVClock::SVTimeStamp m_lTimeStamp2;
	SVClock::SVTimeStamp m_lParTimeStamp;
	unsigned short m_dStatusReg0;
	unsigned short m_dStatusReg1;
	unsigned short m_dStatusReg2;
};

long g_lCriticalSecInits= 0;
TriggerDebugData g_TDebugData[MaxDebugData];
long g_CallbackCount = 0;

#endif

class SVTVicLptTriggerFind : public std::binary_function<SVTVicLptTriggerStruct, SVTVicLptCallbackPtr, bool>
{
public:
	bool operator()(const SVTVicLptTriggerStruct& triggerStruct, const SVTVicLptCallbackPtr& pCallback) const
	{
		return (triggerStruct.pCallback == pCallback);
	}
};



SVTVicLptImpl::SVTVicLptImpl() 
: m_numPorts(2), m_numInputs(12), m_numOutputs(24), m_numTriggers(3), m_lBoardVersion( 0 )
{
	try
	{
		#if( _WIN32_WINNT >= 0x0500 )
		  ::InitializeCriticalSectionAndSpinCount( &m_hCriticalSection, BOARD_SELECT_ACK_TIMEOUT + 100 );
		#else
		  ::InitializeCriticalSection( &m_hCriticalSection );
		#endif
		m_bCriticalSectionCreated = TRUE;
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

SVTVicLptImpl::~SVTVicLptImpl()
{
	MaskIrq();
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

unsigned long SVTVicLptImpl::GetNumPorts()
{
	return m_numPorts;
}

unsigned long SVTVicLptImpl::GetNumInputs()
{
	return m_numInputs;
}

unsigned long SVTVicLptImpl::GetNumOutputs()
{
	return m_numOutputs;
}

HRESULT SVTVicLptImpl::Initialize(bool bInit)
{
	HRESULT hr = S_OK;

	if (bInit && !GetActive())
	{

		SetActive( 1 );
		SetCurrentLPT( 1 );
	
		if (!GetActive())
		{
			hr = S_FALSE;
		}
		else
		{
			SetAcquired(1);
			if (!GetAcquired())
			{
				hr = S_FALSE;
			}
			else
			{
				if( GetCurrentLptMode() < LPT_PS2_MODE )
				{
					hr = S_FALSE;
				}
			}
		}

		// Initialize previous Output States
		for( int i = 0 ; i < SVNumOutputPorts ; i++ )
		{
			m_nPreviousOutputs[i] = 0xff;
		}

		// Initialize previous Trigger State
		m_lLastTriggerState = GetStatusPort();

		time_t l_time;
		time(&l_time);
		unsigned long l_lTime = static_cast<unsigned long>(l_time);
		SetRTC( l_lTime );	// 5 command 

//		SetLogValue( 0xF3 );
		// Unlock I/O Board
		if( WriteUnlockString( ) != S_OK )
		{
			WriteUnlockString( );
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
		FILE* l_pfh = fopen("c:\\temp\\DebugCounts.txt","w");
		SVClock::SVTimeStamp l_LastTimeStamp = g_TDebugData[0].m_lTimeStamp1;
		fprintf( l_pfh, "Critical Section inits %d", g_lCriticalSecInits );
		for( int i = 0 ; i < g_CallbackCount ; i++ )
		{
			INT64 l_TVic = static_cast< INT64 >( SVClock::ConvertTo( SVClock::Microseconds, g_TDebugData[i].m_lParTimeStamp ) );
			INT64 l_Start = static_cast< INT64 >( SVClock::ConvertTo( SVClock::Microseconds, g_TDebugData[i].m_lTimeStamp1 ) );
			INT64 l_Last = static_cast< INT64 >( SVClock::ConvertTo( SVClock::Microseconds, ( g_TDebugData[i].m_lTimeStamp1 - l_LastTimeStamp ) ) );
			INT64 l_End = static_cast< INT64 >( SVClock::ConvertTo( SVClock::Microseconds, g_TDebugData[i].m_lTimeStamp2 ) );

			fprintf( l_pfh, "\nTrigger %04d\n", i );
			fprintf( l_pfh, "%I64d TVic \n", l_TVic );
			fprintf( l_pfh, "%I64d Start | Since last Int %I64d \n", l_Start, l_Last );
			fprintf( l_pfh, "%I64d End \n", l_End ); 
			fprintf( l_pfh, "Status Register - %02x %02x %02x\n", g_TDebugData[i].m_dStatusReg0, g_TDebugData[i].m_dStatusReg1, g_TDebugData[i].m_dStatusReg2 );
			l_LastTimeStamp = g_TDebugData[i].m_lTimeStamp1; 
		}
		fclose( l_pfh );
#endif
//		SetLogValue( 0xF4 );
		if (GetAcquired())
		{
			if( WriteLockString() != S_OK )
			{
				WriteLockString();
			}

			// Block the IRQ
			MaskIrq();

			SetAcquired(0);

			// Unload the driver
			if (GetActive())
				SetActive(0);  

			// Stop all triggers
			m_triggerList.clear();
		}
	}

	return hr;
}

// WriteUnlockString 
// Sends a command 0x55 to the Parallel board
// to enable the outputs.
HRESULT SVTVicLptImpl::WriteUnlockString( )
{
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
	return l_hrOk;
}
// WriteLockString 
// Sends a command 0x55 to the Parallel board
// to enable the outputs.
HRESULT SVTVicLptImpl::WriteLockString( )
{
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
	return l_hrOk;
}



// GetBoardVersion
// Get Board Version sends a Sub Command value of 0x56 (GET_BOARD_VERSION)
// This puts the parallel board in a mode that will send the 
// Board version on the next four ControlReadCommands.
//  
HRESULT SVTVicLptImpl::GetBoardVersion( long& p_rlVer )
{
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
	return l_hr;
}


unsigned long SVTVicLptImpl::GetInputCount()
{
	return m_numInputs;
}

unsigned long SVTVicLptImpl::GetOutputCount()
{
	return m_numOutputs ;
}

unsigned long SVTVicLptImpl::GetPortCount()
{
	return m_numPorts;
}

HRESULT SVTVicLptImpl::GetInputValue(unsigned long* pVal)
{

	SVReadWriteLpt( *pVal, SVControlReadDigitalInputs );
	*pVal = *pVal & 0xff;
	return S_OK;
}

HRESULT SVTVicLptImpl::GetInputBit(unsigned long bitNum, bool& bitVal)
{
	HRESULT hr = S_OK;
	unsigned long l_lValue=0;
	if( bitNum < 8 )
	{
		SVReadWriteLpt( l_lValue, SVControlReadDigitalInputs, bitNum );
	}
	bitVal = l_lValue ? true : false;

	return hr;
}

HRESULT SVTVicLptImpl::Lock()
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

HRESULT SVTVicLptImpl::UnLock()
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

LPCTSTR SVTVicLptImpl::GetControlText( long p_lControl )
{
	LPCTSTR l_pString = NULL;
	{
		switch( p_lControl )
		{
			case SVTVicLptImpl::SVControlCameraSetup :
			{
				l_pString = "Camera Setup";
				break;
			}
			case SVTVicLptImpl::SVControlWriteDigital0 :
			{
				l_pString = "Write Digital Bank 0";
				break;
			}
			case SVTVicLptImpl::SVControlWriteDigital1 :
			{
				l_pString = "Write Digital Bank 1";
				break;
			}
			case SVTVicLptImpl::SVControlReadDigitalInputs :
			{
				l_pString = "Read Digital Inputs";
				break;
			}
			case SVTVicLptImpl::SVControlReadCommand :
			{
				l_pString = "Command Read";
				break;
			}
			case SVTVicLptImpl::SVControlCommand :
			{
				l_pString = "Command Write";
				break;
			}
		}
	}
	return l_pString;
}

HRESULT SVTVicLptImpl::SVReadWriteLpt( unsigned long& p_plValue, long p_lControl, long p_lBit)
{
	HRESULT l_hr = S_OK;

	if( Lock() == S_OK )
	{
		// Reset the Quantum
		HANDLE l_hThread = ::GetCurrentThread();
		DWORD l_dwPriority = ::GetThreadPriority( l_hThread );
		::SetThreadPriority( l_hThread, l_dwPriority );

		unsigned short l_nPrevControl = GetControlPort();
		if( (l_nPrevControl & 0xf) != 0 )
		{
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_INVALID_LINE_STATE, _T("Select Line In wrong State"), StdMessageParams );
		}
		// Get Value of control port interrupt Bit 
		unsigned short l_nVal = SVControlEnableInterrupt ;
		SetControlPort( (unsigned short)( l_nVal ) );	// Set Control to known state with select line off...

		unsigned short l_lPort = GetControlPort() & 0x37;
		if( l_lPort != l_nVal )
		{
			l_hr = S_FALSE;
		}

		SVClock::SVTimeStamp l_Start = SVClock::GetTimeStamp();

		// **** Wait for Acknoledge off...
		while( ( GetStatusPort() & 128 ) == 0 )
		{
			SVClock::SVTimeStamp l_Check = SVClock::GetTimeStamp();

			if( SVClock::ConvertTo( SVClock::Microseconds, ( l_Check - l_Start ) ) > BOARD_SELECT_ACK_TIMEOUT )
			{
				if( (GetStatusPort() & 128 ) == 0 )
				{
					l_hr = S_FALSE;
				}
				break;
			}
		}

		long l_lControl = p_lControl;
		switch( p_lControl )
		{
			case SVControlWriteDigital1:
			{
				if( m_lParallelBoardInterfaceBehavior == Function00ForWrite1 )
				{
					l_lControl = SVControlWriteDigital1_110;
				}
			}
			case SVControlCameraSetup:
			case SVControlWriteDigital0:
			case SVControlCommand:
			{
				SetControlPort( (unsigned short)( l_nVal | l_lControl ) );	// Move Data to outputs
				unsigned short l_nNewOutput;
				if( p_lBit < 0 ) // no bits  -  use entire port.
				{
					l_nNewOutput = (unsigned short)p_plValue;
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
				SetDataPort( l_nNewOutput  );			// Write Data to Data Port
				SetPreviousOutputs( p_lControl, l_nNewOutput );
				SetControlPort( (unsigned short)( l_nVal | l_lControl | SVControlSelectMode ) );
				// **** Wait for Acknoledge...
				l_Start = SVClock::GetTimeStamp();
				while( ( GetStatusPort() & 128 ) != 0 )
				{
					SVClock::SVTimeStamp l_Check = SVClock::GetTimeStamp();
					if( SVClock::ConvertTo( SVClock::Microseconds, ( l_Check - l_Start ) ) > BOARD_SELECT_ACK_TIMEOUT )
					{
						if( (GetStatusPort() & 128 ) != 0 )
						{
							l_hr = S_FALSE;
						}
						break;
					}
				}
				SetControlPort( (unsigned short)( l_nVal | l_lControl ) );
				break;
			}
			case SVControlReadDigitalInputs:
			case SVControlReadCommand:
			{
				SetControlPort( (unsigned short)( l_nVal | SVControlBidirectional | p_lControl ) );
				SetControlPort( (unsigned short)( l_nVal | SVControlBidirectional | p_lControl | SVControlSelectMode ) );
				// **** Wait for Acknoledge...
				l_Start = SVClock::GetTimeStamp();
				while( ( GetStatusPort() & 128 ) != 0 )
				{
					SVClock::SVTimeStamp l_Check = SVClock::GetTimeStamp();
					if( SVClock::ConvertTo( SVClock::Microseconds, ( l_Check - l_Start ) ) > BOARD_SELECT_ACK_TIMEOUT )
					{
						if( (GetStatusPort() & 128 ) != 0 )
						{
							l_hr = S_FALSE;
						}
						break;
					}
				}
				if( p_lBit < 0 )
				{
					p_plValue = GetDataPort();
				}
				else
				{
					p_plValue = (GetDataPort() & (1 << p_lBit)) ? 1 : 0 ;
				}
				SetControlPort( (unsigned short)( l_nVal | SVControlBidirectional | p_lControl ) );
				SetControlPort( (unsigned short)( l_nVal | SVControlBidirectional ) );
				break;
			}
			default:
			{
				l_hr = S_FALSE;
				break;
			}
		}
		// Put control port back in to No Mode 
		SetControlPort( l_nVal );
		UnLock();
	}
	else
	{
		l_hr = S_FALSE;
	}

	return l_hr;
}

short SVTVicLptImpl::GetPreviousOutputs( long p_lControl )
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

void SVTVicLptImpl::SetPreviousOutputs( long p_lControl, short p_sValue )
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


HRESULT SVTVicLptImpl::SetOutputBit(unsigned long bitNum, bool bitVal)
{
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
	return hr;
}

HRESULT SVTVicLptImpl::SetOutputValue(unsigned long val)
{
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

	return l_hr;
}

HRESULT SVTVicLptImpl::GetPortInputValue(unsigned portNo, unsigned long* pVal)
{
	return GetInputValue(pVal);
}

HRESULT SVTVicLptImpl::SetPortOutputValue(unsigned portNo, unsigned long val)
{
	return SetOutputValue(val);
}

unsigned long SVTVicLptImpl::GetTriggerCount()
{
	return m_numTriggers;
}

HANDLE SVTVicLptImpl::GetTriggerHandle(unsigned long index)
{
	return (HANDLE)(index + 1);
}

BSTR SVTVicLptImpl::GetTriggerName(HANDLE handle)
{
	char l_szTmp[128];
	l_szTmp[0]=0;

	int l_iIndex = (int) handle - 1;
	if( l_iIndex >= 0 && l_iIndex < m_numTriggers )
	{
		strcpy( &l_szTmp[0], "IO_Board_1.Dig_" );
		int l_iStrLen = strlen( &l_szTmp[0] );
		char* l_sz = _itoa( l_iIndex, &l_szTmp[l_iStrLen], 10 );
		l_szTmp[l_iStrLen+1] = 0;
	}

	BSTR name = _com_util::ConvertStringToBSTR(l_szTmp);

	return name;
}

HRESULT SVTVicLptImpl::AddTriggerCallback(HANDLE handle, SVTVicLptCallbackPtr pCallback, void* pOwner, void* pData)
{
	HRESULT hr = S_FALSE;

	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		TriggerCallbackList& list = it->second;

		// check for dups
		TriggerCallbackList::iterator callbackIt = std::find_if(list.begin(), list.end(), std::bind2nd(SVTVicLptTriggerFind(), pCallback));
		
		if (callbackIt != list.end())
		{
			// DUPLICATE Entry!!!
		}
		else
		{
			// add it
			SVTVicLptTriggerStruct triggerStruct;
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
		SVTVicLptTriggerStruct triggerStruct;
		triggerStruct.bStarted = false;
		triggerStruct.pCallback = pCallback;
		triggerStruct.pOwner = pOwner;
		triggerStruct.pData = pData;

		list.push_back(triggerStruct);
		m_triggerList.insert(std::make_pair(handle, list));
			
		hr = S_OK;
	}
	return hr;
}

HRESULT SVTVicLptImpl::RemoveTriggerCallback(HANDLE handle, SVTVicLptCallbackPtr pCallback)
{
	HRESULT hr = S_FALSE;

	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		// check if it is in the list
		TriggerCallbackList& list = it->second;
		
		TriggerCallbackList::iterator callbackIt = std::find_if(list.begin(), list.end(), std::bind2nd(SVTVicLptTriggerFind(), pCallback));
		if (callbackIt != list.end())
		{
			list.erase(callbackIt);
			hr = S_OK;
		}
	}
	return hr;
}

HRESULT SVTVicLptImpl::RemoveAllTriggerCallbacks(HANDLE handle)
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

HRESULT SVTVicLptImpl::StartTrigger(HANDLE handle)
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
		m_lLastTriggerState = GetStatusPort();

		hr = UnmaskIrq();
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

HRESULT SVTVicLptImpl::StopTrigger(HANDLE handle)
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
		for( it = m_triggerList.begin() ; it != m_triggerList.end() ; ++it )
		{
			TriggerCallbackList& list = it->second;

			if( 0 == list.size() || list[0].bStarted )
			{
				l_bDisableIrq = false;
			}
		}
		if( l_bDisableIrq ) // Make sure all triggers are stopped before Masking the IRQ
		{
			hr = MaskIrq();
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

void SVTVicLptImpl::FireLptInterrupt(unsigned short DataReg, unsigned short StatusReg, unsigned long LoTime, unsigned long HiTime)
{
	::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL );

#ifdef LogDebugData
	g_TDebugData[g_CallbackCount].m_dStatusReg0 = StatusReg;
	g_TDebugData[g_CallbackCount].m_dStatusReg1 = GetStatusPort();
	g_TDebugData[g_CallbackCount].m_lTimeStamp1 = SVClock::GetTimeStamp();

	LARGE_INTEGER l_Frequency;
	LARGE_INTEGER l_Timestamp;

	::QueryPerformanceFrequency( &l_Frequency );

	l_Timestamp.LowPart = LoTime;
	l_Timestamp.HighPart = HiTime;

	double l_Seconds = static_cast< double >( l_Timestamp.QuadPart ) / static_cast< double >( l_Frequency.QuadPart );

	g_TDebugData[g_CallbackCount].m_lParTimeStamp = SVClock::ConvertFrom( SVClock::Seconds, l_Seconds );
#endif

	unsigned short l_StatusReg = GetStatusPort();

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
					(list[i].pCallback)(list[i].pOwner, list[i].pData);
				}
			}
		}
	}
	m_lLastTriggerState = l_StatusReg;
	unsigned short l_TmpStatusReg = GetStatusPort();
	if( l_TmpStatusReg != l_StatusReg)
	{
		FireLptInterrupt(DataReg, StatusReg, LoTime, HiTime);
	}

#ifdef LogDebugData
	// **** Debug logging ****
	g_TDebugData[g_CallbackCount].m_dStatusReg2 = GetStatusPort();
	g_TDebugData[g_CallbackCount].m_lTimeStamp2 = SVClock::GetTimeStamp();
	if( g_CallbackCount < MaxDebugData )
		g_CallbackCount++; //*/
#endif
}


// SetBoardType
// Set Board Type sends a sub command to tell the parallel board 
// to be setup for X1 hardware or to be setup for D-Series hardware
// The valid sub commands are 0x58 and 0x59
HRESULT SVTVicLptImpl::SetBoardType( unsigned long p_lBoardType )
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
HRESULT SVTVicLptImpl::GetBoardType( unsigned long& p_lBoardType )
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
HRESULT SVTVicLptImpl::GetFanState( unsigned long& p_lFanState )
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
HRESULT SVTVicLptImpl::GetFanFreq( long* p_plFanFreq )
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
HRESULT SVTVicLptImpl::GetLog( unsigned long* p_pulTS, unsigned long* p_pucLog, long& p_lSize )
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
HRESULT SVTVicLptImpl::GetRTC( unsigned long& p_lValue )
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
HRESULT SVTVicLptImpl::SetRTC( unsigned long p_lValue )
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
HRESULT SVTVicLptImpl::SetLogValue( unsigned long p_lValue )
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
HRESULT SVTVicLptImpl::GetLockState( bool& p_bLocked )
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
HRESULT SVTVicLptImpl::SetTriggerEdge( unsigned long p_lTriggerEdge )
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


HRESULT SVTVicLptImpl::TriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount )
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

HRESULT SVTVicLptImpl::TriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName )
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


HRESULT SVTVicLptImpl::TriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
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
							l_lMask = SVTVicLptImpl::SVTrigger1;
							break;
						}
						case 2:
						{
							l_lMask = SVTVicLptImpl::SVTrigger2;
							break;
						}
						case 3:
						{
							l_lMask = SVTVicLptImpl::SVTrigger3;
							break;
						}
						case 4:
						{
							l_lMask = SVTVicLptImpl::SVTrigger4;
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

HRESULT SVTVicLptImpl::TriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
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
							l_lMask = SVTVicLptImpl::SVTrigger1;
							break;
						}
						case 2:
						{
							l_lMask = SVTVicLptImpl::SVTrigger2;
							break;
						}
						case 3:
						{
							l_lMask = SVTVicLptImpl::SVTrigger3;
							break;
						}
						case 4:
						{
							l_lMask = SVTVicLptImpl::SVTrigger4;
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
HRESULT SVTVicLptImpl::GetParameterCount( unsigned long *p_pulCount )
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
HRESULT SVTVicLptImpl::GetParameterName( unsigned long p_ulIndex, BSTR *p_pbstrName )
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
HRESULT SVTVicLptImpl::GetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue )
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
HRESULT SVTVicLptImpl::TranslateBoardType( long& p_ulBoardType )
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
HRESULT SVTVicLptImpl::SetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue )
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
		else
		if( p_ulIndex == SVRabbitRTC )
		{
			long l_lRTC = p_pvarValue->lVal;
			if( p_pvarValue->vt == VT_I4)
			{
				l_hrOk = SetRTC( l_lRTC );
			}
		}
		else
		if( p_ulIndex == SVRabbitWriteLog )
		{
			long l_lValue = p_pvarValue->lVal;
			l_hrOk = SetLogValue( l_lValue );
		}

	}

	return l_hrOk;
}
