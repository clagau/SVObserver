// ******************************************************************************
// * COPYRIGHT (c) 2001 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVABPLCType
// * .File Name       : $Workfile:   SVABPLCType.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 15:38:56  $
// ******************************************************************************

#if !defined(AFX_SVABPLCTYPE_H__1838E00E_5A0F_11D5_A959_00106F000C7C__INCLUDED_)
#define AFX_SVABPLCTYPE_H__1838E00E_5A0F_11D5_A959_00106F000C7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "SetupDialogSheet.h"

/////////////////////////////////////////////////////////////////////////////
// SVABPLCTypeClass dialog

class SVABPLCTypeClass : public CPropertyPage
{
	DECLARE_DYNCREATE(SVABPLCTypeClass)

// Construction
public:
	SVABPLCTypeClass();
	~SVABPLCTypeClass();

// Dialog Data
	//{{AFX_DATA(SVABPLCTypeClass)
	enum { IDD = IDD_ABPLCType };

	CButton	svmSLC500Radio;

//	CButton	svmPLC5Radio;

	CButton	svmMicroLogixRadio;

	CEdit	svmManufacturerEdit;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVABPLCTypeClass)
	public:

	virtual BOOL OnSetActive();

	virtual LRESULT OnWizardNext();

//	virtual LRESULT OnWizardBack();
	protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

   SetupDialogSheetClass*  svmpParentSheet;
protected:
	// Generated message map functions
	//{{AFX_MSG(SVABPLCTypeClass)

	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVABPLCTYPE_H__1838E00E_5A0F_11D5_A959_00106F000C7C__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVABPLCType.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:38:56   bWalter
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
*/