//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTAReferenceAdjustmentDialogClass
//* .File Name       : $Workfile:   SVAdjustmentDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:27:06  $
//******************************************************************************

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#ifndef SVADJUSTMENTDIALOG_H
#define SVADJUSTMENTDIALOG_H

//******************************************************************************
//* INCLUDE(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// Includes which are necessary to declare or define types, constants and macros
////////////////////////////////////////////////////////////////////////////////

#include "SVView.h"

//******************************************************************************
//* CONSTANT(S):
//******************************************************************************

//******************************************************************************
//* MACRO(S):
//******************************************************************************

//******************************************************************************
//* TYPE(S):
//******************************************************************************

//******************************************************************************
//* FUNCTION DECLARATION(S):
//******************************************************************************

//******************************************************************************
//* CONSTANT VARIABLE(S):
//******************************************************************************

//******************************************************************************
//* VARIABLE(S):
//******************************************************************************

//******************************************************************************
//* CLASS(ES) & TYPE(S):
//* STRUCTURE(S) & TYPE(S):
//* UNION(S) & TYPE(S):
//******************************************************************************

class SVAnalyzerClass;
class SVRefExtentClass;
class SVToolAdjustmentDialogSheetClass;

////////////////////////////////////////////////////////////////////////////////
// .Title       : Dialog Class SVTAReferenceAdjustmentDialogClass 
// -----------------------------------------------------------------------------
// .Description : e.g. This dialog class ...
//              :
//              :
// -----------------------------------------------------------------------------
// .Export
//	 Public Method				Description
//  :
//  :
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :dd.mm.yyyy				First Implementation
//	:
////////////////////////////////////////////////////////////////////////////////
class SVTAReferenceAdjustmentDialogClass : public CDialog
{
//******************************************************************************
// Class Wizard Generated Message Map Entries:
//******************************************************************************
protected:

	//{{AFX_MSG(SVTAReferenceAdjustmentDialogClass)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnChangeContrastEdit();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

//******************************************************************************
// Friend Declaration(s):
//******************************************************************************

//******************************************************************************
// Constructor(s):
//******************************************************************************
public:
	SVTAReferenceAdjustmentDialogClass( CWnd* pParent = NULL, SVAnalyzerClass* A = NULL );   // Standardkonstruktor

//******************************************************************************
// Destructor(s):
//******************************************************************************

//******************************************************************************
// Operator(s):
//******************************************************************************

//******************************************************************************
// Operation(s) Of Writing Access And Data Exchange:
//******************************************************************************

//******************************************************************************
// Operation(s) Of Reading Access:
//******************************************************************************

//******************************************************************************
// Operation(s) Of Representation:
//******************************************************************************

//******************************************************************************
// Operation(s) Of Process:
//******************************************************************************

//******************************************************************************
// Class Wizard Generated Virtual Function(s):
//******************************************************************************
	//{{AFX_VIRTUAL(SVTAReferenceAdjustmentDialogClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

//******************************************************************************
// Class Wizard Generated Dialog Data:
//******************************************************************************
protected:

	//{{AFX_DATA(SVTAReferenceAdjustmentDialogClass)
	enum { IDD = IDD_RA_DETAIL_DIALOG };
	SVSliderCtrl	contrastSlider;
	BYTE	contrastEdit;
	//}}AFX_DATA

//******************************************************************************
// Data Element(s):
//******************************************************************************
public:
protected:
	SVToolAdjustmentDialogSheetClass*	parentParentDialog;
	SVAnalyzerClass*				currentAnalyzer;
	double							originalContrast;
	SVRefExtentClass*				rEx;
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////

//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif	//	SVADJUSTMENTDIALOG_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVAdjustmentDialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:27:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   17 Apr 2003 16:33:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/