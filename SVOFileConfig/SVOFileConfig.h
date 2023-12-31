//******************************************************************************
//* COPYRIGHT (c) 2008 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOFileConfigDlg
//* .File Name       : $Workfile:   SVOFileConfig.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 11:31:38  $
//******************************************************************************

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

class SVOFileConfigDlg;

class SVOFileConfigApp : public CWinApp
{
protected:
	DECLARE_SERIAL( SVOFileConfigApp )

public:
	SVOFileConfigDlg *pdlg;
	SVOFileConfigApp();
	virtual ~SVOFileConfigApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOFileConfigApp)
	public:
	virtual BOOL InitInstance() override;
	virtual void Serialize(CArchive& ar) override;
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(SVOFileConfigApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern SVOFileConfigApp theApp;

