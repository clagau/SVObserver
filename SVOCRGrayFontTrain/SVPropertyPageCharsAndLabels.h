//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVPropertyPageCharsAndLabels
//* .File Name       : $Workfile:   SVPropertyPageCharsAndLabels.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 10:31:48  $
//******************************************************************************

#if !defined(AFX_PROPERTYPAGECHARSANDLABELS_H__68C04C5F_ECEF_11D2_A7A0_00106F000C7D__INCLUDED_)
#define AFX_PROPERTYPAGECHARSANDLABELS_H__68C04C5F_ECEF_11D2_A7A0_00106F000C7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

//
// macros used to access the fields of the OcrFont object
//
#define CorObj_svOcrFontImages(sv) (&((sv)->images))
#define CorObj_svOcrFontLabels(sv) (&((sv)->labels))

#define __PIXEL_EDIT_DELAY  1              // Timer event ID

/////////////////////////////////////////////////////////////////////////////
// CSVPropertyPageCharsAndLabels dialog

class CSVPropertyPageCharsAndLabels : public CPropertyPage
{
	DECLARE_DYNCREATE(CSVPropertyPageCharsAndLabels)

	enum enumPixelEditorState
	{
		ENUM_CHAR_NOT_DEFINED = -1,
		ENUM_CHAR_CANCEL = 0,
		ENUM_CHAR_OK = 1,

		ENUM_EDIT_RUNNING = 2
	};

public:
	CSVPropertyPageCharsAndLabels();
	~CSVPropertyPageCharsAndLabels();

	void CleanUpWitEdit();

	//{{AFX_DATA(CSVPropertyPageCharsAndLabels)
	enum { IDD = IDD_PAGE_CHAR_AND_LABEL };
	CStatic	m_staticPenSize;
	CString	m_csStaticDirections;
	CListBox	m_listCharLabels;
	CString	m_csAddCharLabel;
	int		m_radioPenWhite;
	//}}AFX_DATA

	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSVPropertyPageCharsAndLabels)
	public:
	virtual BOOL OnKillActive();
	//virtual void OnCancel();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	void InitializeWitObjects();
	void SetForSelectCharFromList();
	void RebuildCharacterList();
	void CancelWitEdit();
	void DisplayPenSize();
	void SendPenColorToEdit();
	void OnPixelEditorCompletes();
	void OnStartPixelEditor();

	enum enumPixelEditorState m_enumPixelEditorState;

	BOOL m_bFirstTime;

	SVLVFastOCR *m_pFastOCR;

	int  m_nBrushSize;
	CorObj *m_pObjBrushColor;
	CorObj *m_pObjEditedCharImage; 

	CorEdit    m_hCorEdit;    // handle to edit operator
	CorDisplay m_hCorDisplay; // handle to display operator

	CorObj          *m_pObjExtractedCharImage; // pointer to image object extracted by SVPropertyPageSelectCharImages
	CorOcrFont      *m_pOcrFont;

	CorImageVector  *m_pCharImageVec;
	CorImage        *m_pCharImage;

	CorStringVector *m_pCharLabelVec;
	CorString       *m_pCharLabel;

	// Generated message map functions
	//{{AFX_MSG(CSVPropertyPageCharsAndLabels)
	afx_msg void OnButtonModify();
	afx_msg void OnCharOk();
	afx_msg void OnCharCancel();
	afx_msg void OnDblclkListCharLabels();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeListCharLabels();
	afx_msg void OnRadioPenWhite();
	afx_msg void OnRadioPenBlack();
	afx_msg void OnDeltaposSpinPenSize(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCharEditReset();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGECHARSANDLABELS_H__68C04C5F_ECEF_11D2_A7A0_00106F000C7D__INCLUDED_)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOCRGrayFontTrain\SVPropertyPageCharsAndLabels.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 10:31:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 15:08:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 15:20:54   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Aug 26 1999 14:24:20   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Port code from v2.42 and bug fixes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/