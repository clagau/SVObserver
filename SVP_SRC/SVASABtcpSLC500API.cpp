//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVASABtcpSLC500API
//* .File Name       : $Workfile:   SVASABtcpSLC500API.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 15:47:26  $
//******************************************************************************

#include "stdafx.h"
#include "SVASABtcpSLC500API.h"

SVASABtcpSLC500API::SVASABtcpSLC500API(long* aplErr )
: SVASABtcpClass( aplErr )
{
	// TODO: Add your specialized code here.
   m_ConnectionData.svmPLCModel =           SV_PLCMODEL_SLC500;
   m_ConnectionData.svmPLCSubModel =        SV_PLCSUBMODEL_DONTCARE;
   m_ConnectionData.svmConnectionMethod =   SV_PLCCONNECTIONMETHOD_ASABTCP;

   Initialize();
   strcpy (svmczNonUCPortId, "AB:NAME");
}

SVASABtcpSLC500API::~SVASABtcpSLC500API(void)
{
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVASABtcpSLC500API.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:47:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/