// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVPipesConnectionDlg
// * .File Name       : $Workfile:   SVPipesConnectionDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 14:03:24  $
// ******************************************************************************

#pragma once

#include "resource.h"
#include "SVLibrary/SVDialog.h"

class SVPipesConnectionDlg : public SVDialog  
{
public:
	CString mszConnectionKey;
	SVPipesConnectionDlg(CString &szConnection, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(SVPipesConnectionDlg)
	enum { IDD = IDD_PIPES_SETUP };
	CString	mszConnection;
	CString	mszPipe;
	CString	mszServer;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVPipesConnectionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVPipesConnectionDlg)
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString mszPipeKey;
	CString mszServerKey;
	CString GetRegistryKey (CString &szConnection);
};

