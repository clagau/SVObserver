// ******************************************************************************
// * COPYRIGHT (c) 2008 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVASABtcpParametersPage
// * .File Name       : $Workfile:   SVASABtcpParametersPage.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   :     $Date:   25 Apr 2013 15:46:36  $
// ******************************************************************************

#ifndef SVASABTCPPARAMETERSPAGE_H_INCLUDED
#define SVASABTCPPARAMETERSPAGE_H_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif 

#include "SetupDialogSheet.h"

/////////////////////////////////////////////////////////////////////////////
// SVASABtcpParametersPage dialog
//
// ASABtcp setup page allows setup of ip address and ENI or * EIP/CIP options.

class SVASABtcpParametersPage : public CPropertyPage
{
	DECLARE_DYNCREATE(SVASABtcpParametersPage)

// Construction
public:
	SVASABtcpParametersPage();
	~SVASABtcpParametersPage();

// Dialog Data
	//{{AFX_DATA(SVASABtcpParametersPage)
	enum { IDD = IDD_ABSLC500ASABTCP };
	CEdit	svmIPAddressEdit;
	CEdit	svmPLCTypeEdit;
	CEdit	svmManufacturerEdit;
	CEdit	svmConnectionEdit;
	DWORD   m_dwIPAddress;
//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVASABtcpParametersPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
   SetupDialogSheetClass*  svmpParentSheet;

protected:
	// Generated message map functions
	//{{AFX_MSG(SVASABtcpParametersPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	DWORD ExtractIPAddress( CString p_strIPAddress );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // #ifndef SVASABTCPPARAMETERSPAGE_H_INCLUDED

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVASABtcpParametersPage.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:46:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Sep 2012 11:02:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  783
 * SCR Title:  Update SVObserver to Validate PLC Sub-system Before Entering a Running Mode
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed IP edit to IP control.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:57:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/