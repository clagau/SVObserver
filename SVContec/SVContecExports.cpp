// ******************************************************************************
// * COPYRIGHT (c) 2013 by Seidenader Vision, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVContecExports
// * .File Name       : $Workfile:   SVContecExports.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   30 Oct 2013 11:31:28  $
// ******************************************************************************

#include "StdAfx.h"
#include "SVContecExports.h"

#include "SVContecImpl.h"
#include "SVIOLibrary/SVIOParameterEnum.h"

long g_lRefCount;

SVContecImpl g_Contec;

HRESULT WINAPI SVTVicLptDestroy(bool p_bClose)
{
	HRESULT l_hrOk = S_OK;

	if ( p_bClose || ::InterlockedDecrement( &g_lRefCount ) <= 0 )
	{
		::InterlockedExchange( &g_lRefCount, 0 );

		l_hrOk = g_Contec.Initialize( false );
	}

	return l_hrOk;
}

HRESULT WINAPI SVCreate()
{
	HRESULT l_hrOk = S_OK;

	if ( ::InterlockedIncrement( &g_lRefCount ) == 1 )
	{
		l_hrOk = g_Contec.Initialize( true );

		g_Contec.m_lLptTriggerEdge = 0;
		g_Contec.m_lIOBrdTriggerEdge = 0;
		if ( l_hrOk == S_OK )
		{
			// Trigger Section

			// Input Section
			if ( l_hrOk == S_OK )
			{
			}

			// Output Section
			if ( l_hrOk == S_OK )
			{
			}

			if ( l_hrOk != S_OK )
			{
				SVTVicLptDestroy( true );
			}
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVDestroy()
{
	HRESULT l_hrOk = S_OK;

	SVTVicLptDestroy(false);

	return l_hrOk;
}

HRESULT WINAPI SVInputGetCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		*p_pulCount = g_Contec.GetInputCount();
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

// SEJ - I'm guessing here that channel means bitNo...
HRESULT WINAPI SVInputGetValue( unsigned long p_ulChannel, bool *p_pbValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pbValue != NULL )
	{
		if ( 0 <= p_ulChannel && p_ulChannel < g_Contec.GetInputCount() )
		{
			bool l_bState;
			l_hrOk = g_Contec.GetInputBit( p_ulChannel, l_bState );

			if ( l_hrOk == S_OK )
			{
				*p_pbValue = l_bState;
			}
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVInputSetValue( unsigned long p_ulChannel, bool p_bValue )
{

	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVInputGetPortCount( unsigned long *p_pulCount )
{

	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		*p_pulCount = g_Contec.GetPortCount();

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

// p_ulPort represents the physical port number (LPT 1/2/3/4)
HRESULT WINAPI SVInputGetPortValue( unsigned long p_ulPort, unsigned long *p_pulValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pulValue != NULL )
	{
		if ( 0 <= p_ulPort && p_ulPort < g_Contec.GetNumPorts() )
		{
			unsigned long l_lState = 0;

			l_hrOk = g_Contec.GetPortInputValue( p_ulPort, &l_lState );

			if ( l_hrOk == S_OK )
			{
				*p_pulValue = l_lState;
			}
		}
	}

	return l_hrOk;
}

HRESULT WINAPI SVInputSetPortValue( unsigned long p_ulPort, unsigned long p_ulValue )
{
	HRESULT l_hrOk = S_FALSE;

	return l_hrOk;
}

HRESULT WINAPI SVOutputGetCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		*p_pulCount = g_Contec.GetOutputCount();
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVOutputGetValue( unsigned long p_ulChannel, bool *p_pbValue )
{
	if ( p_pbValue != NULL )
	{
		long l_port = p_ulChannel / 8;
		*p_pbValue = ( g_Contec.m_nPreviousOutputs[ l_port ] & ( 1 << ( p_ulChannel % 8 ) ) ) != 0;
	}
	return S_OK;
}

HRESULT WINAPI SVOutputSetValue( unsigned long p_ulChannel, bool p_bValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( 0 <= p_ulChannel && p_ulChannel < g_Contec.GetOutputCount() )
	{
		l_hrOk = g_Contec.SetOutputBit( p_ulChannel, p_bValue );
	}

	return l_hrOk;
}

HRESULT WINAPI SVOutputGetPortCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		*p_pulCount = g_Contec.GetPortCount();
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVOutputGetPortValue( unsigned long p_ulPort, unsigned long *p_pulValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( 0 <= p_ulPort && p_ulPort < g_Contec.GetNumPorts())
	{
		*p_pulValue = g_Contec.m_nPreviousOutputs[0] | 
			( g_Contec.m_nPreviousOutputs[1] << 8 ) | 
			( g_Contec.m_nPreviousOutputs[2] << 16 );
	}

	return l_hrOk;
}

HRESULT WINAPI SVOutputSetPortValue( unsigned long p_ulPort, unsigned long p_ulValue )
{

	HRESULT l_hrOk = S_FALSE;

	if ( 0 <= p_ulPort && p_ulPort < g_Contec.GetPortCount())
	{
		l_hrOk = g_Contec.SetPortOutputValue( p_ulPort, p_ulValue );
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetCount( unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		*p_pulCount = g_Contec.GetTriggerCount();

		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetHandle( unsigned long *p_pulHandle, unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pulHandle != NULL )
	{
		*p_pulHandle = ( unsigned long )g_Contec.GetTriggerHandle( p_ulIndex );
		l_hrOk = S_OK;
	}

	return l_hrOk;
}

HRESULT WINAPI SVTriggerGetName( unsigned long p_ulHandle, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pbstrName != NULL && p_ulHandle != NULL)
	{
		// free any crap on input...
		if (*p_pbstrName != NULL)
			::SysFreeString(*p_pbstrName);

		*p_pbstrName = g_Contec.GetTriggerName( ( HANDLE )p_ulHandle );
		
		if (*p_pbstrName != NULL)
			l_hrOk = S_OK;
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerRegister( unsigned long p_ulHandle, SVContecCallbackPtr p_pCallback, void *p_pOwner, void *p_pData )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pCallback != NULL && 0 < p_ulHandle)
	{
		l_hrOk = S_OK;

		g_Contec.AddTriggerCallback( ( HANDLE )p_ulHandle, p_pCallback, p_pOwner, p_pData );
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerUnregister( unsigned long p_ulHandle, SVContecCallbackPtr p_pCallback )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pCallback != NULL && 0 < p_ulHandle )
	{
		l_hrOk = g_Contec.RemoveTriggerCallback( ( HANDLE )p_ulHandle, p_pCallback );
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerUnregisterAll( unsigned long p_ulHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle )
	{
		l_hrOk = g_Contec.RemoveAllTriggerCallbacks((HANDLE)p_ulHandle);
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerStart( unsigned long p_ulHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle)
	{
		l_hrOk = g_Contec.StartTrigger((HANDLE)p_ulHandle);
	} 

	return l_hrOk;
}

HRESULT WINAPI SVTriggerStop( unsigned long p_ulHandle )
{
	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle )
	{
		l_hrOk = g_Contec.StopTrigger((HANDLE)p_ulHandle);
	} 

	return l_hrOk;
}

// SVTriggerGetParameterCount
// Gets the number of parameters
// for the trigger represented by p_ulHandle
HRESULT WINAPI SVTriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount )
{
	return g_Contec.TriggerGetParameterCount( p_ulHandle, p_pulCount );
}

// SVTriggerGetParameterName
// Gets the parameter name.
// The caller is responsible for freeing the BSTR.
HRESULT WINAPI SVTriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	return g_Contec.TriggerGetParameterName( p_ulHandle, p_ulIndex, p_pbstrName);
}

// SVTriggerGetParameterValue
// Gets the parameter value for
// the trigger represented by p_ulHandle.
HRESULT WINAPI SVTriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	return g_Contec.TriggerGetParameterValue( p_ulHandle, p_ulIndex, p_pvarValue );
}

// SVTriggerSetParameterValue
// Sets the parameter value for
// the trigger represented by p_ulHandle.
HRESULT WINAPI SVTriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	return g_Contec.TriggerSetParameterValue( p_ulHandle, p_ulIndex, p_pvarValue);
}


// Digital I/O Parameters..No handles...

// SVGetParameterCount
// This function returns the number of available parameters.
HRESULT WINAPI SVGetParameterCount( unsigned long *p_pulCount )
{
	return g_Contec.GetParameterCount( p_pulCount );
}

// SVGetParameterName
// This function creates a BSTR with the name of the parameter
// It is the responsibility of the caller to free the BSTR.
HRESULT WINAPI SVGetParameterName( unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	return g_Contec.GetParameterName( p_ulIndex, p_pbstrName );
}

// SVGetParameterValue
// This function Gets the parameter value specified by p_ulIndex.
HRESULT WINAPI SVGetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	return g_Contec.GetParameterValue( p_ulIndex, p_pvarValue );
}

// SVSetParameterValue
// This function sets the value specified by p_ulIndex.
HRESULT WINAPI SVSetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	return g_Contec.SetParameterValue( p_ulIndex, p_pvarValue );
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVContec\SVContecExports.cpp_v  $
 * 
 *    Rev 1.0   30 Oct 2013 11:31:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in of Contec files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/