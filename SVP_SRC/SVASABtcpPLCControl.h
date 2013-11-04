//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVASABtcpPLCControl
//* .File Name       : $Workfile:   SVASABtcpPLCControl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 15:47:10  $
//******************************************************************************

#ifndef SVASABtcpPLCControl_H_INCLUDED
#define SVASABtcpPLCControl_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#include"PLCBaseClass.h"

enum SVAsabTcpErrorCodesEnum
{
	ASABSuccess = 0,
	ASABTransactionTimedout = 0x100,	// Transaction not complete within time specified by TimeoutTrans property.
	ASABErorWritingQuery = 0x300, 		// Error writing query to remote host
	ASABConnectionTimeout = 0x400,		// Connection not made within time specified by TimeoutConnect property.
	ASABBadTagFormat = 0x700,			// Bad tag format specifier
	ASABConttrolTerminated = 0x1300,	// Control termination requested before transaction completed.
	ASABInvalidFunctionRequest = 0x1500,// Invalid function requested
	ASABErrorCreatingSocket = 0x1700,	// Error creating socket
	ASABErrorCreatingThread = 0x2000,	// Error creating communication thread
	ASABDemoHasExpired = 0x2500,		// Demo has expired
	ASABErrorRequestingSession = 0x3000,// Error registering session with remote host
	ASABInvalidResponseLength = 0x3001, // Invalid response length
	ASABUnsupoprtedDataType = 0x3010,	// Unsupported data type
};

class SVASABtcpPLCControl;

// ***** Declare an event handling class using the TEventHandler template. *****
typedef TEventHandler<SVASABtcpPLCControl, _DAsabtcp, _DAsabtcpEvents> ICompleteEventHandler;
typedef void (__stdcall *ASAB_IO_CALLBACK_PROC)(unsigned long,unsigned long);


class SVASABtcpPLCControl  
{
	// Public Functions
public:
	SVASABtcpPLCControl();
	virtual ~SVASABtcpPLCControl();

	HRESULT Create();
	HRESULT Destroy();
	bool IsValid();

	// This event firing object follows the prototype of IDispatch to work with 
	// TEventHandler template see TEventHandler.h
	HRESULT OnEventFiringObjectInvoke
	(
	  ICompleteEventHandler* pEventHandler,
	  DISPID dispidMember, 
	  REFIID riid,
	  LCID lcid, 
	  WORD wFlags, 
	  DISPPARAMS* pdispparams, 
	  VARIANT* pvarResult,
	  EXCEPINFO* pexcepinfo, 
	  UINT* puArgErr
	);

	long DataTypeSize (SVPLCDataTypeEnum aDataType);

	HRESULT WriteBlock(SVPLCBlockStruct* p_pBlock, long p_lTimeout, void* p_pCallback);
	HRESULT ReadBlock(SVPLCBlockStruct* p_pBlock, long p_lTimeout, void* p_pCallback);

	HRESULT SetupPLC(LPCTSTR p_szNodeAddress, long p_lTransactionTimeout);
	HRESULT TestCommunication();

	SVPLCBlockStruct* m_pCurrentBlock; 
	// members
private:
	// ***** Declare an instance of a Asabtcp smart pointer. *****
	_DAsabtcpPtr			m_spASABtcp;  

	ICompleteEventHandler* m_pCompleteEventHandler;

	bool m_bControlCreated;
	bool m_bReadOperation;

	ASAB_IO_CALLBACK_PROC m_pCallback;
	static WCHAR pwchLicenseKey[];
	LPCTSTR GetControlErrorString( TCHAR* buf, long l_lBufSize, short l_sError );
};

#endif // #ifndef SVASABtcpPLCControl_H_INCLUDED

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVASABtcpPLCControl.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:47:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/