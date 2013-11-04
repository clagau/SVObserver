//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCExports
//* .File Name       : $Workfile:   SVPLCExports.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 16:02:40  $
//******************************************************************************

#include "stdafx.h"
#include "SVPLCInterfaceClass.h"
#include "SVPLCExports.h"
#include "vector"

HRESULT WINAPI SVSetupConnection( VARIANT* p_pvConnection, long p_lSize )
{
	HRESULT l_hr = -3123;
	_bstr_t l_bstConnection;
	if( p_pvConnection->vt == VT_BSTR )
	{
		l_bstConnection.Attach( p_pvConnection->bstrVal );

		char* l_pTmpBuffer = new char[p_lSize];
		if( l_pTmpBuffer != NULL )
		{
			strcpy_s(l_pTmpBuffer, p_lSize, l_bstConnection );
			l_hr = ThePLCInterface.SetupConnection( l_pTmpBuffer, p_lSize );
			if( l_hr == S_OK )
			{
				_bstr_t l_bstResponse = l_pTmpBuffer;
				p_pvConnection->bstrVal = l_bstResponse.Detach();
			}
			else
			{
				p_pvConnection->bstrVal = NULL;
			}
		}
		delete[] l_pTmpBuffer;
	}
	return l_hr;
}

HRESULT WINAPI SVConstruct( VARIANT p_vPLC, VARIANT p_pvConnection )
{
	HRESULT l_hr = -4109;

	_bstr_t l_bstConnection;
	_bstr_t l_bstPLCID;
	if( p_pvConnection.vt == VT_BSTR && p_vPLC.vt == VT_BSTR )
	{
		l_bstPLCID.Attach( p_vPLC.bstrVal );
		l_bstConnection.Attach( p_pvConnection.bstrVal );
		l_hr = ThePLCInterface.Construct( l_bstPLCID, l_bstConnection );
	}
	return l_hr;
}

HRESULT WINAPI SVDestroy( VARIANT p_vPLC )
{
	HRESULT l_hr = -4110;	// Wrong Variant type.
	if( p_vPLC.vt == VT_BSTR )
	{
		_bstr_t l_bstPLCID;
		l_bstPLCID.Attach( p_vPLC.bstrVal );
		ThePLCInterface.Destroy( l_bstPLCID );
		l_hr = S_OK;
	}
	return l_hr;
}

// The variant p_pvData contains a two dimensional Safearray of Address / data pairs.
HRESULT WINAPI SVSendBlock( VARIANT p_vPLC, VARIANT p_vData )
{
	HRESULT l_hr = -4111;	// Wrong Variant type.
	if( p_vPLC.vt == VT_BSTR )
	{
		_bstr_t l_bstPLCID;
		l_bstPLCID.Attach( p_vPLC.bstrVal );
		l_hr = ThePLCInterface.SendBlock( l_bstPLCID, p_vData );
	}
	return l_hr;
}

HRESULT WINAPI SVSetParameterValue( VARIANT p_vPLC, unsigned long p_ulIndex, VARIANT p_varValue )
{
	HRESULT l_hr = -4112;	// Wrong Variant type.
	if( p_vPLC.vt == VT_BSTR )
	{
		_bstr_t l_bstPLCID;
		l_bstPLCID.Attach( p_vPLC.bstrVal );
		l_hr = ThePLCInterface.SetParameterValue( l_bstPLCID, p_ulIndex, p_varValue );
	}
	return l_hr;
}

HRESULT WINAPI SVGetParameterValue( VARIANT p_vPLC, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hr = -4113;	// Wrong Variant type.
	if( p_vPLC.vt == VT_BSTR )
	{
		_bstr_t l_bstPLCID;
		l_bstPLCID.Attach( p_vPLC.bstrVal );
		l_hr = ThePLCInterface.GetParameterValue( l_bstPLCID, p_ulIndex, p_pvarValue );
	}
	return l_hr;
}

HRESULT WINAPI SVGetParameterCount( unsigned long *p_pulCount )
{
	*p_pulCount = 4;
	return S_OK;
}

HRESULT WINAPI SVGetParameterName( unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	HRESULT l_hr = -4114;
	switch ( p_ulIndex )
	{
		case 0:
		{
			*p_pbstrName = ::SysAllocString( L"Send Queue Depth" );
			l_hr = S_OK;
			break;
		}
		case 1:
		{
			*p_pbstrName = ::SysAllocString( L"Heart Beat Address" );
			l_hr = S_OK;
			break;
		}
		case 2:
		{
			*p_pbstrName = ::SysAllocString( L"Heart Beat Time" );
			l_hr = S_OK;
			break;
		}
		case 3:
		{
			*p_pbstrName = ::SysAllocString( L"Heart Beat Data" );
			l_hr = S_OK;
			break;
		}
		default:
		{
			break;
		}
	}
	return l_hr;
}

HRESULT WINAPI SVGetPLCDataType( long* p_plSize, long* p_plType, VARIANT p_pvAddress )
{
	return ThePLCInterface.GetDataType( p_plSize, p_plType, p_pvAddress );
}

HRESULT WINAPI SVGetInfoFromAddress( long* p_plFileNum, long* p_plElement, long* p_plBitNum, VARIANT p_vAddress )
{
	return ThePLCInterface.GetInfoFromAddress( p_plFileNum, p_plElement, p_plBitNum, p_vAddress );
}

HRESULT WINAPI SVAddOffsetToAddress( VARIANT* p_pvAddress, long p_lOffset, long p_lBit )
{
	return ThePLCInterface.AddOffsetToAddress( p_pvAddress, p_lOffset, p_lBit);
}

HRESULT WINAPI SVSetHeartBeatCallback( SVGetStateCallback& p_callback )
{
	return ThePLCInterface.SetHeartBeatCallback( p_callback );
}

long WINAPI SVGetMaxDTSize( void )
{
	return ThePLCInterface.GetMaxDTSize( );
}

HRESULT WINAPI SVGetDefaultPLCAddress( VARIANT* p_pvAddress )
{
	return ThePLCInterface.GetDefaultPLCAddress( p_pvAddress );
}

HRESULT WINAPI SVTestCommunications(VARIANT p_vPLC)
{
	HRESULT l_hr = -4415;
	if( p_vPLC.vt == VT_BSTR )
	{
		_bstr_t l_bstPLCID;
		l_bstPLCID.Attach( p_vPLC.bstrVal );
		l_hr = ThePLCInterface.TestCommunications( l_bstPLCID );
	}
	return l_hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVPLCExports.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 16:02:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/