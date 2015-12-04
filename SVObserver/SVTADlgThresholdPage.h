//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVToolAdjustmentDialogThresholdPage
//* .File Name       : $Workfile:   SVTADlgThresholdPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   26 Jun 2014 18:21:16  $
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVMFCControls\SVEditNumbers.h"
#include "SVTaskObjectInterfaceClass.h"
#include "svhistogram.h"
//TODO: MZA(10.Nov 2014): Move this files to SVOGui project and then remove folder from include and Namespace add-on add PictureDisplay declaration.
#include "SVOGui/PictureDisplay.h"
#pragma endregion Includes

class SVToolAdjustmentDialogSheetClass;
class SVToolClass;
class SVUnaryImageOperatorListClass;
class SVThresholdClass;
class SVBoolValueObjectClass;
class SVImageClass;

// Threshold Dialog
void AFXAPI DDX_SVThresholdText( CDataExchange* pDX, int nIDC, DWORD& Value );
BOOL GetThreshold( CWnd* pWnd, DWORD& Value );
BOOL GetThreshold( HWND hWnd, DWORD& Value );
void SetThreshold( CWnd* pWnd, DWORD Value );
void SetThreshold( HWND hWnd, DWORD Value );

class SVToolAdjustmentDialogThresholdPageClass : public CPropertyPage, public SVTaskObjectInterfaceClass
{
	//******************************************************************************
	// Class Wizard Generated Message Map Entries:
	//******************************************************************************
	// Generated message map functions
	//{{AFX_MSG(SVToolAdjustmentDialogThresholdPageClass)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeUpperEdit();
	afx_msg void OnChangeLowerEdit();
	afx_msg void OnUpperActivateCheck();
	afx_msg void OnLowerActivateCheck();
	afx_msg void OnCheck1();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnAutoThresholdCheck();
	afx_msg void OnWhiteBackgroundRadio();
	afx_msg void OnBlackBackgroundRadio();
	afx_msg void OnChangeAutothresholdEdit();
	afx_msg void OnUTFormulaButton();
	afx_msg void OnATMFormulaButton();
	afx_msg void OnLTFormulaButton();
	afx_msg void OnUseExternATMCheck();
	afx_msg void OnUseExternLTCheck();
	afx_msg void OnUseExternUTCheck();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg LRESULT OnBoundChange(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	void UpdateLowerThresholdFromFormula();
	void UpdateUpperThresholdFromFoumula();

	SVImageClass* getReferenceImage(SVToolClass* pTool, SVThresholdClass* pCurrentThreshold);

public:
	SVToolAdjustmentDialogThresholdPageClass(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* pParent);
	virtual ~SVToolAdjustmentDialogThresholdPageClass();

	virtual HRESULT SetInspectionData() override;

	//************************************
	// Method:    setImages
	// Description: Set image to the activeX-control. 
	// Access:    public 
	// Returns:   void
	//************************************
	void setImages();

	void initThreshold();

	//******************************************************************************
	// Class Wizard Generated Virtual Function(s):
	//******************************************************************************
	//{{AFX_VIRTUAL(SVToolAdjustmentDialogThresholdPageClass)
public:
	virtual BOOL OnSetActive();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV
	//}}AFX_VIRTUAL

	//******************************************************************************
	// Class Wizard Generated Dialog Data:
	//******************************************************************************
protected:
	//{{AFX_DATA(SVToolAdjustmentDialogThresholdPageClass)
	enum { IDD = IDD_TA_THRESHOLD_DIALOG };
	SvMc::CEditNumbers	m_editLowerThres;
	SvMc::CEditNumbers	m_editUpperThres;
	SvMc::CEditNumbersFloat	m_editAutoThreshold;
	CButton	m_autoThresholdWhiteRadio;
	CButton	m_autoThresholdBlackRadio;
	CSliderCtrl	m_autoThresholdCtrl;
	SvOg::PictureDisplay m_dialogImage;
	CSliderCtrl	m_lowerThreshold;
	CSliderCtrl	m_upperThreshold;
	BOOL	m_upperThresholdActive;
	BOOL	m_thresholdActive;
	BOOL	m_lowerThresholdActive;
	CString	m_strWhitePixel;
	BOOL	m_bUseAutoThreshold;
	CString	m_csEditAutoThreshold;
	CString	m_csUpperThres;
	CString	m_csLowerThres;
	BOOL	m_bUseExternATM;
	BOOL	m_bUseExternLT;
	BOOL	m_bUseExternUT;
	//}}AFX_DATA

public:
	SVToolAdjustmentDialogSheetClass* m_pParentDialog;
	BOOL m_isEdit;

protected:
	SVToolClass* m_pTool;
	SVThresholdClass* m_pCurrentThreshold;

	SVBoolValueObjectClass* m_pUseExternATM;
	SVBoolValueObjectClass* m_pUseExternLT;
	SVBoolValueObjectClass* m_pUseExternUT;

private:
	long m_lo;		// storage of slider pos, if deactivated...
	long m_up;		// storage of slider pos, if deactivated...
	double m_dAutoThreshold;
	long m_upperThres;	// current slider pos...
	long m_lowerThres;	// current slider pos...
	long m_histState;

public:
	SVHistogram m_histogram;
};

////////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// DevStudio inserts additional declarations immediate in front of the preceding line
////////////////////////////////////////////////////////////////////////////////


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVTADlgThresholdPage.h_v  $
 * 
 *    Rev 1.2   26 Jun 2014 18:21:16   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use SVPictureDisplay-control
 * cleanup
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Jul 2013 14:01:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  850
 * SCR Title:  Fix Bug in Threshold where edit box is not updated or grayed when appropriate
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Upper and Lower Threshold Edit boxs so they have updated information from the formula when formula is  used.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   24 Apr 2013 11:23:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   21 Oct 2008 15:00:02   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  625
 * SCR Title:  Functional Requirement for Tool Adjustment Dialog - Histogram Control
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added Histogram Control
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   09 Aug 2005 08:25:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   22 Apr 2003 16:21:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   04 Apr 2002 10:25:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  286
 * SCR Title:  Upgrade SVObserver to Version 3.37 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code syntax to allow rational rose to reverse engineer properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   23 Mar 2000 16:45:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  118
 * SCR Title:  Integration of LUT Operator
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to work with new In Place Image Operator List
 * Revised to Run complete ToolSet istead of just the Operator List.
 * Revised to do a fast image and result view update, so 
 * changes can be seen immediately.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 13:43:14   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   19 Oct 1999 17:30:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  48
 * SCR Title:  Threshold Page Formula
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Check in. Revised Threshold adjustment page class.
 * (Moved from SVToolAdjustmentDialog)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
