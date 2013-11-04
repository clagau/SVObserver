//******************************************************************************
//* COPYRIGHT (c) 2008 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPLCManager
//* .File Name       : $Workfile:   SVPLCManager.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   23 Jul 2013 18:02:16  $
//******************************************************************************

#ifndef SVPLCMANAGER_H
#define SVPLCMANAGER_H

#include <map>
#include <vector>
#include "SVIOLibrary/SVIOPLCLoadLibraryClass.h"

class SVConfigurationObject;

class SVPLCManager
{
public:
	SVPLCManager();
	~SVPLCManager();
	HRESULT OpenPLC   ( CString p_strDLL );
	HRESULT ClosePLC  ( );
	HRESULT ConstructPLC ( CString p_strPLCName, CString p_strConnection );
	HRESULT DestroyPLC( CString p_strPLCName );
	HRESULT Startup( SVConfigurationObject* p_pConfig );
	HRESULT Shutdown();
	HRESULT SetupConnection( LPTSTR p_pszConnection, long p_lSize );
	HRESULT SendIntBlock( CString p_strPLCName, LPCTSTR* p_pszAddress, VARIANT* p_pvValue, long p_lSize );
	HRESULT SendStringBlock( CString p_strPLCName, LPCTSTR* p_pszAddresses, LPCTSTR* p_pszValues, long p_lSize );

	HRESULT GetDataType( long* p_plSize, long* p_plType, LPCTSTR p_pstrAddress );
	HRESULT GetInfoFromAddress( long* p_plFileNum, long* p_plElement, long* p_plBitNum, LPCTSTR p_pstrAddress );
	HRESULT AddOffsetToAddress( LPSTR p_pvAddress, long p_lOffset, long p_lBit);

	HRESULT SetParameterValue( CString p_strPLCName, long p_lIndex, VARIANT p_vValue );
	HRESULT GetParameterValue( CString p_strPLCName, long p_lIndex, VARIANT* p_pvValue );
	HRESULT GetParameterName( long p_lIndex, BSTR* p_pvValue );
	HRESULT GetParameterCount( unsigned long* p_plIndex );
	bool TestConnections( );
	bool IsOpen();
	HRESULT GetStateFunction( DWORD* p_pdwState );
	long GetMaxDTSize( void );
	HRESULT GetDefaultPLCAddress( CString& p_rstrAddress );
private:
	SVIOPLCLoadLibraryClass m_PLC;
	std::vector<CString> m_PLCIDs;	// This is used for cleaning up when we are finished.

};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPLCManager.h_v  $
 * 
 *    Rev 1.1   23 Jul 2013 18:02:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added include for std vector
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 13:25:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Sep 2012 11:43:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  783
 * SCR Title:  Update SVObserver to Validate PLC Sub-system Before Entering a Running Mode
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added test connection for PLC to prevent going online if the plc is configured but not present.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Dec 2009 12:25:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
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

