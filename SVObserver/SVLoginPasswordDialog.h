//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLoginPasswordDialogClass
//* .File Name       : $Workfile:   SVLoginPasswordDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:18:08  $
//******************************************************************************

#ifndef AFX_SVLOGINPASSWORDDIALOG_H__7CA453A1_1B0B_11D2_AEC4_444553540000__INCLUDED_
#define AFX_SVLOGINPASSWORDDIALOG_H__7CA453A1_1B0B_11D2_AEC4_444553540000__INCLUDED_

class SVLoginPasswordDialogClass : public CDialog
{
// Konstruktion
public:
	SVLoginPasswordDialogClass(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(SVLoginPasswordDialogClass)
	enum { IDD = IDD_LOGIN_PASSWORD_DIALOG };
	CEdit	PasswordEdit;
	CEdit	LoginEdit;
	BOOL	BUseAutoLogin;
	//}}AFX_DATA

public:
	CString StrDefaultLogin;
	CString StrLogin;
	CString StrPassword;

// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVLoginPasswordDialogClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	BOOL bOldUseAutoLogin;

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(SVLoginPasswordDialogClass)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnAutoLoginCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};












/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SVNewLoginChangePasswordDialogClass 

class SVNewLoginChangePasswordDialogClass : public CDialog
{
// Konstruktion
public:
	SVNewLoginChangePasswordDialogClass(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(SVNewLoginChangePasswordDialogClass)
	enum { IDD = IDD_CHANGE_LOGIN_PASSWORD_DIALOG };
	CEdit	RepeatPasswordEdit;
	CEdit	PasswordEdit;
	CEdit	LoginEdit;
	//}}AFX_DATA

public:

	BOOL	BEnterNewLogin;
	CString StrLogin;
	CString StrPassword;
	CString StrRepeatPassword;


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(SVNewLoginChangePasswordDialogClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(SVNewLoginChangePasswordDialogClass)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio fügt zusätzliche Deklarationen unmittelbar vor der vorhergehenden Zeile ein.

#endif // AFX_SVLOGINPASSWORDDIALOG_H__7CA453A1_1B0B_11D2_AEC4_444553540000__INCLUDED_

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVLoginPasswordDialog.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:18:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   22 Apr 2003 10:57:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/