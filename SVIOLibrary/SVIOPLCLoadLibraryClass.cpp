//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOPLCLoadLibraryClass
//* .File Name       : $Workfile:   SVIOPLCLoadLibraryClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 12:47:06  $
//******************************************************************************

#include "stdafx.h"
#include "SVIOPLCLoadLibraryClass.h"
#include "AtlBase.h"
#include "AtlConv.h"
#include <comutil.h>

SVIOPLCLoadLibraryClass::SVIOPLCLoadLibraryClass(void)
{

	m_hModule = NULL;
	Init();
}

void SVIOPLCLoadLibraryClass::Init(void)
{
	m_pSetupConnection = NULL;
	m_pConstruct = NULL;
	m_pDestroy = NULL;
	m_pSetParameterValue = NULL;
	m_pGetParameterValue = NULL;
	m_pGetParameterCount = NULL;
	m_pGetParameterName = NULL;
	m_pSendBlock = NULL;
	m_pGetDataType = NULL;
	m_pGetInfoFromAddress = NULL;
	m_pAddOffsetToAddress = NULL;
	m_pSetHeartBeatCallback = NULL;
	m_pGetMaxDTAddress = NULL;
	m_pGetDefaultPLCAddress = NULL;
}

SVIOPLCLoadLibraryClass::~SVIOPLCLoadLibraryClass(void)
{
   OutputDebugString( _T("Exiting ~SVIOPLCLoadLibraryClass\n" ));
}

HRESULT SVIOPLCLoadLibraryClass::Open( LPCTSTR p_szLibrary )
{
	HRESULT l_hr = S_OK;

	if( m_hModule == NULL )
	{
		m_hModule = ::LoadLibrary( p_szLibrary );
		if( m_hModule != NULL )
		{
			
			m_pSetupConnection = reinterpret_cast<SVSetupConnectionPtr>(GetProcAddress(m_hModule, _T("SVSetupConnection")));
			m_pConstruct = reinterpret_cast<SVConstructPtr>(GetProcAddress(m_hModule, _T("SVConstruct")));
			m_pSetParameterValue = reinterpret_cast<SVSetParameterValuePtr>(GetProcAddress(m_hModule, _T("SVSetParameterValue")));
			m_pGetParameterValue = reinterpret_cast<SVGetParameterValuePtr>(GetProcAddress(m_hModule, _T("SVGetParameterValue")));
			m_pGetParameterCount = reinterpret_cast<SVGetParameterCountPtr>(GetProcAddress(m_hModule, _T("SVGetParameterCount")));
			m_pGetParameterName = reinterpret_cast<SVGetParameterNamePtr>(GetProcAddress(m_hModule, _T("SVGetParameterName")));
			m_pSendBlock = reinterpret_cast<SVSendBlockPtr>(GetProcAddress(m_hModule, _T("SVSendBlock" )));
			m_pDestroy = reinterpret_cast<SVDestroyPtr>(GetProcAddress(m_hModule, _T("SVDestroy" )));
			m_pGetDataType = reinterpret_cast<SVGetDataTypePtr>( GetProcAddress( m_hModule, _T("SVGetPLCDataType")));
			m_pGetInfoFromAddress = reinterpret_cast<SVGetInfoFromAddressPtr>( GetProcAddress( m_hModule, _T("SVGetInfoFromAddress")));
			m_pAddOffsetToAddress = reinterpret_cast<SVAddOffsetToAddressPtr>( GetProcAddress( m_hModule, _T("SVAddOffsetToAddress")));
			m_pSetHeartBeatCallback = reinterpret_cast<SVSetHeartBeatCallbackPtr>( GetProcAddress( m_hModule, _T("SVSetHeartBeatCallback")));
			m_pGetMaxDTAddress = reinterpret_cast<SVGetMaxDTSizePtr>( GetProcAddress( m_hModule, _T("SVGetMaxDTSize")));
			m_pGetDefaultPLCAddress = reinterpret_cast<SVGetDefaultPLCAddressPtr>( GetProcAddress( m_hModule, _T("SVGetDefaultPLCAddress")));
			m_pTestCommunications = reinterpret_cast<SVTestCommunicationsPtr>( GetProcAddress( m_hModule, _T("SVTestCommunications")));

			if( m_pSetupConnection != NULL &&
				m_pConstruct != NULL &&
				m_pSendBlock != NULL &&
				m_pSetParameterValue != NULL &&
				m_pGetParameterValue != NULL &&
				m_pGetParameterCount != NULL &&
				m_pDestroy != NULL &&
				m_pGetParameterName != NULL &&
				m_pGetDataType != NULL && 
				m_pGetInfoFromAddress != NULL && 
				m_pAddOffsetToAddress != NULL &&
				m_pSetHeartBeatCallback != NULL &&
				m_pGetMaxDTAddress != NULL &&
				m_pGetDefaultPLCAddress != NULL )
			{
				l_hr = S_OK;
			}
			else
			{
				l_hr = -4115;
			}
		}
		else
		{
			char *pszBuff;
			pszBuff = NULL;
			FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
						   NULL, GetLastError(), 0, (char *) &pszBuff, 0, NULL);
			AfxMessageBox( pszBuff );
			LocalFree (pszBuff);
		}
	}

	if ( l_hr != S_OK )
	{
		Close();
	}

	return l_hr;
}

bool SVIOPLCLoadLibraryClass::IsOpen( )
{
	return m_hModule != NULL;
}


HRESULT SVIOPLCLoadLibraryClass::Close()
{
	HRESULT l_hr = -4200;
	if( m_hModule )
	{
		Init();
		FreeLibrary( m_hModule );
		m_hModule = NULL;
		l_hr = S_OK;
	}
	return l_hr;
}

// opposite of construct( plcId, ... )
HRESULT SVIOPLCLoadLibraryClass::Destroy( LPCTSTR p_szPLCID  )
{
	HRESULT l_hr = -4201;
	if( m_pDestroy != NULL )
	{
		VARIANT l_vPLCID;
		_bstr_t l_bstPLCID( p_szPLCID );
		l_vPLCID.vt = VT_BSTR;
		l_vPLCID.bstrVal = l_bstPLCID.Detach();

		l_hr = m_pDestroy( l_vPLCID );
	}
	return l_hr;
}

HRESULT SVIOPLCLoadLibraryClass::SetupConnection( LPTSTR p_szConnection, long p_lSize )
{
	HRESULT l_hr = -4202;
	if( m_pSetupConnection != NULL )
	{
		VARIANT l_vConnection;
		_bstr_t l_bstConnection( p_szConnection );
		l_vConnection.vt = VT_BSTR;
		l_vConnection.bstrVal = l_bstConnection.Detach();

		l_hr = m_pSetupConnection( &l_vConnection, p_lSize);
		if( l_hr == S_OK )
		{
			l_bstConnection.Attach( l_vConnection.bstrVal);
			strcpy_s( p_szConnection, p_lSize, l_bstConnection );
		}
	}
	return l_hr;
}

HRESULT SVIOPLCLoadLibraryClass::Construct( LPCTSTR p_szPLCID, LPCTSTR p_szConnection)
{
	HRESULT l_hr = -4203;
	if( m_pConstruct != NULL )
	{
		VARIANT l_vConnection;
		VARIANT l_vPLCID;
		_bstr_t l_bstPLCID( p_szPLCID );
		_bstr_t l_bstConnection( p_szConnection );
		l_vConnection.vt = VT_BSTR;
		l_vConnection.bstrVal = l_bstConnection.Detach();
		l_vPLCID.vt = VT_BSTR;
		l_vPLCID.bstrVal = l_bstPLCID.Detach();
		l_hr = m_pConstruct( l_vPLCID, l_vConnection );
	}
	return l_hr;
}


HRESULT SVIOPLCLoadLibraryClass::SendIntBlock( LPCTSTR p_szPLCID, LPCTSTR* p_apstrAddresses, VARIANT* p_pvValue, long p_lSize )
{
	HRESULT l_hr = -4204;
	if( m_pSendBlock != NULL )
	{
		SAFEARRAY* psaData;
		SAFEARRAYBOUND l_rgsbound[2];
		l_rgsbound[0].cElements = p_lSize;
		l_rgsbound[0].lLbound = 0;
		l_rgsbound[1].cElements = 2;
		l_rgsbound[1].lLbound = 0;
		psaData = ::SafeArrayCreate( VT_VARIANT, 2, l_rgsbound );
		if( psaData )
		{
			for( int i = 0 ; i < p_lSize ; i++ )
			{
				// Address....
				long l_rgInd[2];
				VARIANT l_vAddress;
				::VariantInit( &l_vAddress );
				_bstr_t l_bstValue = p_apstrAddresses[i];
				l_rgInd[0] = i;
				l_rgInd[1] = 0;
				l_vAddress.vt = VT_BSTR;
				l_vAddress.bstrVal = l_bstValue.Detach();
				l_hr = ::SafeArrayPutElement( psaData, l_rgInd, &l_vAddress );
				::VariantClear( &l_vAddress );
				if( l_hr != S_OK )
				{
					break;
				}

				// Values.....
				l_rgInd[1] = 1;
				l_hr = ::SafeArrayPutElement( psaData, l_rgInd, &p_pvValue[i] );
				if( l_hr != S_OK )
				{
					break;
				}
			}
			if( l_hr == S_OK )
			{
				_bstr_t l_bstPLCID( p_szPLCID );
				VARIANT l_vPLCID;
				VARIANT l_vData;
				l_vData.vt = VT_ARRAY | VT_VARIANT;
				l_vData.parray = psaData;
				l_vPLCID.vt = VT_BSTR;
				l_vPLCID.bstrVal = l_bstPLCID.Detach();

				VARIANT l_vPLCData;	// Type & Data.
				SAFEARRAY* l_saPLCData = NULL;
				SAFEARRAYBOUND l_saBound;
				l_saBound.cElements = 2;
				l_saBound.lLbound = 0;
				l_saPLCData = ::SafeArrayCreate( VT_VARIANT, 1, &l_saBound );
				ASSERT( l_saPLCData );

				VARIANT l_vType;	// PLC Data Transfer Type not the actual data type.
				l_vType.vt = VT_I4;
				l_vType.lVal = SV_PLC_INT16;


				long l_lInd;
				l_lInd = 0;
				l_hr = ::SafeArrayPutElement( l_saPLCData, &l_lInd, &l_vType );
				ASSERT( l_hr == S_OK );
				l_lInd = 1;
				l_hr = ::SafeArrayPutElement( l_saPLCData,&l_lInd, &l_vData );
				ASSERT( l_hr == S_OK );

				l_vPLCData.vt = VT_ARRAY | VT_VARIANT;
				l_vPLCData.parray = l_saPLCData;


				l_hr = m_pSendBlock( l_vPLCID, l_vPLCData );
			}
		}
	}
	return l_hr;
}


HRESULT SVIOPLCLoadLibraryClass::SendStringBlock( LPCTSTR p_szPLCID, LPCTSTR* p_pAddresses, LPCTSTR* p_pStringValues, long p_lSize )
{
	HRESULT l_hr = -4205;
	if( m_pSendBlock != NULL )
	{
		// build a safearray of bstrs for addresses.
		// build a safearray of bstrs for values.
		SAFEARRAYBOUND rgsabound[2];
		rgsabound[0].cElements = p_lSize;
		rgsabound[0].lLbound = 0;
		rgsabound[1].cElements = 2;
		rgsabound[1].lLbound = 0;

		SAFEARRAY* psaData = SafeArrayCreate(VT_VARIANT, 2, rgsabound);
		if( psaData == NULL )
		{
			l_hr = -3125 ;	// Probably a serious condition like our of memory.
		}
		else
		{
			for( long i = 0 ; i < p_lSize ; i++ )
			{
				VARIANT l_vAddress;
				::VariantInit( &l_vAddress );
				l_vAddress.vt = VT_BSTR;
				long l_rgInd[2];
				l_rgInd[0] = i;
				l_rgInd[1]=0;
				_bstr_t l_bstTmp;
				l_bstTmp = p_pAddresses[i];
				l_vAddress.bstrVal = l_bstTmp.Detach();
				l_hr = ::SafeArrayPutElement( psaData, l_rgInd, &l_vAddress );
				::VariantClear( &l_vAddress );

				if( l_hr != S_OK )
					break;

				VARIANT l_vValue;
				::VariantInit( &l_vValue );
				l_vValue.vt = VT_BSTR;
				l_rgInd[1]=1;
				l_bstTmp = p_pStringValues[i];
				l_vValue.bstrVal = l_bstTmp.Detach();
				l_hr = ::SafeArrayPutElement( psaData, l_rgInd, &l_vValue );
				::VariantClear( &l_vValue );
				if( l_hr != S_OK )
					break;
			}

			if( l_hr == S_OK )
			{
				_bstr_t l_bstPLCID( p_szPLCID );
				VARIANT l_vPLCID;
				VARIANT l_vData;	// Addresses and Values
				l_vData.vt = VT_ARRAY | VT_VARIANT;
				l_vData.parray = psaData;
				l_vPLCID.vt = VT_BSTR;
				l_vPLCID.bstrVal = l_bstPLCID.Detach();

				VARIANT l_vPLCData;	// Type & Data.
				SAFEARRAY* l_saPLCData = NULL;
				SAFEARRAYBOUND l_saBound;
				l_saBound.cElements = 2;
				l_saBound.lLbound = 0;
				l_saPLCData = ::SafeArrayCreate( VT_VARIANT, 1, &l_saBound );
				ASSERT( l_saPLCData );

				VARIANT l_vType;	// PLC Data Transfer Type not the actual data type.
				l_vType.vt = VT_I4;
				l_vType.lVal = SV_PLC_STRING;


				long l_lInd;
				l_lInd = 0;
				l_hr = ::SafeArrayPutElement( l_saPLCData, &l_lInd, &l_vType );
				ASSERT( l_hr == S_OK );
				l_lInd = 1;
				l_hr = ::SafeArrayPutElement( l_saPLCData,&l_lInd, &l_vData );
				VariantClear( &l_vData );
				ASSERT( l_hr == S_OK );

				l_vPLCData.vt = VT_ARRAY | VT_VARIANT;
				l_vPLCData.parray = l_saPLCData;

				l_hr = m_pSendBlock( l_vPLCID, l_vPLCData );
			}
		}
	}
	return l_hr;
}


HRESULT SVIOPLCLoadLibraryClass::SetParameterValue( VARIANT p_vPLC, unsigned long p_lParameter, VARIANT p_vValue )
{
	HRESULT l_hr = -4206;
	if( m_pSetParameterValue != NULL )
	{
		l_hr = m_pSetParameterValue( p_vPLC, p_lParameter, p_vValue );
	}
	return l_hr;
}

HRESULT SVIOPLCLoadLibraryClass::GetParameterValue( VARIANT p_vPLC, unsigned long p_lParameter, VARIANT* p_vValue )
{
	HRESULT l_hr = -4207;
	if( m_pGetParameterValue != NULL )
	{
		l_hr = m_pGetParameterValue( p_vPLC, p_lParameter, p_vValue );
	}
	return l_hr;
}

HRESULT SVIOPLCLoadLibraryClass::GetParameterCount( unsigned long* p_lCount )
{
	HRESULT l_hr = -4208;
	if( m_pGetParameterCount != NULL )
	{
		l_hr = m_pGetParameterCount(p_lCount );
	}

	return l_hr;
}

HRESULT SVIOPLCLoadLibraryClass::GetParameterName( unsigned long p_lParameter, BSTR* p_pbstrName )
{
	HRESULT l_hr = -4209;
	if( m_pGetParameterName != NULL )
	{
		l_hr = m_pGetParameterName(p_lParameter, p_pbstrName );
	}
	return l_hr;
}

HRESULT SVIOPLCLoadLibraryClass::GetDataType( long* p_plSize, long* p_pType, LPCTSTR p_pstrAddress )
{
	HRESULT l_hr = -4210;
	if( m_pGetDataType != NULL )
	{
		VARIANT l_vAddress;
		_bstr_t l_bstAddress( p_pstrAddress );
		l_vAddress.vt = VT_BSTR;
		l_vAddress.bstrVal = l_bstAddress.Detach();
		l_hr = m_pGetDataType( p_plSize, p_pType, l_vAddress );
	}
	return l_hr;
}

HRESULT SVIOPLCLoadLibraryClass::GetInfoFromAddress( long* p_plFileNum, long* p_plElement, long* p_plBitNum, LPCTSTR p_pstrAddress )
{
	HRESULT l_hr = -4211;
	if( m_pGetInfoFromAddress != NULL )
	{
		VARIANT l_vAddress;
		_bstr_t l_bstAddress( p_pstrAddress );
		l_vAddress.vt = VT_BSTR;
		l_vAddress.bstrVal = l_bstAddress.Detach();
		l_hr = m_pGetInfoFromAddress( p_plFileNum, p_plElement, p_plBitNum, l_vAddress );
	}
	return l_hr;
}

HRESULT SVIOPLCLoadLibraryClass::AddOffsetToAddress( LPSTR p_pszAddress, long p_lOffset, long p_lBit)
{
	HRESULT l_hr = -4212;
	if( m_pAddOffsetToAddress != NULL )
	{
		VARIANT l_vAddress;
		_bstr_t l_bstAddress( p_pszAddress );
		l_vAddress.vt = VT_BSTR;
		l_vAddress.bstrVal = l_bstAddress.Detach();
		l_hr = m_pAddOffsetToAddress( &l_vAddress, p_lOffset, p_lBit );
		if( l_hr == S_OK  && l_vAddress.vt == VT_BSTR )
		{
			l_bstAddress.Attach( l_vAddress.bstrVal );
			strcpy_s( p_pszAddress, 128, static_cast<LPSTR>(l_bstAddress) );
			l_hr = S_OK;
		}
	}
	return l_hr;
}

HRESULT SVIOPLCLoadLibraryClass::SetHeartBeatCallback( SVGetStateCallback& p_callback )
{
	HRESULT l_hr = -4213;
	if( m_pSetHeartBeatCallback != NULL)
	{
		l_hr = m_pSetHeartBeatCallback(p_callback);
	}
	return l_hr;
}

long SVIOPLCLoadLibraryClass::GetMaxDTSize( void )
{
	if( m_pGetMaxDTAddress != NULL)
	{
		return m_pGetMaxDTAddress();
	}
	return 256;
}

HRESULT SVIOPLCLoadLibraryClass::GetDefaultPLCAddress( CString& p_rstrAddress )
{
	HRESULT l_hr = -4214;
	if( m_pGetDefaultPLCAddress != NULL)
	{
		VARIANT l_vAddress;
		::VariantInit( &l_vAddress );
		l_hr = m_pGetDefaultPLCAddress( &l_vAddress );
		if( l_hr == S_OK )
		{
			_bstr_t l_bstTmp;
			l_bstTmp.Attach( l_vAddress.bstrVal );
			p_rstrAddress = (LPCTSTR) l_bstTmp;
		}
	}
	else
	{
		p_rstrAddress = _T("Default");
	}
	return l_hr;
}

HRESULT SVIOPLCLoadLibraryClass::TestCommunications(LPCTSTR p_szPLCID)
{
	HRESULT l_hr = -4215;
	if( m_pTestCommunications )
	{
		_bstr_t l_bstPLCID( p_szPLCID );
		VARIANT l_vPLCID;
		l_vPLCID.vt = VT_BSTR;
		l_vPLCID.bstrVal = l_bstPLCID.Detach();
		l_hr = m_pTestCommunications(l_vPLCID);
	}
	return l_hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIOLibrary\SVIOPLCLoadLibraryClass.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 12:47:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/