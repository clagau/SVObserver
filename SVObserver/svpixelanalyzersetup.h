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

#if !defined(AFX_SVPIXELANALYZERSETUPCLASS_H__99AE51A1_5F9D_11D3_A79C_00106F000C76__INCLUDED_)
#define AFX_SVPIXELANALYZERSETUPCLASS_H__99AE51A1_5F9D_11D3_A79C_00106F000C76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SVPixelAnalyzerSetupClass.h : header file
//

#include "SVTaskObjectInterfaceClass.h"

class SVPixelAnalyzerClass;
class SVToolClass;

/////////////////////////////////////////////////////////////////////////////
// SVPixelAnalyzerSetupClass dialog

class SVPixelAnalyzerSetupClass : public CDialog, public SVTaskObjectInterfaceClass
{
// Construction
public:
	SVPixelAnalyzerSetupClass(SVPixelAnalyzerClass * apAnalyzer,
                              CWnd* pParent = NULL);   // standard constructor

	virtual HRESULT SetInspectionData();

// Dialog Data
	//{{AFX_DATA(SVPixelAnalyzerSetupClass)
	enum { IDD = IDD_PIXEL_ANALYZER_DIALOG };
	CSliderCtrl	msvGrayscaleSld;
	CButton	msvOtherRad;
	CButton	msvWhiteRad;
	CButton	msvBlackRad;
	CEdit	msvGrayScaleEdt;
	long	msvGrayscaleToCount;
	CString	msvHighGrayscaleValue;
	CString	msvLowGrayscaleValue;
	int		msvGrayscaleSliderValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVPixelAnalyzerSetupClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
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


    SVErrorClass            msvError;

    SVToolClass             *msvpTool;
    SVPixelAnalyzerClass    *msvpAnalyzer;
    CWnd                    *msvpParent;

    unsigned long           msvulMaxGrayscale;
    unsigned long           msvulMinGrayscale;


protected:

	// Generated message map functions
	//{{AFX_MSG(SVPixelAnalyzerSetupClass)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnWhiteRad();
	afx_msg void OnBlackRad();
	afx_msg void OnOtherRad();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeGrayscaleEdt();
	virtual void OnCancel();
	afx_msg void OnPixelSetRange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVPIXELANALYZERSETUPCLASS_H__99AE51A1_5F9D_11D3_A79C_00106F000C76__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\svpixelanalyzersetup.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:23:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   04 Sep 2012 15:56:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   09 Aug 2005 07:59:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   22 Apr 2003 13:52:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
