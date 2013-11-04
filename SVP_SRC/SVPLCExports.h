//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCExports
//* .File Name       : $Workfile:   SVPLCExports.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 16:03:12  $
//******************************************************************************

#ifndef SVPLCEXPORTS_H
#define SVPLCEXPORTS_H

#include "SVCallbackdef.h"

HRESULT WINAPI SVSetupConnection( VARIANT* p_pvConnection, long p_lSize );

HRESULT WINAPI SVConstruct( VARIANT p_vPLC, VARIANT p_pvConnection );
HRESULT WINAPI SVDestroy( VARIANT p_vPLC );

HRESULT WINAPI SVSendBlock( VARIANT p_vPLC, VARIANT p_pvData );

HRESULT WINAPI SVGetParameterCount( unsigned long *p_pulCount );
HRESULT WINAPI SVGetParameterName(  unsigned long p_ulIndex, BSTR *p_pbstrName );
HRESULT WINAPI SVSetParameterValue( VARIANT p_vPLC, unsigned long p_ulIndex, VARIANT p_varValue );
HRESULT WINAPI SVGetParameterValue( VARIANT p_vPLC, unsigned long p_ulIndex, VARIANT *p_pvarValue );

// PLC specific functions that each plc needs to implement.
HRESULT WINAPI SVGetPLCDataType( long* p_plType, VARIANT p_vAddress );
HRESULT WINAPI SVGetInfoFromAddress( long* p_plFileNum, long* p_plElement, long* p_plBitNum, VARIANT p_vAddress );
HRESULT WINAPI SVAddOffsetToAddress(  VARIANT* p_pvAddress, long p_lOffset, long p_lBit );
HRESULT WINAPI SVSetHeartBeatCallback( SVGetStateCallback& p_callback );
long WINAPI SVGetMaxDTSize( void );
HRESULT WINAPI SVGetDefaultPLCAddress( VARIANT* p_pvAddress );
HRESULT WINAPI SVTestCommunications(VARIANT p_vPLC);

#endif    // SVPLCEXPORTS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVPLCExports.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 16:03:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/