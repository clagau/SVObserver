//******************************************************************************
//* COPYRIGHT (c) 1999 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCRFontTrainingCtrl
//* .File Name       : $Workfile:   SVOCRFontTrainingCtl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   24 Apr 2013 16:37:40  $
//******************************************************************************

#if !defined(AFX_SVOCRFONTTRAININGCTL_H__9AEFBD76_F061_11D2_A7B2_00106F000C7D__INCLUDED_)
#define AFX_SVOCRFONTTRAININGCTL_H__9AEFBD76_F061_11D2_A7B2_00106F000C7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSVOCRFontTrainingCtrl : public COleControl
{
	DECLARE_DYNCREATE(CSVOCRFontTrainingCtrl)

// Constructor
public:
	CSVOCRFontTrainingCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVOCRFontTrainingCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CSVOCRFontTrainingCtrl();

	DECLARE_OLECREATE_EX(CSVOCRFontTrainingCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CSVOCRFontTrainingCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CSVOCRFontTrainingCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CSVOCRFontTrainingCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CSVOCRFontTrainingCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CSVOCRFontTrainingCtrl)
	afx_msg long DoSVOCRFontTraining(LPCTSTR PathFileSampleCharImage, LPCTSTR PathFileFontImagesAndLabels);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CSVOCRFontTrainingCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CSVOCRFontTrainingCtrl)
	dispidDoSVOCRFontTraining = 1L,
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVOCRFONTTRAININGCTL_H__9AEFBD76_F061_11D2_A7B2_00106F000C7D__INCLUDED)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOCRFontTraining\SVOCRFontTrainingCtl.h_v  $
 * 
 *    Rev 1.0   24 Apr 2013 16:37:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   16 May 2000 15:17:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 15:21:16   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   Aug 26 1999 14:22:00   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Port code and bug fixes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/