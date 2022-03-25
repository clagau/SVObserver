//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMaskEditorPropPage
//* .File Name       : $Workfile:   SVMaskEditorPropPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Apr 2013 14:08:22  $
//******************************************************************************

#pragma once

////////////////////////////////////////////////////////////////////////////
// SVMaskEditorPropPage : See SVMaskEditorPropPage.cpp.cpp for implementation.

class SVMaskEditorPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(SVMaskEditorPropPage)
	DECLARE_OLECREATE_EX(SVMaskEditorPropPage)

// Constructor
public:
	SVMaskEditorPropPage();
	virtual ~SVMaskEditorPropPage();

// Dialog Data
	//{{AFX_DATA(SVMaskEditorPropPage)
	enum { IDD = IDD_PROPPAGE_SVMASKEDITOR };
	CStatic	image;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(SVMaskEditorPropPage)
	virtual BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

