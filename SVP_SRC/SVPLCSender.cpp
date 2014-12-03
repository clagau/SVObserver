//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCSender
//* .File Name       : $Workfile:   SVPLCSender.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Dec 2014 13:35:50  $
//******************************************************************************

#include "stdafx.h"
#include "SVPLCSender.h"
#include "PLCBaseClass.h"
#include <vector>
#include <boost/bind.hpp>
#include "SVStatusLibrary/SVException.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"

int MyConsoleOutput(char *fmt, ...);

// Constructors
SVPLCSender::SVPLCSender(void)
:m_QueueDepth(0)
,m_MaxQueueDepth(10)
,m_pPLC(NULL)
{
}

SVPLCSender::SVPLCSender(CString p_strName)
:m_QueueDepth(0)
,m_MaxQueueDepth(10)
,m_pPLC(NULL)
,m_strName(p_strName)
{
}

SVPLCSender::SVPLCSender( const SVPLCSender& p_rPLCSender )
{
	if( &p_rPLCSender != this )
		*this = p_rPLCSender;
}

SVPLCSender& SVPLCSender::operator=( const SVPLCSender& p_rPLCSender )
{
	if( &p_rPLCSender != this )
	{
		m_MaxQueueDepth = p_rPLCSender.m_MaxQueueDepth;
		m_pPLC = p_rPLCSender.m_pPLC;
		m_QueueDepth = p_rPLCSender.m_QueueDepth;
		m_strName = p_rPLCSender.m_strName;
		// We probably should not copy the thread it should not be running.
		m_thread = p_rPLCSender.m_thread;
		assert( !m_thread.IsActive() );
	}
	return *this;
}

SVPLCSender::~SVPLCSender(void)
{
	StopThread();
}

HRESULT SVPLCSender::StartThread( CString p_strPLCID )
{
	CString strTmp = _T("PLC Send Thread ") + p_strPLCID;
	return m_thread.Create( &SVPLCSender::OnAPCEvent, boost::bind(&SVPLCSender::OnThreadEvent, this, _1), strTmp, SVNone);
}

void SVPLCSender::StopThread()
{

	if( m_thread.IsActive() )
	{
		MyConsoleOutput( _T("**** About to Destroy PLCSender Thread\n"));
		m_thread.Destroy();
		MyConsoleOutput( _T("**** Sender Thread Destroyed\n"));
	}
	
}

// The data is a std::pair of this, and data.
// The data is a single dimensioned array of variants
// The first VARIANT element is PLC Block type - VT_I4
// The second element is a multi dimensioned array of variants VT_ARRAY | VT_VARIANT
// - The first element is an array of addresses. VT_ARRAY | VT_VARIANT
// - The second element is an array of values. VT_ARRAY | VT_VARIANT
void SVPLCSender::OnAPCEvent(DWORD data)
{
	HRESULT l_hr = SVMSG_PLC_APC_DATA_BAD;

	if( data != 0 )
	{
		SVPLCSenderData* l_pvPLCDataPair = (SVPLCSenderData* )data;
		SVPLCSender* l_pSender = l_pvPLCDataPair->first;
		VARIANT l_vData = l_pvPLCDataPair->second;

		// Check variant data for type of block 
		if( l_vData.vt == (VT_ARRAY | VT_VARIANT) )
		{
			// The first element of the variant array is Type.
			// The second element of the variant array is a second array of variants which are addresses and values.
			VARIANT l_vType;
			::VariantInit( &l_vType );
			long rgInd[1];
			rgInd[0] = 0;
			l_hr = ::SafeArrayGetElement( l_vData.parray, rgInd, &l_vType ); 
			if( l_hr == S_OK )
			{
				long l_lType = l_vType.lVal;
				VARIANT l_vBlockData;
				::VariantInit( &l_vBlockData );
				rgInd[0] = 1;
				// Get the second element of the variant array.
				l_hr = ::SafeArrayGetElement( l_vData.parray, rgInd, &l_vBlockData );
				if( l_hr == S_OK )
				{
					switch( l_lType )
					{
						case SV_PLC_INT16:
						case SV_PLC_INT32:
						{
							l_hr = l_pSender->SendIntBlock( l_vBlockData ) ;
							break;
						}

						case SV_PLC_STRING:
						{
							l_hr = l_pSender->SendStringBlock( l_vBlockData );
							break;
						}

						default:
						{
							VariantClear( &l_vBlockData );
							l_hr = -4102;
							break;
						}
					}
				}
			}
		}
		// Decrement the Depth pointer.
		::InterlockedDecrement( &l_pSender->m_QueueDepth );

		::KeepPrevError( l_hr, ::VariantClear( &l_vData ));	// This should clean up all variant data..
		delete l_pvPLCDataPair;
	}

	if( l_hr != S_OK )
	{
//		ASSERT(0);
//		SVException svE;
//		SETEXCEPTION0( svE, l_hr );
//		svE.LogException();
	}
}

void SVPLCSender::OnThreadEvent( bool& p_WaitForEvents )
{
}

// Input : VARIANT Array Variants. 
// First variant is Type.
// Second variant is a 2d variant array of Addresses and values.
HRESULT SVPLCSender::SendBlock( VARIANT p_vData )
{
	HRESULT l_hr = S_OK;
	// Check queue depth. If too large then free variant data and leave.
	if( m_QueueDepth < m_MaxQueueDepth )
	{
		SVPLCSenderData* l_pPair = new SVPLCSenderData( this, p_vData);
		::InterlockedIncrement( &m_QueueDepth );
		l_hr = m_thread.Signal( (void*) l_pPair );
	}
	else
	{
		::VariantClear( &p_vData );
		OutputDebugString("Que is too Full!\n");
		l_hr = -4103 ;	// Some Error code representing a Full Queue...
	}
	return l_hr;
}

HRESULT SVPLCSender::SetPLC( SVPLCBaseClass* p_pPLC )
{
	m_pPLC = p_pPLC;
	return S_OK;
}

SVPLCBaseClass* SVPLCSender::GetPLC()
{
	return m_pPLC;
}

HRESULT SVPLCSender::SendIntBlock ( VARIANT p_vData )
{
	HRESULT l_hr = -4104;
	if( p_vData.vt == (VT_ARRAY | VT_VARIANT) )
	{
		// Build value array of TCHAR that will hold the strings.
		// build SVPLCBlockStruct
		SAFEARRAY* l_psaData = p_vData.parray;
		long l_lUBound, l_lLBound;
		l_hr = ::SafeArrayGetUBound( l_psaData, 1, &l_lUBound );
		l_hr = ::SafeArrayGetLBound( l_psaData, 1, &l_lLBound );
		
		if( l_hr == S_OK && l_psaData->cDims == 2 )
		{
			long l_rgInd[2];
			short l_awValues[128];		// INT16 the only one that is treated normal..
			long* l_plValues = reinterpret_cast<long*>(l_awValues);
			float* l_pfltValues = reinterpret_cast<float*>(l_awValues);
			double* l_pdValues = reinterpret_cast<double*>(l_awValues);
			LPSTR l_paAddresses[128];
			std::vector<_bstr_t> l_Addresses;
			long l_lSize = l_lUBound - l_lLBound + 1;
			for( long i = 0 ; i < l_lSize && i < 128 ; i++ )
			{
				// PLC Element Address..
				_bstr_t l_TmpAddress;
				VARIANT l_vAddress;
				::VariantInit( &l_vAddress );
				l_rgInd[0] = i;
				l_rgInd[1] = 0;
				l_hr = ::SafeArrayGetElement( l_psaData, l_rgInd, &l_vAddress );
				if( l_hr != S_OK )
				{
					break;
				}

				if( l_vAddress.vt != VT_BSTR ) // Check our addresses type
				{
					ASSERT(0);
					l_hr = -4105;
					break;
				}
				l_TmpAddress.Attach( l_vAddress.bstrVal );	// Transfer ownership.
				l_vAddress.bstrVal = NULL;

				l_Addresses.push_back( l_TmpAddress );
				l_paAddresses[i] = l_Addresses[i];

				::VariantClear( &l_vAddress );

				// PLC Element Value..
				VARIANT l_vValue;
				::VariantInit( &l_vValue );
				l_rgInd[1] = 1;
				l_hr = ::SafeArrayGetElement( l_psaData, l_rgInd, &l_vValue );
				if( l_hr != S_OK )
				{
					break;
				}
				switch( l_vValue.vt )
				{
					case VT_I2:
					{
						l_awValues[i] = l_vValue.iVal;
						break;
					}
					case VT_I4:
					{
						l_plValues[i] = l_vValue.lVal;
						break;
					}
					case VT_R4:
					{
						l_pfltValues[i] = l_vValue.fltVal;
						break;
					}
					case VT_R8:
					{
						l_pdValues[i] = l_vValue.dblVal;
						break;
					}
					default:
					{
						l_awValues[i] = 0;
						break;
					}

				}
			}
			if( l_hr == S_OK )
			{
				l_hr = m_pPLC->SendIntBlock( (LPSTR*)l_paAddresses, l_plValues, l_lSize );
			}
			::VariantClear( &p_vData );
		}
	}
	else
	{
		l_hr = -4105;
	}
	return l_hr;

}

HRESULT SVPLCSender::SendStringBlock ( VARIANT p_vData )
{
	HRESULT l_hr = S_OK;
	if( p_vData.vt == (VT_ARRAY | VT_VARIANT) )
	{
		// Build value array of char* that will hold the strings.
		SAFEARRAY* l_psaData = p_vData.parray;
		long l_lUBound, l_lLBound;
		l_hr = ::SafeArrayGetUBound( l_psaData, 1, &l_lUBound );
		::KeepPrevError( l_hr, ::SafeArrayGetLBound( l_psaData, 1, &l_lLBound ));

		if( l_hr == S_OK && l_psaData->cDims == 2 )
		{
			std::vector<_bstr_t> l_bstAddresses;
			std::vector<_bstr_t> l_bstValues;

			char* l_apAddressStrings[128];
			char* l_apValueStrings[128];

			long l_StringSize = m_pPLC->DataTypeSize(SV_PLC_STRING);
			long l_lElements = l_lUBound - l_lLBound + 1;

			ASSERT( l_lElements < 128 );
			if( l_lElements > 128 )
			{
				l_lElements = 128;
			}

			for( long i = 0 ; i < l_lElements ; i++ ) // Number of Elements
			{
				long l_rgInd[2];

				l_rgInd[0] = i;
				l_rgInd[1] = 0;

				// String address
				_bstr_t l_bstAddress;
				VARIANT l_vAddress;
				::VariantInit( &l_vAddress );
				l_hr = ::SafeArrayGetElement( l_psaData, l_rgInd, &l_vAddress );
				if( l_hr != S_OK )
					break;
				if( l_vAddress.vt != VT_BSTR )
				{
					l_hr = -4105;
					break;
				}
				l_bstAddress.Attach( l_vAddress.bstrVal );	// Transfer ownership.
				l_vAddress.bstrVal = NULL;

				l_bstAddresses.push_back( l_bstAddress );
				l_apAddressStrings[i] = static_cast<char*>( l_bstAddresses[i] );
				l_vAddress.vt = VT_EMPTY;

				// String Value 
				l_rgInd[1] = 1;
				_bstr_t l_bstrValue;
				VARIANT l_vValue;
				::VariantInit( &l_vValue );
				l_hr = ::SafeArrayGetElement( l_psaData, l_rgInd, &l_vValue );
				if( l_hr != S_OK )
					break;
				if( l_vValue.vt != VT_BSTR )
				{
					l_hr = -4105;
					break;
				}
				l_bstrValue.Attach( l_vValue.bstrVal );
				l_vValue.bstrVal = NULL;

				l_bstValues.push_back( l_bstrValue );
				l_apValueStrings[i] = static_cast<char*>( l_bstValues[i] );
				l_vValue.vt = VT_EMPTY;
			}

			if( l_hr == S_OK )
			{
				l_hr = m_pPLC->SendStringBlock( l_apAddressStrings, l_apValueStrings, l_lElements );
			}
			::VariantClear( &p_vData );
		}
		else
		{
			l_hr = -4105;
		}

	}		
	else
	{
		l_hr = -4105;	// Wrong Variant type.
	}

	return l_hr;
}

HRESULT SVPLCSender::TestCommunications ( )
{

	return m_pPLC->TestCommunications();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVP_SRC\SVPLCSender.cpp_v  $
 * 
 *    Rev 1.1   01 Dec 2014 13:35:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added thread attributes and lables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 16:04:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/