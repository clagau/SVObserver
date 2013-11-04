//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTwoPointRotateDialogClass
//* .File Name       : $Workfile:   SVTwoPointRotateDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:52:54  $
//******************************************************************************

#ifndef AFX_SVTWOPOINTROTATEDIALOG_H__079C6B61_23DA_11D2_AEC4_444553540000__INCLUDED_
#define AFX_SVTWOPOINTROTATEDIALOG_H__079C6B61_23DA_11D2_AEC4_444553540000__INCLUDED_

class SVResultListClass;
class SVToolClass;
class SVToolSetClass;
class SVAnalyzerClass;

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SVTwoPointRotateDialogClass 

class SVTwoPointRotateDialogClass : public CDialog
{
// Konstruktion
public:
	SVTwoPointRotateDialogClass( CWnd* pParent = NULL );   
	// Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(SVTwoPointRotateDialogClass)
	enum { IDD = IDD_TWO_POINT_ROTATE_DIALOG };
	CComboBox	SecondYCombo;
	CComboBox	SecondXCombo;
	CComboBox	FirstYCombo;
	CComboBox	FirstXCombo;
	//}}AFX_DATA

public:
	SVToolClass*					 PTool;
	SVToolSetClass*					 PToolSet;
	SVResultListClass*				 PResultList;
	SVAnalyzerClass*				 PAnalyzer;

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVTwoPointRotateDialogClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(SVTwoPointRotateDialogClass)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnLearnButton();
	afx_msg void OnSelChangeFirstXCombo();
	afx_msg void OnSelChangeFirstYCombo();
	afx_msg void OnSelChangeSecondXCombo();
	afx_msg void OnSelChangeSecondYCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_SVTWOPOINTROTATEDIALOG_H__079C6B61_23DA_11D2_AEC4_444553540000__INCLUDED_

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVTwoPointRotateDialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 15:52:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 17:17:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 */