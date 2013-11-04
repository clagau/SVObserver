// ******************************************************************************
// * COPYRIGHT (c) 2001 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SetupDialogSheet
// * .File Name       : $Workfile:   SetupDialogSheet.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 15:35:38  $
// ******************************************************************************

#if !defined(AFX_SETUPDIALOGSHEET_H__27827DBB_45F8_11D4_A921_00106F000B22__INCLUDED_)
#define AFX_SETUPDIALOGSHEET_H__27827DBB_45F8_11D4_A921_00106F000B22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

/////////////////////////////////////////////////////////////////////////////
// SetupDialogSheetClass
#include "PLCBaseClass.h"

#pragma warning (disable:4275)

class SV_PLCAPI_DS SetupDialogSheetClass : public CPropertySheet
{
	DECLARE_DYNAMIC(SetupDialogSheetClass)

// Construction
public:
	SetupDialogSheetClass(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	SetupDialogSheetClass(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SetupDialogSheetClass)
protected:

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:

	virtual ~SetupDialogSheetClass();


   long     Init ();


   long     BuildSetupParameterString ();

   static long BuildSetupParameterString ( const TCHAR* apManufacturer,
                                          const TCHAR* apPLCType,
                                          const TCHAR* apConnectionType,
                                          const TCHAR* apDriverName,
                                          const TCHAR* apIPAddress,
                                          const TCHAR* apDHNodeAddress,
                                          const TCHAR* apChannelNbr,
                                          const TCHAR* apEquipmentNbr,
                                          TCHAR* apParameterString,
                                          long   alParameterStringSize );


// Used to build list of RSLinxConnections (presented by RSLinx).

   CStringArray   svmConnections; 

   // Connection Parameters structure
   SVPLCConnectionParameters* m_pPars;

   CString        svmManufacturer;

   CString        svmPLCType;

   CString        svmConnection;

   CString        svmDriverName;

   CString        svmIPAddress;

   CString        svmDHNodeAddress;

   CString        svmConnectionParameters;

   CString        svmChannelNbr;

   CString        svmEquipmentNbr;


   CPropertyPage* svmpManufacturerPage;

   CPropertyPage* svmpABPLCTypePage;

   CPropertyPage* svmpABSLC500ConnectionPage;

   CPropertyPage* svmpABEthernetPage;

   CPropertyPage* svmpABKTXPage;

   CPropertyPage* svmpABSLC500DDEPage;

   CPropertyPage* svmpRSLinxSearchPage;

   CPropertyPage* svmpSiemensS7ApplicomPage;

   CPropertyPage* svmpASABtcpPage;

   CPropertyPage* svmpHeartBeatPage;

// If svmlLastButtonNext is TRUE, then that means that we got here by
// pressing the next button.  If svmlLastButtonNext is FALSE, it means
// we got here by pressing the back button.

   long           svmlLastButtonNext;

   CString		m_strHeartBeatAddress;
   long			m_lHeartBeatTime;


// These values are shadowed SVPLCBaseClass values.

   HINSTANCE   svmhRSLinxLib;

 	HINSTANCE   svmhApplicomLib;

	// Generated message map functions
protected:
	//{{AFX_MSG(SetupDialogSheetClass)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPDIALOGSHEET_H__27827DBB_45F8_11D4_A921_00106F000B22__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SetupDialogSheet.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:35:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:56:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Feb 2002 17:28:34   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  131, 141
 * SCR Title:  Add support for Allen-Bradley Micrologix PLC's via the Ethernet
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Changed member variables 
 * 
 * from CPropertyPage* svmpABSLC500EthernetPage;
 * to    CPropertyPage* svmpABEthernetPage;
 * and
 * from CPropertyPage* svmpABSLC500KTXPage;
 * to     CPropertyPage* svmpABKTXPage;
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/