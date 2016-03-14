//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDisplayImageSelect
//* .File Name       : $Workfile:   SVDisplayImageSelect.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:14:22  $
//******************************************************************************

#pragma once
#include "SVImageClass.h"

class SVIPDoc;

class SVDisplayImageSelect : public CDialog
{
public:
	SVImageClass *m_pCurrentImage;
	SVIPDoc			 *m_pDoc;
	SVDisplayImageSelect(CWnd* pParent = NULL);   // standard constructor

	//{{AFX_DATA(SVDisplayImageSelect)
	enum { IDD = IDD_SELECT_DISPLAY_IMAGE };
	CListBox	mImageSelectList;
	//}}AFX_DATA


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVDisplayImageSelect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:

	// Generated message map functions
	//{{AFX_MSG(SVDisplayImageSelect)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

