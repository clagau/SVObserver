//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPixelAnalyzerSetupClass
//* .File Name       : $Workfile:   svpixelanalyzersetup.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:23:08  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "InspectionEngine/SVTaskObjectValueInterface.h"
#pragma endregion Includes

class SVPixelAnalyzerClass;
class SVToolClass;

/////////////////////////////////////////////////////////////////////////////
// SVPixelAnalyzerSetupClass dialog

class SVPixelAnalyzerSetupClass : public CDialog, public SVTaskObjectValueInterface
{
// Construction
public:
	SVPixelAnalyzerSetupClass(SVPixelAnalyzerClass * apAnalyzer,
                              CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVPixelAnalyzerSetupClass();
	HRESULT SetInspectionData();

// Dialog Data
	//{{AFX_DATA(SVPixelAnalyzerSetupClass)
	enum { IDD = IDD_PIXEL_ANALYZER_DIALOG };
	CSliderCtrl	m_GrayscaleSld;
	CButton	m_OtherRad;
	CButton	m_WhiteRad;
	CButton	m_BlackRad;
	CEdit	m_GrayScaleEdt;
	long	m_GrayscaleToCount;
	CString	m_HighGrayscaleValue;
	CString	m_LowGrayscaleValue;
	int		m_GrayscaleSliderValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVPixelAnalyzerSetupClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
    unsigned long           init (SVPixelAnalyzerClass* apAnalyzer, 
                                  CWnd*                 apParent);

/*- SetBlack () -------------------------------------------------------------------------*/
/*- Actions associated with the black radio button. This also forces the radio button    */
/*- state. ------------------------------------------------------------------------------*/
    unsigned long           SetBlack ();

/*- SetWhite () -------------------------------------------------------------------------*/
/*- Actions associated with the white radio button. This also forces the radio button    */
/*- state. ------------------------------------------------------------------------------*/
    unsigned long           SetWhite ();

/*- SetOther () -------------------------------------------------------------------------*/
/*- Actions associated with the other radio button. This also forces the radio button    */
/*- state. ------------------------------------------------------------------------------*/
    unsigned long           SetOther (long alGrayscale);

    unsigned long           SetGrayscale (long alGrayscale);

    SVToolClass             *m_pTool;
    SVPixelAnalyzerClass    *m_pAnalyzer;
    CWnd                    *m_pParent;

    unsigned long           msvulMaxGrayscale;
    unsigned long           msvulMinGrayscale;

protected:

	// Generated message map functions
	//{{AFX_MSG(SVPixelAnalyzerSetupClass)
	virtual void OnOK() override;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnWhiteRad();
	afx_msg void OnBlackRad();
	afx_msg void OnOtherRad();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeGrayscaleEdt();
	virtual void OnCancel() override;
	afx_msg void OnPixelSetRange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
