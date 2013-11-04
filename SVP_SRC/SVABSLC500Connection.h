// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVABSLC500Connection
// * .File Name       : $Workfile:   SVABSLC500Connection.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 15:43:52  $
// ******************************************************************************

#if !defined(AFX_SVABSLC500CONNECTION_H__C51D91D7_46A3_11D4_A922_00106F000B22__INCLUDED_)
#define AFX_SVABSLC500CONNECTION_H__C51D91D7_46A3_11D4_A922_00106F000B22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "SetupDialogSheet.h"

/////////////////////////////////////////////////////////////////////////////
// SVABSLC500ConnectionClass dialog

class SVABSLC500ConnectionClass : public CPropertyPage
{
// Construction
public:
	SVABSLC500ConnectionClass();   // standard constructor

// Dialog Data
	//{{AFX_DATA(SVABSLC500ConnectionClass)
	enum { IDD = IDD_ABSLC500Connection };
	CEdit	svmPLCTypeEdit;
	CEdit	svmManufacturerEdit;
	int		svmConnectionRadioIndex;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVABSLC500ConnectionClass)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

   SetupDialogSheetClass*  svmpParentSheet;

protected:
	// Generated message map functions
	//{{AFX_MSG(SVABSLC500ConnectionClass)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVABSLC500CONNECTION_H__C51D91D7_46A3_11D4_A922_00106F000B22__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVABSLC500Connection.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:43:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:56:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   12 Sep 2000 10:57:14   Jim
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