// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRSLinxSearch
// * .File Name       : $Workfile:   svrslinxsearch.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 16:05:28  $
// ******************************************************************************

#if !defined(AFX_SVRSLINXSEARCH_H__C51D91D9_46A3_11D4_A922_00106F000B22__INCLUDED_)
#define AFX_SVRSLINXSEARCH_H__C51D91D9_46A3_11D4_A922_00106F000B22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// svrslinxsearch.h : header file
//
#include "SetupDialogSheet.h"
#include <dtl.h>

/////////////////////////////////////////////////////////////////////////////
// SVRSLinxSearchClass dialog

class SVRSLinxSearchClass : public CPropertyPage
{
	DECLARE_DYNCREATE(SVRSLinxSearchClass)

// Construction
public:
	SVRSLinxSearchClass();
	~SVRSLinxSearchClass();

// Dialog Data
	//{{AFX_DATA(SVRSLinxSearchClass)
	enum { IDD = IDD_RSLINX_SEARCH };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVRSLinxSearchClass)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
   static DWORD WINAPI BuildRSLinxConnectionList (LPVOID apCallingClass);

   SetupDialogSheetClass*  svmpParentSheet;

   DTLDRIVER               svmDtlDrivers [DTL_MAX_DRIVERS];

   long                    svmlNbrDrivers;

   DWORD                   svmdwBuildRSLinxConnectionListThreadId;
   HANDLE                  svmhBuildRSLinxConnectionListThread;

protected:
	// Generated message map functions
	//{{AFX_MSG(SVRSLinxSearchClass)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVRSLINXSEARCH_H__C51D91D9_46A3_11D4_A922_00106F000B22__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\svrslinxsearch.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 16:05:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:59:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   12 Sep 2000 10:57:20   Jim
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  14
 * SCR Title:  Stage 2: Expand PLC Support
 * Checked in by:  Jim;  James A. Brown
 * Change Description:  
 *   These files were added to support dialog configuration of PLC connection.  
 * 
 * Supported connection types are:
 * Siemens S7 300 and 400 series via profibus.
 * AllenBradley SLC505 via Ethernet.
 * AllenBradley SLC504 via Data Highway.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////

*/