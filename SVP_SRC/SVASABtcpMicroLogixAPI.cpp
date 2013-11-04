//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVASABtcpMicroLogixAPI
//* .File Name       : $Workfile:   SVASABtcpMicroLogixAPI.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 15:45:48  $
//******************************************************************************

#include "stdafx.h"
#include "SVASABtcpMicroLogixAPI.h"

SVASABtcpMicroLogixAPI::SVASABtcpMicroLogixAPI(long* aplErr)
: SVASABtcpClass( aplErr )
{
	// TODO: Add your specialized code here.
   m_ConnectionData.svmPLCModel =           SV_PLCMODEL_MICROLOGIX;
   m_ConnectionData.svmPLCSubModel =        SV_PLCSUBMODEL_DONTCARE;
   m_ConnectionData.svmConnectionMethod =   SV_PLCCONNECTIONMETHOD_ASABTCP;
   m_ConnectionData.svmPLCManufacturer =	SV_PLCMANUFACTURER_ALLENBRADLEY;

   _tcscpy( m_ConnectionData.svmczManufacturer, SV_ALLENBRADLEY);

   Initialize();
   strcpy (svmczNonUCPortId, "AB:NAME");
}

SVASABtcpMicroLogixAPI::~SVASABtcpMicroLogixAPI(void)
{
   OutputDebugString( "Exiting ~SVASABtcpMicroLogixAPI\n" );
}

long SVASABtcpMicroLogixAPI::SetupPLC(LPCTSTR p_strNodeAddress, long p_lConnectTimeOut )
{
	CString l_strTmp = p_strNodeAddress;
	l_strTmp += _T(",,,ENI:");
	HRESULT l_hr = SVASABtcpClass::SetupPLC( l_strTmp, p_lConnectTimeOut );
	SetTimeOutLimit( p_lConnectTimeOut + 20 );
	return l_hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVASABtcpMicroLogixAPI.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:45:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/