//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCManager
//* .File Name       : $Workfile:   SVPLCManager.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:25:42  $
//******************************************************************************

#include "stdafx.h"
#include <boost\config.hpp>
#include <boost\function.hpp>
#include <boost\bind.hpp>
#include <set>
#include "SVPLCManager.h"
#include "SVConfigurationObject.h"
#include "SVObserver.h"

extern BOOL GlobalRCGetState( DWORD* );

SVPLCManager::SVPLCManager()
{

}

SVPLCManager::~SVPLCManager()
{
}

// OpenPLC is the first function that needs to be called.  It is also
// the only function that will cause the map create a key.
HRESULT SVPLCManager::OpenPLC( CString p_strDLL )
{
	HRESULT l_hr = m_PLC.Open( p_strDLL );
	if( l_hr != S_OK )
	{
		SVException svE;
		SETEXCEPTION0( svE, l_hr );
		svE.LogException();
	}
	return l_hr;
}

bool SVPLCManager::TestConnections( )
{
	bool l_bRet = true;
	for( std::vector<CString>::iterator it = m_PLCIDs.begin() ; it != m_PLCIDs.end(); ++it)
	{
		l_bRet = l_bRet && m_PLC.TestCommunications( *it ) == S_OK;
	}
	return l_bRet;
}
bool SVPLCManager::IsOpen( )
{
	return m_PLC.IsOpen();
}

HRESULT SVPLCManager::ClosePLC( )
{
	HRESULT hr = S_OK;
	std::vector<CString>::iterator l_it;
	
	for(l_it = m_PLCIDs.begin() ; l_it != m_PLCIDs.end() ; ++l_it )
	{
		::KeepPrevError( hr, m_PLC.Destroy(*l_it));
	}
	::KeepPrevError( hr, m_PLC.Close());
	return hr;
}

HRESULT SVPLCManager::ConstructPLC( CString p_strPLCName, CString p_strConnection )
{
	HRESULT hr = S_OK;
	hr = m_PLC.Construct( p_strPLCName, p_strConnection );
	if( std::find( m_PLCIDs.begin(), m_PLCIDs.end(), p_strPLCName ) == m_PLCIDs.end() )
	{
		m_PLCIDs.push_back( p_strPLCName );
	}
	return hr;
}

HRESULT SVPLCManager::DestroyPLC( CString p_strPLCName )
{
	HRESULT hr = S_FALSE;
	hr = m_PLC.Destroy( p_strPLCName );
	return hr;
}

HRESULT SVPLCManager::GetStateFunction( DWORD* p_pdwState )
{
	HRESULT l_hr = -3140;
	if( GlobalRCGetState( p_pdwState ))
	{
		l_hr = S_OK;
	}
	return l_hr;
}

HRESULT SVPLCManager::Startup( SVConfigurationObject* p_pConfig )
{
	HRESULT l_hr = S_FALSE;


	std::vector<CString>::iterator l_it;
	if( p_pConfig != NULL && p_pConfig->GetPLCCount() > 0 )
	{
		// Get a set of plc names.
		m_PLCIDs.clear(); // Clear internal list of valid plc ids
		l_hr = p_pConfig->GetPLCs( m_PLCIDs );
		for( l_it = m_PLCIDs.begin(); l_it != m_PLCIDs.end() ; ++l_it )
		{
			CString l_strPLCName = (*l_it);

			HRESULT l_tmpHr = OpenPLC( TheSVObserverApp.GetPLCDLL() );
			if( l_tmpHr == S_OK )
			{
				l_tmpHr = ConstructPLC( l_strPLCName, p_pConfig->GetConnectString() );

				long l_lQueueSize = p_pConfig->GetQueueSize( l_strPLCName ) ;
				_variant_t l_vTmp( l_lQueueSize );
				SetParameterValue( l_strPLCName, 0, l_vTmp );	// Queue Size..

			}
			if( l_hr == S_OK )
			{
				l_hr = l_tmpHr;
			}
		}
		if( l_hr == S_OK )
		{
			// HeartBeat Callback.
			SVGetStateCallback l_StateCallbackFunc ( boost::bind( &SVPLCManager::GetStateFunction, this, _1));
			l_hr = m_PLC.SetHeartBeatCallback( l_StateCallbackFunc );

			VARIANT l_vValue;
			CString l_strValue;
			// Get the data from the storage and put it in the dll
			// HeartBeat Address
			l_hr = p_pConfig->GetHeartBeatAddress( l_strValue );
			if( l_hr == S_OK )
			{
				_bstr_t l_bstValue( l_strValue );
				l_vValue.bstrVal = l_bstValue.Detach();
				l_vValue.vt = VT_BSTR;
				l_hr = SetParameterValue( _T(""), 1, l_vValue );
			}
			// HeartBeat Time.
			long l_lTime;
			l_hr = p_pConfig->GetHeartBeatTime( l_lTime );
			if( l_hr == S_OK )
			{
				l_vValue.lVal = l_lTime;
				l_vValue.vt = VT_I4;
				l_hr = SetParameterValue( _T(""), 2, l_vValue );
			}

		}
	}
	return l_hr;
}


HRESULT SVPLCManager::Shutdown()
{
	HRESULT l_hr = S_OK;

	std::vector<CString>::iterator l_it;

	// Close individual plcs
	// Get a set of plc names.
	for( l_it = m_PLCIDs.begin(); l_it != m_PLCIDs.end() ; ++l_it )
	{
		CString& l_strPLCName = (*l_it);
		HRESULT l_tmpHr = DestroyPLC( l_strPLCName );
		if( l_hr == S_OK )
		{
			l_hr = l_tmpHr;
		}
	}
	m_PLCIDs.clear();

	// Then close the library.
	if( l_hr != S_OK )
	{
		ASSERT(0);
	}

	l_hr = ClosePLC( );
	return l_hr;
}

HRESULT SVPLCManager::SetupConnection( LPTSTR p_pszConnection, long p_lSize )
{
	HRESULT hr = S_FALSE;
	hr = m_PLC.SetupConnection( p_pszConnection, p_lSize );
	return hr;
}

HRESULT SVPLCManager::SendIntBlock( CString p_strPLCName, LPCTSTR* p_pszAddresses, VARIANT* p_pvValue, long p_lSize )
{
	HRESULT hr = S_FALSE;
	hr = m_PLC.SendIntBlock( p_strPLCName, p_pszAddresses, p_pvValue, p_lSize );
	return hr;
}

HRESULT SVPLCManager::SendStringBlock( CString p_strPLCName, LPCTSTR* p_pszAddresses, LPCTSTR* p_pszValues, long p_lSize )
{
	HRESULT hr = S_FALSE;
	hr = m_PLC.SendStringBlock( p_strPLCName, p_pszAddresses, p_pszValues, p_lSize );
	return hr;
}

HRESULT SVPLCManager::GetDataType( long* p_plSize, long* p_plType, LPCTSTR p_pstrAddress )
{
	return m_PLC.GetDataType( p_plSize, p_plType, p_pstrAddress );
}

HRESULT SVPLCManager::GetInfoFromAddress( long* p_plFileNum, long* p_plElement, long* p_plBitNum, LPCTSTR p_pstrAddress )
{
	return m_PLC.GetInfoFromAddress( p_plFileNum, p_plElement, p_plBitNum, p_pstrAddress );
}

HRESULT SVPLCManager::AddOffsetToAddress( LPSTR p_pvAddress, long p_lOffset, long p_lBit)
{
	return m_PLC.AddOffsetToAddress( p_pvAddress, p_lOffset, p_lBit );
}


HRESULT SVPLCManager::SetParameterValue( CString p_strPLCName, long p_lIndex, VARIANT p_vValue )
{
	HRESULT l_hr = -3126;
	_variant_t l_vTmp( p_strPLCName );
	l_hr = m_PLC.SetParameterValue( l_vTmp.Detach(), p_lIndex, p_vValue );
	return l_hr;
}

HRESULT SVPLCManager::GetParameterValue( CString p_strPLCName, long p_lIndex, VARIANT* p_pvValue )
{
	HRESULT l_hr;
	_variant_t l_vTmp( p_strPLCName );
	l_hr = m_PLC.GetParameterValue( l_vTmp.Detach() , p_lIndex, p_pvValue );
	return l_hr;
}

HRESULT SVPLCManager::GetParameterName( long p_lIndex, BSTR* p_pbstValue )
{
	HRESULT l_hr;
	l_hr = m_PLC.GetParameterName( p_lIndex, p_pbstValue );
	return l_hr;
}

HRESULT SVPLCManager::GetParameterCount( unsigned long* p_plIndex )
{
	HRESULT l_hr;
	l_hr = m_PLC.GetParameterCount( p_plIndex );
	return l_hr;
}
long SVPLCManager::GetMaxDTSize( void )
{
	return m_PLC.GetMaxDTSize();
}

HRESULT SVPLCManager::GetDefaultPLCAddress( CString& p_rstrAddress )
{
	return m_PLC.GetDefaultPLCAddress( p_rstrAddress );
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCManager.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:25:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   04 Oct 2012 10:49:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   07 Sep 2012 11:43:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  783
 * SCR Title:  Update SVObserver to Validate PLC Sub-system Before Entering a Running Mode
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added test connection for PLC to prevent going online if the plc is configured but not present.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   30 Jan 2012 11:47:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   30 Aug 2010 13:51:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  700
 * SCR Title:  Remove String Buffer Problems
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code to remove or fix uses of CString GetBuffer and ReleaseBuffer methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Dec 2008 13:08:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Better error handling in startup(...
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:09:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

