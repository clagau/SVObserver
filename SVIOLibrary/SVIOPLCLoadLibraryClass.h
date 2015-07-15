//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOPLCLoadLibraryClass
//* .File Name       : $Workfile:   SVIOPLCLoadLibraryClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 12:46:26  $
//******************************************************************************

#ifndef SVIOPLCLOADLIBRARYCLASS_H
#define SVIOPLCLOADLIBRARYCLASS_H


#include "SVP_SRC\SVPLCInterfaceClass.h"


class SVIOPLCLoadLibraryClass
{
public:
	typedef HRESULT (WINAPI *SVSetupConnectionPtr)( VARIANT* p_pvConnection, long p_lSize );
	typedef HRESULT (WINAPI *SVConstructPtr)( VARIANT p_pvPLC, VARIANT p_pvConnection );
	typedef HRESULT (WINAPI *SVDestroyPtr)( VARIANT p_pvPLC );
	typedef HRESULT (WINAPI *SVSendBlockPtr)( VARIANT p_pvPLC, VARIANT p_pvData );
	typedef HRESULT (WINAPI *SVSetParameterValuePtr)( VARIANT, unsigned long , VARIANT );
	typedef HRESULT (WINAPI *SVGetParameterValuePtr)( VARIANT, unsigned long , VARIANT* );
	typedef HRESULT (WINAPI *SVGetParameterCountPtr)( unsigned long* );
	typedef HRESULT (WINAPI *SVGetParameterNamePtr)( unsigned long, BSTR* );
	typedef HRESULT (WINAPI *SVGetDataTypePtr)( long* p_plSize, long* p_plType, VARIANT p_vAddress );
	typedef HRESULT (WINAPI *SVGetInfoFromAddressPtr)( long* p_plFileNum, long* p_plElement, long* p_plBitNum, VARIANT p_vAddress );
	typedef HRESULT (WINAPI *SVAddOffsetToAddressPtr)( VARIANT* p_pvAddress, long p_lOffset, long p_lBit);
	typedef HRESULT (WINAPI *SVSetHeartBeatCallbackPtr)(SVGetStateCallback& );
	typedef long (WINAPI *SVGetMaxDTSizePtr)( void );
	typedef HRESULT (WINAPI *SVGetDefaultPLCAddressPtr)(VARIANT*p_pvAddress );
	typedef HRESULT (WINAPI *SVTestCommunicationsPtr)(VARIANT);

	SVIOPLCLoadLibraryClass(void);
	~SVIOPLCLoadLibraryClass(void);

	void Init( void );
	HRESULT Open( LPCTSTR p_szLibrary );
	HRESULT Close( );
	bool IsOpen();

	HRESULT SetupConnection( LPTSTR p_szConnection, long p_lSize );

	HRESULT Construct( LPCTSTR p_szPLCID, LPCTSTR p_szConnection);
	HRESULT Destroy  ( LPCTSTR p_szPLCID );

	HRESULT SendIntBlock( LPCTSTR p_szPLCID, LPCTSTR* p_pstrAddress, VARIANT* p_pvValue, long p_lSize );
	HRESULT SendStringBlock( LPCTSTR p_szPLCID, LPCTSTR* p_pstrAddresses, LPCTSTR* p_pvValues, long p_lSize );

	HRESULT GetDataType( long* p_plSize, long* p_plType, LPCTSTR p_pstrAddress );
	HRESULT GetInfoFromAddress( long* p_plFileNum, long* p_plElement, long* p_plBitNum, LPCTSTR p_pstrAddress );
	HRESULT AddOffsetToAddress( LPSTR p_pvAddress, long p_lOffset, long p_lBit);

	HRESULT SetParameterValue( VARIANT p_vPLC, unsigned long p_lParameter, VARIANT p_vValue );
	HRESULT GetParameterValue( VARIANT p_vPLC, unsigned long p_lParameter, VARIANT* p_vValue );
	HRESULT GetParameterCount( unsigned long* p_lCount );
	HRESULT GetParameterName( unsigned long p_lParameter, BSTR* p_pbstrName );
	HRESULT SetHeartBeatCallback( SVGetStateCallback& p_callback );
	long GetMaxDTSize( void );
	HRESULT GetDefaultPLCAddress( CString& p_rstrAddress );
	HRESULT TestCommunications(LPCTSTR p_szPLCID);

private:
	SVSetupConnectionPtr m_pSetupConnection;
	SVConstructPtr m_pConstruct;
	SVSetParameterValuePtr m_pSetParameterValue;
	SVGetParameterValuePtr m_pGetParameterValue;
	SVGetParameterCountPtr m_pGetParameterCount;
	SVGetParameterNamePtr m_pGetParameterName;
	SVSendBlockPtr m_pSendBlock;
	SVDestroyPtr m_pDestroy;
	SVGetDataTypePtr m_pGetDataType;
	SVGetInfoFromAddressPtr m_pGetInfoFromAddress;
	SVAddOffsetToAddressPtr m_pAddOffsetToAddress;
	SVSetHeartBeatCallbackPtr m_pSetHeartBeatCallback;
	SVGetMaxDTSizePtr m_pGetMaxDTAddress;
	SVGetDefaultPLCAddressPtr m_pGetDefaultPLCAddress;
	SVTestCommunicationsPtr m_pTestCommunications;
	HMODULE m_hModule;
};

#endif // SVIOPLCLOADLIBRARYCLASS_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVIOLibrary\SVIOPLCLoadLibraryClass.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 12:46:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/