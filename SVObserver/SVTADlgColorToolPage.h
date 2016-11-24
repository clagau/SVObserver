//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgColorToolPage
//* .File Name       : $Workfile:   SVTADlgColorToolPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   26 Jun 2014 18:21:16  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTaskObjectValueInterface.h"
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "SVOGui/PictureDisplay.h"
#pragma endregion Includes

class SVColorToolClass;
class SVToolClass;
class SVBoolValueObjectClass;

/////////////////////////////////////////////////////////////////////////////
// SVTADlgColorToolPage dialog

class SVTADlgColorToolPageClass : public CPropertyPage, public SVTaskObjectValueInterface
{
public:
	SVTADlgColorToolPageClass( const SVGUID& rInspectionID, const SVGUID& rTaskObjectID );
	virtual ~SVTADlgColorToolPageClass();

	HRESULT SetInspectionData();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVTADlgColorToolPageClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVTADlgColorToolPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnConvertToHsi();
	afx_msg void OnTrainColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void setImages();

protected:
	// Dialog Data
	//{{AFX_DATA(SVTADlgColorToolPageClass)
	enum { IDD = IDD_TA_COLOR_TOOL_DIALOG };
	SvOg::PictureDisplay	m_dialogImage;
	BOOL	m_convertToHSI;
	//}}AFX_DATA

	SVColorToolClass* m_pTool;
	SVBoolValueObjectClass* m_pConvertToHSI;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


