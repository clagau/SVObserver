//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVPropertyPageSelectCharImages
//* .File Name       : $Workfile:   SVPropertyPageSelectCharImages.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 16:39:12  $
//******************************************************************************

#if !defined(AFX_PROPERTYPAGESELECTCHARIMAGES_H__68C04C5E_ECEF_11D2_A7A0_00106F000C7D__INCLUDED_)
#define AFX_PROPERTYPAGESELECTCHARIMAGES_H__68C04C5E_ECEF_11D2_A7A0_00106F000C7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define __SKIP_START_BUTTON  2                   // Timer id

/////////////////////////////////////////////////////////////////////////////
// CSVPropertyPageSelectCharImages dialog

class CSVPropertyPageSelectCharImages : public CPropertyPage
{
	DECLARE_DYNCREATE(CSVPropertyPageSelectCharImages)

	enum enumROIGetDataResult
	{
		ENUM_ROI_OK = 0,
		ENUM_ROI_CANCEL = 1,
		ENUM_ROI_RESET = 2,
		ENUM_ROI_ACTIVE = 3,   // Wit 'getData' active - 10 Aug 1999 - frb.
		ENUM_ROI_ERROR = 4
	};

public:
	CSVPropertyPageSelectCharImages();
	~CSVPropertyPageSelectCharImages();

	void CleanUpWitGetData();   // 11 Aug 1999 - frb.

	//{{AFX_DATA(CSVPropertyPageSelectCharImages)
	enum { IDD = IDD_PAGE_SELECT_CHAR_IMAGES };
	CStatic	m_staticScaleFactor;
	CSpinButtonCtrl	m_spinCtrlScaleFactor;
	CString	m_csStaticDirections;
	//}}AFX_DATA

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSVPropertyPageSelectCharImages)
	public:
	//virtual void OnOK();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	//virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	enum enumROIGetDataResult  m_enumROIGetDataResult;
	double m_fScaleFactor;
	BOOL m_bFirstRoiSelected;

	SVLVFastOCR *m_pFastOCR;

	//abb
	CorDisplay m_hCorDisplay;
	CorGetData m_hCorGetData;

	CorObj   *m_pObjRoi;
	CorObj   *m_pObjStartImage;
	CorImage *m_pStartImage;

	void InitializeWitObjects();
	void DisplayScaleFactor();
	void SetDisplay();
	void SetGetData();
	void SetButtons();

	// Generated message map functions
	//{{AFX_MSG(CSVPropertyPageSelectCharImages)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonStartNewCharacter();
	afx_msg void OnRoiOk();
	afx_msg void OnRoiCancel();
	afx_msg void OnRoiReset();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGESELECTCHARIMAGES_H__68C04C5E_ECEF_11D2_A7A0_00106F000C7D__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOCRFontTraining\SVPropertyPageSelectCharImages.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 16:39:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   07 Aug 2000 12:55:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  173
 * SCR Title:  Fix OCR Font Training - Pressing ROI Ok button when no ROI is selected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated OCR FontTraining OCX to trap the error associated with trying to process a font image when no ROI was selected.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   16 May 2000 15:18:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 15:24:28   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Aug 26 1999 14:24:22   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Port code from v2.42 and bug fixes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/