//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatModelPageClass
//* .File Name       : $Workfile:   SVPatSelectModelPageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:20:10  $
//******************************************************************************

#ifndef SVPATMODELPAGECLASS_H
#define SVPATMODELPAGECLASS_H

#include "SVLibrary/SVBitmap.h"
#include "SVImageLibrary/SVImageInfoClass.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVDlgImage.h"
#include "SVDlgImageGraph.h"

class SVToolClass;
class SVPatternAnalyzerClass;
class SVPatAnalyzeSetupDlgSheet;

class SVPatModelPageClass : public CPropertyPage
{
	DECLARE_DYNCREATE(SVPatModelPageClass)

// Construction
public:
	SVPatModelPageClass();
	virtual ~SVPatModelPageClass();

// Dialog Data
	//{{AFX_DATA(SVPatModelPageClass)
	enum { IDD = IDD_PAT_SETUP_MODEL };
	SVDlgImageGraphClass m_svToolImage;
	SVDlgImageGraphClass m_svModelImage;
	CSliderCtrl	m_YSlider;
	CSliderCtrl	m_XSlider;
	CEdit	m_ModelPositionXEditCtrl;
	CEdit	m_ModelPositionYEditCtrl;
	CEdit	m_ModelWidthEditCtrl;
	CEdit	m_ModelHeightEditCtrl;
	CButton m_CircularOverscanCheckbox;

	int		m_nXPos;
	int		m_nYPos;
	long	m_lModelWidth;
	long	m_lModelHeight;
	CString	m_strModelName;
	BOOL m_bCircularOverscan;
	//}}AFX_DATA

	int m_nMaxX;
	int m_nMaxY;
	
	SVPatternAnalyzerClass* m_pPatAnalyzer;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVPatModelPageClass)
	public:
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	SVPatAnalyzeSetupDlgSheet* m_pSheet;

	long m_sourceImageWidth;
	long m_sourceImageHeight;

	CString	m_strOldModelName;

	BOOL ProcessOnKillfocus(UINT nId);
	void DrawModelPosition();
	void SetControlTextLength();
	void AdjustSliderX(); 
	void AdjustSliderY(); 
	void InitModelImage();
	void InitializeData(); 
	BOOL GetModelFile(BOOL bMode);
	UINT RestoreModelFromFile();
	
	// Validation routines
	bool ValidateModelParameters(UINT& nMsgID);
	bool ValidateModelWidth(UINT& nMsgID);
	bool ValidateModelHeight(UINT& nMsgID);
	bool ValidateModelFilename(UINT& nMsgID);

	//
	void ClampModelPositionX();
	void ClampModelPositionY();

	void DrawCircularOverscanFigure(SVDlgImageGraphClass& rImage, const CRect& outerRect, const CRect& innerRect, COLORREF color);
	
	// Generated message map functions
	//{{AFX_MSG(SVPatModelPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnPaint();
	afx_msg void OnFileButton();
	afx_msg void OnKillXPosition();
	afx_msg void OnKillYPosition();
	afx_msg void OnKillModelHeight();
	afx_msg void OnKillModelWidth();
	afx_msg void OnCreateModel();
	afx_msg void OnKillFileName();
	afx_msg void OnCircularOverscanClicked();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPatSelectModelPageClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:20:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.10   10 Feb 2011 15:09:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.9   16 Dec 2009 12:25:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.8   31 Dec 2008 11:23:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  644
 * SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to support Circular Overscan.
 * Revised Field validation logic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.7   19 Jan 2004 10:45:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  370
 * SCR Title:  Display buffers have reached maxium number of available
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed all non-statndard paint functionality in replace of SVDlgImageClass functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.6   29 Oct 2003 14:04:32   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  393
 * SCR Title:  Create Model for Pattern Analyzer will save even if you press cancel
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed return type of GetModelFile to return FALSE if the user pressed cancel.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.5   22 Apr 2003 13:51:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.4   06 Mar 2003 13:17:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  328
 * SCR Title:  Upgrade SVObserver to MIL 7.1
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated to work with MIL 7.1
 * MvgaDispXXX no longer works. Create a copy of the DIB instead.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.3   30 Jan 2003 15:36:32   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OnPaint, InitModelImage, OnInitDialog, destructor and OnCreateModel methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
