//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
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

#include "resource.h"		// main symbols
#include "SVXMLLibrary/SVXMLCTreeCtrl.h"

class SVOFileConfigDlg;

class SVOFileConfigApp : public CWinApp
{
	COleTemplateServer m_server;

protected:
	DECLARE_SERIAL( SVOFileConfigApp )

public:
	SVOFileConfigDlg *pdlg;
	SvXml::SVXMLCTreeCtrl::SVBranchHandle m_Inspection;
	unsigned long ulVersion;
	virtual void LoadIODoc( LPCTSTR szFileName );
	virtual void LoadIPDoc( LPCTSTR szFileName );
	SVOFileConfigApp();
	virtual ~SVOFileConfigApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOFileConfigApp)
	public:
	virtual BOOL InitInstance();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(SVOFileConfigApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern SVOFileConfigApp theApp;

