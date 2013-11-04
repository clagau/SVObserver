// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVSiemensS7Applicom
// * .File Name       : $Workfile:   svsiemenss7applicom.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 16:06:34  $
// ******************************************************************************

#if !defined(AFX_SVSIEMENSS7APPLICOM_H__5220D6A1_57E8_11D4_A930_00106F0026BE__INCLUDED_)
#define AFX_SVSIEMENSS7APPLICOM_H__5220D6A1_57E8_11D4_A930_00106F0026BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SetupDialogSheet.h"

/////////////////////////////////////////////////////////////////////////////
// SVSiemensS7ApplicomClass dialog

class SVSiemensS7ApplicomClass : public CPropertyPage
{
	DECLARE_DYNCREATE(SVSiemensS7ApplicomClass)

// Construction
public:
	SVSiemensS7ApplicomClass();
	~SVSiemensS7ApplicomClass();

// Dialog Data
	//{{AFX_DATA(SVSiemensS7ApplicomClass)
	enum { IDD = IDD_S7300Profibus };
	CEdit	svmEquipmentNbrEdit;
	CEdit	svmChannelNbrEdit;
	CEdit	svmConnectionEdit;
	CEdit	svmPLCTypeEdit;
	CEdit	svmManufacturerEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVSiemensS7ApplicomClass)
	public:
	virtual BOOL OnWizardFinish();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
   SetupDialogSheetClass*  svmpParentSheet;

protected:
	// Generated message map functions
	//{{AFX_MSG(SVSiemensS7ApplicomClass)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVSIEMENSS7APPLICOM_H__5220D6A1_57E8_11D4_A930_00106F0026BE__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\svsiemenss7applicom.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 16:06:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:59:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   12 Sep 2000 10:57:12   Jim
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