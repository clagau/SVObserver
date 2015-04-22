//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLutDlg
//* .File Name       : $Workfile:   SVLutDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:19:10  $
//******************************************************************************


#ifndef __SVLUTDLG_H__
#define __SVLUTDLG_H__

#include "SVEnumerateCombo.h"
#include "SVOMFCLibrary\SVDlgGraph.h"
#include "SVVirtualCamera.h"

class SVLutDlg : public CPropertySheet
{
public:
	typedef std::map< SVGUID, SVLut > SVLutMap;

//	DECLARE_DYNAMIC(SVLutDlg)
	SVLutDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~SVLutDlg();

	bool Create( SVVirtualCameraPtrSet& setCameras, SVLutMap& raLut );

// Operator(s):
protected:
//    void CreatePages();
	void DestroyAllPages();

// Class Wizard Generated Message Map Entries
protected:
	//{{AFX_MSG(SVLutDlg)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Class Wizard Generated Virtual Function(s):
	virtual BOOL OnInitDialog();
	//{{AFX_VIRTUAL(SVLutDlg)
	//}}AFX_VIRTUAL

// Debug Operation(s):
public:

// Data Element(s):
protected:
};




class SVLutDlgPage : public CPropertyPage
{
// Construction
public:
	SVLutDlgPage( SVLutDlg* Parent, const CString& sCaption);

// Operator(s):
public:
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVLutDlgPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
public:
	SVLut                       mLut;
	SVVirtualCamera*            mpCamera;   // for band information
	SvCl::SVObjectLongArrayClass      malBandData;

	//{{AFX_DATA(SVLutDlgPage)
	enum { IDD = IDD_LUT_DIALOG };
	CSliderCtrl	mUpperSlider;
	CSliderCtrl	mLowerSlider;
	SvOml::SVDlgGraphClass	mLUTGraph;
	SVEnumerateComboClass	mLutModeCombo;
	CString	mstrUpperClipValue;
	CString	mstrLowerClipValue;
	BOOL	mbUseLUT;
	BOOL	mbContinuousRecalcLUT;
	int		m_iBand;
	int		m_iX1;
	int		m_iX2;
	int		m_iY1;
	int		m_iY2;
	CString	m_sRangeXMax;
	CString	m_sRangeYMax;
	CString	m_sRangeXYMax;
	//}}AFX_DATA

	static BOOL CALLBACK GraphMousePointCallback( POINT Point, LPVOID pThis_ );


protected:
	SVLutDlg* mpParentDialog;
	SVLutTransformOperationEnum meLutMode;

	int miCurrentBand;	// for rgb color systems
    int miNumBands;
    int miBandWidth;
    int miCamBand;

	COLORREF mrgbLineColor;

	CPoint mptLastMousePos;
	static const CPoint ptNoLastMousePos;

	static SVLutTransformOperationMap m_mapOperations;

	long mlMaxLutValue;

protected:
	
	void Refresh();
	void SetLutParameters(const SVLutTransformParameters& param);
	void ShowControls(bool bShow, SVLutTransformOperationEnum e = LutTransformTypeUnknown );
	void EnableControls(bool bEnable);

	// Generated message map functions
	//{{AFX_MSG(SVLutDlgPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnActivateCheck();
	afx_msg void OnSelchangeLutModeCombo();
	afx_msg void OnContinuousRecalcCheck();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnColorBandAll();
	afx_msg void OnColorBandBlue();
	afx_msg void OnColorBandGreen();
	afx_msg void OnColorBandRed();
	afx_msg void OnColorBandSync();
	afx_msg void OnChangeX1();
	afx_msg void OnChangeX2();
	afx_msg void OnChangeY1();
	afx_msg void OnChangeY2();
	//}}AFX_MSG
	void OnChangeKnee();
	afx_msg LRESULT OnGraphRefresh(WPARAM mp1, LPARAM mp2);
	DECLARE_MESSAGE_MAP()

};



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif	//#ifndef __SVLUTDLG_H__

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVLutDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:19:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   08 Aug 2012 15:42:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  761
 * SCR Title:  Fix issue with Clip on Adjust LUT Dialog using GigE cameras
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated code to fix issues with LUT and Light Reference Dialogs when camera has no LUT or Light Reference.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   28 Sep 2011 12:58:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   03 Mar 2010 12:03:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove duplicate containers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 Jun 2007 13:28:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   22 Apr 2003 10:58:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jan 2003 09:51:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   General Lut changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jan 2003 12:37:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   First iteration of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
